/**
 * @date Fri Jan 27 14:10:23 2012 +0100
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief Test the LU decomposition and the determinant
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE math-lu_det_inv Tests
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <blitz/array.h>
#include <stdint.h>
#include <bob/math/lu.h>
#include <bob/math/det.h>
#include <bob/math/inv.h>
#include <bob/math/linear.h>


struct T {
  blitz::Array<double,2> A33_1, A24_1, A33_2, A33_3;
  blitz::Array<double,2> L33_1, L24_1, L33_2;
  blitz::Array<double,2> U33_1, U24_1;
  blitz::Array<double,2> P33_1, P24_1;
  blitz::Array<double,2> A33_1_inv, I33;
  double det_A33_1, det_A33_2, det_A33_3, eps;

  T(): A33_1(3,3), A24_1(2,4), A33_2(3,3), A33_3(3,3), 
    L33_1(3,3), L24_1(2,2), L33_2(3,3),
    U33_1(3,3), U24_1(2,4), 
    P33_1(3,3), P24_1(2,2), 
    A33_1_inv(3,3), I33(3,3), 
    det_A33_1(-0.2766), 
    det_A33_2(4.), 
    det_A33_3(0.), 
    eps(2e-4)
  {
    A33_1 = 0.8147, 0.9134, 0.2785, 0.9058, 0.6324, 0.5469, 0.1270, 0.0975, 
      0.9575;
    L33_1 = 1.0000, 0., 0., 0.8994, 1.0000, 0., 0.1402, 0.0256, 1.0000;
    U33_1 = 0.9058, 0.6324, 0.5469, 0., 0.3446, -0.2134, 0., 0., 0.8863;
    P33_1 = 0, 1, 0, 1, 0, 0, 0, 0, 1;

    A24_1 = 0.7922, 0.6557, 0.8491, 0.6787,
      0.9595, 0.0357, 0.9340, 0.7577;
    L24_1 = 1., 0., 0.8256, 1.;
    U24_1 = 0.9595, 0.0357, 0.9340, 0.7577, 0., 0.6262, 0.0780, 0.0531;
    P24_1 = 0, 1, 1, 0;

    A33_1_inv = -1.9960, 3.0632, -1.1690, 2.8840, -2.6919, 0.6987, -0.0289, 
                -0.1322, 1.1283;
    I33 = 1., 0., 0., 0., 1., 0., 0., 0., 1.;

    A33_2 = 2, -1, 0, -1, 2, -1, 0, -1, 2.;
    L33_2 = 1.414213562373095, 0, 0, -0.707106781186547, 1.224744871391589, 0, 
              0, -0.816496580927726, 1.154700538379251;

    A33_3 = 0, 0, 0, 0, 0, 0, 0, 0, 0.;
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

template<typename T>  
void checkBlitzClose( blitz::Array<T,1>& t1, blitz::Array<T,1>& t2, 
  const double eps )
{
  check_dimensions( t1, t2);
  for( int i=0; i<t1.extent(0); ++i)
    BOOST_CHECK_SMALL( fabs( t2(i)-t1(i) ), eps);
}

template<typename T>  
void checkBlitzClose( blitz::Array<T,2>& t1, blitz::Array<T,2>& t2, 
  const double eps )
{
  check_dimensions( t1, t2);
  for( int i=0; i<t1.extent(0); ++i)
    for( int j=0; j<t1.extent(1); ++j)
      BOOST_CHECK_SMALL( fabs( t2(i,j)-t1(i,j) ), eps);
}

BOOST_FIXTURE_TEST_SUITE( test_setup, T )

BOOST_AUTO_TEST_CASE( test_lu_3x3 )
{
  blitz::Array<double,2> L(3,3);
  blitz::Array<double,2> U(3,3);
  blitz::Array<double,2> P(3,3);

  bob::math::lu(A33_1, L, U, P);

  checkBlitzClose(L, L33_1, eps);
  checkBlitzClose(U, U33_1, eps);
  checkBlitzClose(P, P33_1, eps);
}
  
BOOST_AUTO_TEST_CASE( test_lu_2x4 )
{
  blitz::Array<double,2> L(2,2);
  blitz::Array<double,2> U(2,4);
  blitz::Array<double,2> P(2,2);

  bob::math::lu(A24_1, L, U, P);

  checkBlitzClose(L, L24_1, eps);
  checkBlitzClose(U, U24_1, eps);
  checkBlitzClose(P, P24_1, eps);
}

BOOST_AUTO_TEST_CASE( test_chol_3x3 )
{
  blitz::Array<double,2> L(3,3);
  bob::math::chol(A33_2, L);
  checkBlitzClose(L, L33_2, eps);
}
  
BOOST_AUTO_TEST_CASE( test_det_3x3 )
{
  blitz::Array<double,2> det(3,3);

  BOOST_CHECK_SMALL( fabs(bob::math::det(A33_1) - det_A33_1), eps);
  BOOST_CHECK_SMALL( fabs(bob::math::det(A33_2) - det_A33_2), eps);
  BOOST_CHECK_SMALL( fabs(bob::math::det(A33_3) - det_A33_3), eps);
}

BOOST_AUTO_TEST_CASE( test_slogdet_3x3 )
{
  blitz::Array<double,2> det(3,3);

  int sign;
  double det_A33_v = exp(bob::math::slogdet(A33_1, sign));
  det_A33_v *= sign;
  BOOST_CHECK(sign == -1);
  BOOST_CHECK_SMALL( fabs(det_A33_v - det_A33_1), eps);

  det_A33_v = exp(bob::math::slogdet(A33_2, sign));
  det_A33_v *= sign;
  BOOST_CHECK(sign == 1);
  BOOST_CHECK_SMALL( fabs(det_A33_v - det_A33_2), eps);

  det_A33_v = exp(bob::math::slogdet(A33_3, sign));
  det_A33_v *= sign;
  BOOST_CHECK(sign == 0);
  BOOST_CHECK_SMALL( fabs(det_A33_v - det_A33_3), eps);
}



BOOST_AUTO_TEST_CASE( test_inv_3x3 )
{
  blitz::Array<double,2> inv(3,3);

  // Invert a matrix and compare to reference
  bob::math::inv(A33_1, inv);
  checkBlitzClose(inv, A33_1_inv, eps);

  blitz::Array<double,2> I(3,3);
  bob::math::prod(A33_1, inv, I);
  checkBlitzClose(I, I33, eps);
}

BOOST_AUTO_TEST_SUITE_END()

