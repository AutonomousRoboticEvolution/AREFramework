#pragma once
#include "CAT.h"

class CATVREP : public CAT
{
public:
	CATVREP();
	~CATVREP();
	virtual void createMorphology() {};
//	virtual void mutate(float mutationRate){};
	virtual vector<int> getObjectHandles(int catHandle);
	virtual vector<int> getJointHandles(int parentHandle);
	virtual int getMainHandle();
	void update();
	int catHandle;
	shared_ptr<Morphology> clone() const;
	void create();
	vector <float> catInputs();
	vector <float> vestibularUpdate();
	vector <float> tactileUpdate();
	int floorHandle;
	vector <int> feet;
	vector <float> proprioUpdate();
	vector <int> jointHandles;
	vector<int> outputHandles;
	vector<int> outputValues;


};

