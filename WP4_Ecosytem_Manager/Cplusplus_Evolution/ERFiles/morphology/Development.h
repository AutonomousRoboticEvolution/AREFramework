#pragma once
#include "BaseMorphology.h"
#include <memory>
#include <fstream>
#include "../module/ER_Module.h"
#include "../module/ModuleFactory.h"
//#include "../eigen/Eigen/Dense"
#include "../eigen/Eigen/Dense"
#include <vector>

using namespace std;
using namespace Eigen;

class Development :
	public BaseMorphology
{
public:
	Development();
	~Development();
//	typedef shared_ptr<ER_Module> ModulePointer;
	vector <shared_ptr<ER_Module>> getCreatedModules();
	int getAmountBrokenModules();

	void init();
	void mutate();
	void create();
	float callFitness();
	void update();

	void checkForceSensors();

	bool checkLCollisions(shared_ptr <ER_Module> module, vector<int> exceptionHandles);

	int mutateERLGenome(float mutationRate);
	int mutateControlERLGenome(float mutationRate);
	vector<float> checkCollisionReturnPos(int objectHandle, vector<float> rayOrigin, vector<float> rayVector);
	bool checkCollisionReturn(int objectHandle, vector<float> rayOrigin, vector<float> rayVector);
	//	ER_LSystem getMultiPurposeLSystemPointer();
	void init_noMorph();
	// object creators
	void createAtPosition(float x, float y, float z);

	vector<float> eulerToDirectionalVector(vector<float> eulerAngles);

	void checkControl(int individual, int sceneNum);
	shared_ptr<Morphology> clone() const;
	float fitness;
	// delete functions below???
	void printSome();
	int getMainHandle();

	float positionFirstObject[3] = { 0.0f, 0.0f, 0.1f };

	void initCustomMorphology();

	void saveGenome(int individual, int sceneNum, float fitness); // overrides baseMorphology function

	void loadGenome(int individualNumber, int sceneNum);
	//void recallAndCreate();
	void crossover(shared_ptr<Morphology>, float crossoverRate);

	struct OriginVector {
		vector<float> originVector;
	};

	struct CollisionPars {
		bool collision;
		float scalar;
		float collisionDistance;
	};

	struct PlanePoints {
		float points[4][3];
	};
	float checkArea(float interSection[3], float pps[4][3]);
	CollisionPars checkIntersection(float pps[4][3], float color[3], float rayVector[3], float rayOrigin[3], float vectorDirections[3]);
	void cubeDrawing(vector<vector<float>> rotatedPoints, float color[3]);
	vector<int> debugDrawings;
	bool checkCollisionBasedOnRotatedPoints(int objechHandle);

protected:

private:
	void checkGenome(int individualNumber, int sceneNum);
};


