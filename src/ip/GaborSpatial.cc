/**
 * @date Wed Apr 13 20:12:03 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file provides a class to perform Gabor filtering
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#include <bob/ip/GaborSpatial.h>
#include <bob/core/assert.h>

bob::ip::GaborSpatial::GaborSpatial( const double f, const double theta, 
  const double gamma, const double eta, const int spatial_size, 
  const bool cancel_dc, 
  const enum bob::ip::Gabor::NormOption norm_opt,
//  const enum bob::sp::Convolution::SizeOption size_opt,
  const enum bob::sp::Extrapolation::BorderType border_type):
  m_f(f), m_theta(theta), m_gamma(gamma), m_eta(eta), 
  m_spatial_size(spatial_size), m_cancel_dc(cancel_dc),
  m_norm_opt(norm_opt), // m_size_opt(size_opt), 
  m_border_type(border_type), m_tmp(0,0)
{
  computeFilter();
}

bob::ip::GaborSpatial::~GaborSpatial() { }

void bob::ip::GaborSpatial::operator()( 
  const blitz::Array<std::complex<double>,2>& src,
  blitz::Array<std::complex<double>,2>& dst)
{ 
  // Check input
  bob::core::array::assertZeroBase(src);

  // Check output
  bob::core::array::assertZeroBase(dst);
  // TODO: size if different Conv::SizeOption
  bob::core::array::assertSameShape(dst,src);

  // Convolution with the Gabor Filter
  if(m_border_type == bob::sp::Extrapolation::Zero)
    bob::sp::conv( src, m_kernel, dst, bob::sp::Conv::Same); // m_size_opt
  else
  {
    int h_tmp = src.extent(0)+m_spatial_size-1;
    int w_tmp = src.extent(1)+m_spatial_size-1;
    m_tmp.resize(h_tmp,w_tmp);
    if(m_border_type == bob::sp::Extrapolation::NearestNeighbour)
      bob::sp::extrapolateNearest(src, m_tmp);
    else if(m_border_type == bob::sp::Extrapolation::Circular)
      bob::sp::extrapolateCircular(src, m_tmp);
    else if(m_border_type == bob::sp::Extrapolation::Mirror)
      bob::sp::extrapolateMirror(src, m_tmp);
    bob::sp::conv( m_tmp, m_kernel, dst, bob::sp::Conv::Valid); 
  }
}

void bob::ip::GaborSpatial::computeFilter()
{
  // Compute some constant values used later
  const double pi2 = M_PI*M_PI;
  const double cos_theta = cos(m_theta);
  const double sin_theta = sin(m_theta);
  const double gamma2 = m_gamma * m_gamma;
  const double f2 = m_f*m_f;
  const double f2_gamma2 = f2 / gamma2;
  const double f2_eta2 = f2 / ( m_eta * m_eta );
  const double exp_m_pi2_gamma2 = exp(-pi2 * gamma2);
  const double two_pi_f = 2 * M_PI * m_f;
  const std::complex<double> J(0.,1.);
  const int size_by_2 = m_spatial_size / 2;

  // Resize the spatial filter
  if( m_kernel.extent(0) != m_spatial_size || 
    m_kernel.extent(1) != m_spatial_size)
      m_kernel.resize( m_spatial_size, m_spatial_size );

  // Compute the kernel filter
  // G(y,x) = normalization * exp( -f**2/gamma**2*x'**2 - f**2/eta**2*y'**2) * 
  //   ( exp(J*2*PI*f*x') - (m_cancel_dc ? exp(-PI**2*gamma**2) : 0.)
  // where x' = x*cos(theta) + y*sin(theta) and y' = -x*sin(theta) + y*cos(theta)
  blitz::firstIndex y;
  blitz::secondIndex x;
  m_kernel = exp( -f2_gamma2 * 
      ((x-size_by_2)*cos_theta + (y-size_by_2)*sin_theta) * 
        ((x-size_by_2)*cos_theta + (y-size_by_2)*sin_theta) 
       -f2_eta2 * 
      (-(x-size_by_2)*sin_theta + (y-size_by_2)*cos_theta) * 
      (-(x-size_by_2)*sin_theta + (y-size_by_2)*cos_theta) ) * 
    (exp( J*two_pi_f*((x-size_by_2)*cos_theta + (y-size_by_2)*sin_theta) ) - 
      (m_cancel_dc?exp_m_pi2_gamma2:0.));

  // Normalize the filter if required
  if(m_norm_opt==bob::ip::Gabor::SpatialFactor) {
    const double norm_factor = f2 / ( M_PI * m_gamma * m_eta );
    m_kernel *= std::complex<double>(norm_factor,0.);
  }
  else if(m_norm_opt==bob::ip::Gabor::ZeroMeanUnitVar) {
    int n_el = m_spatial_size*m_spatial_size;
    // Zero mean
    m_kernel -= ( sum(m_kernel) / std::complex<double>(n_el,0.) );
    // Unit variance
    m_kernel /= ( sum(norm(m_kernel)) / std::complex<double>(n_el,0.) ); 
  }
}

