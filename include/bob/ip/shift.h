/**
 * @date Mon Mar 7 20:06:35 2011 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file defines a function to shift a 2D or 3D array/image.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_SHIFT_H
#define BOB_IP_SHIFT_H

#include "bob/core/array_index.h"
#include "bob/core/assert.h"
#include "bob/ip/crop.h"

namespace bob {
/**
 * \ingroup libip_api
 * @{
 *
 */
  namespace ip {

    namespace detail {
      /**
        * @brief Checks the given shifting parameters wrt. given input 
        *   dimensions, and throws an exception if the shifted area and the
        *   input source array/iamge have no common points.
        * @param shift_y The y-offset of the top left corner of the shifted
        *   area wrt. to the y-index of the top left corner of the 
        *   blitz::array.
        * @param shift_x The x-offset of the top left corner of the shifted 
        *   area wrt. to the x-index of the top left corner of the 
        *   blitz::array.
        * @param src_height The height of the input image
        * @param src_width The width of the input image
        */
      void shiftParameterCheck( const int shift_y, const int shift_x,
        const size_t src_height, const size_t src_width);
    }


    /**
      * @brief Function which shifts a 2D blitz::array/image of a given type.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @param src The input blitz array
      * @param dst The output blitz array
      * @param shift_y The y-offset of the top left corner of the shifted 
      *   area wrt. to the y-index of the top left corner of the 
      *   blitz::array.
      * @param shift_x The x-offset of the top left corner of the shifted 
      *   area wrt. to the x-index of the top left corner of the 
      *   blitz::array.
      * @param allow_out Whether an exception should be raised or not if the 
      *   shifted blitz::array has no pixel in common with the input 
      *   blitz::array.
      * @param zero_out Whether the shifted which is out of the boundary
      *   of the input blitz array should be filled with zero values, or 
      *   with the intensity of the closest pixel in the neighbourhood.
      */
    template<typename T>
    void shift(const blitz::Array<T,2>& src, blitz::Array<T,2>& dst, 
      const int shift_y, const int shift_x, 
      const bool allow_out = false, const bool zero_out = false)
    {
      // Check parameters and throw exception if required
      if( !allow_out )
        detail::shiftParameterCheck( shift_y, shift_x, src.extent(0), 
          src.extent(1) );
      // Check input
      bob::core::array::assertZeroBase(src);
      // Check output
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertSameShape(dst, src);
      
      // Shift the 2D array
      blitz::Array<bool,2> src_mask, dst_mask; 
      detail::cropNoCheck<T,false>(src, src_mask, dst, dst_mask, shift_y,
        shift_x, src.extent(0), src.extent(1), zero_out);
    }


    /**
      * @brief Function which shifts a 3D blitz::array/image of a given type.
      *   The first dimension is the number of planes, the second one the 
      *   height (y-axis), whereas the third one is the width (x-axis).
      * @param src The input blitz array
      * @param dst The output blitz array
      * @param shift_y The y-offset of the top left corner of the shifted
      *   area wrt. to the y-index of the top left corner of the 
      *   blitz::array.
      * @param shift_x The x-offset of the top left corner of the shifted
      *   area wrt. to the x-index of the top left corner of the 
      *   blitz::array.
      * @param allow_out Whether an exception should be raised or not if the 
      *   shifted blitz::array has no pixel in common with the input 
      *   blitz::array.
      * @param zero_out Whether the shifted which is out of the boundary
      *   of the input blitz array should be filled with zero values, or 
      *   with the intensity of the closest pixel in the neighbourhood.
      */
    template<typename T>
    void shift(const blitz::Array<T,3>& src, blitz::Array<T,3>& dst, 
      const int shift_y, const int shift_x, const bool allow_out = false,
      const bool zero_out = false)
    {
      // Check parameters and throw exception if required
      if( !allow_out )
        detail::shiftParameterCheck( shift_y, shift_x, src.extent(1), 
          src.extent(2) );
      // Check input
      bob::core::array::assertZeroBase(src);
      // Check output
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertSameShape(dst, src);
    
      for( int p=0; p<dst.extent(0); ++p) {
        // Prepare reference array to 2D slices
        const blitz::Array<T,2> src_slice = 
          src( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<T,2> dst_slice = 
          dst( p, blitz::Range::all(), blitz::Range::all() );
        // Shift the 2D array
        blitz::Array<bool,2> src_mask, dst_mask; 
        detail::cropNoCheck<T,false>(src_slice, src_mask, dst_slice, 
          dst_mask, shift_y, shift_x, src.extent(1), src.extent(2), 
          zero_out);
      }
    }

    /**
      * @brief Function which shifts a 2D blitz::array/image of a given type,
      *   taking into consideration masks. Masks are used to specify which
      *   pixels are 'valid' in the input and output arrays/images.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @param src The input blitz array
      * @param src_mask The input blitz array mask, specifying the valid
      *   pixels of src.
      * @param dst The output blitz array
      * @param dst_mask The output blitz array mask, specifying the valid
      *   pixels of dst.
      * @param shift_y The y-offset of the top left corner of the shifted
      *   area wrt. to the y-index of the top left corner of the 
      *   blitz::array.
      * @param shift_x The x-offset of the top left corner of the shifted 
      *   area wrt. to the x-index of the top left corner of the 
      *   blitz::array.
      * @param allow_out Whether an exception should be raised or not if 
      *   the shifted blitz::array has no pixel in common with the input 
      *   blitz::array.
      * @param zero_out Whether the shifted which is out of the boundary
      *   of the input blitz array should be filled with zero values, or 
      *   with the intensity of the closest pixel in the neighbourhood.
      */
    template<typename T>
    void shift(const blitz::Array<T,2>& src, 
      const blitz::Array<bool,2>& src_mask, blitz::Array<T,2>& dst, 
      blitz::Array<bool,2>& dst_mask, const int shift_y, const int shift_x, 
      const bool allow_out = false, const bool zero_out = false)
    {
      // Check parameters and throw exception if required
      if( !allow_out )
        detail::shiftParameterCheck( shift_y, shift_x, src.extent(0), 
          src.extent(1) );
      // Check input
      bob::core::array::assertZeroBase(src);
      bob::core::array::assertZeroBase(src_mask);
      bob::core::array::assertSameShape(src, src_mask);
      // Check output
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertZeroBase(dst_mask);
      bob::core::array::assertSameShape(dst, dst_mask);
      bob::core::array::assertSameShape(dst, src);
      
      // Shift the 2D array
      detail::cropNoCheck<T,true>(src, src_mask, dst, dst_mask, shift_y,
        shift_x, src.extent(0), src.extent(1), zero_out);
    }

    /**
      * @brief Function which shifts a 2D blitz::array/image of a given type,
      *   taking into consideration masks. Masks are used to specify which
      *   pixels are 'valid' in the input and output arrays/images.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @param src The input blitz array
      * @param src_mask The input blitz array mask, specifying the valid
      *   pixels of src.
      * @param dst The output blitz array
      * @param dst_mask The output blitz array mask, specifying the valid
      *   pixels of dst.
      * @param shift_y The y-offset of the top left corner of the shifted
      *   area wrt. to the y-index of the top left corner of the 
      *   blitz::array.
      * @param shift_x The x-offset of the top left corner of the shifted 
      *   area wrt. to the x-index of the top left corner of the 
      *   blitz::array.
      * @param allow_out Whether an exception should be raised or not if 
      *   the shifted blitz::array has no pixel in common with the input 
      *   blitz::array.
      * @param zero_out Whether the shifted which is out of the boundary
      *   of the input blitz array should be filled with zero values, or 
      *   with the intensity of the closest pixel in the neighbourhood. 
      */
    template<typename T>
    void shift(const blitz::Array<T,3>& src, const blitz::Array<bool,3>& src_mask,
      blitz::Array<T,3>& dst, blitz::Array<bool,3>& dst_mask,
      const int shift_y, const int shift_x, const bool allow_out = false,
      const bool zero_out = false)
    {
      // Check parameters and throw exception if required
      if( !allow_out )
        detail::shiftParameterCheck( shift_y, shift_x, src.extent(1), 
          src.extent(2) );
      // Check input
      bob::core::array::assertZeroBase(src);
      bob::core::array::assertZeroBase(src_mask);
      bob::core::array::assertSameShape(src, src_mask);
      // Check output
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertZeroBase(dst_mask);
      bob::core::array::assertSameShape(dst, dst_mask);
      bob::core::array::assertSameShape(dst, src);
      
      for( int p=0; p<dst.extent(0); ++p) {
        // Prepare reference array to 2D slices
        const blitz::Array<T,2> src_slice = 
          src( p, blitz::Range::all(), blitz::Range::all() );
        const blitz::Array<bool,2> src_mask_slice =
          src_mask( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<T,2> dst_slice = 
          dst( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<bool,2> dst_mask_slice = 
          dst_mask( p, blitz::Range::all(), blitz::Range::all() );
        // Shift the 2D array
        detail::cropNoCheck<T,true>(src_slice, src_mask_slice, dst_slice, 
          dst_mask_slice, shift_y, shift_x, src.extent(1), src.extent(2),
          zero_out);
      }
    }

  }
/**
 * @}
 */
}

#endif /* BOB_IP_SHIFT_H */
