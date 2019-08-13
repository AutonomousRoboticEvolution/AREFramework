#pragma once
#include "FixedBaseMorphology.h"
class CustomMorphology : public FixedBaseMorphology
{
public:
	CustomMorphology();
	~CustomMorphology();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	void create();

	void mutate();

	void saveGenome(int indNum, float fitness);

    bool loadGenome(int individualNumber, int sceneNum);
	void init();
	shared_ptr<Morphology> clone() const;
	string name;
	vector<int> outputHandles;
	vector<int> outputValues;
    float fitness;

};

