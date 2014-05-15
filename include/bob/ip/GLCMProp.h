/**
 * @author Ivana Chingovska <ivana.chingovska@idiap.ch>
 * @date Wed Jan 30 12:09:24 CET 2013
 *
 * This file defines a function to compute the Grey Level Co-occurence Matrix (GLCM)
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_GLCMPROP_H
#define BOB_IP_GLCMPROP_H

#include <blitz/array.h>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include "bob/core/assert.h"
#include "bob/core/array_copy.h"
#include "bob/core/cast.h"

namespace bob { namespace ip {

  /**
   * This class contains a number of texture properties of the Grey-Level Co-occurence Matrix (GLCM). The texture properties are selected from several publications:
   *
   * [1] R. M. Haralick, K. Shanmugam, I. Dinstein; "Textural Features for Image calssification",
   * in IEEE Transactions on Systems, Man and Cybernetics, vol.SMC-3, No. 6, p. 610-621.
   * [2] L. Soh and C. Tsatsoulis; Texture Analysis of SAR Sea Ice Imagery Using Gray Level Co-Occurrence Matrices, IEEE Transactions on Geoscience and Remote Sensing, vol. 37, no. 2, March 1999.
   * [3] D A. Clausi, An analysis of co-occurrence texture statistics as a function of grey level quantization, Can. J. Remote Sensing, vol. 28, no.1, pp. 45-62, 2002
   * [4] http://murphylab.web.cmu.edu/publications/boland/boland_node26.html
   * [5] http://www.mathworks.com/matlabcentral/fileexchange/22354-glcmfeatures4-m-vectorized-version-of-glcmfeatures1-m-with-code-changes
   * [6] http://www.mathworks.ch/ch/help/images/ref/graycoprops.html
   */
  class GLCMProp {

    public: //api

      /**
       * @brief Complete constructor
       */

      GLCMProp();


      /**
       * @brief Copy constructor
       */
      GLCMProp(const GLCMProp& other) {}

      /**
       * @brief Destructor
       */
      virtual ~GLCMProp();

      /**
       * @brief Assignment
       */
      GLCMProp& operator= (const GLCMProp& other) { return *this; }

      /**
      * @brief Get the shape of the output array for the property
      */
      const blitz::TinyVector<int,1> get_prop_shape(const blitz::Array<double,3>& glcm) const;

      /**
       * @brief Compute each of the single GLCM properties from a 3D blitz::Array which is the GLCM matrix
       *
       * The following method provides texture properties of the GLCM matrix. Here is a list of all the implemented features.
       * f1. angular second moment [1] / energy [6]
       * f2. energy [4]
       * f3. sum of squares (variance) [1]
       * f4. contrast [1] == contrast [6]
       * f5. correlation [1]
       * f6. inverse difference moment [1] = homogeneity [2], homop[5]
       * f7. sum average [1]
       * f8. sum variance [1]
       * f9. sum entropy [1]
       * f10. entropy [1]
       * f11. difference variance [4]
       * f12. difference entropy [1]
       * f13. dissimilarity [4]
       * f14. homogeneity [6]
       * f15. cluster prominence [2]
       * f16. cluster shade [2]
       * f17. maximum probability [2]
       * f18. information measure of correlation 1 [1]
       * f19. information measure of correlation 2 [1]
       * f20. inverse difference (INV) is homom [3]
       * f21. inverse difference normalized (INN) [3]
       * f22. inverse difference moment normalized [3]
       * f23. auto-correlation [2]
       * f24. correlation as in MATLAB Image Processing Toolbox method graycoprops() ([6])
       */

      void angular_second_moment(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void energy(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void variance(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void contrast(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void auto_correlation(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void correlation(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void correlation_m(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void inv_diff_mom(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void sum_avg(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void sum_var(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void sum_entropy(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void entropy(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void diff_var(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void diff_entropy(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void dissimilarity(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void homogeneity(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void cluster_prom(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void cluster_shade(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void max_prob(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void inf_meas_corr1(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void inf_meas_corr2(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void inv_diff(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void inv_diff_norm(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;
      void inv_diff_mom_norm(const blitz::Array<double,3>& glcm, blitz::Array<double,1>& prop) const;

    protected:
    /**
     * @brief Methods
     */

    /**
     * @brief Normalizes the glcm matrix (by offset. The separate matrix for each offset is separately normalized))
     */
    const blitz::Array<double,3> normalize_glcm(const blitz::Array<double,3>& glcm) const;

};

}}

#endif /* BOB_IP_GLCMPROP_H */

