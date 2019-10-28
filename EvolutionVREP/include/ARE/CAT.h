#pragma once
#ifndef CAT_H
#define CAT_H

#include "ARE/FixedBaseMorphology.h"


class CAT : public FixedBaseMorphology
{
public:
	CAT();
	~CAT();
    CAT(const Settings& st);
    float fitness;
	void init_noMorph();
	/**
		@brief This method creates the morphology and its control
	*/
	virtual	void init();
	virtual void grow(int);
	/**
		@brief This method updates the control and its position
	*/
	void update() {};
	void create() {};  //empty function


//	virtual void mutate(float mutationRate){};
	virtual	void setMainHandlePosition(float position[3]) {};
	virtual int getMainHandle();
	void saveGenome(int indNum, float fitness);
	bool loadGenome(int individualNumber, int sceneNum);
	int catHandle;
    std::shared_ptr<Morphology> clone() const;
	/**
		@brief This method mutates the controller
	*/
	void mutate();
	int floorHandle;
    std::vector <int> feet;
    std::vector <int> jointHandles;

	struct VestibularAttributes {
        std::shared_ptr<VestibularAttributes> clone() const {
			return std::make_unique<VestibularAttributes>(*this);
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
    std::shared_ptr<VestibularAttributes> va;
};

#endif //CAT_H
