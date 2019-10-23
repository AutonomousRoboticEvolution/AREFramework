#pragma once

//#include "Genome.h"
#include "DefaultGenome.h"
#include "Settings.h"
#include <multineat/Population.h>
#include "../exp_plugin_loader.hpp"

class EA
{
public:

    typedef std::shared_ptr<EA> (Factory)(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);

    EA(){}
    EA(const Settings& st);
	virtual ~EA();

    virtual void split_line(std::string & line, std::string delim, std::list<std::string>& values);

	///set the environment type, evolution type...
	std::shared_ptr<Settings> settings; 
	///random number generator for EA
	std::shared_ptr<RandNum> randomNum;
	/// container of current population genomes
	std::vector<std::shared_ptr<Genome>> populationGenomes;
	/// container of next generation genomes
	std::vector<std::shared_ptr<Genome>> nextGenGenomes;  
	/// used in NEAT
	std::string neatSaveFile = "/testNEAT";


	virtual void update() = 0;  // This is now only used by NEAT but can also be done for the other genomes. However, by passing the update function to the EA different EA objects can contain different scenarios making the plugin more flexible. 

	/// storage vectors
	/// This genome will be evaluated so needs to be assigned
	// std::shared_ptr<Genome> newGenome; // for creating one genome at a time
	// std::vector<int> popIndNumbers;
	// std::vector<float> popFitness; // used by client-server
	// std::vector<float> nextGenFitness; // used by client-server
	std::vector<int> popNextIndNumbers;
	/// This method initilizes setting for EA and random number generator seed
	void setSettings(std::shared_ptr<Settings> st, std::shared_ptr<RandNum> rn);
	/// This method sets the fitness value of an individual
	virtual void setFitness(int individual, float fitness) = 0;
	/// This method initilizes a population of genomes
	virtual void init() = 0;			// initializes EA
	virtual void selection() = 0;		// selection operator
	virtual void replacement() = 0;		// replacement operator
	virtual void mutation() = 0;		// mutation operator
	virtual void end() = 0;				// last call to the EA, when simulation stops
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

    std::function<std::shared_ptr<Genome>
        (int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)> createGenome;
};
