/**
 * @file io/cxx/test/image_codec.cc
 * @date Wed Jun 22 17:50:08 2011 +0200
 * @author Andre Anjos <andre.anjos@idiap.ch>
 *
 * @brief ImageArrayCodec tests
 *
 * Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ImageArrayCodec Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>

#include <blitz/array.h>
#include <bob/core/cast.h>
#include "bob/core/logging.h"
#include "bob/io/utils.h"

struct T {
  blitz::Array<uint8_t,2> a;
  blitz::Array<uint8_t,3> b;
  blitz::Array<uint16_t,3> c;
  blitz::Array<uint8_t,2> d;
  blitz::Array<uint8_t,3> e;

  T() {
    a.resize(6,4);
    a = 1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24;
    b.resize(3,2,4);
    b = 1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24;
    c.resize(3,6,4);
    c = 2;
    d.resize(6,4);
    d = 1, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 1, 0, 1, 1, 1, 1,
        0, 1, 0, 1, 0, 0, 0, 1;
    e.resize(3,2,4); // Colours are multiple of 8 for GIF quantization
    e = 0, 8, 64,  0, 248,   0, 128,  0,
        0, 8, 32,  0, 248, 248,  64, 64,
        0, 8,  8, 64,   0, 248,  32,  0;
  }

  ~T() { }

};


template<typename T, typename U>
void check_equal(blitz::Array<T,2> a, blitz::Array<U,2> b)
{
  BOOST_REQUIRE_EQUAL(a.extent(0), b.extent(0));
  BOOST_REQUIRE_EQUAL(a.extent(1), b.extent(1));
  for (int i=0; i<a.extent(0); ++i) {
    for (int j=0; j<a.extent(1); ++j) {
      BOOST_CHECK_EQUAL(a(i,j), bob::core::cast<T>(b(i,j)));
    }
  }
}

template<typename T, typename U>
void check_equal(blitz::Array<T,3> a, blitz::Array<U,3> b)
{
  BOOST_REQUIRE_EQUAL(a.extent(0), b.extent(0));
  BOOST_REQUIRE_EQUAL(a.extent(1), b.extent(1));
  BOOST_REQUIRE_EQUAL(a.extent(2), b.extent(2));
  for (int i=0; i<a.extent(0); ++i) {
    for (int j=0; j<a.extent(1); ++j) {
      for (int k=0; k<a.extent(2); ++k) {
        BOOST_CHECK_EQUAL(a(i,j,k), bob::core::cast<T>(b(i,j,k)));
      }
    }
  }
}

BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( image_gif )
{
  std::string filename = bob::core::tmpfile(".gif");
  bob::io::save(filename.c_str(), e);
  check_equal( bob::io::load<uint8_t,3>(filename.c_str()), e );
  boost::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE( image_bmp )
{
  std::string filename = bob::core::tmpfile(".bmp");
  std::cout << "saved = " << bob::core::array::cast<uint32_t>(b) << std::endl;
  bob::io::save(filename.c_str(), b);
  std::cout << "loaded = " << bob::core::array::cast<uint32_t>(bob::io::load<uint8_t,3>(filename.c_str())) << std::endl;
  check_equal( bob::io::load<uint8_t,3>(filename.c_str()), b );
  boost::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE( image_png )
{
  // Grayscale 8 bits
  {
    std::string filename = bob::core::tmpfile(".png");
    bob::io::save(filename.c_str(), a);
    check_equal( bob::io::load<uint8_t,2>(filename.c_str()), a);
    boost::filesystem::remove(filename);
  }

  // Color 8 bits
  {
    std::string filename = bob::core::tmpfile(".png");
    bob::io::save(filename.c_str(), b);
    check_equal( bob::io::load<uint8_t,3>(filename.c_str()), b);
    boost::filesystem::remove(filename);
  }

  // Color 16 bits
  {
    std::string filename = bob::core::tmpfile(".png");
    bob::io::save(filename.c_str(), c);
    check_equal( bob::io::load<uint16_t,3>(filename.c_str()), c);
    boost::filesystem::remove(filename);
  }
}

BOOST_AUTO_TEST_CASE( image_tiff )
{
  // Grayscale 8 bits
  {
    std::string filename = bob::core::tmpfile(".tiff");
    bob::io::save(filename.c_str(), a);
    check_equal( bob::io::load<uint8_t,2>(filename.c_str()), a);
    boost::filesystem::remove(filename);
  }

  // Color 8 bits
  {
    std::string filename = bob::core::tmpfile(".tiff");
    bob::io::save(filename.c_str(), b);
    check_equal( bob::io::load<uint8_t,3>(filename.c_str()), b);
    boost::filesystem::remove(filename);
  }

  // Color 16 bits
  {
    std::string filename = bob::core::tmpfile(".tiff");
    bob::io::save(filename.c_str(), c);
    check_equal( bob::io::load<uint16_t,3>(filename.c_str()), c);
    boost::filesystem::remove(filename);
  }
}

/*
BOOST_AUTO_TEST_CASE( image_jpg )
{
  std::string filename = bob::core::tmpfile(".jpg");
  bob::io::save(filename.c_str(), b);
  check_equal( bob::io::load<uint8_t,3>(filename.c_str()), b );
  boost::filesystem::remove(filename);
}
*/

BOOST_AUTO_TEST_CASE( image_pbm )
{
  std::string filename = bob::core::tmpfile(".pbm");
  bob::io::save(filename.c_str(), d);
  check_equal( bob::io::load<uint8_t,2>(filename.c_str()), d );
  boost::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE( image_pgm )
{
  std::string filename = bob::core::tmpfile(".pgm");
  bob::io::save(filename.c_str(), a);
  check_equal( bob::io::load<uint8_t,2>(filename.c_str()), a );
  boost::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE( image_ppm )
{
  std::string filename = bob::core::tmpfile(".ppm");
  bob::io::save(filename.c_str(), b);
  check_equal( bob::io::load<uint8_t,3>(filename.c_str()), b );
  boost::filesystem::remove(filename);
}

BOOST_AUTO_TEST_SUITE_END()
