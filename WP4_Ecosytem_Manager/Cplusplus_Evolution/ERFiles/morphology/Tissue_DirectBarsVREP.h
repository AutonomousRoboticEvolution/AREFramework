#pragma once
#include "Tissue_DirectBars.h"

struct Organs {
	vector<float> coordinates;
	vector<float> orientations;
};

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

	int createRobot();

	void create();

	void init();

	void mutate();

	void saveGenome(int indNum, float fitness);
	// this loads genome
	bool loadGenome(int individualNumber, int sceneNum);

	void initMorphology();

	void mutateMorphology(float mutationRate);

	vector<int> outputHandles;
	vector<int> outputValues;

	// Mutable parameters
	vector<Organs> organs; // Coordinates and orientations
	int organsNumber;

	float fitness; //TODO Do I need this variable?

};
