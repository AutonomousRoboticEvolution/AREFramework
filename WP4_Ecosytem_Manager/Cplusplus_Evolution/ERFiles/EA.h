#pragma once

#include "GenomeFactory.h"
#include "Genome.h"
#include "Settings.h"

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
	///used to create genome
	unique_ptr<GenomeFactory> gf;

	/// container of current population genomes
	vector<shared_ptr<Genome>> populationGenomes;
	/// container of next generation genomes
	vector<shared_ptr<Genome>> nextGenGenomes;  //after mutation

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
	void setFitness(int individual, float fitness);

	/**
		@brief This method initilizes a population of genomes
	*/
	virtual void init() = 0;
	/**
		@brief Selection operator
	*/
	virtual void selection() = 0;
	/**
		@brief Replacement operator
	*/
	virtual void replacement() = 0;
	/**
		@brief Mutation operator
	*/
	virtual void mutation() = 0;
	/**
		@brief Creates the individual in VREP
		@param indNum the ID of the individual
	*/
	void createIndividual(int indNum);
	/**
		@brief Load an individual
		@param individualNum the ID of the individual
		@param sceneNum ID of environment scene?
	*/
	void loadIndividual(int individualNum, int sceneNum);
	/**
		@brief Save the population fitness values
		@param generation: current generation
		@param vector<float> popfit a vector of fitness value
	*/
	void savePopFitness(int generation, vector<float> popfit);
	/**
		@brief Save the population fitness values
		@param generation: current generation
	*/
	void savePopFitness(int generation);
	/**
		@brief Load the best individual based on the evolutionary progression documen
		@param sceneNum: ID of environment scene
	*/
	void loadBestIndividualGenome(int sceneNum);
	/**
		@brief Load all the population genomes?
	*/
	void loadPopulationGenomes();
};
