#pragma once

#include "EA.h"

class EA_SteadyState : public EA
{
public:
	EA_SteadyState();
	~EA_SteadyState();

	void split_line(string & line, string delim, list<string>& values);
	vector<int> evaluationQueue;
	
	// base functions of EA
	void init();
	void selection();
	void replacement();
	void mutation();
	void update() {}; // not used in this class
	void createIndividual(int indNum) {}; // not used in this class
	void initNewGenome(int indnum);
	void end() {}; // not used in this class


	// unique functions
	void initializePopulation();
	void selectIndividuals(); // random tournament
	void replaceIndividuals(); // random
	void replaceNewIndividual(int indNum, int sceneNum, float fitness);
	virtual shared_ptr<Genome> initNewGenome();
	virtual void saveGenome(shared_ptr<Genome>);

	void createNewGenRandomSelect();
	
	void replaceNewPopRandom(int numAttempts);
	void replaceNewRank();
	void loadPopulationGenomes(int scenenum);
};

