/**
 * @date Fri Apr 29 12:13:22 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file provides a class to process images with the Sobel operator
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_SOBEL_H
#define BOB_IP_SOBEL_H

#include <stdexcept>
#include <boost/format.hpp>

#include "bob/core/assert.h"
#include "bob/core/cast.h"
#include "bob/sp/conv.h"
#include "bob/sp/extrapolate.h"

namespace bob {
/**
 * \ingroup libip_api
 * @{
 *
 */
  namespace ip {

  /**
   * @brief This class can be used to process images with the Sobel operator
  */
  class Sobel
  {
    public:

      /**
        * @brief Constructor: generates the Sobel kernel
        */
      Sobel(const bool up_positive=false, const bool left_positive=false,
        const bob::sp::Conv::SizeOption size_opt=bob::sp::Conv::Same,
        const bob::sp::Extrapolation::BorderType
          border_type=bob::sp::Extrapolation::Mirror);

      /**
        * @brief Copy constructor
        */
      Sobel(const Sobel& other);

      /**
        * @brief Destructor
        */
      virtual ~Sobel() {}

      /**
       * @brief Assignment operator
       */
      Sobel& operator=(const Sobel& other);

      /**
       * @brief Equal to
       */
      bool operator==(const Sobel& b) const;
      /**
       * @brief Not equal to
       */
      bool operator!=(const Sobel& b) const;

      /**
       * @brief Getters
       */
      bool getUpPositive() const { return m_up_positive; }
      bool getLeftPositive() const { return m_left_positive; }
      bob::sp::Conv::SizeOption getSizeOption() const { return m_size_opt; }
      bob::sp::Extrapolation::BorderType getConvBorder() const
      { return m_border_type; }
      const blitz::Array<double, 2>& getKernelY() const { return m_kernel_y; }
      const blitz::Array<double, 2>& getKernelX() const { return m_kernel_x; }

      /**
       * @brief Setters
       */
      void setUpPositive(const bool up_positive)
      { m_up_positive = up_positive; computeKernels(); }
      void setLeftPositive(const bool left_positive)
      { m_left_positive = left_positive; computeKernels(); }
      void setSizeOption(const bob::sp::Conv::SizeOption size_opt)
      { m_size_opt = size_opt; }
      void setConvBorder(const bob::sp::Extrapolation::BorderType border_type)
      { m_border_type = border_type; }

      /**
        * @brief Process a 2D blitz Array/Image by applying the Sobel operator
        *   The resulting 3D array will contain two planes:
        *     - The first one for the convolution with the y-kernel
        *     - The second one for the convolution with the x-kernel
        * @warning The selected type should be signed (e.g. int64_t or double)
        */
      template <typename T> void operator()(const blitz::Array<T,2>& src,
        blitz::Array<T,3>& dst);

    private:
      /**
        * @brief Generates the Sobel kernels
        */
      void computeKernels();

      // Attributes
      blitz::Array<double, 2> m_kernel_y;
      blitz::Array<double, 2> m_kernel_x;
      bool m_up_positive;
      bool m_left_positive;
      bob::sp::Conv::SizeOption m_size_opt;
      bob::sp::Extrapolation::BorderType m_border_type;
  };

  template <typename T>
  void Sobel::operator()(const blitz::Array<T,2>& src, blitz::Array<T,3>& dst)
  {
    // Check that dst has two planes
    if( dst.extent(0) != 2 ) {
      boost::format m("destination array extent for the first dimension (0) is not 2, but %d");
      m % dst.extent(0);
      throw std::runtime_error(m.str());
    }
    // Check that dst has zero bases
    bob::core::array::assertZeroBase(dst);

    // Define slices for y and x
    blitz::Array<T,2> dst_y = dst(0, blitz::Range::all(), blitz::Range::all());
    blitz::Array<T,2> dst_x = dst(1, blitz::Range::all(), blitz::Range::all());
    // TODO: improve the way we deal with types
    if(m_border_type == bob::sp::Extrapolation::Zero || m_size_opt == bob::sp::Conv::Valid)
    {
     bob::sp::conv(src, bob::core::array::cast<T>(m_kernel_y), dst_y, m_size_opt);
     bob::sp::conv(src, bob::core::array::cast<T>(m_kernel_x), dst_x, m_size_opt);
    }
    else
    {
      blitz::Array<T,2> tmpy(bob::sp::getConvOutputSize(src, bob::core::array::cast<T>(m_kernel_y), bob::sp::Conv::Full));
      blitz::Array<T,2> tmpx(bob::sp::getConvOutputSize(src, bob::core::array::cast<T>(m_kernel_x), bob::sp::Conv::Full));
      if(m_border_type == bob::sp::Extrapolation::NearestNeighbour) {
        bob::sp::extrapolateNearest(src, tmpy);
        bob::sp::extrapolateNearest(src, tmpx);
      }
      else if(m_border_type == bob::sp::Extrapolation::Circular) {
        bob::sp::extrapolateCircular(src, tmpy);
        bob::sp::extrapolateCircular(src, tmpx);
      }
      else if(m_border_type == bob::sp::Extrapolation::Mirror) {
        bob::sp::extrapolateMirror(src, tmpy);
        bob::sp::extrapolateMirror(src, tmpx);
      }
      bob::sp::conv(tmpy, bob::core::array::cast<T>(m_kernel_y), dst_y, bob::sp::Conv::Valid);
      bob::sp::conv(tmpx, bob::core::array::cast<T>(m_kernel_x), dst_x, bob::sp::Conv::Valid);
    }
  }

}}

#endif /* BOB_SOBEL_H */
