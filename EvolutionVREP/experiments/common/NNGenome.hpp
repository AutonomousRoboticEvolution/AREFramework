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


    NNGenome() : Genome() {}
    NNGenome(misc::RandNum::Ptr rn, settings::ParametersMapPtr param) :
        Genome(rn,param){
    }
    NNGenome(const NNGenome &ngen) :
        Genome(ngen), nn_genome(ngen.nn_genome){}

    Genome::Ptr clone() const override {
        return std::make_shared<NNGenome>(*this);
    }


    void init() override;
    void init(NEAT::RNG &rng);
    Phenotype::Ptr develop() override{}
    void mutate(NEAT::RNG &rng);
    void mutate() override{}
    Genome::Ptr crossover(const Genome::Ptr &);

    const NEAT::Genome& get_nn_genome(){return nn_genome;}

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & nn_genome;
    }

private:
    NEAT::Genome nn_genome;
    NEAT::Parameters neat_parameters;
};

}
#endif //NNGENOME_H
