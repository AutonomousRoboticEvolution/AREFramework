#pragma once
#include "Tissue_DirectBars.h"


//struct Vector3 {
//    float x;
//    float y;
//    float z;
//};

struct Gaussians {
	vector<float> medians;
	vector<float> sigmas;
};

struct Organs {
	vector<float> coordinates;
};

//struct Sensors {
//    Vector3 coordinates;
//};


class Tissue_DirectBarsVREP : public Tissue_DirectBars
{
public:
	Tissue_DirectBarsVREP();
	~Tissue_DirectBarsVREP();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();

	int createRobot();

	void create();

	void init();

	void mutate();

	void saveGenome(int indNum, int sceneNum, float fitness);
	// this loads genome
	bool loadGenome(int individualNumber, int sceneNum);

	void initMorphology();

	void mutateMorphology(float mutationRate);

	vector<int> outputHandles;
	vector<int> outputValues;

	vector<Organs> organs;
	vector<Gaussians> gaussians;

	float fitness; //TODO Do I need this variable?

};
