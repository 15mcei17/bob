/**
 * @date Thu Apr 28 20:09:16 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief Test the integral image function for 2D arrays
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IP-Integral Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <blitz/array.h>
#include <stdint.h>

#include "bob/core/cast.h"
#include "bob/ip/integral.h"


struct T {
  blitz::Array<uint32_t,2> a2;
  blitz::Array<double,2> a2s, isi;
  blitz::Array<uint64_t,2> a2s_b, isi_b;

  T(): a2(4,4), a2s(4,4), isi(4,4), a2s_b(5,5), isi_b(5,5)
  {
    a2 =   0,  1,  2,  3,
           4,  5,  6,  7,
           8,  9, 10, 11,
          12, 13, 14, 15;

    a2s =  0.,  1.,  3.,   6.,
           4., 10., 18.,  28.,
          12., 27., 45.,  66.,
          24., 52., 84., 120.;

    a2s_b = 0,  0,  0,  0,   0,
            0,  0,  1,  3,   6,
            0,  4, 10, 18,  28,
            0, 12, 27, 45,  66,
            0, 24, 52, 84, 120;

    isi =  0.,   1.,   5.,   14.,
          16.,  42.,  82.,  140.,
          80., 187., 327.,  506.,
         224., 500., 836., 1240.;

    isi_b = 0,   0,   0,   0,    0,
            0,   0,   1,   5,   14,
            0,  16,  42,  82,  140,
            0,  80, 187, 327,  506,
            0, 224, 500, 836, 1240;

  }

  ~T() {}
};

template<typename T, typename U, int d>
void check_dimensions( blitz::Array<T,d>& t1, blitz::Array<U,d>& t2)
{
  BOOST_REQUIRE_EQUAL(t1.dimensions(), t2.dimensions());
  for( int i=0; i<t1.dimensions(); ++i)
    BOOST_CHECK_EQUAL(t1.extent(i), t2.extent(i));
}

template<typename T, typename U>
void checkBlitzEqual( blitz::Array<T,2>& t1, blitz::Array<U,2>& t2)
{
  check_dimensions( t1, t2);
  for( int i=0; i<t1.extent(0); ++i)
    for( int j=0; j<t1.extent(1); ++j)
      BOOST_CHECK_EQUAL(t1(i,j), bob::core::cast<T>(t2(i,j)));
}

template<typename T, typename U>
void checkBlitzEqual( blitz::Array<T,3>& t1, blitz::Array<U,3>& t2)
{
  check_dimensions( t1, t2);
  for( int i=0; i<t1.extent(0); ++i)
    for( int j=0; j<t1.extent(1); ++j)
      for( int k=0; k<t1.extent(2); ++k)
        BOOST_CHECK_EQUAL(t1(i,j,k), bob::core::cast<T>(t2(i,j,k)));
}

BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( test_integral_2d )
{
  blitz::Array<uint32_t,2> b2(4,4);
  // Integral image computation
  bob::ip::integral(a2, b2);
  checkBlitzEqual(a2s, b2);
}

BOOST_AUTO_TEST_CASE( test_integral_2d_addZeroBorder )
{
  blitz::Array<uint32_t,2> b2(5,5);
  // Integral image computation
  bob::ip::integral(a2, b2, true);
  checkBlitzEqual(a2s_b, b2);
}

BOOST_AUTO_TEST_CASE( test_integral_square_2d )
{
  blitz::Array<uint32_t,2> b2(4,4), s2(4,4);
  // Integral image computation
  bob::ip::integral(a2, b2, s2);
  checkBlitzEqual(a2s, b2);
  checkBlitzEqual(isi, s2);
}

BOOST_AUTO_TEST_CASE( test_integral_square_2d_addZeroBorder )
{
  blitz::Array<uint32_t,2> b2(5,5), s2(5,5);
  // Integral image computation
  bob::ip::integral(a2, b2, s2, true);
  checkBlitzEqual(a2s_b, b2);
  checkBlitzEqual(isi_b, s2);
}


BOOST_AUTO_TEST_SUITE_END()
