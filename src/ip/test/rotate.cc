/**
 * @date Fri Mar 11 17:12:09 2011 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief Test the rotation function for 2D and 3D arrays/images
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IP-Rotate Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <blitz/array.h>
#include <stdint.h>
#include "bob/core/logging.h"
#include "bob/core/array_convert.h"
#include "bob/ip/rotate.h"
#include "bob/ip/shear.h"

#include "bob/io/utils.h"
#include <algorithm>

#include <random/discrete-uniform.h>
#include <random/uniform.h>

#include <boost/filesystem.hpp>

struct T {
  blitz::Array<uint32_t,2> a2, a2r_90, a2r_180, a2r_270, a8;
  blitz::Array<uint32_t,2> a5, a55, a33;
  blitz::Array<bool,2> a8m, a8m_45;
  double eps;

  T(): a2(3,4), a2r_90(4,3), a2r_180(3,4), a2r_270(4,3), a8(8,8),
       a5(3,5), a55(5,5), a33(3,3), a8m(8,8), a8m_45(11,11), eps(0.03)
  {
    a2 = 0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11;

    a2r_90 = 3, 7, 11,
        2, 6, 10,
        1, 5, 9,
        0, 4, 8;

    a2r_180 = 11, 10, 9, 8,
        7, 6, 5, 4,
        3, 2, 1, 0;

    a2r_270 = 8, 4, 0,
        9, 5, 1,
        10, 6, 2,
        11, 7, 3;

    a8 = 1;
    a8m = true;
    // The following might likely be improved. Do not consider that as a pure
    // reference mask. Bilinear interpolation might be better with masks, as
    // the mask keep decreasing in size while performing successive shearing
    // operations.
    a8m_45 =
      false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, true, false, false, false, false, false, false,
      false, false, false, true, true, true, false, false, false, false, false,
      false, false, false, true, true, true, true, true, false, false, false,
      false, true, true, true, true, true, true, true, false, false, false,
      true, true, true, true, true, true, true, true, true, true, false,
      false, false, true, true, true, true, true, true, false, false, false,
      false, false, false, true, true, true, true, false, false, false, false,
      false, false, false, false, true, true, true, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false;
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


template<typename T, typename U>
void checkBlitzClose( blitz::Array<T,2>& t1, blitz::Array<U,2>& t2,
  const double eps )
{
  int y_min = std::min( t1.extent(0), t2.extent(0));
  int x_min = std::min( t1.extent(1), t2.extent(1));
  BOOST_CHECK_LE( abs(t1.extent(0)-t2.extent(0)), 1);
  BOOST_CHECK_LE( abs(t1.extent(1)-t2.extent(1)), 1);
  double diff = 0.;
  for( int i=0; i<y_min; ++i)
    for( int j=0; j<x_min; ++j)
      diff += abs( t1(i,j) - bob::core::cast<T>(t2(i,j)) );
  diff = (diff/(y_min*x_min)) /
    (std::numeric_limits<T>::max()-std::numeric_limits<T>::min()+1);
  BOOST_CHECK_SMALL( diff, eps );
}

template<typename T,typename U>
void checkBlitzClose( blitz::Array<T,3>& t1, blitz::Array<U,3>& t2,
  const double eps )
{
  int p_min = std::min( t1.extent(0), t2.extent(0));
  int y_min = std::min( t1.extent(1), t2.extent(1));
  int x_min = std::min( t1.extent(2), t2.extent(2));
  BOOST_CHECK_EQUAL( t1.extent(0), t2.extent(0) );
  BOOST_CHECK_LE( abs(t1.extent(1)-t2.extent(1)), 1);
  BOOST_CHECK_LE( abs(t1.extent(2)-t2.extent(2)), 1);
  double diff = 0.;
  for( int i=0; i<p_min; ++i)
    for( int j=0; j<y_min; ++j)
      for( int k=0; k<x_min; ++k)
        diff += abs( t1(i,j,k) - bob::core::cast<T>(t2(i,j,k)) );
  diff = (diff/(y_min*x_min*p_min)) /
    (std::numeric_limits<T>::max()-std::numeric_limits<T>::min()+1);
  BOOST_CHECK_SMALL( diff, eps );
}


BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( test_rotate_2d_mod90_uint32 )
{
  blitz::Array<double,2> b2;

  // Rotation of 0
  b2.resize( bob::ip::getRotatedShape(a2,0.) );
  bob::ip::rotate(a2, b2, 0.);
  checkBlitzEqual(a2, b2);

  // Rotation of 90
  b2.resize( bob::ip::getRotatedShape(a2,90.) );
  bob::ip::rotate(a2, b2, 90.);
  checkBlitzEqual(a2r_90, b2);

  // Rotation of 180
  b2.resize( bob::ip::getRotatedShape(a2,180.) );
  bob::ip::rotate(a2, b2, 180.);
  checkBlitzEqual(a2r_180, b2);

  // Rotation of 270
  b2.resize( bob::ip::getRotatedShape(a2,270.) );
  bob::ip::rotate(a2, b2, 270.);
  checkBlitzEqual(a2r_270, b2);
}


BOOST_AUTO_TEST_CASE( test_rotate_2d_generic_uint32 )
{
  // Get path to the XML Schema definition
  char *testdata_cpath = getenv("BOB_TESTDATA_DIR");
  if( !testdata_cpath || !strcmp( testdata_cpath, "") ) {
    bob::core::error << "Environment variable $BOB_TESTDATA_DIR " <<
      "is not set. " << "Have you setup your working environment " <<
      "correctly?" << std::endl;
    throw std::runtime_error("test failed");
  }
  // Load original image
  boost::filesystem::path testdata_path_img( testdata_cpath);
  testdata_path_img /= "image.hdf5";
  blitz::Array<uint8_t,2> img = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  blitz::Array<double,2> img_processed;

  // 5 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,5.) );
  bob::ip::rotate( img, img_processed, 5., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r5.hdf5";
  blitz::Array<uint8_t,2> img_ref_r5 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r5, img_processed, eps);

  // 10 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,10.) );
  bob::ip::rotate( img, img_processed, 10., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r10.hdf5";
  blitz::Array<uint8_t,2> img_ref_r10 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r10, img_processed, eps);

  // 15 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,15.) );
  bob::ip::rotate( img, img_processed, 15., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r15.hdf5";
  blitz::Array<uint8_t,2> img_ref_r15 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r15, img_processed, eps);

  // 30 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,30.) );
  bob::ip::rotate( img, img_processed, 30., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r30.hdf5";
  blitz::Array<uint8_t,2> img_ref_r30 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r30, img_processed, eps);

  // 45 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,45.) );
  bob::ip::rotate( img, img_processed, 45., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r45.hdf5";
  blitz::Array<uint8_t,2> img_ref_r45 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r45, img_processed, eps);

  // 70 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,70.) );
  bob::ip::rotate( img, img_processed, 70., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r70.hdf5";
  blitz::Array<uint8_t,2> img_ref_r70 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r70, img_processed, eps);

  // 237 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,237.) );
  bob::ip::rotate( img, img_processed, 237., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_r237.hdf5";
  blitz::Array<uint8_t,2> img_ref_r237 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_r237, img_processed, eps);

  // -25 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,-25.) );
  bob::ip::rotate( img, img_processed, -25., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_rn25.hdf5";
  blitz::Array<uint8_t,2> img_ref_rn25 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,2>();
  checkBlitzClose( img_ref_rn25, img_processed, eps);
}

BOOST_AUTO_TEST_CASE( test_rotate_3d_generic_uint32 )
{
  // Get path to the XML Schema definition
  char *testdata_cpath = getenv("BOB_TESTDATA_DIR");
  if( !testdata_cpath || !strcmp( testdata_cpath, "") ) {
    bob::core::error << "Environment variable $BOB_TESTDATA_DIR " <<
      "is not set. " << "Have you setup your working environment " <<
      "correctly?" << std::endl;
    throw std::runtime_error("test failed");
  }
  // Load original image
  boost::filesystem::path testdata_path_img( testdata_cpath);
  testdata_path_img /= "imageColor.hdf5";
  blitz::Array<uint8_t,3> img = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,3>();
  blitz::Array<double,3> img_processed;

  // 5 degrees
  img_processed.resize(bob::ip::getRotatedShape(img,5.) );
  bob::ip::rotate( img, img_processed, 5., bob::ip::Rotate::Shearing);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "imageColor_r5.hdf5";
  blitz::Array<uint8_t,3> img_ref_r5 = bob::io::open(testdata_path_img.string().c_str(), 'r')->read_all<uint8_t,3>();
  checkBlitzClose( img_ref_r5, img_processed, eps);
}

BOOST_AUTO_TEST_CASE( test_rotate_2d_mask )
{
  blitz::Array<double,2> b8;
  blitz::Array<bool,2> b8_mask;

  // Rotation of 45
  b8.resize( bob::ip::getRotatedShape(a8, 45.) );
  b8_mask.resize( b8.shape() );
  bob::ip::rotate(a8, a8m, b8, b8_mask, 45.);
  checkBlitzEqual(a8m_45, b8_mask);
}

BOOST_AUTO_TEST_CASE( test_get_angle_to_horizontal )
{
  // check that the getAngleToHorizontal function returns reasonable results
  const double epsilon = 1e-8;

  // along the axis
  BOOST_CHECK_SMALL(bob::ip::getAngleToHorizontal(0,0,0,1), epsilon);
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(0,1,0,0), 180., epsilon);
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(0,0,1,0), 90., epsilon);
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(1,0,0,0), -90., epsilon);

  // 45 degrees
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(0,0,1,1), 45., epsilon);
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(0,1,1,0), 135., epsilon);
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(1,1,0,0), -135., epsilon);
  BOOST_CHECK_CLOSE(bob::ip::getAngleToHorizontal(1,0,0,1), -45., epsilon);
}


BOOST_AUTO_TEST_SUITE_END()
