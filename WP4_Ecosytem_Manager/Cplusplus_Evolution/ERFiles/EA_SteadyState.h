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
	void init();
	void selection();
	void replacement();
	void mutation();

	/**
		@brief This method initilizes a population of genomes
	*/
	void initializePopulation();
	void selectIndividuals(); // random tournament, empty function?
	void replaceIndividuals(); // random empty function?
	void replaceNewIndividual(int indNum, int sceneNum, float fitness);

	/**
		@brief This method creates a new morphology base on a randomly selected parent (copy)
	*/
	void createNewGenRandomSelect();

	void replaceNewPopRandom(int numAttempts);
	void replaceNewRank();
	void loadPopulationGenomes(int scenenum);
};
