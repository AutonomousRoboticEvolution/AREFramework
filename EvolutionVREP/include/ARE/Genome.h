#pragma once
#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include "ARE/Settings.h"
#include "misc/RandNum.h"
#include "ARE/Phenotype.h"

namespace are {
class Genome
{
public:

    typedef std::shared_ptr<Genome> Ptr;
    typedef std::shared_ptr<const Genome> ConstPtr;
    typedef Genome::Ptr (Factory)(int, misc::RandNum::Ptr);

    Genome(){}
    Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param);
    Genome(const Genome& gen) :
        parameters(gen.parameters),
        randomNum(gen.randomNum),
        initialized(gen.initialized),
        fitness(gen.fitness),
        genomeFitness(gen.genomeFitness),
        isEvaluated(gen.isEvaluated),
        individualNumber(gen.individualNumber)
    {}
    virtual ~Genome();

    /// This method deep copies the genome
    virtual Genome::Ptr clone() const = 0;

    /// Initialize the morph and its control
    virtual void init() = 0;


    virtual Phenotype::Ptr develop() = 0;

    virtual void mutate() = 0;

//    virtual void savePhenotype(int indNum, int sceneNum);
//    /// loads genome from .csv
//    virtual bool loadGenome(int indNum, int sceneNum);
//    /// loads genome from signal : TODO BUG????
//    virtual bool loadGenome(std::istream &input, int indNum) ;
//    virtual void saveGenome(int indNum){};



//    /// Returns a genome string from existing genome
//    virtual const std::string generateGenome() const{}

//    /// prints out a few genome debugging statements
//    virtual void checkGenome(); // for debugging
//    void clearGenome();



    //Getters & Setters
    float get_fitness(){return fitness;}
    void set_fitness(float f){fitness = f;}
    int get_individualNumber(){return individualNumber;}
    void set_individualNumber(int in){individualNumber =in;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}

protected:
    /// simulation setting shared by genome and EA
    settings::ParametersMapPtr parameters;
    /// random number generator
    misc::RandNum::Ptr randomNum;

    bool initialized = false;

    /// fitness value; make vector for multi-objective
    float fitness;
    float genomeFitness;
    /// mark the end of evaluation for a generation
    bool isEvaluated = false;

    /// mark the ID of individual
    int individualNumber;
};

/**
 * @brief The EmptyGenome class is meant to be used when no genome is needed.
 */
class EmptyGenome : public Genome
{
public:
    Genome::Ptr clone() const{return nullptr;}
    void init(){}
    Phenotype::Ptr develop(){return nullptr;}
    void mutate(){}
};


}//are

#endif //GENOME_H
