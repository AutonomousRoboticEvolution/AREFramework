#pragma once

#include "FixedBaseMorphology.h"


class CustomMorphology : public FixedBaseMorphology
{
public:
	CustomMorphology();
	~CustomMorphology();
	virtual std::vector<int> getObjectHandles(int parentHandle);
	virtual std::vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	void create();

	void mutate();

	void saveGenome(int indNum, float fitness);

    bool loadGenome(int individualNumber, int sceneNum);
	void init();
    std::shared_ptr<Morphology> clone() const;
    std::string name;
    std::vector<int> outputHandles;
    std::vector<int> outputValues;
    float fitness;
};

