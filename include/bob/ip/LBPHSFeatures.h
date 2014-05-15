/**
 * @date Wed Apr 27 20:40:12 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file provides a class to extract features based on histogram
 *   sequences of Local Binary Patterns, as described in:
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_LBPHS_FEATURES_H
#define BOB_IP_LBPHS_FEATURES_H

#include "bob/core/cast.h"
#include "bob/ip/block.h"
#include "bob/ip/histo.h"
#include "bob/ip/LBP.h"
#include <list>

namespace bob {
/**
 * \ingroup libip_api
 * @{
 *
 */
  namespace ip {

  /**
   * @brief This class can be used to extract features based on histogram
   *   sequences of Local Binary Patterns, as described in the following
   *   article:
   */
  class LBPHSFeatures
  {
    public:

      /**
        * @brief Constructor: generates the LBPHSFeatures object using an LBP operator with the specified parameters
        */
      LBPHSFeatures( const int block_h, const int block_w, const int overlap_h,
          const int overlap_w, const double lbp_r = 1, const int lbp_p = 8,
          const bool circular = false, const bool to_average = false,
          const bool add_average_bit = false, const bool uniform = false,
          const bool rotation_invariant = false):
        m_lbp(lbp_p, lbp_r, circular, to_average, add_average_bit, uniform, rotation_invariant),
        m_block_h(block_h), m_block_w(block_w), m_overlap_h(overlap_h),
        m_overlap_w(overlap_w), m_lbp_r(lbp_r), m_lbp_p(lbp_p)
      {
      }

      /**
        * @brief Constructor: generates the LBPHSFeatures object using the specified LBP operator
        */
      LBPHSFeatures( const int block_h, const int block_w, const int overlap_h,
          const int overlap_w, const bob::ip::LBP& lbp):
        m_lbp(lbp),
        m_block_h(block_h), m_block_w(block_w), m_overlap_h(overlap_h),
        m_overlap_w(overlap_w), m_lbp_r(lbp.getRadius()), m_lbp_p(lbp.getNNeighbours())
      {
      }

      /**
        * @brief Destructor
        */
      virtual ~LBPHSFeatures() {}

      /**
        * @brief Process a 2D blitz Array/Image by extracting LBPHS features.
        * @param src The 2D input blitz array
        * @param dst A container (with a push_back method such as an STL list)
        *   of 1D uint32_t blitz arrays.
        */
      template <typename T, typename U>
      void operator()(const blitz::Array<T,2>& src, U& dst);

      /**
        * @brief Function which returns the number of blocks when applying
        *   the LBPHSFeatures extractor on a 2D blitz::array/image.
        *   The first dimension is the height (y-axis), whereas the second
        *   one is the width (x-axis).
        * @param src The input blitz array
        */
      template<typename T>
      const int getNBlocks(const blitz::Array<T,2>& src);

      /**
        * @brief Returns the number of bins in each LBP histogram
        */
      inline const uint64_t getNBins() { return m_lbp.getMaxLabel(); }

    private:
      /**
        * Attributes
        */
      bob::ip::LBP m_lbp;
      int m_block_h;
      int m_block_w;
      int m_overlap_h;
      int m_overlap_w;
      double m_lbp_r;
      int m_lbp_p;
  };

  template <typename T, typename U>
  void LBPHSFeatures::operator()(const blitz::Array<T,2>& src,
    U& dst)
  {
    // cast to double
    blitz::Array<double,2> double_version = bob::core::array::cast<double>(src);

    // get all the blocks
    std::list<blitz::Array<double,2> > blocks;
    blockReference(double_version, blocks, m_block_h, m_block_w, m_overlap_h, m_overlap_w);

    // compute an lbp histogram for each block
    for( std::list<blitz::Array<double,2> >::const_iterator it = blocks.begin();
      it != blocks.end(); ++it)
    {
      // extract lbp using operator()
      blitz::Array<uint16_t,2> lbp_tmp_block(m_lbp.getLBPShape(*it));
      m_lbp(*it, lbp_tmp_block);

      // Compute the LBP histogram
      blitz::Array<uint64_t, 1> lbp_histo(m_lbp.getMaxLabel());
      histogram<uint16_t>(lbp_tmp_block, lbp_histo, 0, m_lbp.getMaxLabel()-1,
        m_lbp.getMaxLabel());

      // Push the resulting processed block in the container
      dst.push_back(lbp_histo);
    }
  }

  template<typename T>
  const int LBPHSFeatures::getNBlocks(const blitz::Array<T,2>& src)
  {
    const blitz::TinyVector<int,3> res = getBlock3DOutputShape(src, m_block_h,
      m_block_w, m_overlap_h, m_overlap_w);
    return res(0);
  }


}}

#endif /* BOB_IP_LBPHS_FEATURES_H */
