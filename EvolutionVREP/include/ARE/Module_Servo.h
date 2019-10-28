#pragma once
#ifndef MODULE_SERVO_H
#define MODULE_SERVO_H

#include "ARE/ER_Module.h"


class Module_Servo :
	public ER_Module
{
public:
    Module_Servo() : ER_Module(){}
	~Module_Servo();
	int init();
	int mutate(float mutationRate);
	int createModule(std::vector<float> configuration, int relativePosHandle, int parentHandle);
	int createModuleBackup(std::vector<float> configuration, int relativePosHandle, int parentHandle);
    std::vector<int> getFreeSites(std::vector<int>);
    std::vector<int> getObjectHandles();
    std::shared_ptr<ER_Module> clone();
	float bendAngle = 0.25;
	void setModuleColor();
	void removeModule();
    std::vector<float> updateModule(std::vector<float> input);
    std::stringstream getModuleParams();
	void controlModule(float input);
	bool upDown = true; 
	void setModuleParams(std::vector<std::string>);
	void createControl();
    std::vector<float> getPosition();
	virtual std::stringstream getControlParams();
	float previousPosition = -1;

private:
	int objectPhysics = 8;

};

#endif //MODULE_SERVO_H
