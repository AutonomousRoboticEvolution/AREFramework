#pragma once

#include <algorithm>
#include <vector>
#include "Development.h"


class ER_CPPN_Encoding : public Development
{
public:
	ER_CPPN_Encoding();
	~ER_CPPN_Encoding();

	void init() override;
	void mutate() override;

	int initializeGenome(int type);
	int mutateCPPN(float mutationRate);
	int mutateControlERGenome(float mutationRate);
	bool queried = false;
	int maxIterations = 4;
    std::shared_ptr<Control> cppn;
	void deleteModuleFromGenome(int num);

	void checkControl(int individual, int sceneNum);
    std::shared_ptr<Morphology> clone() const override;
	void update() override;
	float fitness;
	void printSome() override;

	void initializeGenomeCustom(int type);
	void initializeQuadruped(int type);
	virtual const std::string generateGenome(int individual, float fitness) const override;
	float getFitness() override;
	bool loadGenome(std::istream &input, int individualNumber) override;
	void symmetryMutation(float mutationRate); 
	void crossover(std::shared_ptr<Morphology>, float crossoverRate) override;


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
        std::shared_ptr<MODULEPARAMETERS> clone() const {
			return std::make_unique<MODULEPARAMETERS>(*this);
		};
		bool expressed = true;
		int maxChilds;
		bool queried = false;

		// not stored in genome
        std::vector<int> childSiteStates; // which attachment site has which child object. -1 = no child
		float rgb[3];
		// parameter identifiers
		int handle;
		float color[3] = { 0.45f,0.45f,0.45f };
        std::shared_ptr<Control> control;
		float moduleColor[3];
		int type = -1; // cube, servo, leaf, etc.

		// for direct encoding
		int parent;
		int parentSite;
		int orientation;
	};

	struct GENOTYPE {
        std::shared_ptr<GENOTYPE> clone() const {
			return std::make_unique<GENOTYPE>(*this);
		};
        std::vector<std::shared_ptr<MODULEPARAMETERS>> moduleParameters; // one struct of parameters for each state
		//HORMONEGENOME hormoneGenome;
		
		int numberOfModules = 1; // initial amount of modules
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
    std::shared_ptr<GENOTYPE> genome;
	

private:
	void checkGenome(int individualNumber, int sceneNum);
};