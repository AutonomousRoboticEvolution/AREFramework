#pragma once

#include "ER_Module.h"

using namespace std;

class ModuleGenotype : public ER_Module // This class server as a placeholder for genotypic elements without VREP dependencies
{
public:
	ModuleGenotype();
	~ModuleGenotype();

	int init();
	int mutate(float mutationRate);
	int createModule(vector<float> configuration, int relativePosHandle, int parentHandle);
	void createControl();
	stringstream getModuleParams();
	stringstream getControlParams();
	void setModuleParams(vector<string>);
	void removeModule();
	vector<float> getPosition();
	shared_ptr<ER_Module> clone(); 
	vector<int> getFreeSites(vector<int>);
	vector<float> updateModule(vector<float> input); // should return output
	vector<int> getObjectHandles(); 
	void setModuleColor(); 	
};

