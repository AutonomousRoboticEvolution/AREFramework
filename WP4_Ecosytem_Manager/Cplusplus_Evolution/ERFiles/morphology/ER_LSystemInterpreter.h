#pragma once

#include <vector>
#include <iostream>
#include "../module/ModuleFactory.h"
#include "ER_LSystem.h"
#include "Modular_Morphology.h"
#include "../VREPUI/VREPUI.h"

using namespace Eigen;
using namespace std;


class ER_LSystemInterpreter : public ER_LSystem
{
public:
	ER_LSystemInterpreter();
	~ER_LSystemInterpreter();

	bool checkJointModule();

	void init();
	void create();
	void update(); 
	void getEnvironmentObjects();

	bool incrementer = false;
	float simTime = 0;

	void checkForceSensors();
	void savePhenotype(int ind, float fitness);
	void initializeGenomeCustom(int type);
	void initializeLRobot(int type);
	bool checkLCollisions(shared_ptr <ER_Module> module, vector<int> exceptionHandles);

	int initializeLSystem(int increments, float initialPosition[3]);
	float getFitness();
	void init_noMorph();
	// object creators
	void createAtPosition(float x, float y, float z);
	shared_ptr<Morphology> clone() const;

	void updateColors();

	void printSome();
	int getMainHandle();

	float positionFirstObject[3] = { 0.0f, 0.0f, 0.1f };
	void initCustomMorphology();
	void incrementLSystem(); 
	virtual bool loadGenome(std::istream &input, int indNum) override;

	void setColors();

	float checkArea(float interSection[3], float pps[4][3]);
	void cubeDrawing(vector<vector<float>> rotatedPoints, float color[3]);
	vector<int> debugDrawings;
	bool checkCollisionBasedOnRotatedPoints(int objechHandle);
	int getAmountBrokenModules();
	void shiftRobotPosition();
	void symmetryMutation(float mutationRate);
	vector<shared_ptr<ER_Module>> modules;
	shared_ptr<ModuleFactory> moduleFactory;

	void setPhenValue();

//	VREPUI *vrepUI;
	shared_ptr<VREPUI> vrepUI;

};