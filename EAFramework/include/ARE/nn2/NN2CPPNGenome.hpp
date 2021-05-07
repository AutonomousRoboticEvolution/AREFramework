#ifndef NN2CPPNGENOME_HPP
#define NN2CPPNGENOME_HPP

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "nn2/cppn.hpp"

#include "ARE/Genome.h"


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
    }
    NN2CPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
    }
//    NN2CPPNGenome(const NEAT::Genome &neat_gen) : neat_genome(neat_gen){}
    NN2CPPNGenome(const NN2CPPNGenome &gen) :
        Genome(gen), cppn(gen.cppn){}
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

    const nn2_cppn_t& get_cppn(){return cppn;}

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
    }


private:
    nn2_cppn_t cppn;
};


}//are

#endif //NN2CPPNGENOME_HPP

