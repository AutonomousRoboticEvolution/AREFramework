#ifndef CPPN_HPP_
#define CPPN_HPP_

#include "nn.hpp"
#include "evo_float.hpp"
#include "random.hpp"
#include <random>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>

using namespace boost;

namespace nn2{

namespace cppn{

struct default_params{
    struct cppn{
        static constexpr int _mutation_type = 0;
        static constexpr bool _mutate_connections = true;
        static constexpr bool _mutate_neurons = true;
        static constexpr double _mutation_rate = 0.5;
        static constexpr double _rate_mutate_conn = 0.1;
        static constexpr double _rate_mutate_neur = 0.1;
        static constexpr double _rate_add_neuron = 0.1;
        static constexpr double _rate_del_neuron = 0.01;
        static constexpr double _rate_add_conn = 0.1;
        static constexpr double _rate_del_conn = 0.01;
        static constexpr double _rate_change_conn = 0.1;
        static constexpr double _rate_crossover = 0.1;

        static constexpr size_t _min_nb_neurons = 0;
        static constexpr size_t _max_nb_neurons = 5;
        static constexpr size_t _min_nb_conns = 10;
        static constexpr size_t _max_nb_conns = 100;
    };
    struct evo_float{
        static constexpr double mutation_rate = 0.1f;
        static constexpr double cross_rate = 0.0f;
        static constexpr nn2::evo_float::mutation_t mutation_type = nn2::evo_float::polynomial;
        static constexpr nn2::evo_float::cross_over_t cross_over_type = nn2::evo_float::no_cross_over;
        static constexpr double eta_m = 15.0f;
        static constexpr double eta_c = 15.0f;
        static constexpr double max = 1;
        static constexpr double min = -1;
    };
};

template<typename Params = default_params>
struct AfParams{
    AfParams(){}
    AfParams(const AfParams& p):
        type(p.type),
        p0(p.p0),
        p1(p.p1)
    {}

    int type = -1;
    nn2::EvoFloat<1,Params> p0;
    nn2::EvoFloat<1,Params> p1;

    void random(){
        type = std::uniform_int_distribution<>(0,2)(rgen_t::gen);
        p0.random();
        p1.random();
    }
    void mutate(){
        p0.mutate();
        p1.mutate();
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & type;
        ar & p0;
        ar & p1;
    }

};
enum{sine = 0, sigmoid, gaussian, cube, polynome, linear};


}//cppn

// Activation function for Compositional Pattern Producing Network
template <typename Params>
struct AfCppn : public Af<Params> {
    typedef AfCppn<Params> this_t;
    typedef Params params_t;
    double operator() (double p) const {
        switch (this->_params.type) {
        case cppn::sine:
            return sin(this->_params.p0.data(0)*p*10 + this->_params.p1.data(0));
        case cppn::sigmoid:
            return ((1.0 / (1 + exp(-this->_params.p0.data(0)*p*10 + this->_params.p1.data(0)))) - 0.5) * 2;
        case cppn::gaussian:
            return exp(-fabs(this->_params.p0.data(0)*10)*powf(p+this->_params.p1.data(0), 2));
       // case cppn::linear:
       //     return std::min(std::max(this->_params.p0.data(0)*p+this->_params.p1.data(0), -3.0), 3.0) / 3.0;
        // case cppn::cube:
        //     return (p+this->_params.p0.data(0))*(p+this->_params.p0.data(0))*(p+this->_params.p0.data(0)) + this->_params.p1.data(0);
        // case cppn::polynome:
        //     return this->_params.p0.data(0)*p*p*p*p + this->_params.p1.data(0)*p*p*p;
        default:
            assert(0);
        }
        return 0;
    }


    friend std::ostream &operator <<(std::ostream& ostr, this_t& obj){
        if(obj._params.type == cppn::sine)
            ostr << "sin_" <<  obj._params.p0.data(0) << "_" << obj._params.p1.data(0);
        else if(obj._params.type == cppn::sigmoid)
            ostr << "sigm_" <<  obj._params.p0.data(0) << "_" << obj._params.p1.data(0);
        else if(obj._params.type == cppn::gaussian)
            ostr << "gaus_" <<  obj._params.p0.data(0) << "_" << obj._params.p1.data(0);
        else if(obj._params.type == cppn::linear)
            ostr << "lin_" << obj._params.p0.data(0) << "_" << obj._params.p1.data(0);
        return ostr;
    }

    void set_params(const params_t& p){
        this->_params.type = p.type;
        this->_params.p0 = p.p0;
        this->_params.p1 = p.p1;
    }
};

template <typename N, typename C, typename Params>
class CPPN : public NN<N, C>
{
public:
    typedef NN<N, C> nn_t;
    typedef N neuron_t;
    typedef C conn_t;
    typedef typename nn_t::io_t io_t;
    typedef typename nn_t::weight_t weight_t;
    typedef typename nn_t::vertex_desc_t vertex_desc_t;
    typedef typename nn_t::edge_desc_t edge_desc_t;
    typedef typename nn_t::adj_it_t adj_it_t;
    typedef typename nn_t::graph_t graph_t;
    typedef NN<N,C> this_t;


    CPPN(){}
    CPPN(size_t nb_inputs, size_t nb_outputs) :
        _nb_inputs(nb_inputs), _nb_outputs(nb_outputs)
    {}
    CPPN(const CPPN<N,C,Params> &cppn) :
        nn_t(cppn), _nb_inputs(cppn._nb_inputs), _nb_outputs(cppn._nb_outputs){}

    void _random_neuron_params() {
        BGL_FORALL_VERTICES_T(v, this->_g, graph_t) {
            this->_g[v].get_pfparams().random();
            this->_g[v].get_afparams().random();
        }
    }

    void init() override{
        this->_init();
        this->set_all_biases(std::vector<double>(this->get_nb_neurons(),0));
    }

    void build_fixed_structure(){
        // io
        this->set_nb_inputs(_nb_inputs);
        this->set_nb_outputs(_nb_outputs);

        for(const vertex_desc_t &v : this->_outputs){
            cppn::AfParams<Params> af_params;
            af_params.type = 3;
            af_params.p0.random();
            af_params.p1.random();
            this->_g[v].set_afparams(af_params);
            this->_g[v].get_pfparams().random();
        }

        //create the hidden layer with 4 neurons one for each activation function type and random parameters.
        std::vector<vertex_desc_t> hidden_neurons;
        for(size_t i = 0; i < 4; ++i){
            hidden_neurons.push_back(this->add_neuron("n"));
            cppn::AfParams<Params> af_params;
            af_params.type = i;
            af_params.p0.random();
            af_params.p1.random();
            this->_g[hidden_neurons.back()].set_afparams(af_params);
            this->_g[hidden_neurons.back()].get_pfparams().random();
        }

        //fully connect input to hidden and hidden to input
        this->full_connect(this->_inputs,hidden_neurons,trait<typename N::weight_t>::zero());
        this->full_connect(hidden_neurons,this->_outputs,trait<typename N::weight_t>::zero());

        //interconnect all hidden neurons;
        for(vertex_desc_t &v : hidden_neurons)
            for(vertex_desc_t &w : hidden_neurons)
                if(v != w)
                    this->add_connection(v,w,trait<typename N::weight_t>::zero());

        //Start with random weights
        BGL_FORALL_EDGES_T(e, this->_g, graph_t)
                this->_g[e].get_weight().random();
    }

    void random(){
        // io
        this->set_nb_inputs(_nb_inputs);
        this->set_nb_outputs(_nb_outputs);
        _random_neuron_params();

        // neurons
        size_t nb_neurons = std::uniform_int_distribution<>(Params::cppn::_min_nb_neurons, Params::cppn::_max_nb_neurons)(rgen_t::gen);
        for (size_t i = 0; i < nb_neurons; ++i)
            _add_neuron();//also call the random params

        // conns
        size_t nb_conns = std::uniform_int_distribution<>(Params::cppn::_min_nb_conns, Params::cppn::_max_nb_conns)(rgen_t::gen);
        for (size_t i = 0; i < nb_conns; ++i)
            _add_conn_nodup();
        this->simplify();
        this->set_all_biases(std::vector<double>(this->get_nb_neurons(),0));
    }

    void crossover(const this_t& parent, this_t& offspring){
        offspring = *this;

        std::vector<int> degrees1;
        std::vector<vertex_desc_t> v_descs1;
        BGL_FORALL_VERTICES_T(v,this->_g,graph_t){
            degrees1.push_back(degree(v,this->_g));
            v_descs1.push_back(v);
        }
        std::vector<int> degrees2;
        std::vector<vertex_desc_t> v_descs2;
        BGL_FORALL_VERTICES_T(v,parent.get_graph(),graph_t){
            degrees2.push_back(degree(v,parent.get_graph()));
            v_descs2.push_back(v);
        }

        std::vector<std::pair<vertex_desc_t,vertex_desc_t>> candidates;
        for(int i = 0; i < degrees1.size(); i++){
            for(int j = 0; j < degrees2.size(); j++)
            if(degrees1[i] == degrees2[j])
                candidates.push_back(std::make_pair(v_descs1[i],v_descs2[j]));
        }

        if(candidates.empty()){
            return;
        }
        std::uniform_real_distribution<> dist(0,1);

        for(const auto& c : candidates){
            if(dist(rgen_t::gen) < Params::cppn::_rate_crossover)
                offspring.get_graph()[c.first].get_af() = parent.get_graph()[c.second].get_af();
        }
    }

    void mutate(){
        if(Params::cppn::_mutation_type != 1)
            mutate_uniform();
        else if(Params::cppn::_mutation_type == 1)
            mutate_roulette_wheel();

    }

    void mutate_uniform(){
        if(Params::cppn::_mutate_connections)
            _change_connections_noio();
        if(Params::cppn::_mutate_neurons)
            _change_neurons();


        std::uniform_real_distribution<> dist(0,1);

        if (dist(rgen_t::gen) < Params::cppn::_rate_add_conn)
            _add_conn_nodup();

        if (dist(rgen_t::gen) < Params::cppn::_rate_del_conn)
            _del_conn();

        if (dist(rgen_t::gen) < Params::cppn::_rate_add_neuron)
            _add_neuron_on_conn();

        if (dist(rgen_t::gen) < Params::cppn::_rate_del_neuron)
            _del_neuron();
    }

    void mutate_roulette_wheel(){

        std::uniform_real_distribution<> dist_0(0,1);
        if(Params::cppn::_mutation_rate < dist_0(rgen_t::gen))
            return;


        auto sum = [](std::vector<float> v, int max) -> double{
            double res = 0;
            for(int i = 0; i < max; i++)
                res += v[i];
            return res;
        };

        std::vector<float> prob_v = {Params::cppn::_rate_mutate_conn,
                Params::cppn::_rate_change_conn,
                Params::cppn::_rate_mutate_neur,
                Params::cppn::_rate_add_conn,
                Params::cppn::_rate_del_conn,
                Params::cppn::_rate_add_neuron,
                Params::cppn::_rate_del_neuron};


        std::uniform_real_distribution<> dist(0,sum(prob_v,prob_v.size()));
        double choice = dist(rgen_t::gen);

        if(choice <= prob_v[0])
            BGL_FORALL_EDGES_T(e, this->_g, graph_t)
                    this->_g[e].get_weight().mutate();
        else if(prob_v[0] < choice && choice <= sum(prob_v,2))
            _change_connections();
        else if(sum(prob_v,2) < choice && choice <= sum(prob_v,3))
            _change_neurons();
        else if(sum(prob_v,3) < choice && choice <= sum(prob_v,4))
            _add_conn_nodup();
        else if(sum(prob_v,4) < choice && choice <= sum(prob_v,5))
            _del_conn();
        else if(sum(prob_v,5) < choice && choice <= sum(prob_v,6))
            _add_neuron_on_conn();
        else if(sum(prob_v,6) < choice && choice <= sum(prob_v,7))
            _del_neuron();

    }

    // serialize the graph "by hand"...
    template<typename Archive>
    void save(Archive& a, const unsigned v) const {
        //            dbg::trace("cppn", DBG_HERE);
        std::vector<int> inputs;
        std::vector<int> outputs;
        std::vector<typename neuron_t::af_t::params_t> afparams;
        std::vector<typename neuron_t::pf_t::params_t> pfparams;
        std::map<vertex_desc_t, int> nmap;
        std::vector<std::pair<int, int> > conns;
        std::vector<weight_t> weights;

        BGL_FORALL_VERTICES_T(v, this->_g, graph_t) {
            if (this->is_input(v))
                inputs.push_back(afparams.size());
            if (this->is_output(v))
                outputs.push_back(afparams.size());
            nmap[v] = afparams.size();
            afparams.push_back(this->_g[v].get_afparams());
            pfparams.push_back(this->_g[v].get_pfparams());
        }
        BGL_FORALL_EDGES_T(e, this->_g, graph_t) {
            conns.push_back(std::make_pair(nmap[source(e, this->_g)],
                            nmap[target(e, this->_g)]));
            weights.push_back(this->_g[e].get_weight());
        }
        assert(pfparams.size() == afparams.size());
        assert(weights.size() == conns.size());

        a & afparams;
        a & pfparams;
        a & weights;
        a & conns;
        a & inputs;
        a & outputs;
        a & _nb_inputs;
        a & _nb_outputs;
    }

    template<typename Archive>
    void load(Archive& a, const unsigned v) {
        //                dbg::trace("nn", DBG_HERE);
        std::vector<int> inputs;
        std::vector<int> outputs;
        std::vector<typename neuron_t::af_t::params_t> afparams;
        std::vector<typename neuron_t::pf_t::params_t> pfparams;
        std::map<size_t, vertex_desc_t> nmap;
        std::vector<std::pair<int, int> > conns;
        std::vector<weight_t> weights;

        a & afparams;
        a & pfparams;
        a & weights;
        a & conns;
        a & inputs;
        a & outputs;
        a & _nb_inputs;
        a & _nb_outputs;

        assert(pfparams.size() == afparams.size());

        assert(weights.size() == conns.size());
        this->set_nb_inputs(inputs.size());
        this->set_nb_outputs(outputs.size());
        for (size_t i = 0; i < this->get_nb_inputs(); ++i)
            nmap[inputs[i]] = this->get_input(i);
        for (size_t i = 0; i < this->get_nb_outputs(); ++i)
            nmap[outputs[i]] = this->get_output(i);

        for (size_t i = 0; i < afparams.size(); ++i)
            if (std::find(inputs.begin(), inputs.end(), i) == inputs.end()
                    && std::find(outputs.begin(), outputs.end(), i) == outputs.end())
                nmap[i] = this->add_neuron("n", pfparams[i], afparams[i]);
            else {
                this->_g[nmap[i]].set_pfparams(pfparams[i]);
                this->_g[nmap[i]].set_afparams(afparams[i]);
            }


        //assert(nmap.size() == num_vertices(this->_g));
        for (size_t i = 0; i < conns.size(); ++i)
            this->add_connection(nmap[conns[i].first], nmap[conns[i].second], weights[i]);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER();

    void write_dot(std::ostream& ofs){
        ofs << "digraph G {" << std::endl;
        BGL_FORALL_VERTICES_T(v, this->_g, graph_t) {
          ofs << this->_g[v].get_id();
          if (this->is_input(v))
              ofs << " [label=\"i_"<<this->_g[v].get_id()<<"\",layer=0]" << std::endl;
          else if(this->is_output(v))
              ofs << " [label=\"o_"<<this->_g[v].get_id()<<"\",layer=2]" << std::endl;
          else
              ofs << " [label=\"" << this->_g[v].get_af() <<"\",layer=1]" << std::endl;
        }
        BGL_FORALL_EDGES_T(e, this->_g, graph_t) {
          ofs << this->_g[source(e, this->_g)].get_id()
              << " -> " << this->_g[target(e, this->_g)].get_id();
          ofs << "[label=\"" <<  this->_g[e].get_weight() << "\"]" << std::endl;
        }
        ofs << "}" << std::endl;
    }

    void set_outputs_fixed(){
        BOOST_FOREACH(vertex_desc_t v, this->_outputs) {
          this->_g[v].set_fixed();
        }
    }

private:

    template <class Graph>
    typename boost::graph_traits<Graph>::vertex_descriptor
    random_vertex(Graph& g) {
        assert(num_vertices(g));
        int nbv = num_vertices(g);
        using namespace boost;
        if (num_vertices(g) > 1) {
            std::size_t n = std::uniform_int_distribution<>(0,num_vertices(g)-1)(rgen_t::gen);
            typename graph_traits<Graph>::vertex_iterator i = vertices(g).first;
            while (n-- > 0) ++i;
            return *i;
        } else
            return *vertices(g).first;
    }

    template <class Graph>
    typename boost::graph_traits<Graph>::edge_descriptor
    random_edge(Graph& g) {
        assert(num_edges(g));
        using namespace boost;
        if (num_edges(g) > 1) {
            std::size_t n = std::uniform_int_distribution<>(0,num_edges(g)-1)(rgen_t::gen);
            typename graph_traits<Graph>::edge_iterator i = edges(g).first;
            while (n-- > 0) ++i;
            return *i;
        } else
            return *edges(g).first;
    }


    vertex_desc_t _random_src(){
        vertex_desc_t v;
        do
            v = random_vertex(this->_g);
        while (this->is_output(v));
        return v;
    }

    vertex_desc_t _random_tgt(){
        vertex_desc_t v;
        do
            v = random_vertex(this->_g);
        while (this->is_input(v));
        return v;
    }
public:
    void _add_neuron(){
        vertex_desc_t n = this->add_neuron("n");
        this->_g[n].get_pfparams().random();
        this->_g[n].get_afparams().random();
    }

    void _add_neuron_on_conn(){
        if (!num_edges(this->_g))
            return;
        edge_desc_t e = random_edge(this->_g);
        vertex_desc_t src = source(e, this->_g);
        vertex_desc_t tgt = target(e, this->_g);
        weight_t w = this->_g[e].get_weight();
        vertex_desc_t n = this->add_neuron("n");
        this->_g[n].get_pfparams().random();
        this->_g[n].get_afparams().random();
        //
        remove_edge(e, this->_g);
        this->add_connection(src, n, w);// todo : find a kind of 1 ??
        this->add_connection(n, tgt, w);
    }

    void _del_neuron(){
        assert(num_vertices(this->_g));

        if (this->get_nb_neurons() <= this->get_nb_inputs() + this->get_nb_outputs())
            return;
        vertex_desc_t v;
        do
            v = random_vertex(this->_g);
        while (this->is_output(v) || this->is_input(v));

        clear_vertex(v, this->_g);
        remove_vertex(v, this->_g);
    }



    void _add_conn(){
        vertex_desc_t src, tgt;
        size_t max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                nb_tries = 0;
        do {
            src = _random_src();
            tgt = _random_tgt();
        } while (src == tgt && ++nb_tries < max_tries);
        weight_t w;
        w.random();
        this->add_connection(src, tgt, w);
    }

    void _add_conn_nodup(){
        vertex_desc_t src, tgt;
        // this is only an upper bound; a connection might of course
        // be possible even after max_tries tries.
        size_t max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                nb_tries = 0;
        do {
            src = _random_src();
            tgt = _random_tgt();
        } while (src == tgt && is_adjacent(this->_g, src, tgt) && ++nb_tries < max_tries);
        if (nb_tries < max_tries) {
            weight_t w;
            w.random();
            this->add_connection(src, tgt, w);
        }
    }

    void _add_conn_nodup_noio(){
        vertex_desc_t src, tgt;
        // this is only an upper bound; a connection might of course
        // be possible even after max_tries tries.
        size_t max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                nb_tries = 0;
        do {
            src = _random_src();
            tgt = _random_tgt();
        } while (src == tgt && is_adjacent(this->_g, src, tgt) && this->is_input(src) && this->is_output(tgt) && ++nb_tries < max_tries);
        if (nb_tries < max_tries) {
            weight_t w;
            w.random();
            this->add_connection(src, tgt, w);
        }
    }


    void _del_conn(){
        if (!this->get_nb_connections())
            return;
        remove_edge(random_edge(this->_g), this->_g);
    }

    void _change_neurons(){
        BGL_FORALL_VERTICES_T(v, this->_g, graph_t) {
            this->_g[v].get_afparams().mutate();
            this->_g[v].get_pfparams().mutate();
        }
    }

    void _change_connections(){
        BGL_FORALL_EDGES_T(e, this->_g, graph_t)
            if (std::uniform_real_distribution<>(0,1)(rgen_t::gen) < Params::cppn::_rate_change_conn) {
                vertex_desc_t src = source(e, this->_g);
                vertex_desc_t tgt = target(e, this->_g);
                weight_t w = this->_g[e].get_weight();
                remove_edge(e, this->_g);
                int max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                nb_tries = 0;
                if (std::uniform_int_distribution<>(0,1)(rgen_t::gen))
                    do
                        src = _random_src();
                    while(++nb_tries < max_tries && is_adjacent(this->_g, src, tgt));
                else
                    do
                        tgt = _random_tgt();
                    while(++nb_tries < max_tries && is_adjacent(this->_g, src, tgt));
                if (nb_tries < max_tries)
                    this->add_connection(src, tgt, w);
                return;
            }
    }

    void _change_connections_noio(){
        BGL_FORALL_EDGES_T(e, this->_g, graph_t)
                this->_g[e].get_weight().mutate();

        BGL_FORALL_EDGES_T(e, this->_g, graph_t)
            if (std::uniform_real_distribution<>(0,1)(rgen_t::gen) < Params::cppn::_rate_change_conn) {
                vertex_desc_t src = source(e, this->_g);
                vertex_desc_t tgt = target(e, this->_g);
                weight_t w = this->_g[e].get_weight();
                remove_edge(e, this->_g);
                int max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                nb_tries = 0;
                if (std::uniform_int_distribution<>(0,1)(rgen_t::gen))
                    do
                        src = _random_src();
                    while(++nb_tries < max_tries && is_adjacent(this->_g, src, tgt) && this->is_input(src) && this->is_output(tgt));
                else
                    do
                        tgt = _random_tgt();
                    while(++nb_tries < max_tries && is_adjacent(this->_g, src, tgt) && this->is_input(src) && this->is_output(tgt));
                if (nb_tries < max_tries)
                    this->add_connection(src, tgt, w);
                return;
            }
    }

private:
    size_t _nb_inputs = 2;
    size_t _nb_outputs = 1;


};

typedef CPPN<Neuron<PfWSum<EvoFloat<1,cppn::default_params>>,AfCppn<cppn::AfParams<cppn::default_params>>>,
Connection<EvoFloat<1,cppn::default_params>>,cppn::default_params> default_cppn_t;


}//nn2

#endif //CPPN_HPP_
