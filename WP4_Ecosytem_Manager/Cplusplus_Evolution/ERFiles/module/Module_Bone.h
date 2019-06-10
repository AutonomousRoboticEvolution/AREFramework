#pragma once
#include "ER_Module.h"

class Module_Bone :
	public ER_Module
{
public:
	Module_Bone();
	~Module_Bone();

	int init();
	int mutate(float mutationRate);
	vector<int> createBone(vector<float> configuration, int relativePosHandle, int parentHandle);
	int createModule(vector<float> configuration, int relativePosHandle, int parentHandle);
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
	void updateMorph(int num) { /* TODO */ };

private:
	int objectPhysics = 8;

};

