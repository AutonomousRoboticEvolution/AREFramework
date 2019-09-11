#pragma once
//#include "FixedBaseMorphology.h"
#include "Component.h"

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


class Tissue_DirectBars : public Component	
{
public:
	Tissue_DirectBars();
	~Tissue_DirectBars();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	void create();

	void init();
	struct Genome {
		// Placeholder for mutable parameters
	};
	shared_ptr<Genome> genome;

	void saveGenome(int indNum, float fitness);
	// this loads genome
	bool loadGenome(int individualNumber, int sceneNum);

	vector<Actuator> actuators;
	vector<Sensor> sensors;
	vector<Body> bodies;

	vector<int> outputHandles;
	vector<int> outputValues;

};

