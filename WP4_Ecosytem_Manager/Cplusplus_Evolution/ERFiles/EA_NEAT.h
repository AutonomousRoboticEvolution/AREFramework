#pragma once

#include "EA.h"
#include "NEAT_LIB\Genome.h"

class EA_NEAT : public EA
{
public:
	EA_NEAT();
	~EA_NEAT();

	void split_line(string & line, string delim, list<string>& values);
	vector<NEAT::Genome*> genomeBuffer; // stores the pointers to the NEAT 
	NEAT::Genome* currentNeatIndividual;
	unique_ptr<NEAT::NeuralNetwork> currentNet; 
	shared_ptr<NEAT::Population> population;
	
	shared_ptr<Genome> currentGenome; // This will hold the robot information created by the CPPN etc.
	// base functions of EA
	void init();
	vector<float> Evaluate(NEAT::Genome genome);
	void selection();
	void replacement();
	void mutation();
	void update();
	void end(); // last call to get the fitness of the individual

	shared_ptr<Genome> er_genome; // ER genome not NEAT

	// unique functions
	void initializePopulation();
	bool compareByFitness(const shared_ptr<Genome> a, const shared_ptr<Genome> b);
	void createIndividual(int ind);
	void loadPopulationGenomes(int scenenum);
};

