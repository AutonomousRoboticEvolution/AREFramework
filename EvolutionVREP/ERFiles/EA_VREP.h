#pragma once

#include <memory>
#include "EA.h"


class EA_VREP :
	public EA
{
public:
	EA_VREP();
	~EA_VREP();
	void init();
	void selection();
	void replacement();
	void mutation();

	void initNewGenome(std::shared_ptr<Settings> st, bool client);
	void loadIndividual(int individualNum, int sceneNum);
	void initializePopulation(std::shared_ptr<Settings> st, bool client);
	void loadBestIndividualGenome(int sceneNum) {};
};
