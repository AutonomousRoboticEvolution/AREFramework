#include "Modular_Morphology.h"
#include <iostream>

Modular_Morphology::~Modular_Morphology(){
//	cout << endl << "DELETED MORPHOLOGY" << endl << endl; 
};

vector <Modular_Morphology::ModulePointer> Modular_Morphology::getCreatedModules() {
	vector<ModulePointer> nullVec;
	return nullVec;
}

void Modular_Morphology::updateRobotEnergy()
{
	robotEnergy = 0;
	for (int i = 0; i < createdModules.size(); i++) {
		robotEnergy += createdModules[i]->energy;
	}
}

void Modular_Morphology::update()
{
	if (createdModules.size() < 2) {
		simStopSimulation();
	}
}

