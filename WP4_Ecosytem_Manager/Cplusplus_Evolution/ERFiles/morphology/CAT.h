#pragma once
#include "FixedBaseMorphology.h"

class CAT : public FixedBaseMorphology
{
public:
	CAT();
	~CAT();
	float fitness;
	void init_noMorph();
	virtual	void init();
	virtual void grow(int);
	void update() {};
	void create() {};


//	virtual void mutate(float mutationRate){};
	virtual	void setMainHandlePosition(float position[3]) {};
	virtual int getMainHandle();
	void saveGenome(int indNum, float fitness);
	bool loadGenome(int individualNumber, int sceneNum);
	int catHandle;
	shared_ptr<Morphology> clone() const;
	void mutate();
	int floorHandle;
	vector <int> feet;
	vector <int> jointHandles;

	struct VestibularAttributes {
		shared_ptr<VestibularAttributes> clone() const {
			return make_unique<VestibularAttributes>(*this);
		};
		float headPosition[3];
		float headOrientation[3];
		float errorHeadPosition[3];
		float previousHeadPosition[3];
		float headAcceleration[3];

		float headRotation[3];
		float errorHeadRotation[3];
		float headRotAcceleration[3];
		float previousHeadRotation[3]; 

		float normalizedHeadOrientation[6];
	};
	shared_ptr<VestibularAttributes> va;
};

