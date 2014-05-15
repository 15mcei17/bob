/**
 * @date Thu Mar 17 18:46:09 2011 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file defines a function to perform power-law gamma correction
 *   on a 2D array/image.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_GAMMA_CORRECTION_H
#define BOB_IP_GAMMA_CORRECTION_H

#include <cmath>
#include <limits>
#include <stdexcept>
#include <boost/format.hpp>
#include <blitz/array.h>

#include "bob/core/assert.h"

namespace bob {
/**
 * \ingroup libip_api
 * @{
 *
 */
  namespace ip {

    namespace detail {
      /**
        * @brief Function which performs a gamma correction on a 2D
        *   blitz::array/image of a given type.
        *   The first dimension is the height (y-axis), whereas the second
        *   one is the width (x-axis).
        * @warning No check is performed on the dst blitz::array/image.
        * @param src The input blitz array
        * @param dst The output blitz array
        * @param gamma The gamma value for power-law gamma correction
        */
      template<typename T>
      void gammaCorrectionNoCheck(const blitz::Array<T,2>& src,
        blitz::Array<double,2>& dst, const double gamma)
      {
        dst = blitz::pow( src, gamma);
      }

    }


    /**
      * @brief Function which performs a gamma correction on a 2D
      *   blitz::array/image of a given type.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @param src The input blitz array
      * @param dst The output blitz array (always double)
      * @param gamma The gamma value for power-law gamma correction
      */
    template<typename T>
    void gammaCorrection(const blitz::Array<T,2>& src,
      blitz::Array<double,2>& dst, const double gamma)
    {
      // Check input/output
      bob::core::array::assertZeroBase(src);
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertSameShape(dst, src);

      // Check parameters and throw exception if required
      if( gamma < 0.) {
        boost::format m("parameter `gamma' was set to %f, but should be greater or equal zero");
        m % gamma;
        throw std::runtime_error(m.str());
      }

      // Perform gamma correction for the 2D array
      detail::gammaCorrectionNoCheck(src, dst, gamma);
    }

  }
/**
 * @}
 */
}

#endif /* BOB_IP_GAMMA_CORRECTION_H */
