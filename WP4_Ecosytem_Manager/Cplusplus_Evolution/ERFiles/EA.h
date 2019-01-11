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

	shared_ptr<Settings> settings;
	shared_ptr<RandNum> randomNum;
	unique_ptr<GenomeFactory> gf;

	// population storage
	vector<shared_ptr<Genome>> populationGenomes;
	vector<shared_ptr<Genome>> nextGenGenomes; // creating the next generation

	// storage vectors
	// This genome will be evaluated so needs to be assigned
	// shared_ptr<Genome> newGenome; // for creating one genome at a time
	vector<int> popIndNumbers;
	// vector<float> popFitness; // used by client-server
	// vector<float> nextGenFitness; // used by client-server
	vector<int> popNextIndNumbers;


	// base functions of EA
	void setSettings(shared_ptr<Settings> st, shared_ptr<RandNum> rn);

	void setFitness(int individual, float fitness);

	// Initialization
	virtual void init() = 0;
	// Selection operator
	virtual void selection() = 0; 
	// Replacement operator
	virtual void replacement() = 0; 
	// Mutation operator
	virtual void mutation() = 0;
	
	// Creates the individual in VREP
	void createIndividual(int indNum);


	
	// Load an individual	
	void loadIndividual(int individualNum, int sceneNum);
	// Save the population fitness values
	void savePopFitness(int generation, vector<float> popfit);
	// Alternative function
	void savePopFitness(int generation);
	// Load the best individual based on the evolutionary progression document. Make sure sceneNum is correct.
	void loadBestIndividualGenome(int sceneNum);
	// To continue evolution
	void loadPopulationGenomes();
};

