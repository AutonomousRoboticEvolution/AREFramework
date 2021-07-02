#ifndef NN2CPPNGENOME_HPP
#define NN2CPPNGENOME_HPP

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "nn2/cppn.hpp"
#include "ARE/Genome.h"
#include "ARE/morphology_descriptors.hpp"

namespace are {


struct cppn_params{
    static float _rate_add_neuron;
    static float _rate_del_neuron;
    static float _rate_add_conn;
    static float _rate_del_conn;
    static float _rate_change_conn;

    static size_t _min_nb_neurons;
    static size_t _max_nb_neurons;
    static size_t _min_nb_conns;
    static size_t _max_nb_conns;

    static int nb_inputs;
    static int nb_outputs;
};

using nn2_cppn_t = nn2::CPPN<cppn_params>;

class NN2CPPNGenome : public Genome
{
public:
    typedef std::shared_ptr<NN2CPPNGenome> Ptr;
    typedef std::shared_ptr<const NN2CPPNGenome> ConstPtr;

    NN2CPPNGenome() : Genome(){
        static int static_id = 0;
        _id = static_id++;
        cppn = nn2_cppn_t(cppn_params::nb_inputs,cppn_params::nb_outputs);
        type = "nn2_cppn_genome";
    }
    NN2CPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
        static int static_id = 0;
        _id = static_id++;
        cppn = nn2_cppn_t(cppn_params::nb_inputs,cppn_params::nb_outputs);
        type = "nn2_cppn_genome";
    }
    NN2CPPNGenome(const nn2_cppn_t &nn2_cppn_gen) : cppn(nn2_cppn_gen){
        static int static_id = 0;
        _id = static_id++;
    }
    NN2CPPNGenome(const NN2CPPNGenome &gen) :
        Genome(gen), cppn(gen.cppn), morph_desc(gen.morph_desc){}
    ~NN2CPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<NN2CPPNGenome>(*this);
    }

    void init() override {
        cppn.init();
    }

    void random(){
        cppn.random();
    }

    void mutate() override {
        cppn.mutate();
    }

    const nn2_cppn_t& get_cppn() const {return cppn;}

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

    const int id() const {return _id;}
    const std::vector<int>& get_parents_ids() const {return parents_ids;}
    void set_parents_ids(const std::vector<int>& ids){parents_ids = ids;}

private:
    std::vector<int> parents_ids;
    nn2_cppn_t cppn;
    CartDesc morph_desc;
};


}//are

#endif //NN2CPPNGENOME_HPP

