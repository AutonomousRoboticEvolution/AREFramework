#pragma once
#include "EdgarsAmazingMorphologyClass.h"

class EdgarsAmazingMorphologyClassVREP : public EdgarsAmazingMorphologyClass
{
public:
	EdgarsAmazingMorphologyClassVREP();
	~EdgarsAmazingMorphologyClassVREP();
	virtual vector<int> getObjectHandles(int parentHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	void update();
	virtual int getMainHandle();
	int morphologyInterpreter();// createWei();
	void create();

	void init();
	void loadGenome(int individualNumber, int sceneNum);
	//int morphologyInterpreter();

	vector<int> outputHandles;
	vector<int> outputValues;

};

