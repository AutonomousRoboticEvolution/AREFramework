#pragma once

#include <vector>
#include "ER_CPPN_Encoding.h"
#include "../module/ER_Module.h"
#include "../module/ModuleFactory.h"


class ER_CPPN_Interpreter : public ER_CPPN_Encoding
{
public:
	ER_CPPN_Interpreter();
	~ER_CPPN_Interpreter();

	void init();
	void create();
	void update(); 
	void updateColors();
	void checkForceSensors();
	void initializeGenomeCustom(int type) {};
	void initializeLRobot(int type) {};
	bool checkLCollisions(std::shared_ptr <ER_Module> module, std::vector<int> exceptionHandles);

	int initializeCPPNEncoding(float initialPosition[3]);
	float getFitness();
	void init_noMorph();
	// object creators
	void createAtPosition(float x, float y, float z);
	bool checkJointModule();
    std::shared_ptr<Morphology> clone() const;

	void printSome();

	void savePhenotype(int ind, float fitness);

	float positionFirstObject[3] = { 0.0f, 0.0f, 0.1f };
	void initCustomMorphology();
	void incrementLSystem(); 
	virtual bool loadGenome(std::istream &input, int indNum) override;

	void setColors();

	float checkArea(float interSection[3], float pps[4][3]);
	void cubeDrawing(std::vector<std::vector<float>> rotatedPoints, float color[3]);
    std::vector<int> debugDrawings;
	bool checkCollisionBasedOnRotatedPoints(int objechHandle);
	int getAmountBrokenModules();
	//void shiftRobotPosition();
	void symmetryMutation(float mutationRate);
    std::vector<std::shared_ptr<ER_Module>> modules;
    std::shared_ptr<ModuleFactory> moduleFactory;

	int CANYOUFINDTHIS;

	void setPhenValue();
};

