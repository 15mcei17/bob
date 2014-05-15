/**
 * @date Wed Apr 20 20:21:19 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * Rewritten:
 * @date Wed Apr 10 17:39:21 CEST 2013
 * @author Manuel Günther <manuel.guenther@idiap.ch>
 *
 * @brief Test the LBP functions for 2D arrays/images
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IP-Lbp Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "bob/ip/LBP.h"

#include <iostream>

struct T {
  blitz::Array<uint8_t,2> a1, a2;
  blitz::Array<uint16_t, 2> w1, w2;
  uint16_t lbp_4_a1, lbp_4_a2, lbp_4_a1_u2, lbp_4_a2_u2, lbp_4_ri, lbp_4_ur,
           lbp_8_a1, lbp_8_a2, lbp_8_a1_u2, lbp_8_a2_u2, lbp_8_ri, lbp_8_ur,
           lbp_16_a1, lbp_16_a2, lbp_16_a1_u2, lbp_16_a2_u2, lbp_16_ri, lbp_16_ur,

           lbp_4_d, lbp_8_d, lbp_16_d,
           lbp_4_a1_t, lbp_4_a2_t, lbp_8_a1_t, lbp_8_a2_t, lbp_16_a1_t, lbp_16_a2_t;

  T(): a1(3,3), a2(3,3), w1(3,3), w2(3,3)
  {
    a1 = 0, 1, 2,
         3, 4, 5,
         6, 7, 8;

    a2 = 8, 7, 6,
         5, 4, 3,
         2, 1, 0;

    w1 =  255, 254, 238,
          31, 30, 14,
          17, 16, 0;

    w2 =  0, 1, 57,
          224, 225, 249,
          238, 239, 255;

    // normal LBP4
    lbp_4_a1 = 6;
    lbp_4_a2 = 9;
    lbp_4_a1_u2 = 7;
    lbp_4_a2_u2 = 9;
    lbp_4_ri = 2;
    lbp_4_ur = 3;

    // normal LBP8
    lbp_8_a1 = 30;
    lbp_8_a2 = 225;
    lbp_8_a1_u2 = 29;
    lbp_8_a2_u2 = 33;
    lbp_8_ri = 8;
    lbp_8_ur = 5;

    // normal LBP16
    lbp_16_a1 = 1020;
    lbp_16_a2 = 64515;
    lbp_16_a1_u2 = 120;
    lbp_16_a2_u2 = 128;
    lbp_16_ri = 128;
    lbp_16_ur = 9;

    // direction coded LBP
    lbp_4_d = 10;
    lbp_8_d = 170;
    lbp_16_d = 43690;

    // transitional LBP
    lbp_4_a1_t = 3;
    lbp_4_a2_t = 12;
    lbp_8_a1_t = 135;
    lbp_8_a2_t = 120;
    lbp_16_a1_t = 32895;
    lbp_16_a2_t = 32640;

  }

  ~T() {}
};

template<typename T, int d>
void check_dimensions( blitz::Array<T,d>& t1, blitz::Array<T,d>& t2)
{
  BOOST_REQUIRE_EQUAL(t1.dimensions(), t2.dimensions());
  for( int i=0; i<t1.dimensions(); ++i)
    BOOST_CHECK_EQUAL(t1.extent(i), t2.extent(i));
}

template<typename T>
void checkBlitzEqual( blitz::Array<T,2>& t1, blitz::Array<T,2>& t2)
{
  check_dimensions( t1, t2);
  for( int i=0; i<t1.extent(0); ++i)
    for( int j=0; j<t1.extent(1); ++j)
      BOOST_CHECK_EQUAL(t1(i,j), t2(i,j));
}

template<typename T>
void checkBlitzEqual( blitz::Array<T,3>& t1, blitz::Array<T,3>& t2)
{
  check_dimensions( t1, t2);
  for( int i=0; i<t1.extent(0); ++i)
    for( int j=0; j<t1.extent(1); ++j)
      for( int k=0; k<t1.extent(2); ++k)
        BOOST_CHECK_EQUAL(t1(i,j,k), t2(i,j,k));
}

BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( test_lbp4_1_uint8 )
{
  // LBP 4,1
  bob::ip::LBP lbp(4);

  // rectangular LBP
  BOOST_CHECK_EQUAL( lbp_4_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 16, lbp.getMaxLabel() );

  // circular LBP
  lbp = bob::ip::LBP(4, 1., true);
  BOOST_CHECK_EQUAL( lbp_4_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 16, lbp.getMaxLabel() );

  // to average
  lbp = bob::ip::LBP(4, 1., true, true);
  BOOST_CHECK_EQUAL( lbp_4_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 16, lbp.getMaxLabel() );

  // add average bit; for our test patterns, the average bit is always added
  lbp = bob::ip::LBP(4, 1., true, true, true);
  BOOST_CHECK_EQUAL( lbp_4_a1*2+1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_a2*2+1, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 32, lbp.getMaxLabel() );

  // uniform LBP
  lbp = bob::ip::LBP(4, 1., true, false, false, true);
  BOOST_CHECK_EQUAL( lbp_4_a1_u2, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_a2_u2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 15, lbp.getMaxLabel() );

  // rotation invariant LBP
  lbp = bob::ip::LBP(4, 1., true, false, false, false, true);
  BOOST_CHECK_EQUAL( lbp_4_ri, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_ri, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 6, lbp.getMaxLabel() );

  // rotation invariant uniform LBP
  lbp = bob::ip::LBP(4, 1., true, false, false, true, true);
  BOOST_CHECK_EQUAL( lbp_4_ur, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_ur, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 6, lbp.getMaxLabel() );
}

BOOST_AUTO_TEST_CASE( test_lbp8_1_uint8 )
{
  // LBP 8,1
  bob::ip::LBP lbp(8);

  // rectangular LBP
  BOOST_CHECK_EQUAL( lbp_8_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 256, lbp.getMaxLabel() );

  // circular LBP
  lbp = bob::ip::LBP(8, 1., true);
  BOOST_CHECK_EQUAL( lbp_8_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 256, lbp.getMaxLabel() );

  // to average
  lbp = bob::ip::LBP(8, 1., true, true);
  BOOST_CHECK_EQUAL( lbp_8_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 256, lbp.getMaxLabel() );

  // add average bit; for our test patterns, the average bit is always added
  lbp = bob::ip::LBP(8, 1., true, true, true);
  BOOST_CHECK_EQUAL( lbp_8_a1*2+1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2*2+1, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 512, lbp.getMaxLabel() );

  // uniform LBP
  lbp = bob::ip::LBP(8, 1., true, false, false, true);
  BOOST_CHECK_EQUAL( lbp_8_a1_u2, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2_u2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 59, lbp.getMaxLabel() );

  // rotation invariant LBP
  lbp = bob::ip::LBP(8, 1., true, false, false, false, true);
  BOOST_CHECK_EQUAL( lbp_8_ri, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_ri, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 36, lbp.getMaxLabel() );

  // rotation invariant uniform LBP
  lbp = bob::ip::LBP(8, 1., true, false, false, true, true);
  BOOST_CHECK_EQUAL( lbp_8_ur, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_ur, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 10, lbp.getMaxLabel() );
}

BOOST_AUTO_TEST_CASE( test_lbp16_1_uint8 )
{
  // circular LBP 16,1
  bob::ip::LBP lbp(16, 1., true);

  BOOST_CHECK_EQUAL( lbp_16_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 65536, lbp.getMaxLabel() );

  // to average
  lbp = bob::ip::LBP(16, 1., true, true);
  BOOST_CHECK_EQUAL( lbp_16_a1, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_a2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 65536, lbp.getMaxLabel() );

  // add average bit DOESN'T WORK
//  lbp = bob::ip::LBP(16, 1., true, true, true);
//  BOOST_CHECK_EQUAL( lbp_16_a1*2, lbp(a1,1,1) );
//  BOOST_CHECK_EQUAL( lbp_16_a2*2, lbp(a2,1,1) );

  // uniform LBP
  lbp = bob::ip::LBP(16, 1., true, false, false, true);
  BOOST_CHECK_EQUAL( lbp_16_a1_u2, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_a2_u2, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 243, lbp.getMaxLabel() );

  // rotation invariant LBP
  lbp = bob::ip::LBP(16, 1., true, false, false, false, true);
  BOOST_CHECK_EQUAL( lbp_16_ri, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_ri, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 4116, lbp.getMaxLabel() );

  // rotation invariant uniform LBP
  lbp = bob::ip::LBP(16, 1., true, false, false, true, true);
  BOOST_CHECK_EQUAL( lbp_16_ur, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_ur, lbp(a2,1,1) );
  BOOST_CHECK_EQUAL( 18, lbp.getMaxLabel() );
}

BOOST_AUTO_TEST_CASE( test_lbp_image )
{
  // LBP 8,1
  bob::ip::LBP lbp(8);
  blitz::TinyVector<int,2> resolution = lbp.getLBPShape(a1);
  BOOST_CHECK_EQUAL(resolution[0], 1);
  BOOST_CHECK_EQUAL(resolution[1], 1);
  blitz::Array<uint16_t, 2> result(1,1);

  lbp(a1,result);
  BOOST_CHECK_EQUAL( lbp_8_a1, result(0,0) );
  lbp(a2,result);
  BOOST_CHECK_EQUAL( lbp_8_a2, result(0,0) );
}

BOOST_AUTO_TEST_CASE( test_lbp_wrap )
{
  // LBP 8,1
  bob::ip::LBP lbp(8, 1., false, false, false, false, false, bob::ip::ELBP_REGULAR, bob::ip::LBP_BORDER_WRAP);
  blitz::TinyVector<int,2> resolution = lbp.getLBPShape(a1);
  BOOST_CHECK_EQUAL(resolution[0], 3);
  BOOST_CHECK_EQUAL(resolution[1], 3);
  blitz::Array<uint16_t, 2> result(3,3);

  lbp(a1,result);
  checkBlitzEqual(result, w1);

  bob::ip::LBP lbp2(8, 1., true, false, false, false, false, bob::ip::ELBP_REGULAR, bob::ip::LBP_BORDER_WRAP);
  resolution = lbp2.getLBPShape(a2);
  BOOST_CHECK_EQUAL(resolution[0], 3);
  BOOST_CHECK_EQUAL(resolution[1], 3);

  lbp2(a2,result);
  checkBlitzEqual(result, w2);
}

BOOST_AUTO_TEST_CASE( test_mb_lbp ){
  // multi-block LBP 8,(3,2)
  bob::ip::LBP lbp832(8, blitz::TinyVector<int,2>(3, 2));
  blitz::Array<double,2> positions = lbp832.getRelativePositions();

  BOOST_CHECK_EQUAL(positions.shape()[0], 9);
  BOOST_CHECK_EQUAL(positions.shape()[1], 4);

  // check some positions
  BOOST_CHECK_EQUAL(positions(0,0), -4);
  BOOST_CHECK_EQUAL(positions(0,1), -1);
  BOOST_CHECK_EQUAL(positions(0,2), -3);
  BOOST_CHECK_EQUAL(positions(0,3), -1);

  BOOST_CHECK_EQUAL(positions(4,0), 2);
  BOOST_CHECK_EQUAL(positions(4,1), 5);
  BOOST_CHECK_EQUAL(positions(4,2), 1);
  BOOST_CHECK_EQUAL(positions(4,3), 3);

  // check that the multi-block LBP (8,1,1) and the LBP 8,1 generate identical results
  bob::ip::LBP lbp811(8, blitz::TinyVector<int,2>(1, 1));
  BOOST_CHECK_EQUAL( lbp_8_a1, lbp811(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2, lbp811(a2,1,1) );

  // compute integral image
  blitz::Array<int,2> ii(4,4);
  bob::ip::integral(a1, ii, true);
  BOOST_CHECK_EQUAL( lbp_8_a1, lbp811(ii,1,1,true) );
  bob::ip::integral(a2, ii, true);
  BOOST_CHECK_EQUAL( lbp_8_a2, lbp811(ii,1,1,true) );

  // check that the required dimensions fit
  blitz::TinyVector<int,2> resolution = lbp811.getLBPShape(a1);
  BOOST_CHECK_EQUAL(resolution[0], 1);
  BOOST_CHECK_EQUAL(resolution[1], 1);
  resolution = lbp811.getLBPShape(ii, true);
  BOOST_CHECK_EQUAL(resolution[0], 1);
  BOOST_CHECK_EQUAL(resolution[1], 1);

  // check that the image operator also extracts the same lbp codes
  blitz::Array<uint16_t,2> result(1,1);
  lbp811(a1,result);
  BOOST_CHECK_EQUAL( lbp_8_a1, result(0,0) );
  lbp811(ii,result, true);
  BOOST_CHECK_EQUAL( lbp_8_a2, result(0,0) );
}

BOOST_AUTO_TEST_CASE( test_omb_lbp ){
  // overlapping multi-block LBP 8,(3,3),(2,1)
  bob::ip::LBP lbp83321(8, blitz::TinyVector<int,2>(3, 3), blitz::TinyVector<int,2>(2,1));
  blitz::Array<double,2> positions = lbp83321.getRelativePositions();

  BOOST_CHECK_EQUAL(positions.shape()[0], 9);
  BOOST_CHECK_EQUAL(positions.shape()[1], 4);

  // check some positions
  BOOST_CHECK_EQUAL(positions(0,0), -2);
  BOOST_CHECK_EQUAL(positions(0,1), 1);
  BOOST_CHECK_EQUAL(positions(0,2), -3);
  BOOST_CHECK_EQUAL(positions(0,3), 0);

  BOOST_CHECK_EQUAL(positions(4,0), 0);
  BOOST_CHECK_EQUAL(positions(4,1), 3);
  BOOST_CHECK_EQUAL(positions(4,2), 1);
  BOOST_CHECK_EQUAL(positions(4,3), 4);

  // check that the image resolution and offset is computed correctly
  blitz::TinyVector<int,2> resolution = lbp83321.getLBPShape(blitz::TinyVector<int,2>(10,10));
  BOOST_CHECK_EQUAL(resolution(0), 6);
  BOOST_CHECK_EQUAL(resolution(1), 4);

  // check that the image resolution and offset is computed correctly
  blitz::TinyVector<int,2> offset = lbp83321.getOffset();
  BOOST_CHECK_EQUAL(offset(0), 2);
  BOOST_CHECK_EQUAL(offset(1), 3);

}

BOOST_AUTO_TEST_CASE( test_lbp_other )
{
  // direction-coded LBP; should be identical for both test patterns
  bob::ip::LBP lbp(4, 1., true, false, false, false, false, bob::ip::ELBP_DIRECTION_CODED);
  BOOST_CHECK_EQUAL( lbp_4_d, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_d, lbp(a2,1,1) );
  lbp = bob::ip::LBP(8, 1., true, false, false, false, false, bob::ip::ELBP_DIRECTION_CODED);
  BOOST_CHECK_EQUAL( lbp_8_d, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_d, lbp(a2,1,1) );
  lbp = bob::ip::LBP(16, 1., true, false, false, false, false, bob::ip::ELBP_DIRECTION_CODED);
  BOOST_CHECK_EQUAL( lbp_16_d, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_d, lbp(a2,1,1) );

  // transitional LBP
  lbp = bob::ip::LBP(4, 1., true, false, false, false, false, bob::ip::ELBP_TRANSITIONAL);
  BOOST_CHECK_EQUAL( lbp_4_a1_t, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_4_a2_t, lbp(a2,1,1) );
  lbp = bob::ip::LBP(8, 1., true, false, false, false, false, bob::ip::ELBP_TRANSITIONAL);
  BOOST_CHECK_EQUAL( lbp_8_a1_t, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_8_a2_t, lbp(a2,1,1) );
  lbp = bob::ip::LBP(16, 1., true, false, false, false, false, bob::ip::ELBP_TRANSITIONAL);
  BOOST_CHECK_EQUAL( lbp_16_a1_t, lbp(a1,1,1) );
  BOOST_CHECK_EQUAL( lbp_16_a2_t, lbp(a2,1,1) );
}

BOOST_AUTO_TEST_SUITE_END()
