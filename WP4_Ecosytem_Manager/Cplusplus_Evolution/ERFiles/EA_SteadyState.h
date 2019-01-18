#pragma once

#include "EA.h"

class EA_SteadyState : public EA
{
public:
	EA_SteadyState();
	~EA_SteadyState();

	void split_line(string & line, string delim, list<string>& values);

	
	// base functions of EA
	void init();
	void selection();
	void replacement();
	void mutation();

	// unique functions
	void initializePopulation();
	void selectIndividuals(); // random tournament
	void replaceIndividuals(); // random
	void replaceNewIndividual(int indNum, int sceneNum, float fitness);
	
	void createNewGenRandomSelect();
	
	void replaceNewPopRandom(int numAttempts);
	void replaceNewRank();
	void loadPopulationGenomes(int scenenum);
};

