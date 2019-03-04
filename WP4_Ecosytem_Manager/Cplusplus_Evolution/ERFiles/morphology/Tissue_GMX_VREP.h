#pragma once
#include "Tissue_GMX.h"

//struct Vector3 {
//    float x;
//    float y;
//    float z;
//};



//struct Sensors {
//    Vector3 coordinates;
//};


class Tissue_GMX_VREP : public Tissue_GMX
{
public:
	Tissue_GMX_VREP();
	~Tissue_GMX_VREP();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();

	shared_ptr<Morphology> clone() const;

	struct Gaussians {
		vector<float> medians;
		vector<float> sigmas;
	};

	struct Organs {
		vector<float> coordinates;
		vector<float> orientations;
	};

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
	vector<Organs> organs;
	vector<Gaussians> gaussians;
	int organsNumber;

	float fitness; //TODO Do I need this variable?

};
