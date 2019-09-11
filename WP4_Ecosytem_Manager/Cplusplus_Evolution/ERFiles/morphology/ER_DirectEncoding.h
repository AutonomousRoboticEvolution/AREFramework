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

	// Essentials
	void init();
	void mutate();
    void update();
    shared_ptr<Morphology> clone() const;

	void initializeRobot(int type);
    int mutateControlERGenome(float mutationRate);

    /// Checks the controller of the module
	void checkControl(int individual, int sceneNum);
	float fitness;
	/// Can be used to print some debugging information
	void printSome();
    /// Can be used if you want to manually create a direct encoding genome
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
	struct MODULEPARAMETERS {
		// State specific parameters
		shared_ptr<MODULEPARAMETERS> clone() const {
			shared_ptr < MODULEPARAMETERS> mp = make_unique<MODULEPARAMETERS>(*this);
			mp->control = this->control->clone();
			return mp;
		};
		/// This means that a phenotype of this module is created and it's children have been attached
		bool expressed = true;
	
		// not stored in genome
		vector<int> childSiteStates; // which attachment site has which child object. -1 = no child 
		float rgb[3];
		// parameter identifiers
        int type = -1; // cube, servo, leaf, etc.
		int handle;
		float color[3] = { 0.45f,0.45f,0.45f };
        float moduleColor[3];

		shared_ptr<Control> control;

		/// ID (position in 'moduleParameters' vector) of the particular module this module should connect to
		int parent;
		/// Every module has a number of connection sites,
		/// this defines to which connection site this particular module is attached
		int parentSite;
		/// 4 possible orientations
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

		int numberOfModules = 1; // initial number of modules
	};

public:
	shared_ptr<GENOTYPE> genome;
	bool checkIfLocationIsOccupied(vector<shared_ptr<MODULEPARAMETERS>> mps, int parentSite, int parent);

private:
	void checkGenome(int individualNumber, int sceneNum);
    int mutateERGenome(float mutationRate);
    int initializeGenome(int type);
    int checkTreeDepth(int attachModule, int increment);
    void deleteModuleFromGenome(int num);

};