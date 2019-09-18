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

    std::vector<int> createBone(std::vector<float> configuration, int relativePosHandle, int parentHandle);
	// Create cube
    std::vector<int> createCube(std::vector<float> configuration, int relativePosHandle, int parentHandle);

	int createModule(std::vector<float> configuration, int relativePosHandle, int parentHandle);
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

    //void updateMorph(int num) override;
    void updateMorph(int num) override { std::cout << "TODO : " << num << std::endl;/* TODO */ };

private:
	int objectPhysics = 8;

};

