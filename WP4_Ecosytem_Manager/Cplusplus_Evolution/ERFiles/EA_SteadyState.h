#pragma once

#include "EA.h"

class EA_SteadyState : public EA
{
public:
	EA_SteadyState();
	~EA_SteadyState();

	void split_line(string & line, string delim, list<string>& values);

	/**
	 * @brief
	 */
	vector<int> evaluationQueue;

	// base functions of EA
	void init();
	void selection();
	void replacement();
	void mutation();
	void update() {}; // not used in this class
	void createIndividual(int indNum) {}; // not used in this class
	void initNewGenome(int indnum);
	/// used for NEAT
	void end() {}; // not used in this class

	/**
		@brief This method initilizes a population of genomes
	*/
	void initializePopulation();
	virtual shared_ptr<Genome> initNewGenome();
	virtual void saveGenome(shared_ptr<Genome>);
	void setFitness(int individual, float fitness);
	virtual shared_ptr<Morphology> getMorph() { return shared_ptr<Morphology>(); };  // not used in this class // NEAT hack
	virtual void loadBestIndividualGenome(int sceneNum) {}; // not used here
    /**
        @brief This method creates a new morphology base on a randomly selected parent (copy)
    */
	void createNewGenRandomSelect();
    void replaceNewPopRandom(int numAttempts);
	void replaceNewRank();
	void loadPopulationGenomes(int scenenum);
};

