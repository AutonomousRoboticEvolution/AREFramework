#pragma once
#include "FixedBaseMorphology.h"

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Body {
	vector<float> medians;
	vector<float> deviations;
};

struct Actuator {
	Vector3 coordinates;
};

struct Sensor {
	Vector3 coordinates;
};


class EdgarsAmazingMorphologyClass : public FixedBaseMorphology
{
public:
	EdgarsAmazingMorphologyClass();
	~EdgarsAmazingMorphologyClass();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	void create();

	void init();

	void saveGenome(int indNum, int sceneNum, float fitness);
	// this loads genome
	void loadGenome(int individualNumber, int sceneNum);

	vector<Actuator> actuators;
	vector<Sensor> sensors;
	vector<Body> bodies;

	vector<int> outputHandles;
	vector<int> outputValues;

};

