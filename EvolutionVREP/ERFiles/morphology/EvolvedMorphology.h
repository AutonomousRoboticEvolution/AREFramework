#pragma once

#include <memory>
#include <fstream>
#include <vector>
#include "BaseMorphology.h"


class EvolvedMorphology : public BaseMorphology
{
public:
	EvolvedMorphology();
	~EvolvedMorphology();
//	typedef shared_ptr<ER_Module> ModulePointer;
//	vector <shared_ptr<ER_Module>> getCreatedModules();
	int getAmountBrokenModules() override;
	/// Holds information of the modules created (phenotype)
    std::vector<std::shared_ptr<ER_Module> > createdModules;
    int getMainHandle() override;
	struct BASEMODULEPARAMETERS {
		// State specific parameters
        std::shared_ptr<BASEMODULEPARAMETERS> clone() const {
			return std::make_unique<BASEMODULEPARAMETERS>(*this);
		};
		bool expressed = true;
		int maxChilds;

		// not stored in genome
        std::vector<int> childSiteStates; // which attachment site has which child object. -1 = no child
		float rgb[3];
		// parameter identifiers
		int handle;
		float color[3] = { 0.45f,0.45f,0.45f };
        std::shared_ptr<Control> control;
		float moduleColor[3];
		int type = -1; // cube, servo, leaf, etc.

		// for direct encoding
		int parent;
		int parentSite;
		int orientation;
		float absPos[3];
        float absOri[3];
	};
    // Override methods
    void init() override;
    void mutate() override;
    void create() override;
    void update() override;
    void createAtPosition(float x, float y, float z) override;
    virtual void saveGenome(int individual, float fitness) override;
    virtual bool loadGenome(int individualNumber, int sceneNum) override;
    virtual bool loadGenome(std::istream &input, int individualNumber) override;

	void savePhenotype(std::vector<std::shared_ptr<BASEMODULEPARAMETERS>> createdModules, int indNum, float fitness);
	void savePhenotype(int ind, float fitness) override {};

    std::vector<std::shared_ptr<BASEMODULEPARAMETERS>> loadBasePhenotype(int indNum);
	void loadPhenotype(int ind) {};

	int mutateERLGenome(float mutationRate);
	int mutateControlERLGenome(float mutationRate);

	void init_noMorph() override;
	int getMaxChilds(int moduleType);
	//vector<shared_ptr<ER_Module>> loadPhenotype(int indNum);
	// object creators

//    std::vector<float> eulerToDirectionalVector(std::vector<float> eulerAngles);

	void checkControl(int individual, int sceneNum);
    std::shared_ptr<Morphology> clone() const override;
	float fitness;
	// delete functions below???
	void printSome() override;

	void updateCreatedModules();

	/// Function that loops through all vertices of shapes and moves robot upward based on the lowest point of the robot.
	void shiftRobotPosition();

	float positionFirstObject[3] = { 0.0f, 0.0f, 0.1f };

	void initCustomMorphology();

	//void recallAndCreate();
	void crossover(std::shared_ptr<Morphology>, float crossoverRate) override;

	struct OriginVector {
        std::vector<float> originVector;
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
    /// Check whether two components are colliding
    bool checkLCollisions(std::shared_ptr <ER_Module> module, std::vector<int> exceptionHandles);
    /// Viability methods
    /// Check for collisions. If there is a colliding object, remove it from the genome representation.
    bool bCheckCollision(int iParentHandle, int createdModulesSize);
    /// Check for ground. If object is above the ground, it can be created
    bool bCheckGround(int createdModulesSize);
    /// Check for orientation. If the orientation of the organ is printable
    bool bCheckOrientation(int createdModulesSize);
    /// Check of number of organs.
    bool bCheckOrgansNumber(int createdModulesSize);

protected:

private:
	void checkGenome(int individualNumber, int sceneNum);
};

//#include "BaseMorphology.h"


