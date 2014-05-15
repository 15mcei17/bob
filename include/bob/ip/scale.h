/**
 * @date Mon Mar 14 11:21:29 2011 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file defines a function to rescale a 2D or 3D array/image.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_SCALE_H
#define BOB_IP_SCALE_H

#include <stdexcept>
#include <boost/format.hpp>

#include "bob/core/assert.h"
#include "bob/core/array_index.h"
#include "bob/core/cast.h"
#include "bob/ip/common.h"

namespace bob {
/**
 * \ingroup libip_api
 * @{
 *
 */
  namespace ip {

    namespace detail {
      /**
       * @brief Function which rescales a 2D blitz::array/image of a given
       *   type, using bilinear interpolation.
       *   The first dimension is the height (y-axis), whereas the second
       *   one is the width (x-axis).
       * @warning No check is performed on the dst blitz::array/image.
       * @param src The input blitz array
       * @param src_mask The input blitz boolean mask array
       * @param dst The output blitz array
       * @param dst_mask The output blitz boolean mask array
       */
      template<typename T, bool mask>
      void scaleNoCheck2D_BI(const blitz::Array<T,2>& src,
        const blitz::Array<bool,2>& src_mask, blitz::Array<double,2>& dst,
        blitz::Array<bool,2>& dst_mask)
      {
        const int height = dst.extent(0);
        const int width = dst.extent(1);

        const double x_ratio = (src.extent(1)-1.) / (width-1.);
        const double y_ratio = (src.extent(0)-1.) / (height-1.);
        for( int y=0; y<height; ++y) {
          double y_src = y_ratio * y;
          double dy2 = y_src - floor(y_src);
          double dy1 = 1. - dy2;
          int y_ind1 = bob::core::array::keepInRange( floor(y_src), 0, src.extent(0)-1);
          int y_ind2 = bob::core::array::keepInRange( y_ind1+1, 0, src.extent(0)-1);
          for( int x=0; x<width; ++x) {
            double x_src = x_ratio * x;
            double dx2 = x_src - floor(x_src);
            double dx1 = 1. - dx2;
            int x_ind1 = bob::core::array::keepInRange( floor(x_src), 0, src.extent(1)-1);
            int x_ind2 = bob::core::array::keepInRange( x_ind1+1, 0, src.extent(1)-1);
            double val = dx1*dy1*src(y_ind1, x_ind1)+dx1*dy2*src(y_ind2, x_ind1)
              + dx2*dy1*src(y_ind1, x_ind2 )+dx2*dy2*src(y_ind2, x_ind2 );
            dst(y,x) = val;
            if( mask) {
              bool all_in_mask = true;
              for( int ym=y_ind1; ym<=y_ind2; ++ym)
                for( int xm=x_ind1; xm<=x_ind2; ++xm)
                  all_in_mask = all_in_mask && src_mask(ym,xm);
              dst_mask(y,x) = all_in_mask;
            }
          }
        }
      }

    }

    namespace Rescale {
      typedef enum Algorithm_ {
        NearestNeighbour,
        BilinearInterp
      } Algorithm;
    }

    /**
     * @brief Function which rescales a 2D blitz::array/image of a given type.
     *   The first dimension is the height (y-axis), whereas the second
     *   one is the width (x-axis).
     * @param src The input blitz array
     * @param dst The output blitz array. The new array is resized according
     *   to the dimensions of this dst array.
     * @param alg The algorithm used for rescaling.
     */
    template<typename T>
    void scale(const blitz::Array<T,2>& src, blitz::Array<double,2>& dst,
      const Rescale::Algorithm alg=Rescale::BilinearInterp)
    {
      // Check and resize src if required
      bob::core::array::assertZeroBase(src);

      // Check and resize dst if required
      bob::core::array::assertZeroBase(dst);

      // Defines output height and width
      const int height = dst.extent(0);
      const int width = dst.extent(1);

      // Check parameters and throw exception if required
      if (height < 1) {
        boost::format m("parameter `height' was set to %d, but should be greater or equal 1");
        m % height;
        throw std::runtime_error(m.str());
      }
      if (width < 1) {
        boost::format m("parameter `width' was set to %d, but should be greater or equal 1");
        m % width;
        throw std::runtime_error(m.str());
      }

      // If src and dst have the same shape, do a simple copy
      if( height==src.extent(0) && width==src.extent(1)) {
        for( int y=0; y<src.extent(0); ++y)
          for( int x=0; x<src.extent(1); ++x)
            dst(y,x) = bob::core::cast<double>(src(y,x));
      }
      // Otherwise, do the rescaling
      else
      {
        // Rescale the 2D array
        switch(alg)
        {
          case Rescale::BilinearInterp:
            {
              // Rescale using Bilinear Interpolation
              blitz::Array<bool,2> src_mask, dst_mask;
              detail::scaleNoCheck2D_BI<T,false>(src, src_mask, dst, dst_mask);
            }
            break;
          default:
            throw std::runtime_error("the given scaling algorithm is not valid");
        }
      }
    }

    /**
     * @brief Function which rescales a 2D blitz::array/image of a given type.
     *   The first dimension is the height (y-axis), whereas the second
     *   one is the width (x-axis).
     * @param src The input blitz array
     * @param src_mask The input blitz boolean mask array
     * @param dst The output blitz array. The new array is resized according
     *   to the dimensions of this dst array.
     * @param dst_mask The output blitz boolean mask array
     * @param alg The algorithm used for rescaling.
     */
    template<typename T>
    void scale(const blitz::Array<T,2>& src, const blitz::Array<bool,2>& src_mask,
      blitz::Array<double,2>& dst, blitz::Array<bool,2>& dst_mask,
      const Rescale::Algorithm alg=Rescale::BilinearInterp)
    {
      // Check and resize src if required
      bob::core::array::assertZeroBase(src);
      bob::core::array::assertZeroBase(src_mask);
      bob::core::array::assertSameShape(src, src_mask);

      // Check and resize dst if required
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertZeroBase(dst_mask);
      bob::core::array::assertSameShape(dst, dst_mask);

      // Defines output height and width
      const int height = dst.extent(0);
      const int width = dst.extent(1);

      // Check parameters and throw exception if required
      if (height < 1) {
        boost::format m("parameter `height' was set to %d, but should be greater or equal 1");
        m % height;
        throw std::runtime_error(m.str());
      }
      if (width < 1) {
        boost::format m("parameter `width' was set to %d, but should be greater or equal 1");
        m % width;
        throw std::runtime_error(m.str());
      }

      // If src and dst have the same shape, do a simple copy
      if( height==src.extent(0) && width==src.extent(1)) {
        for( int y=0; y<src.extent(0); ++y)
          for( int x=0; x<src.extent(1); ++x)
            dst(y,x) = bob::core::cast<double>(src(y,x));
        detail::copyNoCheck(src_mask,dst_mask);
      }
      // Otherwise, do the rescaling
      else
      {
        // Rescale the 2D array
        switch(alg)
        {
          case Rescale::BilinearInterp:
            {
              // Rescale using Bilinear Interpolation
              detail::scaleNoCheck2D_BI<T,true>(src, src_mask, dst, dst_mask);
            }
            break;
          default:
            throw std::runtime_error("the given scaling algorithm is not valid");
        }
      }
    }

    /**
     * @brief Function which rescales a 3D blitz::array/image of a given type.
     *   The first dimension is the number of color plane, the second is the 
     * height (y-axis), whereas the third one is the width (x-axis).
     * @param src The input blitz array
     * @param dst The output blitz array. The new array is resized according
     *   to the dimensions of this dst array.
     * @param alg The algorithm used for rescaling.
     */
    template <typename T>
    void scale(const blitz::Array<T,3>& src, blitz::Array<double,3>& dst,
      const Rescale::Algorithm alg=Rescale::BilinearInterp)
    {
      // Check number of planes
      bob::core::array::assertSameDimensionLength(src.extent(0), dst.extent(0));

      for( int p=0; p<dst.extent(0); ++p)
        {
        const blitz::Array<T,2> src_slice =
          src( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<double,2> dst_slice =
          dst( p, blitz::Range::all(), blitz::Range::all() );

        // Process one plane
        scale(src_slice, dst_slice, alg);
      }
    }

    template <typename T>
    void scale(const blitz::Array<T,3>& src, const blitz::Array<bool,3>& src_mask,
      blitz::Array<double,3>& dst, blitz::Array<bool,3>& dst_mask,
      const Rescale::Algorithm alg=Rescale::BilinearInterp)
    {
      // Check number of planes
      bob::core::array::assertSameDimensionLength(src.extent(0), dst.extent(0));
      bob::core::array::assertSameDimensionLength(src.extent(0), src_mask.extent(0));
      bob::core::array::assertSameDimensionLength(src_mask.extent(0), dst_mask.extent(0));

      for( int p=0; p<dst.extent(0); ++p)
        {
        const blitz::Array<T,2> src_slice =
          src( p, blitz::Range::all(), blitz::Range::all() );
        const blitz::Array<bool,2> src_mask_slice =
          src_mask( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<double,2> dst_slice =
          dst( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<bool,2> dst_mask_slice =
          dst_mask( p, blitz::Range::all(), blitz::Range::all() );

        // Process one plane
        scale(src_slice, src_mask_slice, dst_slice, dst_mask_slice, alg);
      }
    }

    /**
     * @brief Function which returns the shape of an output 2D blitz::array/
     *   image when rescaling an input image with the given scale factor.
     *   The first dimension is the height (y-axis), whereas the second one 
     *   is the width (x-axis).
     * @param src The 2D input blitz array
     * @param scale_factor The scaling factor to apply
     * @return A blitz::TinyVector containing the shape of the rescaled image
     */
    template <typename T>
    blitz::TinyVector<int,2>
    getScaledShape(const blitz::Array<T,2>& src, const double scale_factor)
    {
      blitz::TinyVector<int,2> dst_shape =
        blitz::floor(src.shape() * scale_factor + 0.5);
      return dst_shape;
    }

    /**
     * @brief Function which returns the shape of an output 3D blitz::array/
     *   image when rescaling an input image with the given scale factor.
     *   The first dimension is the number of color planes, the second one is
     *   the height (y-axis), whereas the third one is the width (x-axis).
     * @param src The 3D input blitz array
     * @param scale_factor The scaling factor to apply
     * @return A blitz::TinyVector containing the shape of the rescaled image
     */
    template <typename T>
    blitz::TinyVector<int,3>
    getScaledShape(const blitz::Array<T,3>& src, const double scale_factor)
    {
      blitz::TinyVector<int,3> dst_shape = src.shape();
      dst_shape(1) = floor(dst_shape(1) * scale_factor + 0.5);
      dst_shape(2) = floor(dst_shape(2) * scale_factor + 0.5);
      return dst_shape;
    }
  }

  /**
   * @}
   */
}

#endif /* BOB_IP_SCALE_H */
