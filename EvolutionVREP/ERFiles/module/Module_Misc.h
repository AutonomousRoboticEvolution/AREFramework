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
	int createModule(vector<float> configuration, int relativePosHandle, int parentHandle);
	int createModuleBackup(vector<float> configuration, int relativePosHandle, int parentHandle);
	vector<int> getFreeSites(vector<int>);
	vector<int> getObjectHandles();
	// For deep copy
	shared_ptr<ER_Module> clone();

	void setModuleColor();
	void removeModule(); 

	vector<float> updateModule(vector<float> input);
	void controlModule(float input);

	stringstream getModuleParams();
	bool upDown = true; 
	void setModuleParams(vector<string>);
	void createControl();
	vector<float> getPosition();
	virtual stringstream getControlParams();
	float previousPosition = -1;

private:
	int objectPhysics = 8;

};

