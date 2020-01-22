#ifndef CPPNGENOME_H
#define CPPNGENOME_H

#include <boost/serialization/export.hpp>
#include "ARE/Genome.h"
#include <multineat/Genome.h>

namespace are {

struct CPPN : public Phenotype
{
    NEAT::NeuralNetwork nn;
};

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
    Phenotype::Ptr develop() override;
    void mutate() override {}

    const NEAT::Genome &get_neat_genome(){return neat_genome;}

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & neat_genome;
    }

private:
    NEAT::Genome neat_genome;
};


}

#endif //CPPNGENOME_H

