#pragma once

#include <vector>;
#include "BaseMorphology.h"
#include <algorithm>

using namespace std;


class ER_CPPN_Encoding : public BaseMorphology
{
public:
	ER_CPPN_Encoding();
	~ER_CPPN_Encoding();

	void init();
	void mutate();

	int initializeGenome(int type);
	int mutateCPPN(float mutationRate);
	int mutateControlERGenome(float mutationRate);
	bool queried = false;
	int maxIterations = 4;
	shared_ptr<CPPN> cppn;
	void deleteModuleFromGenome(int num);

	void checkControl(int individual, int sceneNum);
	shared_ptr<Morphology> clone() const;
	void update();
	float fitness;
	void printSome();

	void initializeGenomeCustom(int type);
	void initializeQuadruped(int type);
	void saveGenome(int individual, int sceneNum, float fitness); // overrides baseMorphology function
	float getFitness();
	void loadGenome(int individualNumber, int sceneNum);
	void symmetryMutation(float mutationRate); 
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

	struct MODULEPARAMETERS {
		// State specific parameters
		shared_ptr<MODULEPARAMETERS> clone() const {
			return make_unique<MODULEPARAMETERS>(*this);
		};
		int currentState;		// The current state of the object
		int newState;			// The state of the object can change in this particular state. 
		bool queried = false;
		int amountChilds;		// How many childs need to be created 
		int connectType;

		bool expressed = true;
	
		vector<int> childStates; // -1 = no child able
		int childObjectType; // cube, light-Sensor, sphere
		int childConnectType; // joint, force sensor
		int maxChilds; 

		vector<int> attachmentSites;
		vector<int> attachmentObjects; // corresponds to the object number used to identify a specific object within a state. 0 is the default parent. 
		vector<int> childSiteStates; // which attachment site has which child object. -1 = no child 
		vector<int> childConfigurations; 
		float rgb[3];
		// parameter identifiers
		int handle;	
		float color[3] = { 0.45f,0.45f,0.45f };
		vector <int>childSites;
		shared_ptr<Control> control;
		float moduleColor[3];
		int type = -1; // cube, servo, leaf, etc.

		// for direct encoding
		int parent;
		int parentSite; 
		int orientation;
	};

	struct GENOTYPE {
		shared_ptr<GENOTYPE> clone() const {
			return make_unique<GENOTYPE>(*this);
		};
		vector<shared_ptr<MODULEPARAMETERS>> moduleParameters; // one struct of parameters for each state 
		//HORMONEGENOME hormoneGenome;
		
		int amountModules = 1; // initial amount of modules
		int mask1 = 65281;
		int mask2 = 65282;
		int maxIt = 200;
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
	shared_ptr<GENOTYPE> genome;
	

private:
	void checkGenome(int individualNumber, int sceneNum);
};