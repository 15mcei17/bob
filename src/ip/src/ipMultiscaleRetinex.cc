#include "ip/ipMultiscaleRetinex.h"
#include "ip/ipMSRSQIGaussian.h"
#include "ip/ipRescaleGray.h"


/////////////////////////////////////////////////////////////////////////////////////////

namespace Torch {

/////////////////////////////////////////////////////////////////////////////////////////
// Constructor
ipMultiscaleRetinex::ipMultiscaleRetinex() 
	    : 	ipCore()
{
  addIOption("s_nb", 1, "Number of different scales (Singlescale Retinex <-> 1)");
  addIOption("s_min", 1, "Minimum scale: (2*s_min+1)");
  addIOption("s_step", 1, "Scale step: (2*s_step)");
  addDOption("sigma", 0.6, "Variance of the kernel for the minimum scale");
}

/////////////////////////////////////////////////////////////////////////////////////////
// Destructor
ipMultiscaleRetinex::~ipMultiscaleRetinex()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
// Check if the input tensor has the right dimensions and type
bool ipMultiscaleRetinex::checkInput(const Tensor& input) const
{
	// Accept only 3D tensors of Torch::Image type
	if (    input.nDimension() != 3 ||
	        input.getDatatype() != Tensor::Short)
	{
		warning("ipMultiscaleRetinex::checkInput(): Incorrect Tensor type and dimension.");
		return false;
	}
	// Accept only gray images
	if (	input.size(2) !=1 )
	{
		warning("ipMultiscaleRetinex::checkInput(): Non grayscale image (multiple channels).");
		return false;
	}

	// OK
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Allocate (if needed) the output tensors given the input tensor dimensions
bool ipMultiscaleRetinex::allocateOutput(const Tensor& input)
{
	if (	m_output == 0 ||
		m_output[0]->nDimension() != 3 ||
		m_output[0]->size(0) != input.size(0) ||
		m_output[0]->size(0) != input.size(1) ||
		m_output[0]->size(0) != input.size(2) )
	{
		cleanup();

		// Need allocation
		m_n_outputs = 1;
		m_output = new Tensor*[m_n_outputs];
		m_output[0] = new ShortTensor(input.size(0), input.size(1), input.size(2));
		return true;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Process some input tensor (the input is checked, the outputs are allocated)
bool ipMultiscaleRetinex::processInput(const Tensor& input)
{
	// Get parameters
  const int s_nb = getIOption("s_nb");
  const int s_min = getIOption("s_min");
  const int s_step = getIOption("s_step");
	const double sigma =  getDOption("sigma");

	// Prepare pointers to access pixels
	const ShortTensor* t_input = (ShortTensor*)&input;
	ShortTensor* t_output = (ShortTensor*)m_output[0];

  // An index for the 3D tensor is: [y * stride_h + x * stride_w + p * stride_p]
  const int width = input.size(1);
  const int height = input.size(0);


	// Compute Multi-scale Gaussian Filtering
	Tensor** filtered_array = new Tensor*[s_nb];
	ipCore *gaussian = 0;


	for (int s = 0; s < s_nb; s++)
		filtered_array[s] = 0;
    

	// Generate the Gaussian filters
	for (int s = 0; s < s_nb; s++) 
	{
		// size of the kernel 
		int s_size = s_min + s * s_step;
		// sigma of the kernel
		double s_sigma = sigma * s_size / s_min;

		gaussian = new ipMSRSQIGaussian();
		CHECK_FATAL(gaussian->setIOption("RadiusX", s_size) == true);
 		CHECK_FATAL(gaussian->setIOption("RadiusY", s_size) == true);
 		CHECK_FATAL(gaussian->setDOption("Sigma", s_sigma) == true);
 		CHECK_FATAL(gaussian->setBOption("Weighed", false) == true);
		CHECK_FATAL(gaussian->process(*t_input) == true);

   		// Copy the output of ipGaussian to the array of filters 
		filtered_array[s] = new ShortTensor(input.size(0), input.size(1), input.size(2));
    const ShortTensor *out_g = (const ShortTensor*)&gaussian->getOutput(0);
		filtered_array[s]->copy( out_g ); 
		delete gaussian;
	}

	// Allocate a tensor which contains double (because of the use of the non linear function)
	DoubleTensor* dst_double=new DoubleTensor(input.size(0), input.size(1), 1);

	// Initialize the tensor to zero
	dst_double->fill(0.);
	
	// Filter the image several times and update the tensor values
	for (int s = 0; s < s_nb ; s++)
	{
    const ShortTensor *s_filter = (const ShortTensor*)filtered_array[s];
   	for (int y = 0; y < height; y++ )
		{
     	for (int x = 0; x < width; x++ )
     	{
				// +1 inside the log to avoid log(0). Could choose a smaller value
				// TODO: make alternative nonlinear transformation such as
				// arctan, sigmoid possible instead of logarithm
        (*dst_double)(y,x,0) += log( (*t_input)(y,x,0) + 1. ) - log( (*s_filter)(y,x,0) + 1.);
			}
		}
	}

	// Rescale the values in [0,255] and copy it into the output Tensor
	ipCore *rescale = new ipRescaleGray();
	CHECK_FATAL(rescale->process(*dst_double) == true);
  const ShortTensor* out = ((const ShortTensor*)&rescale->getOutput(0));
	t_output->copy( out );

	// clean up
	delete dst_double;
	delete rescale;

	for (int s = 0; s < s_nb ; s++)
		delete filtered_array[s];

	delete[] filtered_array;
	return true;
}

}

