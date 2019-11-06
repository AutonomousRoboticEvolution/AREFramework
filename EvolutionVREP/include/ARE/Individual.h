#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <ARE/Genome.h>
#include <ARE/Control.h>
#include <ARE/Morphology.h>

namespace are {

class Individual
{
public:
    typedef std::shared_ptr<Individual> Ptr;
    typedef std::shared_ptr<const Individual> ConstPtr;

    Individual(){}
    Individual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    Individual(const Individual& ind) :
        outputs(ind.outputs),
        fitness(ind.fitness),
        morphGenome(ind.morphGenome),
        ctrlGenome(ind.ctrlGenome),
        morphology(ind.morphology),
        control(ind.control),
        createGenome(ind.createGenome)
    {}
    virtual ~Individual();


    virtual void update(const std::vector<double> &inputs,double delta_time);

    //Getters & Setters
    const std::vector<double> &get_outputs(){return outputs;}
    const Morphology::Ptr &get_morphology(){return morphology;}
    const Control::Ptr &get_control(){return control;}
    const Genome::Ptr &get_morph_genome(){return morphGenome;}
    const Genome::Ptr &get_ctrl_genome(){return ctrlGenome;}

protected:
    std::vector<double> outputs;
    double fitness;
    Genome::Ptr morphGenome;
    Genome::Ptr ctrlGenome;
    Morphology::Ptr morphology;
    Control::Ptr control;

    std::function<Genome::Factory> createGenome;

    virtual void createControler() = 0;
    virtual void createMorphology() = 0;

private:
    virtual void init()
    {
        createMorphology();
        createControler();
    }

};

}

#endif //INDIVIDUAL_H
