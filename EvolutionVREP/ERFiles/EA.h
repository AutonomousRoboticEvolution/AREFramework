#pragma once

#include "GenomeFactory.h"
#include "Genome.h"
#include "Settings.h"
#include "../NEAT_LIB/NEAT_LIB/Genome.h"
#include "../NEAT_LIB/NEAT_LIB/Population.h"
#include "../NEAT_LIB/NEAT_LIB/Assert.h"

class EA
{
public:
	EA();
	virtual ~EA();
	void split_line(string & line, string delim, list<string>& values);

	///set the environment type, evolution type...
	shared_ptr<Settings> settings; 
	///random number generator for EA
	shared_ptr<RandNum> randomNum;
	/// container of current population genomes
	vector<shared_ptr<Genome>> populationGenomes;
	/// container of next generation genomes
	vector<shared_ptr<Genome>> nextGenGenomes;  
	/// used in NEAT
	shared_ptr<NEAT::Population> population;
	string neatSaveFile = "/testNEAT";


	virtual void update() = 0;  // This is now only used by NEAT but can also be done for the other genomes. However, by passing the update function to the EA different EA objects can contain different scenarios making the plugin more flexible. 

	/// storage vectors
	/// This genome will be evaluated so needs to be assigned
	// shared_ptr<Genome> newGenome; // for creating one genome at a time
	// vector<int> popIndNumbers;
	// vector<float> popFitness; // used by client-server
	// vector<float> nextGenFitness; // used by client-server
	vector<int> popNextIndNumbers;
	/**
		@brief This method initilizes setting for EA and random number generator seed
	*/
	void setSettings(shared_ptr<Settings> st, shared_ptr<RandNum> rn);
	/**
		@brief This method sets the fitness value of an individual
	*/
	virtual void setFitness(int individual, float fitness) = 0;
	/**
		@brief This method initilizes a population of genomes
	*/
	virtual void init() = 0;			// initializes EA
	virtual void selection() = 0;		// selection operator
	virtual void replacement() = 0;		// replacement operator
	virtual void mutation() = 0;		// mutation operator
	virtual void end() = 0;				// last call to the EA, when simulation stops
	/**
		@brief Creates the individual in VREP
		@param indNum the ID of the individual
	*/
	virtual void createIndividual(int indNum) = 0;
	/**
		@brief Load an individual
		@param individualNum the ID of the individual
		@param sceneNum ID of environment scene?
	*/
	void loadIndividual(int individualNum, int sceneNum);
	/**
		@brief Save the population fitness values
		@param generation: current generation
		@param vector<float> popfit is a vector containing the fitness valus of the population
		@param vector<float> popIndividuals is a vector containing the ID's of the individuals
	*/
	void savePopFitness(int generation, vector<float> popfit, vector<int> popInividuals);
	/**
		@brief Save the population fitness values
		@param generation: current generation
	*/
	void savePopFitness(int generation);
	/**
		@brief Load the best individual based on the evolutionary progression documen
		@param sceneNum: ID of environment scene
	*/
	virtual void loadBestIndividualGenome(int sceneNum) = 0; 
	/**
		@brief Load all the population genomes?
	*/
	void loadPopulationGenomes();
	virtual shared_ptr<Morphology> getMorph() = 0;
};
