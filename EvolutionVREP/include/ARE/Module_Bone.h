#pragma once
#ifndef MODULE_BONE_H
#define MODULE_BONE_H

#include "ARE/ER_Module.h"


class Module_Bone :
	public ER_Module
{
public:
    Module_Bone() : ER_Module(){}
    ~Module_Bone(){}


	int init() override;
	int mutate(float mutationRate) override;

    std::vector<int> createBone(std::vector<float> configuration, int relativePosHandle, int parentHandle);
	// Create cube
    std::vector<int> createCube(std::vector<float> configuration, int relativePosHandle, int parentHandle);

	int createModule(std::vector<float> configuration, int relativePosHandle, int parentHandle) override;
    std::vector<int> getFreeSites(std::vector<int>) override;
    std::vector<int> getObjectHandles() override;
	// For deep copy
    std::shared_ptr<ER_Module> clone() override;

	void setModuleColor() override;
	void removeModule() override;

    std::vector<float> updateModule(std::vector<float> input) override;
	void controlModule(float input);

    std::stringstream getModuleParams() override;
	bool upDown = true; 
	void setModuleParams(std::vector<std::string>) override;
	void createControl() override;
    std::vector<float> getPosition() override;
	virtual std::stringstream getControlParams() override;
	float previousPosition = -1;

    //void updateMorph(int num) override;
    void updateMorph(int num) override { std::cout << "TODO : " << num << std::endl;/* TODO */ };

private:
	int objectPhysics = 8;

};

#endif //MODULE_BONE_H
