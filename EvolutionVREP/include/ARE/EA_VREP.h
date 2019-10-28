#pragma once
#ifndef EA_VREP_H
#define EA_VREP_H

#include <memory>
#include "ARE/EA.h"


class EA_VREP :
	public EA
{
public:
    EA_VREP() : EA(){}
    ~EA_VREP(){}
	void init();
	void selection();
	void replacement();
	void mutation();

	void initNewGenome(std::shared_ptr<Settings> st, bool client);
	void loadIndividual(int individualNum, int sceneNum);
	void initializePopulation(std::shared_ptr<Settings> st, bool client);
	void loadBestIndividualGenome(int sceneNum) {};
};

#endif //EA_VREP_H
