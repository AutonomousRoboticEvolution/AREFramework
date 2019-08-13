#pragma once

#include "EA.h"
//#include "NEAT_LIB\Genome.h"
#include "morphology\MorphologyFactoryVREP.h"

class EA_NEAT : public EA
{
public:
	EA_NEAT();
	~EA_NEAT();

	void split_line(string & line, string delim, list<string>& values);
	vector<NEAT::Genome*> genomeBuffer; // stores the pointers to the NEAT 
	NEAT::Genome* currentNeatIndividual; // placeholder for a pointer to a NEAT genome that needs to be evaluated
	shared_ptr<NEAT::NeuralNetwork> currentNet;
	
	// shared_ptr<Genome> currentGenome; // This will hold the robot information created by the CPPN etc.
	shared_ptr<Morphology> neat_morph; // This will hold the robot information created by the CPPN etc.

	// base functions of EA
	void init();
	vector<float> Evaluate(NEAT::Genome genome);
	void selection();
	void replacement();
	void mutation();
	void update();
	void end(); // last call to get the fitness of the individual

	shared_ptr<Genome> er_genome; // ER genome not NEAT
	virtual shared_ptr<Morphology> getMorph();

	// unique functions
	void initializePopulation();
	bool compareByFitness(const shared_ptr<Genome> a, const shared_ptr<Genome> b);
	void createIndividual(int ind);
	void setFitness(int ind, float fit);
	void loadPopulationGenomes(int scenenum);	
	void loadBestIndividualGenome(int scenenum);
};

