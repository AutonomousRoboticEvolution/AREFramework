#ifndef NNGENOME_H
#define NNGENOME_H

#include <ARE/Genome.h>
#include <ARE/Phenotype.h>
#include <Genome.h>

namespace are {


class NNGenome : public Genome
{
public:

    typedef std::shared_ptr<NNGenome> Ptr;
    typedef std::shared_ptr<const NNGenome> ConstPtr;


    NNGenome() : Genome() {
        rng.Seed(randomNum->getSeed());
    }
    NNGenome(const NEATGenome &ngen) :
        Genome(ngen), neat_genome(ngen.neat_genome){}

    Genome::Ptr clone() const override {
        return std::make_shared<NNGenome>(*this);
    }


    void init() override;
    Phenotype::Ptr develop() override{}
    void mutate() override;

private:
    NEAT::Genome nn_genome;
    NEAT::Parameters neat_parameters;
    NEAT::RNG rng;
};

}
#endif //NNGENOME_H
