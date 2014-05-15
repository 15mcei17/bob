/**
 * @date 2012-03-05
 * @author Manuel Guenther <Manuel.Guenther@idiap.ch>
 *
 * @brief Representations of images as graphs labeled with Gabor jets.
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_MACHINE_GABOR_GRAPH_MACHINE_H
#define BOB_MACHINE_GABOR_GRAPH_MACHINE_H

#include <bob/ip/GaborWaveletTransform.h>
#include <bob/machine/GaborJetSimilarities.h>
#include <bob/io/HDF5File.h>

#include <sstream>

namespace bob{ namespace machine {
  /**
   * @ingroup MACHINE
   * @{
   */

  //! \brief This machine computes graphs labeled with Gabor jets (so-called Gabor graphs) from a Gabor jet image.
  //! Currently, only the extraction of grid-like graph structures is supported.
  //! Furthermore, this class provides functionalities to compare two Gabor graphs (of the same topology) using a specified Gabor jet similarity function.
  class GaborGraphMachine {
    public:
      //! \brief Default constructor that should be used only to call "average" or
      //! one of the similarity functions
      GaborGraphMachine(){}

      //! creates a face grid graph using two reference positions, namely, the eyes
      GaborGraphMachine(
        blitz::TinyVector<int,2> lefteye,
        blitz::TinyVector<int,2> righteye,
        int between,
        int along,
        int above,
        int below
      );

      //! creates a regular grid graph with specified first and last position, and the step size between two nodes
      GaborGraphMachine(
        blitz::TinyVector<int,2> first,
        blitz::TinyVector<int,2> last,
        blitz::TinyVector<int,2> step
      );

      //! Copy constructor
      GaborGraphMachine(const GaborGraphMachine& other);

      //! Assignment operator
      GaborGraphMachine& operator =(const GaborGraphMachine& other);

      //! Equality operator
      bool operator ==(const GaborGraphMachine& other) const;

      //! returns the number of nodes of this graph
      int numberOfNodes() const { return m_node_positions.extent(0);}

      //! Returns the generated node positions (in the usual order (y,x))
      blitz::Array<int,2> nodes() const {return m_node_positions;}

      //! extracts the Gabor jets of the graph from the jet image
      void extract(
        const blitz::Array<double,4>& jet_image,
        blitz::Array<double,3>& graph_jets
      ) const;

      //! extracts the Gabor jets (abs part only) of the graph from the jet image
      void extract(
        const blitz::Array<double,3>& jet_image,
        blitz::Array<double,2>& graph_jets
      ) const;

      //! averages multiple Gabor graphs into one
      void average(
        const blitz::Array<double,4>& many_graph_jets,
        blitz::Array<double,3>& averaged_graph_jets
      ) const;

      //! computes the similarity of two Gabor graphs
      double similarity(
        const blitz::Array<double,2>& model_graph_jets,
        const blitz::Array<double,2>& probe_graph_jets,
        const bob::machine::GaborJetSimilarity& jet_similarity_function
      ) const;

      //! computes the similarity of two Gabor graphs
      double similarity(
        const blitz::Array<double,3>& model_graph_jets,
        const blitz::Array<double,3>& probe_graph_jets,
        const bob::machine::GaborJetSimilarity& jet_similarity_function
      ) const;

      //! computes the similarity of a set of graphs to another graph
      double similarity(
        const blitz::Array<double,3>& many_model_graph_jets,
        const blitz::Array<double,2>& probe_graph_jets,
        const bob::machine::GaborJetSimilarity& jet_similarity_function
      ) const;

      //! computes the similarity of a set of graphs to another graph
      double similarity(
        const blitz::Array<double,4>& many_model_graph_jets,
        const blitz::Array<double,3>& probe_graph_jets,
        const bob::machine::GaborJetSimilarity& jet_similarity_function
      ) const;

      //! saves this machine to file
      void save(bob::io::HDF5File& file) const;

      //! loads this machine from file
      void load(bob::io::HDF5File& file);

    private:
      void checkPositions(int height, int width) const throw();

      // The node positions of the graph
      blitz::Array<int,2> m_node_positions;

      // temporary complex vector of Gabor jet averages
      mutable blitz::Array<std::complex<double>,1> m_averages;
  };

  /**
   * @}
   */
} }
#endif // BOB_MACHINE_GABOR_GRAPH_MACHINE_H
