#pragma once

#include "CustomMorphology.h"
#include "v_repLib.h"


class CustomMorphologyVREP : public CustomMorphology
{
public:
	CustomMorphologyVREP();
	~CustomMorphologyVREP();
	virtual std::vector<int> getObjectHandles(int parentHandle);
	virtual std::vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	void create();


    std::shared_ptr<Morphology> clone() const;

    std::vector<int> sensorHandles;
    std::vector<int> outputHandles;
    std::vector<int> outputValues;
};

