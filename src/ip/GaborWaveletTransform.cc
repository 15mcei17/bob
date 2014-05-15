/**
 * @date 2012-02-27
 * @author Manuel Guenther <Manuel.Guenther@idiap.ch>
 *
 * @brief This file provides a class to perform a Gabor wavelet transform.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#include "bob/core/assert.h"
#include "bob/core/array_copy.h"
#include "bob/ip/GaborWaveletTransform.h"
#include <numeric>
#include <sstream>
#include <fstream>

static inline double sqr(double x){return x*x;}

/**
 * Generates a Gabor kernel.
 * @param resolution The resolution of the image to generate
 * @param k  The frequency vector (i.e. the center of the Gaussian in frequency domain)
 * @param sigma  The standard deviation (i.e. the width of the Gabor wavelet)
 * @param pow_of_k  The power of \f$ k^x \f$ used as a prefactor of the Gabor wavelet
 * @param dc_free   Make the Gabor wavelet DC-free?
 * @param epsilon   The epsilon value below which the wavelet value is considered as zero
 */
bob::ip::GaborKernel::GaborKernel(
  const blitz::TinyVector<unsigned,2>& resolution,
  const blitz::TinyVector<double,2>& k,
  const double sigma,
  const double pow_of_k,
  const bool dc_free,
  const double epsilon
)
: m_x_resolution(resolution[1]),
  m_y_resolution(resolution[0])
{
  // create Gabor wavelet with given parameters
  int32_t start_x = - (int)m_x_resolution / 2, start_y = - (int)m_y_resolution / 2;
  // take care of odd resolutions in the end points
  int32_t end_x = m_x_resolution / 2 + m_x_resolution % 2, end_y = m_y_resolution / 2 + m_y_resolution % 2;

  double k_x_factor = 2. * M_PI / m_x_resolution, k_y_factor = 2. * M_PI / m_y_resolution;
  double kx = k[1], ky = k[0];

  // iterate over all pixels of the images
  for (int y = start_y; y < end_y; ++y){

    // convert relative pixel coordinate into frequency coordinate
    double omega_y = y * k_y_factor;

    for (int x = start_x; x < end_x; ++x){

      // convert relative pixel coordinate into frequency coordinate
      double omega_x = x * k_x_factor;

      // compute value of frequency kernel function
      double omega_minus_k_squared = sqr(omega_x - kx) + sqr(omega_y - ky);
      double sigma_square = sqr(sigma);
      double k_square = sqr(kx) + sqr(ky);
      // assign kernel value
      double wavelet_value = exp(- sigma_square * omega_minus_k_squared / (2. * k_square));

      // prefactor the wavelet value with k^(pow_of_k); the default prefactor 1 might not be the best.
      wavelet_value *= std::pow(k_square, pow_of_k / 2.);

      if (dc_free){
        double omega_square = sqr(omega_x) + sqr(omega_y);

        wavelet_value -= exp(-sigma_square * (omega_square + k_square) / (2. * k_square));
      } // if ! dc_free

      if (std::abs(wavelet_value) > epsilon){
        m_kernel_pixel.push_back(std::make_pair(
          blitz::TinyVector<int,2>((y + m_y_resolution) % m_y_resolution, (x + m_x_resolution) % m_x_resolution),
          wavelet_value
        ));
      }
    } // for x
  } // for y
}

bob::ip::GaborKernel::GaborKernel(
  const bob::ip::GaborKernel& other
)
: m_kernel_pixel(other.m_kernel_pixel.size()),
  m_x_resolution(other.m_x_resolution),
  m_y_resolution(other.m_y_resolution)
{
  std::copy(other.m_kernel_pixel.begin(), other.m_kernel_pixel.end(), m_kernel_pixel.begin());
}

bob::ip::GaborKernel&
bob::ip::GaborKernel::operator =
(
  const bob::ip::GaborKernel& other
)
{
  m_x_resolution = other.m_x_resolution;
  m_y_resolution = other.m_y_resolution;
  m_kernel_pixel.resize(other.m_kernel_pixel.size());
  std::copy(other.m_kernel_pixel.begin(), other.m_kernel_pixel.end(), m_kernel_pixel.begin());
  return *this;
}

bool
bob::ip::GaborKernel::operator ==
(
  const bob::ip::GaborKernel& other
) const
{
  if (m_x_resolution != other.m_x_resolution || m_y_resolution != other.m_y_resolution)
    return false;
  if (m_kernel_pixel.size() != other.m_kernel_pixel.size())
    return false;

  std::vector<std::pair<blitz::TinyVector<unsigned,2>, double> >::const_iterator it1 = m_kernel_pixel.begin(), it2 = other.m_kernel_pixel.begin(), it1end = m_kernel_pixel.end();
  for (; it1 != it1end; ++it1, ++it2)
    if (it1->first[0] != it2->first[0] || it1->first[1] != it2->first[1] || std::abs(it1->second - it2->second) > 1e-8)
      return false;

  // identical.
  return true;
}

bool
bob::ip::GaborKernel::operator !=
(
  const bob::ip::GaborKernel& other
) const
{
  if (m_x_resolution != other.m_x_resolution || m_y_resolution != other.m_y_resolution)
    return true;
  if (m_kernel_pixel.size() != other.m_kernel_pixel.size())
    return true;

  std::vector<std::pair<blitz::TinyVector<unsigned,2>, double> >::const_iterator it1 = m_kernel_pixel.begin(), it2 = other.m_kernel_pixel.begin(), it1end = m_kernel_pixel.end();
  for (; it1 != it1end; ++it1, ++it2)
    if (it1->first[0] != it2->first[0] || it1->first[1] != it2->first[1] || std::abs(it1->second - it2->second) > 1e-8)
      return true;

  // identical.
  return false;
}

/**
 * Performs the convolution of the given image with this Gabor kernel.
 * Please note that both the inpus as well as the output image are in frequency domain.
 * @param frequency_domain_image
 * @param transformed_frequency_domain_image
 */
void bob::ip::GaborKernel::transform(
  const blitz::Array<std::complex<double>,2>& frequency_domain_image,
  blitz::Array<std::complex<double>,2>& transformed_frequency_domain_image
) const
{
  // assert same size
  bob::core::array::assertSameShape(frequency_domain_image, transformed_frequency_domain_image);
  // clear resulting image first
  transformed_frequency_domain_image = std::complex<double>(0);
  // iterate through the kernel pixels and do the multiplication
  std::vector<std::pair<blitz::TinyVector<unsigned,2>, double> >::const_iterator it = m_kernel_pixel.begin(), it_end = m_kernel_pixel.end();
  for (; it < it_end; ++it){
    transformed_frequency_domain_image(it->first) = frequency_domain_image(it->first) * it->second;
  }
}

/**
 * Generates and returns the image for the current kernel.
 * @return The kernel image in frequency domain.
 */
blitz::Array<double,2> bob::ip::GaborKernel::kernelImage() const{
  blitz::Array<double,2> image(m_y_resolution, m_x_resolution);
  image = 0;
  // iterate through the kernel pixels
  std::vector<std::pair<blitz::TinyVector<unsigned,2>, double> >::const_iterator it = m_kernel_pixel.begin(), it_end = m_kernel_pixel.end();
  for (; it < it_end; ++it){
    image(it->first) = it->second;
  }
  return image;
}

/***********************************************************************************
******************     GaborWaveletTransform      **********************************
***********************************************************************************/
/**
 * Initializes a discrete family of Gabor wavelets
 * @param number_of_scales     The number of scales (frequencies) to generate
 * @param number_of_directions The number of directions (orientations) to generate
 * @param sigma                The width (standard deviation) of the Gabor wavelet
 * @param k_max                The highest frequency to generate (maximum: PI)
 * @param k_fac                The logarithmical factor between two scales of Gabor wavelets; should be below one
 * @param pow_of_k             The power of k for the prefactor
 * @param dc_free              Make the Gabor wavelet DC-free?
 */
bob::ip::GaborWaveletTransform::GaborWaveletTransform(
  unsigned number_of_scales,
  unsigned number_of_directions,
  double sigma,
  double k_max,
  double k_fac,
  double pow_of_k,
  bool dc_free
)
: m_sigma(sigma),
  m_pow_of_k(pow_of_k),
  m_k_max(k_max),
  m_k_fac(k_fac),
  m_dc_free(dc_free),
  m_fft(),
  m_ifft(),
  m_number_of_scales(number_of_scales),
  m_number_of_directions(number_of_directions)
{
  computeKernelFrequencies();
}


bob::ip::GaborWaveletTransform::GaborWaveletTransform(
  const bob::ip::GaborWaveletTransform & other
)
: m_sigma(other.m_sigma),
  m_pow_of_k(other.m_pow_of_k),
  m_k_max(other.m_k_max),
  m_k_fac(other.m_k_fac),
  m_dc_free(other.m_dc_free),
  m_fft(),
  m_ifft(),
  m_number_of_scales(other.m_number_of_scales),
  m_number_of_directions(other.m_number_of_directions)
{
  computeKernelFrequencies();
}

bob::ip::GaborWaveletTransform::GaborWaveletTransform(
  bob::io::HDF5File& file
)
{
  load(file);
}


bob::ip::GaborWaveletTransform&
bob::ip::GaborWaveletTransform::operator =
(
  const bob::ip::GaborWaveletTransform & other
)
{
  m_sigma = other.m_sigma;
  m_pow_of_k = other.m_pow_of_k;
  m_k_max = other.m_k_max;
  m_k_fac = other.m_k_fac;
  m_dc_free = other.m_dc_free;
  m_fft = bob::sp::FFT2D();
  m_ifft = bob::sp::IFFT2D();
  m_number_of_scales = other.m_number_of_scales;
  m_number_of_directions = other.m_number_of_directions;

  computeKernelFrequencies();

  return *this;
}

bool
bob::ip::GaborWaveletTransform::operator ==
(
  const bob::ip::GaborWaveletTransform & other
) const
{
#define aeq(x1,x2) std::abs(x1-x2) < 1e-8

  return aeq(m_sigma, other.m_sigma) &&
         aeq(m_pow_of_k,other.m_pow_of_k) &&
         aeq(m_k_max, other.m_k_max) &&
         aeq(m_k_fac, other.m_k_fac) &&
         m_dc_free == other.m_dc_free &&
         m_number_of_scales == other.m_number_of_scales &&
         m_number_of_directions == other.m_number_of_directions;

#undef aeq
}


bool
bob::ip::GaborWaveletTransform::operator !=
(
  const bob::ip::GaborWaveletTransform & other
) const
{
#define neq(x1,x2) std::abs(x1-x2) > 1e-8

  return neq(m_sigma, other.m_sigma) ||
         neq(m_pow_of_k,other.m_pow_of_k) ||
         neq(m_k_max, other.m_k_max) ||
         neq(m_k_fac, other.m_k_fac) ||
         m_dc_free != other.m_dc_free ||
         m_number_of_scales != other.m_number_of_scales ||
         m_number_of_directions != other.m_number_of_directions;

#undef neq
}


/**
 * Private function that computes the frequency vectors of the Gabor kernels
 */
void bob::ip::GaborWaveletTransform::computeKernelFrequencies(){
  // reserve enough space
  m_kernel_frequencies.clear();
  m_kernel_frequencies.reserve(m_number_of_scales * m_number_of_directions);
  // initialize highest frequency
  double k_abs = m_k_max;
  // iterate over the scales
  for (unsigned s = 0; s < m_number_of_scales; ++s){

    // iterate over the directions
    for (unsigned d = 0; d < m_number_of_directions; ++d )
    {
      double angle = M_PI * d / m_number_of_directions;
      // compute center of kernel in frequency domain in Cartesian coordinates
      m_kernel_frequencies.push_back(
        blitz::TinyVector<double,2>(k_abs * sin(angle), k_abs * cos(angle)));
    } // for d

    // move to the next frequency scale
    k_abs *= m_k_fac;
  } // for s
}

/**
 * Generates the kernels for the given image resolution.
 * This function dose not need to be called explicitly to be able to perform the GWT.
 * @param resolution  The resolution of the image to generate the kernels for
 */
void bob::ip::GaborWaveletTransform::generateKernels(
  blitz::TinyVector<unsigned,2> resolution
)
{
  if (resolution[1] != m_fft.getWidth() || resolution[0] != m_fft.getHeight()){
    // new kernels need to be generated
    m_gabor_kernels.clear();
    m_gabor_kernels.reserve(m_kernel_frequencies.size());

    for (unsigned j = 0; j < m_kernel_frequencies.size(); ++j){
      m_gabor_kernels.push_back(bob::ip::GaborKernel(resolution, m_kernel_frequencies[j], m_sigma, m_pow_of_k, m_dc_free));
    }

    // reset fft sizes
    m_fft.setShape(resolution[0], resolution[1]);
    m_ifft.setShape(resolution[0], resolution[1]);
    m_temp_array.resize(blitz::shape(resolution[0],resolution[1]));
    m_temp_array2.resize(m_temp_array.shape());
    m_frequency_image.resize(m_temp_array.shape());
  }
}

/**
 * Generates and returns the images of the Gabor wavelet family in frequency domain.
 * @return  The Gabor wavelets (one per layer)
 */
blitz::Array<double,3> bob::ip::GaborWaveletTransform::kernelImages() const{
  // generate array of desired size
  blitz::Array<double,3> res(m_gabor_kernels.size(), m_temp_array.shape()[0], m_temp_array.shape()[1]);
  // fill in the wavelets
  for (int j = m_gabor_kernels.size(); j--;){
    res(j, blitz::Range::all(), blitz::Range::all()) = m_gabor_kernels[j].kernelImage();
  }
  return res;
}

/**
 * Computes the Gabor wavelet transformation for the given image (in spatial domain)
 * @param gray_image  The source image in spatial domain
 * @param trafo_image The convolution result, in spatial domain
 */
void bob::ip::GaborWaveletTransform::performGWT(
  const blitz::Array<std::complex<double>,2>& gray_image,
  blitz::Array<std::complex<double>,3>& trafo_image
)
{
  // first, check if we need to reset the kernels
  generateKernels(blitz::TinyVector<unsigned,2>(gray_image.extent(0),gray_image.extent(1)));

  // perform Fourier transformation to image
  m_fft(gray_image, m_frequency_image);

  // check that the shape is correct
  bob::core::array::assertSameShape(trafo_image, blitz::shape(m_kernel_frequencies.size(),gray_image.extent(0),gray_image.extent(1)));

  // now, let each kernel compute the transformation result
  for (unsigned j = 0; j < m_gabor_kernels.size(); ++j){
    // get a reference to the current layer of the trafo image
    m_gabor_kernels[j].transform(m_frequency_image, m_temp_array);
    // perform ifft on the trafo image layer
    blitz::Array<std::complex<double>,2> layer(trafo_image(j, blitz::Range::all(), blitz::Range::all()));
    m_ifft(m_temp_array, layer);
  } // for j
}

/**
 * Computes the Gabor jets including absolute values and phases for the given image (in spatial domain).
 * @param gray_image  The source image in spatial domain
 * @param jet_image   The resulting Gabor jet image, including absolute values and phases for each pixel
 * @param do_normalize Shall the Gabor jets be normalized?
 */
void bob::ip::GaborWaveletTransform::computeJetImage(
  const blitz::Array<std::complex<double>,2>& gray_image,
  blitz::Array<double,4>& jet_image,
  bool do_normalize
)
{
  // first, check if we need to reset the kernels
  generateKernels(blitz::TinyVector<unsigned,2>(gray_image.extent(0),gray_image.extent(1)));

  // perform Fourier transformation to image
  m_fft(gray_image, m_frequency_image);

  // check that the shape is correct
  bob::core::array::assertSameShape(jet_image, blitz::shape(gray_image.extent(0), gray_image.extent(1), 2, m_kernel_frequencies.size()));

  // now, let each kernel compute the transformation result
  for (int j = 0; j < (int)m_gabor_kernels.size(); ++j){
    // get a reference to the current layer of the trafo image
    m_gabor_kernels[j].transform(m_frequency_image, m_temp_array2);
    // perform ifft of transformed image
    m_ifft(m_temp_array2, m_temp_array);
    // convert into absolute and phase part
    blitz::Array<double,2> abs_part(jet_image(blitz::Range::all(), blitz::Range::all(), 0, j));
    abs_part = blitz::abs(m_temp_array);
    blitz::Array<double,2> phase_part(jet_image(blitz::Range::all(), blitz::Range::all(), 1, j));
    phase_part = blitz::arg(m_temp_array);
  } // for j

  if (do_normalize){
    // iterate the positions
    for (int y = jet_image.extent(0); y--;){
      for (int x = jet_image.extent(1); x--;){
        // normalize jet
        blitz::Array<double,2> jet(jet_image(y,x,blitz::Range::all(),blitz::Range::all()));
        bob::ip::normalizeGaborJet(jet);
      }
    }
  }
}

/**
 * Computes the Gabor jets including absolute values only for the given image (in spatial domain).
 * @param gray_image  The source image in spatial domain
 * @param jet_image   The resulting Gabor jet image, including only absolute values for each pixel
 * @param do_normalize Shall the Gabor jets be normalized?
 */
void bob::ip::GaborWaveletTransform::computeJetImage(
  const blitz::Array<std::complex<double>,2>& gray_image,
  blitz::Array<double,3>& jet_image,
  bool do_normalize
)
{
  // first, check if we need to reset the kernels
  generateKernels(blitz::TinyVector<int,2>(gray_image.extent(0),gray_image.extent(1)));

  // perform Fourier transformation to image
  m_fft(gray_image, m_frequency_image);

  // check that the shape is correct
  bob::core::array::assertSameShape(jet_image, blitz::shape(gray_image.extent(0), gray_image.extent(1), m_kernel_frequencies.size()));

  // now, let each kernel compute the transformation result
  for (int j = 0; j < (int)m_gabor_kernels.size(); ++j){
    // get a reference to the current layer of the trafo image
    m_gabor_kernels[j].transform(m_frequency_image, m_temp_array2);
    // perform ifft of transformed image
    m_ifft(m_temp_array2, m_temp_array);
    // convert into absolute part
    blitz::Array<double,2> abs_part(jet_image(blitz::Range::all(), blitz::Range::all(), j));
    abs_part = blitz::abs(m_temp_array);
  } // for j

  if (do_normalize){
    // iterate the positions
    for (int y = jet_image.extent(0); y--;){
      for (int x = jet_image.extent(1); x--;){
        // normalize jet
        blitz::Array<double,1> jet(jet_image(y,x,blitz::Range::all()));
        bob::ip::normalizeGaborJet(jet);
      }
    }
  }
}

void bob::ip::GaborWaveletTransform::save(bob::io::HDF5File& file) const{
  file.set("Sigma", m_sigma);
  file.set("PowOfK", m_pow_of_k);
  file.set("KMax", m_k_max);
  file.set("KFac", m_k_fac);
  file.set("DCfree", m_dc_free);
  file.set("NumberOfScales", m_number_of_scales);
  file.set("NumberOfDirections", m_number_of_directions);
}

void bob::ip::GaborWaveletTransform::load(bob::io::HDF5File& file){
  m_sigma = file.read<double>("Sigma");
  m_pow_of_k = file.read<double>("PowOfK");
  m_k_max = file.read<double>("KMax");
  m_k_fac = file.read<double>("KFac");
  m_dc_free = file.read<bool>("DCfree");
  m_number_of_scales = file.read<unsigned>("NumberOfScales");
  m_number_of_directions = file.read<unsigned>("NumberOfDirections");

  computeKernelFrequencies();
}

/**
 * Normalizes the given Gabor jet (absolute values only) to unit length.
 * @param gabor_jet The Gabor jet to be normalized.
 */
void bob::ip::normalizeGaborJet(blitz::Array<double,1>& gabor_jet){
  double norm = sqrt(std::inner_product(gabor_jet.begin(), gabor_jet.end(), gabor_jet.begin(), 0.));
  // normalize the absolute parts of the jets
  gabor_jet /= norm;
}


/**
 * Normalizes the given Gabor jet to unit length.
 * @param gabor_jet The Gabor jet to be normalized, including the phase values (which will not be altered).
 */
void bob::ip::normalizeGaborJet(blitz::Array<double,2>& gabor_jet){
  blitz::Array<double,1> abs_jet = gabor_jet(0, blitz::Range::all());
  double norm = sqrt(std::inner_product(abs_jet.begin(), abs_jet.end(), abs_jet.begin(), 0.));
  // normalize the absolute parts of the jets
  abs_jet /= norm;
}
