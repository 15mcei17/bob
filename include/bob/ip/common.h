/**
 * @date Mon Mar 14 12:10:04 2011 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file defines common functions for processing 2D/3D array image.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_COMMON_H
#define BOB_IP_COMMON_H

#include <blitz/array.h>

namespace bob {
/**
 * \ingroup libip_api
 * @{
 *
 */
  namespace ip {

    namespace detail {
      /**
        * @brief Function which copies a 2D blitz::array/image of a given type.
        * @warning No check is performed on the dst blitz::array/image.
        * @param src The input blitz array
        * @param dst The output blitz array
        */
      template<typename T>
      void copyNoCheck(const blitz::Array<T,2>& src,
        blitz::Array<T,2>& dst)
      {
        blitz::Range  src_y( src.lbound(0), src.ubound(0) ),
                      src_x( src.lbound(1), src.ubound(1) ),
                      dst_y( dst.lbound(0), dst.ubound(0) ),
                      dst_x( dst.lbound(1), dst.ubound(1) );
        dst(dst_y,dst_x) = src(src_y,src_x);
      }

      /**
        * @brief Function which copies a 3D blitz::array/image of a given type.
        * @warning No check is performed on the dst blitz::array/image.
        * @param src The input blitz array
        * @param dst The output blitz array
        */
      template<typename T>
      void copyNoCheck(const blitz::Array<T,3>& src,
        blitz::Array<T,3>& dst)
      {
        blitz::Range  src_p( src.lbound(0), src.ubound(0) ),
                      src_y( src.lbound(1), src.ubound(1) ),
                      src_x( src.lbound(2), src.ubound(2) ),
                      dst_p( dst.lbound(0), dst.ubound(0) ),
                      dst_y( dst.lbound(1), dst.ubound(1) ),
                      dst_x( dst.lbound(2), dst.ubound(2) );
        dst(dst_p,dst_y,dst_x) = src(src_p,src_y,src_x);
      }
    }

  }
/**
 * @}
 */
}

#endif /* BOB_IP_COMMON_H */

