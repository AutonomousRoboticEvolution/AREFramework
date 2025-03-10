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




#ifndef _NN_FFCPG_HPP_
#define _NN_FFCPG_HPP_

#include "nn.hpp"
#include "neuron.hpp"
#include "af.hpp"
#include "pf.hpp"



namespace nn2 {


template<typename W = double, typename P = int>
struct Pf_ff_cpg : public Pf<W,P> {
    typedef W weight_t;
    void init() {
        _w_cache.resize(this->_weights.size());
        for (size_t i = 0; i < this->_weights.size(); ++i)
            _w_cache[i] = trait<weight_t>::single_value(this->_weights[i]);
    }
    std::vector<double> operator() (const trait<double>::vector_t & inputs) const {
        assert(inputs.size() == _w_cache.size());
        //std::cout<<"in:"<<inputs.transpose()<<" w:"<<_w_cache.transpose()<<"=>"<<
        //_w_cache.dot(inputs)<<std::endl;
        if (inputs.size() == 0)
            return {0.0f,0.0f};

        if(this->_params == 0)// weighted sum
            return {_w_cache.dot(inputs),0.2};
        else// dual inputs
            return {_w_cache[0]*inputs[0],inputs[1]};
    }
protected:
    Eigen::VectorXd _w_cache;
};

template<typename P = int>
struct Af_ff_cpg : public Af<P> {
    typedef P params_t;
    Af_ff_cpg() {
      //assert(this->_params.size() == 2);
    }
    float operator()(std::vector<double> p) const {
      if(this->_params == 0)
        return (1.0 / (exp(-5*(p[1])*(p[0])) + 1) - 0.5f)*2.f;
      else
        return p[0];
    }
   protected:
};



template<>
class Neuron<Pf_ff_cpg<>,Af_ff_cpg<>,double> {
 public:
  typedef typename Pf_ff_cpg<>::weight_t weight_t;
  typedef double io_t;
  typedef Pf_ff_cpg<> pf_t;
  typedef Af_ff_cpg<> af_t;

  static io_t zero() {
    return trait<io_t>::zero();
  }

  Neuron() :
    _current_output(zero()),
    _next_output(zero()),
    _fixed(false),
    _in(-1),
    _out(-1) {
  }

  Neuron(const Neuron& n):
      _af(n._af), _pf(n._pf),
      _color(n._color), _id(n._id),
      _index(n._index), _label(n._label),
      _bias(n._bias),
      _current_output(n._current_output),
      _next_output(n._next_output)

  {}

  bool get_fixed() const {
    return _fixed;
  }
  void set_fixed(bool b = true) {
    _fixed = b;
  }

  io_t activate(double delta) {
    if (!_fixed) {
      _next_output = activate_phantom(_current_output, _inputs, delta);
    }
    return _next_output;
  }

  io_t activate_phantom(io_t current_state, const typename trait<io_t>::vector_t &inputs, double delta) const {
      if (_fixed) return current_state;

      io_t output;
      if(_with_bias)
          output = _af({_pf(inputs)[0] + _bias,_pf(inputs)[1]});
      else output = _af(_pf(inputs));
      if (_differential)
          return current_state + (delta * output);
      else
          return output;
  }

  void init() {
    _pf.init();
    _af.init();
    if (get_in_degree() != 0)
      _inputs = trait<io_t>::zero(get_in_degree());
    _current_output = zero();
    _next_output = zero();
  }

  void set_input(unsigned i, const io_t& in) {
    assert(i < _inputs.size());
    _inputs[i] = in;
  }
  const typename trait<io_t>::vector_t &get_inputs() const {return _inputs;}
  void set_weight(unsigned i, const weight_t& w) {
    _pf.set_weight(i, w);
  }

  typename af_t::params_t& get_afparams() {
    return _af.get_params();
  }
  typename pf_t::params_t& get_pfparams() {
    return _pf.get_params();
  }
  const typename af_t::params_t& get_afparams() const {
    return _af.get_params();
  }
  const typename pf_t::params_t& get_pfparams() const {
    return _pf.get_params();
  }
  void set_afparams(const typename af_t::params_t& p) {
    _af.set_params(p);
  }
  void set_pfparams(const typename pf_t::params_t& p) {
    _pf.set_params(p);
  }

  void set_bias(const io_t& b){
      _bias = b;
  }

  void step() {
    _current_output = _next_output;
  }
  void set_in_degree(unsigned k) {
    _pf.set_nb_weights(k);
    _inputs.resize(k);
    if (k == 0)
      return;
    _inputs = trait<io_t>::zero(k);
  }
  unsigned get_in_degree() const {
    return _pf.get_weights().size();
  }

  // for input neurons
  void set_current_output(const io_t& v) {
    _current_output = v;
  }
  void set_next_output(const io_t& v) {
    _next_output = v;
  }

  // standard output
  const io_t& get_current_output() const {
    return _current_output;
  }

  // next output
  const io_t& get_next_output() const {
    return _next_output;
  }

  // i/o
  int get_in() const {
    return _in;
  }
  void set_in(int i) {
    _in = i;
  }
  int get_out() const {
    return _out;
  }
  void set_out(int o) {
    _out = o;
  }
  bool is_input() const {
    return _in != -1;
  }
  bool is_output() const {
    return _out != -1;
  }

  const pf_t& get_pf() const {
    return _pf;
  }
  pf_t& get_pf() {
    return _pf;
  }

  const af_t& get_af() const {
    return _af;
  }
  af_t& get_af() {
    return _af;
  }

  void set_id(const std::string& s) {
    _id = s;
  }
  const std::string& get_id() const {
    return _id;
  }
  const std::string& get_label() const {
    return _label;
  }

  bool has_differential_activation() const {
      return _differential;
  }
  void set_differential_activation(bool differential) {
      _differential = differential;
  }

  void no_bias(){_with_bias=false;}

  // for graph algorithms
  std::string _id;
  std::string _label;
  boost::default_color_type _color;
  int _index;

 protected:

  // activation functor
  af_t _af;
  // potential functor
  pf_t _pf;
  // bias
  io_t _bias;
  // outputs
  io_t _current_output;
  io_t _next_output;
  // cache
  typename trait<io_t>::vector_t _inputs;
  // fixed = current_output is constant
  bool _fixed;
  // -1 if not an input of the nn, id of input otherwise
  int _in;
  // -1 if not an output of the nn, id of output otherwise
  int _out;
  // true if the neuron should activation should be differential
  bool _differential = false;
  bool _with_bias = true;
};

namespace ffcpg {
typedef nn2::Pf_ff_cpg<> pf_t;
typedef nn2::Af_ff_cpg<> af_t;
typedef nn2::Neuron<pf_t,af_t> neuron_t;
typedef nn2::Connection<> connection_t;
}

  template<typename N, typename C>
/**
   * @brief An implementation of CPG networks with sensory inputs for the ARE framework
   */
  class Ffcpg : public NN<N, C> {
   public:
    typedef NN<N, C> nn_t;
    typedef typename nn_t::io_t io_t;
    typedef typename nn_t::vertex_desc_t vertex_desc_t;
    typedef typename nn_t::edge_desc_t edge_desc_t;
    typedef typename nn_t::adj_it_t adj_it_t;
    typedef typename nn_t::graph_t graph_t;
    typedef N neuron_t;
    typedef C conn_t;

    Ffcpg(){}
    /**
    * @brief CPG
    * @param nb_inputs
    * @param nb_hidden
    * @param nb_outputs
    * @param joint_substrate a self-referencing connection vector.
    * -1 values indicate that they are attached to the head.
    * other values indicate the index that joint is attached to.
    * E.g. [-1,-1,-1,0,1,2] you have the first 3 joints attached to the head and the other connected as following:
    * 3->0  4->1  5->2 Example: {-1,-1,-1,0,1,2}
    *
    *
    *          3  0 |--------| 1  4
    *          --*--|        |__*--
    *               |        |
    *               |        |__*--
    *               |--------|2   5
    */
    Ffcpg(size_t nb_inputs,
        size_t nb_hidden,
        size_t nb_outputs,
          std::vector<int> joint_substrate) {
      std::cout << "Contructing an FF-CPG network" << std::endl;

      this->set_nb_inputs(nb_inputs + 1);
      this->set_nb_outputs(nb_outputs);

      for (size_t i = 0; i < nb_outputs; ++i)
        _hidden_neurons.push_back(this->add_neuron(std::string("h") + boost::lexical_cast<std::string>(i)));

      this->full_connect(this->_inputs,_hidden_neurons, trait<typename N::weight_t>::zero());

      // Create oscillators
      for(int i = 0; i < joint_substrate.size(); i++){
          std::stringstream name1,name2;
          name1 << "A" << i;
          name2 << "B" << i;
          _cpg_outputs.push_back(this->add_neuron(name1.str()));
          _cpg_inputs.push_back(this->add_neuron(name2.str()));
          vertex_desc_t &neuron_a = _cpg_outputs.back(); //output of oscillator
          vertex_desc_t &neuron_b = _cpg_inputs.back();
          //set potential function to a sum and activation function to direct
          this->_g[neuron_a].set_pfparams(0);
          this->_g[neuron_a].set_afparams(1);
          this->_g[neuron_a].no_bias();
          this->_g[neuron_b].set_pfparams(0);
          this->_g[neuron_b].set_afparams(1);
          this->_g[neuron_b].no_bias();
          this->create_oscillator_connection(neuron_a, neuron_b,
                                             trait<typename N::weight_t>::zero());
          this->add_connection(neuron_a,this->_outputs[i],
                               trait<typename N::weight_t>::zero()); // connection to the outputs
      }

      this->connect(_hidden_neurons,this->_outputs,trait<typename N::weight_t>::zero());

      // Connect oscillators with each others
      for(int i = 0; i < joint_substrate.size(); i++){
          if(joint_substrate.at(i) < 0){
              for(int j = 0; j < joint_substrate.size(); j++){
                  if(i==j)
                      continue;
                  if(joint_substrate.at(j) == joint_substrate.at(i)){
                      vertex_desc_t &output = _cpg_outputs.at(i);
                      vertex_desc_t &input = _cpg_inputs.at(j);
                      this->add_connection(output, input, 1.0,true);
                      joint_substrate[j]--;
                  }
              }
          }
          else {
              vertex_desc_t &output = _cpg_outputs.at(i);
              vertex_desc_t &input = _cpg_inputs.at(joint_substrate[i]);
              this->add_connection(output, input, 1.0,true);
          }
      }

      // set neuron as classic: sum and sigmoid
      BOOST_FOREACH(vertex_desc_t v, _hidden_neurons) {
           this->_g[v].set_pfparams(0);//sum
           this->_g[v].set_afparams(0); //sigmoid
      }
      //set neuron as dual input filter
      BOOST_FOREACH(vertex_desc_t v, this->_outputs) {
          this->_g[v].set_pfparams(1);//dual inputs
          this->_g[v].set_afparams(0);//sigmoid
      }


    }
    unsigned get_nb_inputs() const {
      return this->_inputs.size()-1;
    }
    void step(const std::vector<io_t>& in,double delta = 0.0) {
      assert(in.size() == this->get_nb_inputs());
      std::vector<io_t> inf = in;
      inf.push_back(1.0f);
      nn_t::_step_integrate(inf,delta);
    }

    std::vector<double> get_hidden_outf(){
        std::vector<double> outputs(_hidden_neurons.size());
        unsigned i = 0;
        for (typename std::vector<vertex_desc_t>::const_iterator it = _hidden_neurons.begin();
             it != _hidden_neurons.end(); ++it, ++i)
          outputs[i] = this->_g[*it].get_current_output();
        return outputs;
    }

    std::vector<double> get_cpg_outf(){
        std::vector<double> outputs(_hidden_neurons.size());
        unsigned i = 0;
        for (typename std::vector<vertex_desc_t>::const_iterator it = _cpg_outputs.begin();
             it != _cpg_outputs.end(); ++it, ++i)
          outputs[i] = this->_g[*it].get_current_output();
        return outputs;
    }

    void init() override{
        this->_init();
        //using vertex_pair = std::pair<vertex_desc_t,vertex_desc_t>;
        BOOST_FOREACH(vertex_desc_t& v, _cpg_outputs) {
            this->_g[v].set_current_output(0.707);
            this->_g[v].set_next_output(0.707);
        }
        BOOST_FOREACH(vertex_desc_t& v, _cpg_inputs) {
            this->_g[v].set_current_output(-0.707);
            this->_g[v].set_next_output(-0.707);
        }
    }
   protected:
    std::vector<vertex_desc_t> _cpg_inputs; // CPG
    std::vector<vertex_desc_t> _cpg_outputs;
    std::vector<vertex_desc_t> _hidden_neurons;

  };
  namespace ffcpg {
    template<int NbInputs, int NbHidden, int Wheels, int PrimaryJoints, int SecondaryJoints>
    struct Count {
      const int nb_inputs = 0; // bias is an input
      const int nb_outputs =PrimaryJoints + SecondaryJoints;
      const int nb_hidden = 0;
      const int nb_weights = 1/2*PrimaryJoints*(PrimaryJoints - 1) + SecondaryJoints*2 + PrimaryJoints; // connection of the oscillators
      const int nb_biases = (PrimaryJoints+SecondaryJoints)*3;
    };
    struct count_t {
        int nb_inputs;
        int nb_outputs;
        int nb_hidden;
        int nb_weights;
        int nb_biases;
        count_t(int NbInputs,  int NbHidden,int Wheels,int PrimaryJoints, int SecondaryJoints){
            nb_inputs = 0;
            nb_outputs = PrimaryJoints + SecondaryJoints;;
            nb_hidden = 0;
            nb_weights = 1/2*PrimaryJoints*(PrimaryJoints - 1) + SecondaryJoints*2 + PrimaryJoints; // connection of the oscillators
            nb_biases = (PrimaryJoints+SecondaryJoints)*3;
        }

    };
  }
}

#endif
