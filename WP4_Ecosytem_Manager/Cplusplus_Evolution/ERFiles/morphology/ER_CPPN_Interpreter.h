#pragma once

#include <vector>;
#include "ER_CPPN_Encoding.h"
#include "../module/ER_Module.h"
#include "../module/ModuleFactory.h"


using namespace std;

class ER_CPPN_Interpreter : public ER_CPPN_Encoding
{
public:
	ER_CPPN_Interpreter();
	~ER_CPPN_Interpreter();
	vector<shared_ptr<ER_Module> > createdModules;

	void init();
	void create();
	void update(); 
	void updateColors();
	void checkForceSensors();
	void initializeGenomeCustom(int type) {};
	void initializeLRobot(int type) {};
	bool checkLCollisions(shared_ptr <ER_Module> module, vector<int> exceptionHandles);

	int initializeCPPNEncoding(float initialPosition[3]);
	float getFitness();
	void init_noMorph();
	// object creators
	void createAtPosition(float x, float y, float z);
	bool checkJointModule();
	shared_ptr<Morphology> clone() const;

	void printSome();
	int getMainHandle();

	void savePhenotype(int ind, float fitness);

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

	int CANYOUFINDTHIS;

	void setPhenValue();
};

