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

#ifndef _NN_RNN_HPP_
#define _NN_RNN_HPP_

#include "nn.hpp"
#include "connection.hpp"
#include "neuron.hpp"

namespace nn2 {
  // a fully connected recurrent neural network
  // only one hidden layer in this version
  // there's one autmatically added input for the bias
  template<typename N, typename C>
  class Rnn : public NN<N, C> {
   public:
    typedef NN<N, C> nn_t;
    typedef typename nn_t::io_t io_t;
    typedef typename nn_t::vertex_desc_t vertex_desc_t;
    typedef typename nn_t::edge_desc_t edge_desc_t;
    typedef typename nn_t::adj_it_t adj_it_t;
    typedef typename nn_t::graph_t graph_t;
    typedef N neuron_t;
    typedef C conn_t;
    Rnn(){}

    Rnn(size_t nb_inputs,
        size_t nb_hidden,
        size_t nb_outputs) {
        std::cout << "Constructing a fully connected RNN" << std::endl;

      // neurons
      this->set_nb_inputs(nb_inputs + 1);
      this->set_nb_outputs(nb_outputs);
      for (size_t i = 0; i < nb_hidden; ++i)
        _hidden_neurons.
        push_back(this->add_neuron(std::string("h") + boost::lexical_cast<std::string>(i)));

      // connections : all neurons connected to each others apart from outputs not connected to inputs.
      this->full_connect(this->_inputs, this->_hidden_neurons,
                         trait<typename N::weight_t>::zero());
      this->full_connect(this->_hidden_neurons, this->_outputs,
                         trait<typename N::weight_t>::zero());
      this->full_connect(this->_hidden_neurons,this->_hidden_neurons,
                         trait<typename N::weight_t>::zero());
      this->full_connect(this->_inputs,this->_outputs,
                         trait<typename N::weight_t>::zero());
      this->full_connect(this->_inputs,this->_inputs,
                         trait<typename N::weight_t>::zero());
      this->full_connect(this->_outputs,this->_outputs,
                         trait<typename N::weight_t>::zero());


      // bias outputs too
      for (size_t i = 0; i < nb_outputs; ++i)
        this->add_connection(this->get_input(nb_inputs), this->get_output(i),
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

  namespace rnn {
    template<int NbInputs, int NbHidden, int NbOutputs>
    struct Count {
      const int nb_inputs = NbInputs + 1; // bias is an input
      const int nb_outputs = NbOutputs;
      const int nb_hidden = NbHidden;
      const int nb_weights =
        nb_inputs * nb_hidden // input to hidden (full)
        + nb_hidden * nb_outputs // hidden to output (full)
        + nb_hidden // hidden to context (1-1)
        + nb_hidden // context to itself (1-1)
        + nb_hidden * nb_hidden // context to hidden (full)
        + nb_hidden // bias context
        + nb_outputs; // bias outputs
      const int nb_biases = NbInputs + NbHidden + NbOutputs;

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
                    nb_inputs * nb_hidden // input to hidden (full)
                    + nb_hidden * nb_outputs // hidden to output (full)
                    + nb_hidden // hidden to context (1-1)
                    + nb_hidden // context to itself (1-1)
                    + nb_hidden * nb_hidden // context to hidden (full)
                    + nb_hidden // bias context
                    + nb_outputs; // bias outputs
            nb_biases = NbInputs + NbHidden + NbOutputs;
        }

    };
  }

  // a basic Rnn with double weights
  //typedef Rnn<Neuron<PfWSum<>, AfSigmoid<> >, Connection<> > rnn_t;

}

#endif
