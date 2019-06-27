#pragma once

#include "EA.h"

class EA_Generational : public EA
{
public:
	EA_Generational();
	~EA_Generational();

	
	// base functions of EA
	void init();
	void selection();
	void replacement();
	void mutation();
	void update() {}; // not used in this class

	void initializePopulation();
	void selectIndividuals(); // random tournament
	void replaceIndividuals(); // random
	void replaceNewIndividual(int indNum, int sceneNum, float fitness);
	void createIndividual(int indNum);
	
	void end() {}; // not used in this class


	void createNewGenRandomSelect();
	void replaceNewPopRandom();
};

