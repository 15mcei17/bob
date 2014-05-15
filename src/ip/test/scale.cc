/**
 * @date Mon Mar 14 11:21:29 2011 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief Test the rescaling function for 2D and 3D arrays/images
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IP-Scale Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <blitz/array.h>
#include <stdint.h>
#include "bob/core/logging.h"
#include "bob/core/cast.h"
#include "bob/ip/scale.h"

#include "bob/io/utils.h"
#include <algorithm>

#include <random/discrete-uniform.h>
#include <random/uniform.h>

#include <boost/filesystem.hpp>

struct T {
  double eps;
  blitz::Array<uint8_t,2> img_44, img_22;
  blitz::Array<bool,2> img_m44, img_m22;

  T(): eps(0.01), img_44(4,4), img_22(2,2), img_m44(4,4), img_m22(2,2)
  {
    img_44 = 0;
    img_22 = 0;

    img_m44 = false, false, true, true,
              false, false, true, true,
              true, true, true, true,
              true, true, true, true;

    img_m22  = false, true,
               true, true;
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
void checkBlitzClose( blitz::Array<T,2>& t1, blitz::Array<U,2>& t2,
  const double eps )
{
  int y_min = std::min( t1.extent(0), t2.extent(0));
  int x_min = std::min( t1.extent(1), t2.extent(1));
  BOOST_CHECK_LE( abs(t1.extent(0)-t2.extent(0)), 1);
  BOOST_CHECK_LE( abs(t1.extent(1)-t2.extent(1)), 1);
  for( int i=0; i<y_min; ++i)
    for( int j=0; j<x_min; ++j)
      BOOST_CHECK_SMALL(std::abs(bob::core::cast<U>(t1(i,j)) - t2(i,j)), 1. + eps);
}


// #define REGENERATE_REFERENCE_IMAGES

BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( test_scale_2d_generic_uint8 )
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
  blitz::Array<uint8_t,2> img = bob::io::load<uint8_t,2>(testdata_path_img.string().c_str());
  blitz::Array<double,2> img_processed;

  // Scale original image and compare with ImageMagick reference image

  // 137x137
  img_processed.resize(137,137);
  bob::ip::scale( img, img_processed, bob::ip::Rescale::BilinearInterp);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_s137x137.hdf5";
#ifdef REGENERATE_REFERENCE_IMAGES
  bob::io::save(testdata_path_img.string().c_str(), bob::core::array::cast<uint8_t>(img_processed));
#else // REGENERATE_REFERENCE_IMAGES
  blitz::Array<uint8_t,2> img_ref_s137 = bob::io::load<uint8_t,2>(testdata_path_img.string().c_str());
  checkBlitzClose( img_ref_s137, img_processed, eps);
#endif // REGENERATE_REFERENCE_IMAGES

  // 77x77
  img_processed.resize(77,77);
  bob::ip::scale( img, img_processed, bob::ip::Rescale::BilinearInterp);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_s77x77.hdf5";
#ifdef REGENERATE_REFERENCE_IMAGES
  bob::io::save(testdata_path_img.string().c_str(), bob::core::array::cast<uint8_t>(img_processed));
#else // REGENERATE_REFERENCE_IMAGES
  blitz::Array<uint8_t,2> img_ref_s77 = bob::io::load<uint8_t,2>(testdata_path_img.string().c_str());
  checkBlitzClose( img_ref_s77, img_processed, eps);
#endif // REGENERATE_REFERENCE_IMAGES

  // 125x75
  img_processed.resize(125,75);
  bob::ip::scale( img, img_processed, bob::ip::Rescale::BilinearInterp);
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_s125x75.hdf5";
#ifdef REGENERATE_REFERENCE_IMAGES
  bob::io::save(testdata_path_img.string().c_str(), bob::core::array::cast<uint8_t>(img_processed));
#else // REGENERATE_REFERENCE_IMAGES
  blitz::Array<uint8_t,2> img_ref_s125x75 = bob::io::load<uint8_t,2>(testdata_path_img.string().c_str());
  checkBlitzClose( img_ref_s125x75, img_processed, eps);
#endif // REGENERATE_REFERENCE_IMAGES

  // 100x100
  img_processed.resize(100,100);
  bob::ip::scale( img, img_processed, bob::ip::Rescale::BilinearInterp);
  checkBlitzClose( img, img_processed, eps);
}

BOOST_AUTO_TEST_CASE( test_scale_2d_mask )
{
  blitz::Array<double,2> b2(2,2);
  blitz::Array<bool,2> b2_mask(2,2);
  bob::ip::scale( img_44, img_m44, b2, b2_mask, bob::ip::Rescale::BilinearInterp);
  checkBlitzEqual( img_22, b2);
  checkBlitzEqual( img_m22, b2_mask);
}

BOOST_AUTO_TEST_SUITE_END()
