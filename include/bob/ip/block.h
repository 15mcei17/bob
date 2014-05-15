/**
 * @date Tue Apr 5 12:38:15 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file defines a function to perform a decomposition by block.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_BLOCK_H
#define BOB_IP_BLOCK_H

#include "bob/core/assert.h"
#include "bob/ip/crop.h"

namespace bob {
/**
 * \ingroup libip_api
 * @{
 */
  namespace ip {

    namespace detail {
      /**
        * @brief Function which performs a block decomposition of a 2D
        *   blitz::array/image of a given type
        */
      template<typename T, typename U>
      void blockReferenceNoCheck(const blitz::Array<T,2>& src, U& dst,
        const size_t block_h, const size_t block_w, const size_t overlap_h,
        const size_t overlap_w)
      {
        // Determine the number of block per row and column
        const int size_ov_h = block_h - overlap_h;
        const int size_ov_w = block_w - overlap_w;
        const int n_blocks_h = (src.extent(0) - (int)overlap_h) / size_ov_h;
        const int n_blocks_w = (src.extent(1) - (int)overlap_w) / size_ov_w;

        // Perform the block decomposition
        blitz::Array<T,2> current_block;
        for( int h=0; h<n_blocks_h; ++h)
          for( int w=0; w<n_blocks_w; ++w) {
            detail::cropNoCheckReference( src, current_block, h*size_ov_h,
              w*size_ov_w, block_h, block_w);
            dst.push_back( current_block );
          }
      }

      /**
        * @brief Function which performs a block decomposition of a 2D
        *   blitz::array/image of a given type, and put the results in a
        *   3D blitz::array/image
        */
      template<typename T>
      void blockNoCheck(const blitz::Array<T,2>& src, blitz::Array<T,3>& dst,
        const int block_h, const int block_w, const int overlap_h,
        const int overlap_w)
      {
        // Determine the number of block per row and column
        const int size_ov_h = block_h - overlap_h;
        const int size_ov_w = block_w - overlap_w;
        const int n_blocks_h = (src.extent(0) - (int)overlap_h) / size_ov_h;
        const int n_blocks_w = (src.extent(1) - (int)overlap_w) / size_ov_w;

        // Perform the block decomposition
        blitz::Array<bool,2> src_mask, dst_mask;
        for( int h=0; h<n_blocks_h; ++h)
          for( int w=0; w<n_blocks_w; ++w) {
            blitz::Array<T,2> current_block =
              dst( h*n_blocks_w+w, blitz::Range::all(), blitz::Range::all());
            cropNoCheck<T,false>( src, src_mask, current_block, dst_mask,
              h*size_ov_h, w*size_ov_w, block_h, block_w, true);
          }
      }


      /**
        * @brief Function which performs a block decomposition of a 2D
        *   blitz::array/image of a given type
        */
      template<typename T>
      void blockNoCheck(const blitz::Array<T,2>& src, blitz::Array<T,4>& dst,
        const size_t block_h, const size_t block_w, const size_t overlap_h,
        const size_t overlap_w)
      {
        // Determine the number of block per row and column
        const int size_ov_h = block_h - overlap_h;
        const int size_ov_w = block_w - overlap_w;
        const int n_blocks_h = (src.extent(0)-(int)overlap_h) / size_ov_h;
        const int n_blocks_w = (src.extent(1)-(int)overlap_w) / size_ov_w;

        // Perform the block decomposition
        blitz::Array<bool,2> src_mask, dst_mask;
        for( int h=0; h<n_blocks_h; ++h)
          for( int w=0; w<n_blocks_w; ++w) {
            blitz::Array<T,2> current_block =
              dst( h, w, blitz::Range::all(), blitz::Range::all());
            cropNoCheck<T,false>( src, src_mask, current_block, dst_mask,
              h*size_ov_h, w*size_ov_w, block_h, block_w, true);
          }
      }

      /**
        * @brief Function which checks the given parameters for a block
        *   decomposition of a 2D blitz::array/image.
        */
      void blockCheckInput(const size_t height, const size_t width,
        const size_t block_h, const size_t block_w, const size_t overlap_h,
        const size_t overlap_w);

      /**
        * @brief Function which checks the given parameters for a block
        *   decomposition of a 2D blitz::array/image.
        */
      template<typename T>
      void blockCheckInput(const blitz::Array<T,2>& src, const size_t block_h,
        const size_t block_w, const size_t overlap_h, const size_t overlap_w)
      {
        // Checks that the src array has zero base indices
        bob::core::array::assertZeroBase( src);

        // Checks the parameters
        blockCheckInput(src.extent(0), src.extent(1), block_h, block_w,
          overlap_h, overlap_w);
      }

    }

    /**
      * @brief Function which performs a decomposition by block of a 2D
      *   blitz::array/image of a given type.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @warning The returned blocks will refer to the same data as the in
      *   input 2D blitz array.
      * @param src The input blitz array
      * @param dst The STL container of 2D block blitz arrays. The STL
      *   container requires to support the push_back method, such as
      *   a STL vector or list.
      * @param block_w The desired width of the blocks.
      * @param block_h The desired height of the blocks.
      * @param overlap_w The overlap between each block along the x axis.
      * @param overlap_h The overlap between each block along the y axis.
      */
    template<typename T, typename U>
    void blockReference(const blitz::Array<T,2>& src, U& dst,
      const size_t block_h, const size_t block_w, const size_t overlap_h,
      const size_t overlap_w)
    {
      // Check input
      detail::blockCheckInput( src, block_h, block_w, overlap_h, overlap_w);

      // Crop the 2D array
      detail::blockReferenceNoCheck(src, dst, block_h, block_w, overlap_h,
        overlap_w);
    }

    /**
      * @brief Function which returns the expected shape of the output
      *   3D blitz array when applying a decomposition by block of a
      *   2D blitz::array/image of a given size.
      *   Dimensions are returned in a 3D TinyVector as
      *   (N_blocks,block_h,block_w)
      * @param height  The height of the input array
      * @param width   The width of the input array
      * @param block_h The desired height of the blocks.
      * @param block_w The desired width of the blocks.
      * @param overlap_h The overlap between each block along the y axis.
      * @param overlap_w The overlap between each block along the x axis.
      */
    const blitz::TinyVector<int,3>
    getBlock3DOutputShape(const size_t height, const size_t width,
      const size_t block_h, const size_t block_w,
      const size_t overlap_h, const size_t overlap_w);

    /**
      * @brief Function which returns the expected shape of the output
      *   3D blitz array when applying a decomposition by block of a
      *   2D blitz::array/image using the block() function.
      *   Dimensions are returned in a 3D TinyVector as
      *   (N_blocks,block_h,block_w)
      * @param src The input blitz array
      * @param block_h The desired height of the blocks.
      * @param block_w The desired width of the blocks.
      * @param overlap_h The overlap between each block along the y axis.
      * @param overlap_w The overlap between each block along the x axis.
      */
    template<typename T>
    const blitz::TinyVector<int,3>
    getBlock3DOutputShape(const blitz::Array<T,2>& src, const size_t block_h,
      const size_t block_w, const size_t overlap_h, const size_t overlap_w)
    {
      // Check input
      detail::blockCheckInput( src, block_h, block_w, overlap_h, overlap_w);

      // Check parameters and returns result
      return getBlock3DOutputShape(src.extent(0), src.extent(1),
        block_h, block_w, overlap_h, overlap_w);
    }

    /**
      * @brief Function which returns the expected shape of the output
      *   4D blitz array when applying a decomposition by block of a
      *   2D blitz::array/image of a given size.
      *   Dimensions are returned in a 4D TinyVector as
      *   (N_blocks_y,N_blocks_x,block_h,block_w)
      * @param height  The height of the input array
      * @param width   The width of the input array
      * @param block_h The desired height of the blocks.
      * @param block_w The desired width of the blocks.
      * @param overlap_h The overlap between each block along the y axis.
      * @param overlap_w The overlap between each block along the x axis.
      */
    const blitz::TinyVector<int,4>
    getBlock4DOutputShape(const size_t height, const size_t width,
      const size_t block_h, const size_t block_w, const size_t overlap_h,
      const size_t overlap_w);

    /**
      * @brief Function which returns the expected shape of the output
      *   4D blitz array when applying a decomposition by block of a

 *   2D blitz::array/image using the block() function.
      *   Dimensions are returned in a 4D TinyVector as
      *   (N_blocks_y,N_blocks_x,block_h,block_w)
      * @param src The input blitz array
      * @param block_h The desired height of the blocks.
      * @param block_w The desired width of the blocks.
      * @param overlap_h The overlap between each block along the y axis.
      * @param overlap_w The overlap between each block along the x axis.
      */
    template<typename T>
    const blitz::TinyVector<int,4> getBlock4DOutputShape(
      const blitz::Array<T,2>& src, const size_t block_h,
      const size_t block_w, const size_t overlap_h, const size_t overlap_w)
    {
      // Check input
      detail::blockCheckInput( src, block_h, block_w, overlap_h, overlap_w);

      // Check parameters and returns result
      return getBlock4DOutputShape(src.extent(0), src.extent(1),
        block_h, block_w, overlap_h, overlap_w);
    }

    /**
      * @brief Function which performs a decomposition by block of a 2D
      *   blitz::array/image of a given type.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @param src The input blitz array
      * @param dst The output 3D blitz arrays. The first coordinate is for the
      *   block index, and the second two are coordinates inside the blocks.
      * @param block_h The desired height of the blocks.
      * @param block_w The desired width of the blocks.
      * @param overlap_h The overlap between each block along the y axis.
      * @param overlap_w The overlap between each block along the x axis.
      */
    template<typename T>
    void block(const blitz::Array<T,2>& src, blitz::Array<T,3>& dst,
      const size_t block_h, const size_t block_w, const size_t overlap_h,
      const size_t overlap_w)
    {
      // Check input
      detail::blockCheckInput( src, block_h, block_w, overlap_h, overlap_w);
      blitz::TinyVector<int,3> shape =
        getBlock3DOutputShape(src, block_h, block_w, overlap_h, overlap_w);
      bob::core::array::assertSameShape( dst, shape);

      // Crop the 2D array
      detail::blockNoCheck(src, dst, block_h, block_w, overlap_h, overlap_w);
    }

    /**
      * @brief Function which performs a decomposition by block of a 2D
      *   blitz::array/image of a given type.
      *   The first dimension is the height (y-axis), whereas the second
      *   one is the width (x-axis).
      * @param src The input blitz array
      * @param dst The output 4D blitz arrays. The first coordinates are for
      *   y- and x-block indices, and the last two are coordinates inside the
      *   blocks.
      * @param block_h The desired height of the blocks.
      * @param block_w The desired width of the blocks.
      * @param overlap_h The overlap between each block along the y axis.
      * @param overlap_w The overlap between each block along the x axis.
      */
    template<typename T>
    void block(const blitz::Array<T,2>& src, blitz::Array<T,4>& dst,
      const size_t block_h, const size_t block_w, const size_t overlap_h,
      const size_t overlap_w)
    {
      // Check input
      detail::blockCheckInput( src, block_h, block_w, overlap_h, overlap_w);
      blitz::TinyVector<int,4> shape =
        getBlock4DOutputShape(src, block_h, block_w, overlap_h, overlap_w);
      bob::core::array::assertSameShape( dst, shape);

      // Crop the 2D array
      detail::blockNoCheck(src, dst, block_h, block_w, overlap_h, overlap_w);
    }

  }

/**
 * @}
 */
}

#endif /* BOB_IP_BLOCK_H */
