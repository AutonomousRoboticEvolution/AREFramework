#pragma once

#include "CAT.h"


class CATVREP : public CAT
{
public:
	CATVREP();
	~CATVREP();
	virtual void createMorphology() {};
//	virtual void mutate(float mutationRate){};
	virtual std::vector<int> getObjectHandles(int catHandle);
	virtual std::vector<int> getJointHandles(int parentHandle);
	virtual int getMainHandle();
	void update();
	int catHandle;
    std::shared_ptr<Morphology> clone() const;
	void create();  ///create a actual cat in v-rep
    std::vector<float> catInputs();
    std::vector<float> vestibularUpdate();
    std::vector<float> tactileUpdate();
	int floorHandle;
    std::vector<int> feet;
    std::vector<float> proprioUpdate();
    std::vector<int> jointHandles;
    std::vector<int> outputHandles;
    std::vector<int> outputValues;


};

