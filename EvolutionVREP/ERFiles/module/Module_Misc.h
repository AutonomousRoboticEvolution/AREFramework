#pragma once

#include "ER_Module.h"


class Module_Misc :
	public ER_Module
{
public:
	Module_Misc();
	~Module_Misc();


	int init();
	int mutate(float mutationRate);
	int createModule(std::vector<float> configuration, int relativePosHandle, int parentHandle);
	int createModuleBackup(std::vector<float> configuration, int relativePosHandle, int parentHandle);
    std::vector<int> getFreeSites(std::vector<int>);
    std::vector<int> getObjectHandles();
	// For deep copy
    std::shared_ptr<ER_Module> clone();

	void setModuleColor();
	void removeModule();

    std::vector<float> updateModule(std::vector<float> input);
	void controlModule(float input);

    std::stringstream getModuleParams();
	bool upDown = true; 
	void setModuleParams(std::vector<std::string>);
	void createControl();
    std::vector<float> getPosition();
	virtual std::stringstream getControlParams();
	float previousPosition = -1;

private:
	int objectPhysics = 8;

};

