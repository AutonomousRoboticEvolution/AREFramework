#pragma once
#include "ER_Module.h"

class Module_Servo :
	public ER_Module
{
public:
	Module_Servo();
	~Module_Servo();
	int init();
	int mutate(float mutationRate);
	int createModule(vector<float> configuration, int relativePosHandle, int parentHandle);
	int createModuleBackup(vector<float> configuration, int relativePosHandle, int parentHandle);
	vector<int> getFreeSites(vector<int>);
	vector<int> getObjectHandles();
	shared_ptr<ER_Module> clone();
	float bendAngle = 0.25;
	void setModuleColor();
	void removeModule(); 
	vector<float> updateModule(vector<float> input);
	stringstream getModuleParams();
	void controlModule(float input);
	bool upDown = true; 
	void setModuleParams(vector<string>);
	void createControl();
	vector<float> getPosition();
	virtual stringstream getControlParams();
	float previousPosition = -1;

private:
	int objectPhysics = 8;

};

