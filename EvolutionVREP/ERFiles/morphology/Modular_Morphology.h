#pragma once
#include <vector>
#include <memory>
#include "BaseMorphology.h"

using namespace std;

class Modular_Morphology : public BaseMorphology // Abstract Class
{
public:
	Modular_Morphology(){};
	~Modular_Morphology();
	bool modular = true;
	typedef shared_ptr<ER_Module> ModulePointer;
	virtual vector <ModulePointer> getCreatedModules() = 0;
	virtual int getAmountBrokenModules() = 0;
	vector<vector <int> > maxModuleTypes;
	int amountIncrement = 1;
	float robotEnergy;
	void updateRobotEnergy();
	virtual void update() = 0;
	virtual void copyNonVREP(shared_ptr<Morphology> morph) {};

};
