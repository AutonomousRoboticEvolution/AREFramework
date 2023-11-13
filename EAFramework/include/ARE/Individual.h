#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <ARE/Genome.h>
#include <ARE/Control.h>
#include <ARE/Morphology.hpp>
#include <ARE/Learner.h>

#include <eigen3/Eigen/Core>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

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
        objectives(ind.objectives),
        morphology(ind.morphology),
        control(ind.control),
        learner(ind.learner),
        parameters(ind.parameters),
        randNum(ind.randNum),
        individual_id(ind.individual_id),
        generation(ind.generation)
    {}
    virtual ~Individual();

    virtual Individual::Ptr clone() = 0;

    virtual void init()
    {
        morphGenome->init();
        createMorphology();
        int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
        bool reload_ctrl = settings::getParameter<settings::Boolean>(parameters,"#reloadController").value;
        if(control == nullptr || instance_type == settings::INSTANCE_SERVER || reload_ctrl){
            ctrlGenome->init();
            createController();
        }
    }

    virtual void update(double delta_time) = 0;
    virtual void mutate()
    {
        morphGenome->mutate();
        ctrlGenome->mutate();
    }
    virtual void crossover(const Individual::Ptr& partner, Individual *child);
    virtual void symmetrical_crossover(const Individual::Ptr& partner, Individual *child1, Individual *child2);


    virtual Eigen::VectorXd descriptor(){return Eigen::VectorXd::Zero(1);}

    void set_randNum(const misc::RandNum::Ptr &rn){randNum = rn;}

    bool isInit(){return (control != nullptr && morphology != nullptr);}

    //Getters & Setters
    const std::vector<double> &get_outputs(){return outputs;}
    const Morphology::Ptr &get_morphology(){return morphology;}
    const Control::Ptr &get_control(){return control;}
    const Genome::Ptr &get_morph_genome(){return morphGenome;}
    const Genome::Ptr &get_ctrl_genome(){return ctrlGenome;}
    void setObjectives(const std::vector<double> &objs){objectives = objs;}
    const std::vector<double> &getObjectives(){return objectives;}
    int get_individual_id(){return individual_id;}
    void set_individual_id(int i){individual_id = i;}
    void set_generation(int g){generation = g;}
    int get_generation(){return generation;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    bool isEvaluated(){return isEval;}
    void set_isEvaluated(bool b){isEval = b;}
    void set_client_id(int cid){client_id = cid;}
    int get_client_id(){return client_id;}

    virtual std::string to_string();
    virtual void from_string(const std::string &str);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & morphGenome;
        arch & individual_id;
        arch & generation;
    }
   const Learner::Ptr & get_learner(){return learner;}

protected:
    std::vector<double> outputs;
    std::vector<double> objectives;
    Genome::Ptr morphGenome;
    Genome::Ptr ctrlGenome;
    Morphology::Ptr morphology;
    Control::Ptr control;
    Learner::Ptr learner;

    settings::ParametersMapPtr parameters;
    misc::RandNum::Ptr randNum;

    bool isEval;

    int individual_id;
    int generation;

    int client_id;
//    std::function<Genome::Factory> createGenome;

    virtual void createController() = 0;
    virtual void createMorphology() = 0;
};



}

#endif //INDIVIDUAL_H
