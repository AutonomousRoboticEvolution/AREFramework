#pragma once
#ifndef EA_H
#define EA_H

#include <chrono>

//#include "Genome.h"
#include "ARE/Genome.h"
#include "ARE/Settings.h"
#include <multineat/Population.h>
#include "ARE/exp_plugin_loader.hpp"
#include "ARE/Individual.h"
#include "ARE/Environment.h"


namespace are{

typedef std::chrono::high_resolution_clock hr_clock;

class EA
{
public:

    typedef std::unique_ptr<EA> Ptr;
    typedef std::unique_ptr<const EA> ConstPtr;
    typedef EA::Ptr (Factory)(const misc::RandNum::Ptr&,const settings::ParametersMapPtr&);

    EA(){}
    EA(const settings::ParametersMapPtr& param);
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


    std::vector<int> popNextIndNumbers;
    /// This method initilizes setting for EA and random number generator seed
    void setSettings(const settings::ParametersMapPtr &param, const misc::RandNum::Ptr &rn);
    /// This method sets the fitness value of an individual
    virtual void setFitness(size_t indIndex, float fitness)
    {
        population[indIndex]->setFitness(fitness);
    }

    /**
     * @brief Update method is called after each evaluation of an individual.
     * @return If the algorithm can continue to the next ind
     */
    virtual bool update(const Environment::Ptr&)
    {
        numberEvaluation++;
        return true;
    }

    /**
     * @brief Epoch method is called at the end of each generation
     */
    virtual void epoch();
    virtual void init() = 0;    // initializes EA


    Individual::Ptr getIndividual(size_t index) const;
    size_t getPopSize() const {return population.size();}


    /// Load an individual
//    void loadIndividual(int individualNum, int sceneNum){}
    /// Save the population fitness values
//    void savePopFitness(int generation, std::vector<float> popfit, std::vector<int> popInividuals);
    /// Save the population fitness values

    /// Load the best individual based on the evolutionary progression documen
//    virtual void loadBestIndividualGenome(int sceneNum) = 0;
    /// Load all the population genomes?
//    void loadPopulationGenomes();
//    virtual std::shared_ptr<Morphology> getMorph() = 0;

    std::function<Genome::Factory> createGenome;


    //GETTERS & SETTERS
    const std::vector<Individual::Ptr> &get_population() const {return population;}
    const settings::ParametersMapPtr &get_parameters() const {return parameters;}
    const misc::RandNum::Ptr get_randomNum() const {return randomNum;}
    void set_randomNum(const misc::RandNum::Ptr& rn){randomNum = rn;}
    void set_generation(int gen){generation = gen;}
    int get_generation() const {return generation;}
    void incr_generation(){generation++;}

    int get_numberEvaluation() const {return numberEvaluation;}

    std::chrono::nanoseconds getEvalCompTime() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>
               (endEvalTime - startEvalTime);
    }
    void set_startEvalTime(const hr_clock::time_point& t){startEvalTime = t;}
    void set_endEvalTime(const hr_clock::time_point& t){endEvalTime = t;}


protected:
    /// This method initilizes a population of genomes
    virtual void evaluation(){}  // This is now only used by NEAT but can also be done for the other genomes. However, by passing the update function to the EA different EA objects can contain different scenarios making the plugin more flexible.
    virtual void selection(){}  	// selection operator
    virtual void replacement(){}		// replacement operator
    virtual void mutation(){}		// mutation operator
    virtual void crossover(){}
    virtual void end(){}				// last call to the EA, when simulation stops


    std::vector<Individual::Ptr> population;
    ///set the environment type, evolution type...
    settings::ParametersMapPtr parameters;
    ///random number generator for EA
    misc::RandNum::Ptr randomNum;

    int generation = 0;
    int numberEvaluation = 0;

    hr_clock::time_point startEvalTime;
    hr_clock::time_point endEvalTime;
};

}//are

#endif //EA_H