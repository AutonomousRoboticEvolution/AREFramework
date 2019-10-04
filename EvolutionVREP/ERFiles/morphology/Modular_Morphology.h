#pragma once

#include <vector>
#include <memory>
#include "BaseMorphology.h"


class Modular_Morphology : public BaseMorphology // Abstract Class
{
public:
	Modular_Morphology(){};
	~Modular_Morphology();
	bool modular = true;
	typedef std::shared_ptr<ER_Module> ModulePointer;
	virtual std::vector <ModulePointer> getCreatedModules() = 0;
	virtual int getAmountBrokenModules() = 0;
	std::vector<std::vector <int> > maxModuleTypes;
	int amountIncrement = 1;
	float robotEnergy;
	void updateRobotEnergy();
	virtual void update() = 0;
	virtual void copyNonVREP(std::shared_ptr<Morphology> morph) {};

};
