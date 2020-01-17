#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <ARE/Genome.h>
#include <ARE/Control.h>
#include <ARE/Morphology.h>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/map.hpp>

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
        control(ind.control)
//        createGenome(ind.createGenome)
    {}
    virtual ~Individual();

    virtual Individual::Ptr clone() = 0;

    virtual void init()
    {
        createMorphology();
        createController();
    }

    virtual void update(double delta_time) = 0;
    void mutate()
    {
        morphGenome->mutate();
        ctrlGenome->mutate();
    }

    //Getters & Setters
    const std::vector<double> &get_outputs(){return outputs;}
    const Morphology::Ptr &get_morphology(){return morphology;}
    const Control::Ptr &get_control(){return control;}
    const Genome::Ptr &get_morph_genome(){return morphGenome;}
    const Genome::Ptr &get_ctrl_genome(){return ctrlGenome;}
    void setFitness(double f){fitness = f;}
    double getFitness(){return fitness;}
    int get_individual_id(){return individual_id;}
    void set_individual_id(int i){individual_id = i;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    bool isEvaluated(){return isEval;}
    void set_isEvaluated(bool b){isEval = b;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    void set_client_id(int cid){client_id = cid;}
    int get_client_id(){return client_id;}

    virtual std::string to_string();
    virtual void from_string(const std::string &str);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & fitness;
        arch & ctrlGenome;
        arch & morphGenome;
        arch & isEval;
        arch & individual_id;
        arch & client_id;
    }

protected:
    std::vector<double> outputs;
    double fitness;
    Genome::Ptr morphGenome;
    Genome::Ptr ctrlGenome;
    Morphology::Ptr morphology;
    Control::Ptr control;

    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;

    bool isEval;

    int individual_id; //TODO id system

    int client_id;
//    std::function<Genome::Factory> createGenome;

    virtual void createController() = 0;
    virtual void createMorphology() = 0;
};



}

#endif //INDIVIDUAL_H
