/**
 * @date Sat Apr 30 17:52:15 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file provides a class to smooth an image with a Gaussian kernel
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_GAUSSIAN_H
#define BOB_IP_GAUSSIAN_H

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
      * @brief This class allows to smooth images with a Gaussian kernel
      */
    class Gaussian
    {
      public:
        /**
         * @brief Creates an object to smooth images with a Gaussian kernel
         * @param radius_y The height of the kernel along the y-axis
         * @param radius_x The width of the kernel along the x-axis
         * @param sigma_y The standard deviation of the kernel along the y-axis
         * @param sigma_x The standard deviation of the kernel along the x-axis
         * @param border_type The interpolation type for the convolution
         */
        Gaussian(const size_t radius_y=1, const size_t radius_x=1,
            const double sigma_y=sqrt(2.5), const double sigma_x=sqrt(2.5),
            const bob::sp::Extrapolation::BorderType border_type =
              bob::sp::Extrapolation::Mirror):
          m_radius_y(radius_y), m_radius_x(radius_x), m_sigma_y(sigma_y),
          m_sigma_x(sigma_x), m_conv_border(border_type)
        {
          computeKernel();
        }

        /**
         * @brief Copy constructor
         */
        Gaussian(const Gaussian& other):
          m_radius_y(other.m_radius_y), m_radius_x(other.m_radius_x),
          m_sigma_y(other.m_sigma_y), m_sigma_x(other.m_sigma_x),
          m_conv_border(other.m_conv_border)
        {
          computeKernel();
        }

        /**
         * @brief Destructor
         */
        virtual ~Gaussian() {}

        /**
         * @brief Assignment operator
         */
        Gaussian& operator=(const Gaussian& other);

        /**
         * @brief Equal to
         */
        bool operator==(const Gaussian& b) const;
        /**
         * @brief Not equal to
         */
        bool operator!=(const Gaussian& b) const;

        /**
         * @brief Resets the parameters of the filter
         * @param radius_y The height of the kernel along the y-axis
         * @param radius_x The width of the kernel along the x-axis
         * @param sigma_y The standard deviation of the kernel along the y-axis
         * @param sigma_x The standard deviation of the kernel along the x-axis
         * @param border_type The interpolation type for the convolution
         */
        void reset( const size_t radius_y=1, const size_t radius_x=1,
          const double sigma_y=sqrt(2.5), const double sigma_x=sqrt(2.5),
          const bob::sp::Extrapolation::BorderType border_type =
            bob::sp::Extrapolation::Mirror);

        /**
         * @brief Getters
         */
        size_t getRadiusY() const { return m_radius_y; }
        size_t getRadiusX() const { return m_radius_x; }
        double getSigmaY() const { return m_sigma_y; }
        double getSigmaX() const { return m_sigma_x; }
        bob::sp::Extrapolation::BorderType getConvBorder() const { return m_conv_border; }
        const blitz::Array<double,1>& getKernelY() const { return m_kernel_y; }
        const blitz::Array<double,1>& getKernelX() const { return m_kernel_x; }

        /**
         * @brief Setters
         */
        void setRadiusY(const size_t radius_y)
        { m_radius_y = radius_y; computeKernel(); }
        void setRadiusX(const size_t radius_x)
        { m_radius_x = radius_x; computeKernel(); }
        void setSigmaY(const double sigma_y)
        { m_sigma_y = sigma_y; computeKernel(); }
        void setSigmaX(const double sigma_x)
        { m_sigma_x = sigma_x; computeKernel(); }
        void setConvBorder(const bob::sp::Extrapolation::BorderType border_type)
        { m_conv_border = border_type; }

        /**
         * @brief Process a 2D blitz Array/Image
         * @param src The 2D input blitz array
         * @param dst The 2D output blitz array
         */
        template <typename T>
        void operator()(const blitz::Array<T,2>& src,
          blitz::Array<double,2>& dst);

        /**
         * @brief Process a 3D blitz Array/Image
         * @param src The 3D input blitz array
         * @param dst The 3D output blitz array
         */
        template <typename T>
        void operator()(const blitz::Array<T,3>& src,
          blitz::Array<double,3>& dst);

      private:
        void computeKernel();

        /**
         * @brief Attributes
         */
        size_t m_radius_y;
        size_t m_radius_x;
        double m_sigma_y;
        double m_sigma_x;
        bob::sp::Extrapolation::BorderType m_conv_border;

        blitz::Array<double, 1> m_kernel_y;
        blitz::Array<double, 1> m_kernel_x;

        blitz::Array<double, 2> m_tmp_int;
        blitz::Array<double, 2> m_tmp_int1;
        blitz::Array<double, 2> m_tmp_int2;
    };

    // Declare template method full specialization
    template <>
    void bob::ip::Gaussian::operator()<double>(const blitz::Array<double,2>& src,
      blitz::Array<double,2>& dst);

    template <typename T>
    void bob::ip::Gaussian::operator()(const blitz::Array<T,2>& src,
      blitz::Array<double,2>& dst)
    {
      // Casts the input to double
      blitz::Array<double,2> src_d = bob::core::array::cast<double>(src);
      // Calls the specialized template function for double
      this->operator()(src_d, dst);
    }

    template <typename T>
    void bob::ip::Gaussian::operator()(const blitz::Array<T,3>& src,
      blitz::Array<double,3>& dst)
    {
      for( int p=0; p<dst.extent(0); ++p) {
        const blitz::Array<T,2> src_slice =
          src( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<double,2> dst_slice =
          dst( p, blitz::Range::all(), blitz::Range::all() );

        // Gaussian smooth plane
        this->operator()(src_slice, dst_slice);
      }
    }

  }
}

#endif
