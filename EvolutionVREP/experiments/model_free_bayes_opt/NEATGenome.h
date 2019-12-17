#ifndef NEATGENOME_H
#define NEATGENOME_H

#include <ARE/Genome.h>
#include <ARE/Phenotype.h>
#include <Genome.h>

namespace are {

struct NN : public Phenotype
{
    typedef std::shared_ptr<NN> Ptr;

    NEAT::NeuralNetwork nn;
};

class NEATGenome : public Genome
{
public:

    typedef std::shared_ptr<NEATGenome> Ptr;
    typedef std::shared_ptr<const NEATGenome> ConstPtr;


    NEATGenome() : Genome() {}
    NEATGenome(const NEATGenome &ngen) :
        Genome(ngen), neat_genome(ngen.neat_genome){}

    Genome::Ptr clone() const override {
        return std::make_shared<NEATGenome>(*this);
    }


    void init() override {}
    Phenotype::Ptr develop() override;
    void mutate() override {}
    NEAT::Genome neat_genome;

};

}
#endif //NEATGENOME_H

