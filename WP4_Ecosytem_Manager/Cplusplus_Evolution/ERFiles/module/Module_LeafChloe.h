#pragma once
#include "ER_Module.h"

class Module_LeafChloe :
	public ER_Module
{
public:
	Module_LeafChloe();
	~Module_LeafChloe();
	int init();
	int mutate(float mutationRate);
	int createModule(vector<float> configuration, int relativePosHandle, int parentHandle);
	vector<int> getFreeSites(vector<int>);
	vector<int> getObjectHandles();
	shared_ptr<ER_Module> clone();
	float bendAngle = 0.25;
	void setModuleColor();
	void removeModule(); 
	vector<float> updateModule(vector<float> input);
	stringstream getModuleParams();
	void setModuleParams(vector<string>);
	void createControl() {};
	vector<float> getPosition();
	virtual stringstream getControlParams() ;
	int amountProxSensors = 5; 
	vector<int> proxSensors;

	int currentStep; 
	int checkStep; 

private:
	int objectPhysics = 8;
};

