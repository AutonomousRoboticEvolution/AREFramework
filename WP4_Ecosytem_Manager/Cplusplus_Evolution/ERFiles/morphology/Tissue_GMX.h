#pragma once
#include "FixedBaseMorphology.h"




class Tissue_GMX : public FixedBaseMorphology
{
public:
	Tissue_GMX();
	~Tissue_GMX();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	void create();

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

	struct Genome {
		// Placeholder for mutable parameters
	};
	shared_ptr<Genome> genome;

	void init();

	void saveGenome(int indNum, float fitness);
	// this loads genome
	bool loadGenome(int individualNumber, int sceneNum);

	vector<Actuator> actuators;
	vector<Sensor> sensors;
	vector<Body> bodies;

	vector<int> outputHandles;
	vector<int> outputValues;

};

