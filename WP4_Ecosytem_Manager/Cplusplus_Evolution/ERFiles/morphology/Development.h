#pragma once
#include "BaseMorphology.h"
#include <memory>
#include <fstream>
//#include "../module/ER_Module.h"
//#include "../module/ModuleFactory.h"
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
//	vector <shared_ptr<ER_Module>> getCreatedModules();
	int getAmountBrokenModules();

	struct BASEMODULEPARAMETERS {
		// State specific parameters
		shared_ptr<BASEMODULEPARAMETERS> clone() const {
			return make_unique<BASEMODULEPARAMETERS>(*this);
		};
		bool expressed = true;
		int maxChilds;

		// not stored in genome
		vector<int> childSiteStates; // which attachment site has which child object. -1 = no child 
		float rgb[3];
		// parameter identifiers
		int handle;
		float color[3] = { 0.45f,0.45f,0.45f };
		shared_ptr<Control> control;
		float moduleColor[3];
		int type = -1; // cube, servo, leaf, etc.

		// for direct encoding
		int parent;
		int parentSite;
		int orientation;
	};
	
	void savePhenotype(vector<shared_ptr<BASEMODULEPARAMETERS>> createdModules, int indNum, float fitness);
	void savePhenotype(int ind, float fitness) {};

	vector<shared_ptr<BASEMODULEPARAMETERS>> loadBasePhenotype(int indNum);
	void loadPhenotype(int ind) {}; 


	void init();
	void mutate();
	void create();
	float callFitness();
	void update();

//	void checkForceSensors();
//	bool checkLCollisions(shared_ptr <ER_Module> module, vector<int> exceptionHandles);

	int mutateERLGenome(float mutationRate);
	int mutateControlERLGenome(float mutationRate);
//	vector<float> checkCollisionReturnPos(int objectHandle, vector<float> rayOrigin, vector<float> rayVector);
//	bool checkCollisionReturn(int objectHandle, vector<float> rayOrigin, vector<float> rayVector);
	//	ER_LSystem getMultiPurposeLSystemPointer();
	void init_noMorph();
	int getMaxChilds(int moduleType);
	//vector<shared_ptr<ER_Module>> loadPhenotype(int indNum);
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

	virtual void saveGenome(int individual, float fitness) override;
	virtual bool loadGenome(int individualNumber, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int individualNumber) override;

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
//	float checkArea(float interSection[3], float pps[4][3]);
//	CollisionPars checkIntersection(float pps[4][3], float color[3], float rayVector[3], float rayOrigin[3], float vectorDirections[3]);
//	void cubeDrawing(vector<vector<float>> rotatedPoints, float color[3]);
//	vector<int> debugDrawings;
//	bool checkCollisionBasedOnRotatedPoints(int objechHandle);

protected:

private:
	void checkGenome(int individualNumber, int sceneNum);
};


