#pragma once

#include "ER_Module.h"


class ModuleGenotype : public ER_Module // This class server as a placeholder for genotypic elements without VREP dependencies
{
public:
	ModuleGenotype();
	~ModuleGenotype();

	int init();
	int mutate(float mutationRate);
	int createModule(std::vector<float> configuration, int relativePosHandle, int parentHandle);
	void createControl();
    std::stringstream getModuleParams();
    std::stringstream getControlParams();
	void setModuleParams(std::vector<std::string>);
	void removeModule();
    std::vector<float> getPosition();
    std::shared_ptr<ER_Module> clone();
    std::vector<int> getFreeSites(std::vector<int>);
    std::vector<float> updateModule(std::vector<float> input); // should return output
    std::vector<int> getObjectHandles();
	void setModuleColor(); 	
};

