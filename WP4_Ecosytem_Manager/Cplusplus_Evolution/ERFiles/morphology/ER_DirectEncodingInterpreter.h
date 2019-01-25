#pragma once

#include <vector>;
#include "ER_DirectEncoding.h"
#include "../module/ER_Module.h"
#include "../module/ModuleFactory.h"


using namespace std;

class ER_DirectEncodingInterpreter : public ER_DirectEncoding
{
public:
	ER_DirectEncodingInterpreter();
	~ER_DirectEncodingInterpreter();
	bool checkJointModule();
	vector<shared_ptr<ER_Module> > createdModules;

	void init();
	void create();
	void update(); 
	void updateColors();
	void checkForceSensors();
	void initializeGenomeCustom(int type) {};
	void initializeLRobot(int type);
	bool checkLCollisions(shared_ptr <ER_Module> module, vector<int> exceptionHandles);

	int initializeDirectEncoding(float initialPosition[3]);
	float getFitness();
	void init_noMorph();
	// object creators
	void createAtPosition(float x, float y, float z);
	shared_ptr<Morphology> clone() const;

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
	void savePhenotype(int ind, float fitness);
	//void loadPhenotype(int ind);
};

