/**
 * @file ip/cxx/test/Gaussian.cc
 * @date Sat Apr 30 17:52:15 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief Test the Gaussian smoothing on 2D images
 *
 * Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IP-Gaussian Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <blitz/array.h>
#include <stdint.h>
#include "bob/core/logging.h"
#include "bob/core/array_convert.h"
#include "bob/ip/Gaussian.h"

#include "bob/io/utils.h"
#include <algorithm>

#include <random/discrete-uniform.h>
#include <random/uniform.h>

#include <boost/filesystem.hpp>

struct T {
  double eps;

  T(): eps(0.03) {}

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


template<typename T>
void checkBlitzClose( blitz::Array<T,2>& t1, blitz::Array<T,2>& t2,
  const double eps )
{
  int y_min = std::min( t1.extent(0), t2.extent(0));
  int x_min = std::min( t1.extent(1), t2.extent(1));
  BOOST_CHECK_LE( abs(t1.extent(0)-t2.extent(0)), 1);
  BOOST_CHECK_LE( abs(t1.extent(1)-t2.extent(1)), 1);
  double diff = 0.;
  for( int i=0; i<y_min; ++i)
    for( int j=0; j<x_min; ++j)
      diff += abs( t1(i,j) - t2(i,j) );
  diff = (diff/(y_min*x_min)) /
    (std::numeric_limits<T>::max()-std::numeric_limits<T>::min()+1);
  BOOST_CHECK_SMALL( diff, eps );
}

template<typename T>
void checkBlitzClose( blitz::Array<T,3>& t1, blitz::Array<T,3>& t2,
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
        diff += abs( t1(i,j,k) - t2(i,j,k) );
  diff = (diff/(y_min*x_min*p_min)) /
    (std::numeric_limits<T>::max()-std::numeric_limits<T>::min()+1);
  BOOST_CHECK_SMALL( diff, eps );
}


BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( test_gaussianSmoothing_2d )
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
  boost::shared_ptr<bob::io::File> image_file = bob::io::open(testdata_path_img.string().c_str(), 'r');
  blitz::Array<uint8_t,2> img = image_file->read_all<uint8_t,2>();
  blitz::Array<double,2> img_d = bob::core::array::cast<double>(img);
  blitz::Array<double,2> img_processed_d(img_d.shape());
  bob::ip::Gaussian g_filter;
  g_filter(img_d,img_processed_d);
  blitz::Array<uint8_t,2> img_processed = bob::core::array::convertFromRange<uint8_t>(
      img_processed_d, blitz::min(img_processed_d), blitz::max(img_processed_d));

  // Compare to reference image
  testdata_path_img = testdata_cpath;
  testdata_path_img /= "image_Gaussian.hdf5";
  boost::shared_ptr<bob::io::File> ref_file = bob::io::open(testdata_path_img.string().c_str(), 'r');
  blitz::Array<uint8_t,2> img_ref = ref_file->read_all<uint8_t,2>();
  checkBlitzClose( img_processed, img_ref, eps);
}

BOOST_AUTO_TEST_SUITE_END()
