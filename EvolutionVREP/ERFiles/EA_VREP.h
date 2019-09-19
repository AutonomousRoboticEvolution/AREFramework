#pragma once

#include <memory>
#include "EA.h"
#include "GenomeFactoryVREP.h"


class EA_VREP :
	public EA
{
public:
	EA_VREP();
	~EA_VREP();
    std::unique_ptr <GenomeFactoryVREP> gf;
	void init();
	void selection();
	void replacement();
	void mutation();

	void initNewGenome(std::shared_ptr<Settings> st, bool client);
	void loadIndividual(int individualNum, int sceneNum);
	void initializePopulation(std::shared_ptr<Settings> st, bool client);
	void loadBestIndividualGenome(int sceneNum) {};
};
