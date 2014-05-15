/**
 * @date Mon Apr 18 20:25:30 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file extract a rectangle of maximal area from a 2D mask of
 *    booleans.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_MAX_RECT_IN_MASK_H
#define BOB_IP_MAX_RECT_IN_MASK_H

#include <blitz/array.h>
#include "bob/core/assert.h"
#include "bob/core/array_index.h"

namespace bob {
  /**
    * \ingroup libip_api
    * @{
    *
    */
  namespace ip {

    namespace detail {
      /**
        * @brief Check if a rectangle only contains true values.
        * @param src The input blitz array
        * @param y0 The y-coordinate of the top left corner of the rectangle
        * @param x0 The x-coordinate of the top left corner of the rectangle
        * @param y1 The y-coordinate of the bottom right corner of the 
        *   rectangle
        * @param x1 The x-coordinate of the bottom right corner of the 
        *   rectangle
        */
      bool isTrue( const blitz::Array<bool,2>& src, int y0, int x0, 
        int y1, int x1);
    }

    /**
      * @brief Function which extracts a rectangle of maximal area from a 
      *   2D mask of booleans (i.e. a 2D blitz array).
      *   The first dimension is the height (y-axis), whereas the second one
      *   is the width (x-axis).
      * @warning The function assumes that the true values on the mask form
      *   a convex area.
      * @param src The 2D input blitz array mask.
      * @result A blitz::TinyVector which contains in the following order:
      *   0/ The y-coordinate of the top left corner
      *   1/ The x-coordinate of the top left corner
      *   2/ The height of the rectangle
      *   3/ The width of the rectangle
      */
    const blitz::TinyVector<int,4> 
    maxRectInMask( const blitz::Array<bool,2>& src);

  }
/**
 * @}
 */
}

#endif /* BOB_IP_MAX_RECT_IN_MASK_H */
