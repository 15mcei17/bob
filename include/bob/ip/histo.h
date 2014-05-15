/**
 * @date Mon Apr 18 16:08:34 2011 +0200
 * @author Francois Moulin <Francois.Moulin@idiap.ch>
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */
#ifndef BOB5SPRO_IP_HISTO_H
#define BOB5SPRO_IP_HISTO_H

#include <stdint.h>
#include <algorithm>
#include <stdexcept>
#include <boost/format.hpp>
#include <blitz/array.h>

#include "bob/core/assert.h"
#include "bob/core/array_type.h"

namespace tca = bob::core::array;
namespace bob {
  namespace ip {

    namespace detail {

      /**
       * Return the histogram size for a given type T
       * @warning This function works only for uint8_t and uint16_t,
       *          otherwise it raises std::runtime_error exception
       */
      template<typename T>
      int getHistoSize() {
        int histo_size = 0;
        tca::ElementType element_type = bob::core::array::getElementType<T>();
        switch(element_type) {
          case tca::t_uint8:
            histo_size = 256;
            break;
          case tca::t_uint16:
            histo_size = 65536;
            break;
          default:
            {
              boost::format m("data type `%s' cannot be histogrammed");
              m % bob::core::array::stringize<T>();
              throw std::runtime_error(m.str());
            }
        }

        return histo_size;
      }


      template<typename T>
      class ReduceHisto {

      public:

        //You need the following public typedefs and statics as blitz use them
        //internally.
        typedef T T_sourcetype;
        typedef blitz::Array<uint64_t,1> T_resulttype;
        typedef T_resulttype T_numtype;

        static const bool canProvideInitialValue = true;
        static const bool needIndex = false;
        static const bool needInit = false;

        ReduceHisto() {
          histo_size = detail::getHistoSize<T>();
          m_result.resize(histo_size);
          reset();
        }

        ReduceHisto(blitz::Array<uint64_t, 1> initialValue) {
          histo_size = detail::getHistoSize<T>();
          m_result.resize(histo_size);
          reset(initialValue);
        }

        //accumulates, doesn't tell the index position
        inline bool operator()(T x) const {
          m_result(x) ++;
          return true;
        }

        //accumulates, tells us the index position
        inline bool operator()(T x, int=0) const {
          m_result(x) ++;
          return true;
        }

        //gets the result, tells us how many items we have seen
        inline blitz::Array<uint64_t, 1> result(int count) const {
          return m_result;
        }

        void reset() const {
          m_result = 0;
        }

        void reset(blitz::Array<uint64_t, 1> initialValue) {
          m_result = initialValue;
        }

        static const char* name() {
          return "histo";
        }

      protected: //representation
        int histo_size;
        mutable blitz::Array<uint64_t, 1> m_result;
      };
    }
  }
}



/**
 * This is the bit that declares the reduction for blitz++
 * Warning: Reductions *must* be declared inside the blitz namespace...
 */
namespace blitz {
  BZ_DECL_ARRAY_FULL_REDUCE(histo, bob::ip::detail::ReduceHisto)
}

namespace bob {
  namespace ip {


    /**
     * Compute an histogram of a 2D array.
     *
     * @warning This function only accepts arrays of @c uint8_t or @c uint16_t.
     *          Any other type raises a std::runtime_error exception
     *
     * @param src source 2D array
     * @param histo result of the function. This array must have 256 elements
     *              for @c uint8_t or 65536 for @c uint16_t
     * @param accumulate if true the result is added to @c histo
     */
    template<typename T>
    void histogram(const blitz::Array<T, 2>& src, blitz::Array<uint64_t, 1>& histo, bool accumulate = false) {
      // GetHistoSize returns an exception if T is not uint8_t or uint16_t
      int histo_size = detail::getHistoSize<T>();

      tca::assertSameShape<uint64_t, 1>(histo, blitz::shape(histo_size));
      tca::assertZeroBase<uint64_t, 1>(histo);

      if (accumulate) {
        histo += blitz::histo(src);
      }
      else {
        histo = blitz::histo(src);
      }
    }

    /**
     * Compute an histogram of a 2D array.
     *
     * @warning This function only accepts arrays of int or float (int8, int16,
     *          int32, int64, uint8, uint16, uint32, float32, float64
     *          and float128)
     *          Any other type raises a std::runtime_error exception
     * @warning You must have @c min <= @c src(i,j) <= @c max, for every i and j
     * @warning If @c min >= @c max or @c nb_bins == 0, a
     *
     * @param src source 2D array
     * @param histo result of the function. This array must have @c nb_bins
     *              elements
     * @param min least possible value in @c src
     * @param max greatest possible value in @c src
     * @param nb_bins number of bins (must not be zero)
     * @param accumulate if true the result is added to @c histo
     */
    template<typename T>
    void histogram(const blitz::Array<T, 2>& src, blitz::Array<uint64_t, 1>& histo, T min, T max, uint32_t nb_bins, bool accumulate = false) {
      tca::ElementType element_type = bob::core::array::getElementType<T>();

      // Check that the given type is supported
      switch (element_type) {
        case tca::t_int8:
        case tca::t_int16:
        case tca::t_int32:
        case tca::t_int64:
        case tca::t_uint8:
        case tca::t_uint16:
        case tca::t_uint32:
        case tca::t_uint64:
        case tca::t_float32:
        case tca::t_float64:
        case tca::t_float128:
          // Valid type
          break;
        default:
          // Invalid type
          {
            boost::format m("data type `%s' cannot be histogrammed");
            m % bob::core::array::stringize<T>();
            throw std::runtime_error(m.str());
          }
      }

      if (max <= min) {
        std::ostringstream oss;
        oss << "the `max' value (" << max << ") should be larger than the `min' value (" << min << ")";
        throw std::runtime_error(oss.str());
      }
      if (nb_bins == 0) {
        throw std::runtime_error("the parameter `nb_bins' cannot be zero");
      }

      tca::assertSameShape<uint64_t, 1>(histo, blitz::shape(nb_bins));
      tca::assertZeroBase<uint64_t, 1>(histo);

      // Handle the special case nb_bins == 1
      if (nb_bins == 1) {
        if (accumulate) {
          histo(0) += histo.size();
        }
        else {
          histo(0) = histo.size();
        }

        return;
      }

      T width = max - min;
      double bin_size = width / static_cast<double>(nb_bins);

      if (!accumulate) {
        histo = 0;
      }

      for(int i = src.lbound(0); i <= src.ubound(0); i++) {
        for(int j = src.lbound(1); j <= src.ubound(1); j++) {
          T element = src(i, j);
          // Convert a value into a bin index
          // TODO: check that element value is in the range [min,max]
          uint32_t index = static_cast<uint32_t>((element - min) / bin_size);
          index = std::min(index, nb_bins-1);
          ++(histo(index));
        }
      }
    }


    /**
     * Performs a histogram equalization of an image.
     *
     * @warning This function only accepts source arrays of int (int8, int16, int32, uint8, uint16, uint32)
     *          and target arrays of type int (see above) or float(float32, float64, float128).
     *          Any other type raises a std::runtime_error exception
     *
     * If the given target image is of integral type, the values will be spread out to fill the complete range of that type.
     * If the target is of type float, the values will be spread out to fill the range of the **source** type.
     *
     * @param src   the source 2D array of integral type
     * @param dst   the target 2D array
     */
    template<typename T1, typename T2>
    void histogram_equalize(const blitz::Array<T1, 2>& src, blitz::Array<T2, 2>& dst) {
      tca::ElementType element_type = bob::core::array::getElementType<T1>();

      // minimum and maximum value of the given type
      T1 src_min, src_max;

      // Check that the given type is supported
      // we here only support integral types lower than 64 bit
      switch (element_type) {
        case tca::t_int8:
        case tca::t_int16:
        case tca::t_int32:
        case tca::t_uint8:
        case tca::t_uint16:
        case tca::t_uint32:
          // Valid type
          src_min = std::numeric_limits<T1>::min();
          src_max = std::numeric_limits<T1>::max();
          break;
        default:
          // Invalid type
          {
            boost::format m("data type `%s' cannot be histogrammed");
            m % bob::core::array::stringize<T1>();
            throw std::runtime_error(m.str());
          }
      }

      // range of the desired type
      T2 dst_min, dst_max;

      element_type = bob::core::array::getElementType<T2>();


      // Check that the given type is supported
      // we here only support integral types lower than 64 bit
      switch (element_type) {
        case tca::t_int8:
        case tca::t_int16:
        case tca::t_int32:
        case tca::t_uint8:
        case tca::t_uint16:
        case tca::t_uint32:
          // Valid type
          dst_min = std::numeric_limits<T2>::min();
          dst_max = std::numeric_limits<T2>::max();
          break;
        case tca::t_float32:
        case tca::t_float64:
        case tca::t_float128:
          dst_min = src_min;
          dst_max = src_max;
          break;
        default:
          // Invalid type
          {
            boost::format m("data type `%s' cannot be histogrammed");
            m % bob::core::array::stringize<T2>();
            throw std::runtime_error(m.str());
          }
      }

      bob::core::array::assertSameShape(src, dst);

      // first, compute histogram of the image
      uint32_t bin_count = src_max - src_min + 1;
      blitz::Array<uint64_t,1> hist(bin_count);
      histogram(src, hist, src_min, src_max, bin_count);

      // now, compute the cumulative histogram distribution function
      blitz::Array<double,1> cdf(bin_count);
      // -- we don't count the black pixels...
      double pixel_count = src.size() - hist(0);
      cdf(0) = 0.;
      for (uint32_t i = 1; i < bin_count; ++i){
        cdf(i) = cdf(i-1) + (double)hist(i) / pixel_count;
      }

      // fill the resulting image
      T2 dst_range = dst_max - dst_min;
      for (int y = src.lbound(0); y <= src.ubound(0); ++y)
        for (int x = src.lbound(1); x <= src.ubound(1); ++x){
          // here, the CDF is indexed by the current pixel value to get
          dst(y + dst.lbound(0), x + dst.lbound(1)) = static_cast<T2>(cdf(src(y,x)) * dst_range + dst_min);
      }

    }
  }
}

#endif /* BOB5SPRO_IP_HISTO_H */
