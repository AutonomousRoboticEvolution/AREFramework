#pragma once

#include <vector>;
#include "Development.h"

using namespace Eigen;
using namespace std;


class ER_LSystem : public Development
{
public:
	ER_LSystem();
	~ER_LSystem();

	void init();
	void mutate();

	int initializeLGenome(int type);
	int mutateERLGenome(float mutationRate);
	int mutateControlERLGenome(float mutationRate);

	void checkControl(int individual, int sceneNum);
	shared_ptr<Morphology> clone() const;
	void update();
	float fitness;
	void printSome();

	void initializeGenomeCustom(int type);
	void initializeQuadruped(int type);
	void initializeSolar(int type);
	virtual const std::string generateGenome(int individual, float fitness) const override;
	float getFitness();
	bool loadGenome(std::istream &input, int individualNumber);
	void symmetryMutation(float mutationRate);
	int getNewSite(int maxCh, int currentSite, vector<int> sites);

	void crossover(shared_ptr<Morphology>, float crossoverRate);
protected:
	enum OBJECTTYPE {
		CUBE, 
		BEND, 
		PANEL
	};
	enum CONNECTTYPE {
		FORCE,
		PRISMATIC
	};

	struct LPARAMETERS {
		// State specific parameters
		shared_ptr<LPARAMETERS> clone() const {
			shared_ptr <LPARAMETERS> lp = make_unique<LPARAMETERS>(*this);
			lp->control = this->control->clone();
			return lp;
		};
		int currentState;		// The current state of the object
		int newState;			// The state of the object can change in this particular state. 

		int amountChilds;		// How much childs need to be created 
		int connectType;

		vector<int> childStates; // -1 = no child able
		int childObjectType; // cube, light-Sensor, sphere
		int childConnectType; // joint, force sensor
		// int maxChilds; //

		vector<int> attachmentSites;
		vector<int> attachmentObjects; // corresponds to the object number used to identify a specific object within a state. 0 is the default parent. 
		vector<int> childSiteStates; // which attachment site has which child object. -1 = no child 
		vector<int> childConfigurations; 
		float rgb[3];
		// parameter identifiers
		int handle;	
		float color[3] = { 0,0,0 };
		vector <int>childSites;
		shared_ptr<Control> control;
		float moduleColor[3];
		int type = -1; // cube, servo, leaf, etc.
	};

	struct LGENOME {
		shared_ptr<LGENOME> clone() const {
			shared_ptr<LGENOME> lg = make_unique<LGENOME>(*this);
			for (int i = 0; i < this->lParameters.size(); i++) {
				lg->lParameters[i] = this->lParameters[i]->clone();
			}
			return lg;
		};
		int amountStates = 5;
		int maxAmountStates = 10;
		int startingState = 0;
		vector<shared_ptr<LPARAMETERS>> lParameters; // one struct of parameters for each state 
		//HORMONEGENOME hormoneGenome;
		int maxObjects = 50;
		int mask1 = 65281;
		int mask2 = 65282;
		int amountIncrement = 5; // 7 was normal
		vector<int> lightSensorHandles;
		int maxIt = 200;
		int currentIt = 0;
		float maxVolume = 1;
		float mutationRate = 0.05;
		int dynamic = 16;
		bool useSymmetryMutation = true;
		bool crossover = true;
		float crossoverRate = 0.1; // at every state, a crossover can occur
								   //void deleteLParameters() { // for when it turns out that lParameters needs to store pointers
								   //	for (int i = 0; i < lParameters.size(); i++) {
								   //		delete lParameters[i];
								   //	
								   //} 
		//LGENOME*::clone() const
		//	LGENOME* clone() const;
	};

public:
	shared_ptr<LGENOME> lGenome;
	

private:
	void checkGenome(int individualNumber, int sceneNum);
};