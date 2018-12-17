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

	void init();

	vector<int> outputHandles;
	vector<int> outputValues;

};

