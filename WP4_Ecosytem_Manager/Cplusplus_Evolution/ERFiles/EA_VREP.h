#pragma once
#include "EA.h"
#include "GenomeFactoryVREP.h"

class EA_VREP :
	public EA
{
public:
	EA_VREP();
	~EA_VREP();
	unique_ptr <GenomeFactoryVREP> gf;
	void init();
	void selection();
	void replacement();
	void mutation();

	void initNewGenome(shared_ptr<Settings> st, bool client);
	void loadIndividual(int individualNum, int sceneNum);
	void initializePopulation(shared_ptr<Settings> st, bool client);
	void loadBestIndividualGenome(int sceneNum);
};

