#pragma once

#include <vector>
#include "Development.h"


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
    std::shared_ptr<Morphology> clone() const;
	void update();
	float fitness;
	void printSome();

	void initializeGenomeCustom(int type);

    virtual const std::string generateGenome(int individual, float fitness) const override;
	float getFitness();
	bool loadGenome(std::istream &input, int individualNumber);
	void symmetryMutation(float mutationRate);
	int getNewSite(int maxCh, int currentSite, std::vector<int> sites);

	void crossover(std::shared_ptr<Morphology>, float crossoverRate);
protected:
	struct LPARAMETERS {
		// State specific parameters
        std::shared_ptr<LPARAMETERS> clone() const {
            std::shared_ptr <LPARAMETERS> lp = std::make_unique<LPARAMETERS>(*this);
			lp->control = this->control->clone();
			return lp;
		};
        /// The current state of the object; L-System state (Symbol)
		int currentState;
		/// Defines module type
        int type = -1; // cube, servo, leaf, etc.
        /// How many children need to be created
        int amountChilds;
        /// Defines which attachment site should attach a module
        std::vector <int>childSites;
        /// Defines what the which module is connected to the specific site.
        std::vector<int> childSiteStates;
        /// in which orientation the children should be placed
        std::vector<int> childConfigurations;

		/// Can store object handle
		int handle;
        /// Default rgb value
		float color[3] = { 0,0,0 };

        std::shared_ptr<Control> control;

	};

	struct LGENOME {
        std::shared_ptr<LGENOME> clone() const {
            std::shared_ptr<LGENOME> lg = std::make_unique<LGENOME>(*this);
			for (int i = 0; i < this->lParameters.size(); i++) {
				lg->lParameters[i] = this->lParameters[i]->clone();
			}
			return lg;
		};

		int amountStates = 5;
        std::vector<std::shared_ptr<LPARAMETERS>> lParameters; // one struct of parameters for each state

	};

public:
    std::shared_ptr<LGENOME> lGenome;
	

private:
	void checkGenome(int individualNumber, int sceneNum);
};