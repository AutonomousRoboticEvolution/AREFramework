#pragma once

#include <vector>;
#include "Development.h"
#include <algorithm>

using namespace std;


class ER_DirectEncoding : public Development
{
public:
	ER_DirectEncoding();
	~ER_DirectEncoding();

	void init();
	void mutate();


	int initializeGenome(int type);
	void initializeRobot(int type);
	int mutateERGenome(float mutationRate);
	int checkTreeDepth(int attachModule, int increment);
	int mutateControlERGenome(float mutationRate);

	void deleteModuleFromGenome(int num);

	void checkControl(int individual, int sceneNum);
	shared_ptr<Morphology> clone() const;
	void update();
	float fitness;
	void printSome();

	void initializeGenomeCustom(int type);
	void initializeQuadruped(int type);
	virtual const std::string generateGenome(int individual, float fitness) const override;
	float getFitness();
	void loadPhenotype(int ind);
	bool loadGenome(std::istream& input, int individualNumber);
	void setGenomeColors();
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
			shared_ptr < MODULEPARAMETERS> mp = make_unique<MODULEPARAMETERS>(*this);
			mp->control = this->control->clone();
			return mp;
		};
		bool expressed = true;
	
		// not stored in genome
		vector<int> childSiteStates; // which attachment site has which child object. -1 = no child 
		float rgb[3];
		// parameter identifiers
		int handle;	
		float color[3] = { 0.45f,0.45f,0.45f };
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
			shared_ptr<GENOTYPE> ug = make_unique<GENOTYPE>(*this);
			for (int i = 0; i < this->moduleParameters.size(); i++) {
				ug->moduleParameters[i] = this->moduleParameters[i]->clone();
			}
			return ug;
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
	bool checkIfLocationIsOccupied(vector<shared_ptr<MODULEPARAMETERS>> mps, int parentSite, int parent);

private:
	void checkGenome(int individualNumber, int sceneNum);
};