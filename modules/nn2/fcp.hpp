//| This file is a part of the nn2 module originally made for the sferes2 framework.
//| Adapted and modified to be used within the ARE framework by Léni Le Goff.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//|
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.

#ifndef _NN_FCP_HPP_
#define _NN_FCP_HPP_

#include "nn.hpp"
#include "connection.hpp"
#include "neuron.hpp"

namespace nn2 {
  // a basic fully connected perceptrons (feed-forward neural network with no hidden layer)
  // only one hidden layer in this version
  // there's one autmatically added input for the bias
  template<typename N, typename C>
  class Fcp : public NN<N, C> {
   public:
    typedef NN<N, C> nn_t;
    typedef typename nn_t::io_t io_t;
    typedef typename nn_t::vertex_desc_t vertex_desc_t;
    typedef typename nn_t::edge_desc_t edge_desc_t;
    typedef typename nn_t::adj_it_t adj_it_t;
    typedef typename nn_t::graph_t graph_t;
    typedef N neuron_t;
    typedef C conn_t;
    Fcp(){}

    Fcp(size_t nb_inputs,
        size_t nb_hidden,
        size_t nb_outputs) {
        std::cout << "Constructing an FCP" << std::endl;

      // neurons
      this->set_nb_inputs(nb_inputs);
      this->set_nb_outputs(nb_outputs);
     
      // connections
      this->full_connect(this->_inputs, this->_outputs,
                         trait<typename N::weight_t>::zero());

    }
    unsigned get_nb_inputs() const {
      return this->_inputs.size() - 1;
    }
    void step(const std::vector<io_t>& in,double delta = 0.0) {
      assert(in.size() == this->get_nb_inputs());
      std::vector<io_t> inf = in;
      inf.push_back(1.0f);
      nn_t::_step(inf);
    }
   protected:
    std::vector<vertex_desc_t> _hidden_neurons;
  };
  namespace fcp {
    template<int NbInputs, int NbHidden, int NbOutputs>
    struct Count {
      const int nb_inputs = NbInputs; 
      const int nb_outputs = NbOutputs;
      const int nb_weights =
        nb_inputs * nb_outputs; // input to output (full)
      const int nb_biases = NbInputs + NbOutputs;
    };
    struct count_t {
        int nb_inputs;
        int nb_outputs;
        int nb_hidden;
        int nb_weights;
        int nb_biases;
        count_t(int NbInputs,  int NbHidden,int NbOutputs){
            nb_inputs = NbInputs + 1; // bias is an input
            nb_outputs = NbOutputs;
            nb_hidden = NbHidden;
            nb_weights =
                    nb_inputs * nb_outputs; // input to output (full)
            nb_biases = NbInputs + NbHidden + NbOutputs;
        }

    };
  }


}

#endif
