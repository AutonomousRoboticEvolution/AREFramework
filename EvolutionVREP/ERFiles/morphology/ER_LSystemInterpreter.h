#pragma once

#include <vector>
#include <iostream>
#include "ER_LSystem.h"
#include "Modular_Morphology.h"
#include "../module/ModuleFactory.h"
#include "../VREPUI/VREPUI.h"


class ER_LSystemInterpreter : public ER_LSystem
{
public:
	ER_LSystemInterpreter();
	~ER_LSystemInterpreter();

	bool checkJointModule();

	void init() override;
	void create() override;
	void update() override;
	void getEnvironmentObjects();

	bool incrementer = false;
	float simTime = 0;

	void checkForceSensors();
	void savePhenotype(int ind, float fitness) override;
	void initializeGenomeCustom(int type);
	void initializeLRobot(int type);

	int initializeLSystem(int increments, float initialPosition[3]);
	float getFitness() override;
	void init_noMorph() override;
	// object creators
	void createAtPosition(float x, float y, float z) override;
    std::shared_ptr<Morphology> clone() const override;

	void updateColors();

	void printSome() override;

	float positionFirstObject[3] = { 0.0f, 0.0f, 0.1f };
	void initCustomMorphology();
	void incrementLSystem(); 
	virtual bool loadGenome(std::istream &input, int indNum) override;

	void setColors();

	float checkArea(float interSection[3], float pps[4][3]);
	void cubeDrawing(std::vector<std::vector<float>> rotatedPoints, float color[3]);
    std::vector<int> debugDrawings;
	bool checkCollisionBasedOnRotatedPoints(int objechHandle);
	int getAmountBrokenModules() override;
	//void shiftRobotPosition();
	void symmetryMutation(float mutationRate);
    std::vector<std::shared_ptr<ER_Module>> modules;
    std::shared_ptr<ModuleFactory> moduleFactory;

	void setPhenValue() override;

//	VREPUI *vrepUI;
    std::shared_ptr<VREPUI> vrepUI;
};