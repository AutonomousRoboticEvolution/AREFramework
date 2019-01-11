#pragma once
#include <vector>;
#include "VREPFunctions.h"
#include "BaseMorphology.h"
#include "eigen\Eigen\Dense"
#include <memory>
#include <fstream>

using namespace Eigen;
using namespace std;

struct HORMONESTATEPARAMETERS {
	bool hormoneProducer;
	bool hormoneReceiver;
	float hormoneAmount;
	float rgbAdjustment[3];
	float hormoneStateThreshold;
	float newState;
};

struct HORMONEPARAMETERS { // parameters per state
	float hormoneDiffusionRate;
	float hormoneProductionRate;
	int directionalDiffusion; // 0 = both ways, 1 = only up branch, 2 = only down.

};

struct HORMONEGENOME {
	int amountHormones = 2;
	vector<HORMONEPARAMETERS> hormoneParameters;
};

struct LPARAMETERS {
	float size[3];
	float position[3];
	float orientation[3];
	float mass;
	float rotate;
	bool rotateParentJoint;
	int currentState;
	int newState; // it can transform to another state after iterations
	int stateType; // cube, sphere, light-Sensor
	int amountChilds;
	float childOrientationShift[3]; // if multiple childs, children are place at specific angles. // angle is added to orientation
	vector<int> childStates; // -1 = no child able
	int childObjectType; // cube, light-Sensor, sphere
	int childConnectType; // joint, force sensor
	float rgb[3];
	// parameter identifiers
	int handle;
	// adaptive parameters:
	//bool growth = false; 
	//float growthRate = 0.01; 
	//int amountChild;
	//float rotateNextChild; 
	//float gravityVector[3]; 
	vector<HORMONESTATEPARAMETERS> hormoneStateParameters;
};

struct LGENOME {
	int amountStates = 10;
	int startingState = 0;
	vector<LPARAMETERS> lParameters; // one struct of parameters for each state 
	HORMONEGENOME hormoneGenome;
	int maxObjects = 50;
	int mask1 = 65281;
	int mask2 = 65282;
	int amountIncrement = 1; // 7 was normal
	vector<int> lightSensorHandles;
	int maxIt = 200;
	int currentIt = 0;
	float maxVolume = 1;
	float mutationRate = 0.05;
	//void deleteLParameters() { // for when it turns out that lParameters needs to store pointers
	//	for (int i = 0; i < lParameters.size(); i++) {
	//		delete lParameters[i];
	//	
	//}
	int initializeLGenome();
	//LGENOME*::clone() const
	//	LGENOME* clone() const;
};

struct OPTIONS {
	bool includeCubes = true;
	bool includeSpheres = false;
	bool includeForceSensors = false;
	bool includeJoints = true;
	int amountStates = 0;
	int fitnessFunction = 0;
	int amountIncrements;
};

class MultiPurposeLSystemV2 : public BaseMorphology
{
public:
	MultiPurposeLSystemV2();
	~MultiPurposeLSystemV2();

	void init();
	void mutate();
	void create();
	void increment(int amountIncrement);
	float callFitness();

	LGENOME lGenome;

	int initializeLGenome(int type);
	int initializeLGenomeWithHormones(int type);
	int mutateLGenome(float mutationRate);
	int mutateLGenomeWithHormones(float mutationRate);
	int initializeLSystem(int increments, float initialPosition[3]);
	int continueLSystem(int increments);
	int initializeHormones(int amountStates);
	int mutateHormones(float mutationRate);

	void init_noMorph();
	// object creators
	int createFirstObject(float[3]);
	int createConnector(int parentState, int parentHandle, int shift);
	int createObject(int parentState, int parentHandle, int newState);
	int createSphere(int parentState, int parentHandle, int newState);
	void createAtPosition(float x, float y, float z);

	vector<float> eulerToDirectionalVector(vector<float> eulerAngles);

	unique_ptr<Morphology> clone() const;
	float fitness;
	bool createRay(int object1, int object2);
	float createRays(int object1, int object2, int amountRays);
	// delete functions below???

	//	void initializeDefaultLProperties();
	//	void randomlyMutateLProperties(float mutationRate);
	void lSystem(int amountIncrement, int parentHandle, int parentState);
	void modularLSystem(int amountIncrement, int parentHandle, int parentState, int moduleFunction);

	void lSystemJointAndCube(int amountIncrement, int parentHandle, int parentState);
	void continueLSystemJointAndCube(int amountIncrement, int parentHandle, int parentState);

	void lSystemNoJoints(int amountIncrement, int parentHandle, int parentState);
	void continueLSystemNoJoints(int amountIncrement, int parentHandle, int parentState);
	ObjectCreator vcreate;
	ObjectRemover vremove;
	SetVREPInfo vset;
	RetrieveVREPInfo vret;
	float evaluate(int sceneNum);
	void increment();
	void saveGenome(int individual, int sceneNum, float fitness);
	void rotate();
	//	void deleteMorphology();
	vector<float> calculateJointPosition(int parentHandle, int newJoint);
	vector<float> calculateJointSpherePosition(int parentHandle, int newJoint);
	void initializeLSystemLightSensors(int amountIncrement, int parentHandle, int parentState);
	bool checkCollision(int objectHandle, int parentHandle, bool withParent, vector<int> lHandles);
	void loadGenome(int individualNumber, int sceneNum);
	//void recallAndCreate();
	int printGenome();
	void setColors();

protected:


	vector<vector<vector<float>>> lStateProperties; //	0 = length, 1 = width, 2 = height, 3 = x, 4 = y, 5 = z, 6 = rotX, 7 = rotY, 8 = rotZ, 9 = mass, 10 = newState, 11 = childState, 12 = color 1, 13 = color 2, 14 = color 3

													//	0 = length
													//	1 = width
													//	2 = height
													//	3 = x
													//	4 = y
													//	5 = z
													//	6 = rotX
													//	7 = rotY
													//	8 = rotZ
													//	9 = mass
													//	10 = newState
													//	11 = childState
													//	12 = color 1
													//	13 = color 2
													//	14 = color 3
													//	15 = amount childs; not implemented yet

private:
	float randFloat(float lower, float upper);

};