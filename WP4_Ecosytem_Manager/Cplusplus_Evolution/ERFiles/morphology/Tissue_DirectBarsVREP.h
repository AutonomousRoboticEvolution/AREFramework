#pragma once
#include "Tissue_DirectBars.h"
#include "Organ.h"
#include "Viability.h"

class Tissue_DirectBarsVREP : public Tissue_DirectBars
{
public:
	Tissue_DirectBarsVREP();
	~Tissue_DirectBarsVREP();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	shared_ptr<Morphology> clone() const;

	virtual int getMainHandle();

	int createMorphology();

	void create();

	void init();

	void mutate();

	void saveGenome(int indNum, float fitness);
	// this loads genome
    //bool loadGenome(std::istream &genomeInput, int individualNumber);
	bool loadGenome(int individualNumber, int sceneNum);

	void initMorphology();

	void mutateMorphology(float mutationRate);

	int createSkeleton();

	// Viability functions
    // struct viabilityStruct {
    //     // This method checks if there is at least one organ of each type
    //     int checkOrgansType(vector<Organ> organs);
    //     int printVolume(vector<float> coordinates);
    //     int collisionDetector(vector<int> allComponents, int componentHandle);
    //     int createTemporalGripper(Organ organ);
    //     void destroyTemporalGripper(int gripperHandle);
    // } viability;
	vector<int> outputHandles;
	vector<int> outputValues;

	shared_ptr<Viability> viability;
	// Mutable parameters
	vector<Organ> organs; // Coordinates and orientations
	int organsNumber;

	float fitness; //TODO Do I need this variable?



};
