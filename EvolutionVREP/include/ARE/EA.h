#pragma once
#ifndef EA_H
#define EA_H

//#include "Genome.h"
#include "ARE/Genome.h"
#include "ARE/Settings.h"
#include <multineat/Population.h>
#include "ARE/exp_plugin_loader.hpp"
#include "ARE/Individual.h"

namespace are{

class EA
{
public:

    typedef std::unique_ptr<EA> Ptr;
    typedef std::unique_ptr<const EA> ConstPtr;
    typedef EA::Ptr (Factory)(misc::RandNum::Ptr,Settings::Ptr);

    EA(){}
    EA(const Settings& st);
    //no copyable object
//    EA(const EA& ea) :
//        settings(ea.settings),
//        randomNum(ea.randomNum),
//        populationGenomes(ea.populationGenomes),
//        population(ea.population),
//        createGenome(ea.createGenome),
//        neatSaveFile(ea.neatSaveFile),
//        popNextIndNumbers(ea.popNextIndNumbers),
//        nextGenGenomes(ea.nextGenGenomes)
//    {}
    virtual ~EA();


    ///set the environment type, evolution type...
    Settings::Ptr settings;
    ///random number generator for EA
    misc::RandNum::Ptr randomNum;

    /// container of current population genomes
    std::vector<Genome::Ptr> populationGenomes;
    /// container of next generation genomes
    std::vector<Genome::Ptr> nextGenGenomes;
    /// used in NEAT
    std::string neatSaveFile = "/testNEAT";



    /// storage vectors
    /// This genome will be evaluated so needs to be assigned
    // std::shared_ptr<Genome> newGenome; // for creating one genome at a time
    // std::vector<int> popIndNumbers;
    // std::vector<float> popFitness; // used by client-server
    // std::vector<float> nextGenFitness; // used by client-server
    std::vector<int> popNextIndNumbers;
    /// This method initilizes setting for EA and random number generator seed
    void setSettings(Settings::Ptr st, misc::RandNum::Ptr rn);
    /// This method sets the fitness value of an individual
    virtual void setFitness(int individual, float fitness) = 0;

    virtual void epoch();

    /// This method initilizes a population of genomes
    virtual void init() = 0;    // initializes EA
    virtual void evaluation() = 0;  // This is now only used by NEAT but can also be done for the other genomes. However, by passing the update function to the EA different EA objects can contain different scenarios making the plugin more flexible.
    virtual void selection(){};		// selection operator
    virtual void replacement(){};		// replacement operator
    virtual void mutation(){};		// mutation operator
    virtual void end(){};				// last call to the EA, when simulation stops
    /// Creates the individual in VREP
    virtual void createIndividual(int indNum) = 0;
    /// Load an individual
    void loadIndividual(int individualNum, int sceneNum);
    /// Save the population fitness values
    void savePopFitness(int generation, std::vector<float> popfit, std::vector<int> popInividuals);
    /// Save the population fitness values
    virtual void savePopFitness(int generation);
    /// Load the best individual based on the evolutionary progression documen
    virtual void loadBestIndividualGenome(int sceneNum) = 0;
    /// Load all the population genomes?
    void loadPopulationGenomes();
    virtual std::shared_ptr<Morphology> getMorph() = 0;

    std::function<Genome::Factory> createGenome;

    const std::vector<Individual::Ptr> &get_population(){return population;}

protected:
    std::vector<Individual::Ptr> population;
};

}//are

#endif //EA_H
