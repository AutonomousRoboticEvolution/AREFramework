#ifndef NN2CPPNGENOME_HPP
#define NN2CPPNGENOME_HPP

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "nn2/cppn.hpp"
#include "ARE/Genome.h"
#include "ARE/morphology_descriptors.hpp"
#include "ARE/Logging.h"

namespace are {

struct cppn_params{
    struct cppn{
        static int _mutation_type;
        static bool _mutate_connections;
        static bool _mutate_neurons;
        static float _mutation_rate;
        static float _rate_mutate_conn;
        static float _rate_mutate_neur;
        static float _rate_add_neuron;
        static float _rate_del_neuron;
        static float _rate_add_conn;
        static float _rate_del_conn;
        static float _rate_change_conn;
        static float _rate_crossover;

        static size_t _min_nb_neurons;
        static size_t _max_nb_neurons;
        static size_t _min_nb_conns;
        static size_t _max_nb_conns;

        static int nb_inputs;
        static int nb_outputs;
    };
    struct evo_float{
        static float mutation_rate;
        static constexpr float cross_rate = 0.f;
        static constexpr nn2::evo_float::mutation_t mutation_type = nn2::evo_float::polynomial;
        static constexpr nn2::evo_float::cross_over_t cross_over_type = nn2::evo_float::no_cross_over;
        static constexpr float eta_m = 15.f;
        static constexpr float eta_c = 15.f;
    };
};


namespace cppn {
using af_t = nn2::AfCppn<nn2::cppn::AfParams<cppn_params>>;
using pf_t = nn2::PfWSum<nn2::EvoFloat<1,cppn_params>>;
using neuron_t = nn2::Neuron<pf_t,af_t>;
using connection_t = nn2::Connection<nn2::EvoFloat<1,cppn_params>>;
}

using nn2_cppn_t = nn2::CPPN<cppn::neuron_t,cppn::connection_t,cppn_params>;

static int static_id;

class NN2CPPNGenome : public Genome
{
public:


    typedef std::shared_ptr<NN2CPPNGenome> Ptr;
    typedef std::shared_ptr<const NN2CPPNGenome> ConstPtr;

    NN2CPPNGenome() : Genome(){
        _id = static_id++;
        cppn = nn2_cppn_t(cppn_params::cppn::nb_inputs,cppn_params::cppn::nb_outputs);
        type = "nn2_cppn_genome";
        parents_ids= std::vector<int>(2,-1);
    }
    NN2CPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
        _id = static_id++;
        cppn = nn2_cppn_t(cppn_params::cppn::nb_inputs,cppn_params::cppn::nb_outputs);
        type = "nn2_cppn_genome";
        parents_ids= std::vector<int>(2,-1);

    }
    NN2CPPNGenome(const nn2_cppn_t &nn2_cppn_gen) : cppn(nn2_cppn_gen){
        _id = static_id++;
    }
    NN2CPPNGenome(const NN2CPPNGenome &gen) :
        Genome(gen), cppn(gen.cppn), morph_desc(gen.morph_desc), parents_ids(gen.parents_ids){
    }
    ~NN2CPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<NN2CPPNGenome>(*this);
    }

    void init() override {
        cppn.init();
    }

    void fixed_structure(){
        cppn.build_fixed_structure();
    }

    void random(){
        cppn.random();
    }

    void mutate() override {
        cppn.mutate();
    }

    void crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override {
        are::nn2_cppn_t cppn_child1, cppn_child2;
        are::nn2_cppn_t cppn_partner = std::dynamic_pointer_cast<NN2CPPNGenome>(partner)->get_cppn();
        cppn.crossover(cppn_partner,cppn_child1);
        cppn_partner.crossover(cppn,cppn_child2);
        *child1 = NN2CPPNGenome(cppn_child1);
        *child2 = NN2CPPNGenome(cppn_child2);
        child1->set_parameters(parameters);
        child2->set_parameters(parameters);
        child1->set_randNum(randomNum);
        child2->set_randNum(randomNum);
        std::dynamic_pointer_cast<NN2CPPNGenome>(child1)->set_parents_ids({_id,partner->id()});
        std::dynamic_pointer_cast<NN2CPPNGenome>(child2)->set_parents_ids({_id,partner->id()});
    }


    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<NN2CPPNGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<NN2CPPNGenome>();
        oarch << *this;
        return sstream.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & cppn;
        arch & morph_desc;
        arch & parents_ids;
    }

    const CartDesc& get_morph_desc() const {return morph_desc;}
    void set_morph_desc(const CartDesc& md){morph_desc = md;}

    const std::vector<int>& get_parents_ids() const {return parents_ids;}
    void set_parents_ids(const std::vector<int>& ids){parents_ids = ids;}
    void set_cppn(const nn2_cppn_t &c){cppn = c;}
    const nn2_cppn_t& get_cppn() const {return cppn;}

    int get_nb_neurons(){return cppn.get_nb_neurons();}
    int get_nb_connections(){return cppn.get_nb_connections();}

    int get_generation(){return generation;}
    void incr_generation(){generation++;}

    void set_randNum(const misc::RandNum::Ptr& rn) override{
        randomNum = rn;
        nn2::rgen_t::gen.seed(randomNum->getSeed());
    }

private:
    std::vector<int> parents_ids;
    nn2_cppn_t cppn;
    CartDesc morph_desc;
    int generation;
};

namespace nn2_cppn {

class NN2CPPNGenomeLog : public Logging
{
public:
    NN2CPPNGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class NbrConnNeurLog : public Logging
{
public:
    NbrConnNeurLog(const std::string& filename) : Logging(filename, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class ParentingLog : public Logging
{
public:
    ParentingLog(const std::string& filename) : Logging(filename, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class GraphVizLog : public Logging
{
public:
    GraphVizLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

}//nn2_cppn
}//are

#endif //NN2CPPNGENOME_HPP

