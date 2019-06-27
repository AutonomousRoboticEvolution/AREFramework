#pragma once

#include <vector>;
#include "Development.h"
#include <algorithm>
#include "NEAT_LIB/NeuralNetwork.h"
#include "../module/ER_Module.h"
#include "../module/ModuleFactory.h"

using namespace std;

class NEAT_CPPN_Encoding : public Development
{
public:
	NEAT_CPPN_Encoding();
	~NEAT_CPPN_Encoding();

	void init();
	void mutate();

	int initializeGenome(int type);
	int mutateCPPN(float mutationRate);
	int mutateControlERGenome(float mutationRate);
	bool queried = false;
	int maxIterations = 4;
	shared_ptr<Control> cppn;
	void deleteModuleFromGenome(int num);

	vector<shared_ptr<ER_Module>> modules; // placeholder for modules. Not sure anymore why this is different from createdModules

	void checkControl(int individual, int sceneNum);
	void createAtPosition(float x, float y, float z);
	bool checkLCollisions(shared_ptr<ER_Module> module, vector<int> exceptionHandles);
	void checkForceSensors();
	int initializeCPPNEncoding(float initialPosition[3]);
	shared_ptr<Morphology> clone() const;
	void update();
	void updateColors();
	void setColors();
	float fitness;
	void printSome();

	void initializeGenomeCustom(int type);
	void initializeQuadruped(int type);
	virtual const std::string generateGenome(int individual, float fitness) const override;
	float getFitness();
	void init_noMorph();
	void setPhenValue();
	void initCustomMorphology(); // not used
	bool loadGenome(std::istream &input, int individualNumber);
	void create();
	bool checkJointModule();
	int getMainHandle();
	void savePhenotype(int ind, float fitness);
	int getAmountBrokenModules();
	float checkArea(float interSection[3], float pps[4][3]);
	bool checkCollisionBasedOnRotatedPoints(int objectHandle);
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
		bool expressed = true;
		int maxChilds;
		bool queried = false;

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
	shared_ptr<GENOTYPE> robot_genome; // genome of the robot
	NEAT::NeuralNetwork *neat_NN; // pointer to the network received by NEAT. This is a phenotype

private:
	void checkGenome(int individualNumber, int sceneNum);
};