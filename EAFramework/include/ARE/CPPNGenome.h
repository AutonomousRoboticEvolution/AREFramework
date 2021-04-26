#ifndef CPPNGENOME_H
#define CPPNGENOME_H

#include <boost/serialization/export.hpp>
#include "ARE/Genome.h"
#include "ARE/morphology_descriptors.hpp"
#include <multineat/Genome.h>

namespace are {

class CPPNGenome : public Genome
{
public:
    CPPNGenome() : Genome(){}
    CPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){}
    CPPNGenome(const NEAT::Genome &neat_gen) : neat_genome(neat_gen){}
    CPPNGenome(const CPPNGenome &gen) :
        Genome(gen),neat_genome(gen.neat_genome){}
    ~CPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<CPPNGenome>(*this);
    }

    void init() override {}
    void mutate() override {}
    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<CPPNGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<CPPNGenome>();
        oarch << *this;
        return sstream.str();
    }

    const NEAT::Genome &get_neat_genome(){return neat_genome;}

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & neat_genome;
        arch & morpho_desc;
    }

    void set_morpho_desc(const CartDesc& md){morpho_desc = md;}
    const CartDesc &get_morpho_desc(){return morpho_desc;}

private:
    NEAT::Genome neat_genome;
    CartDesc morpho_desc;
};


}

#endif //CPPNGENOME_H

