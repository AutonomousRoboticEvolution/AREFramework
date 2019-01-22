#pragma once
#include "CustomMorphology.h"
#include "v_repLib.h"

class CustomMorphologyVREP : public CustomMorphology
{
public:
	CustomMorphologyVREP();
	~CustomMorphologyVREP();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	int createWei();
	void create();

	void init();

	vector<int> outputHandles;
	vector<int> outputValues;

};

