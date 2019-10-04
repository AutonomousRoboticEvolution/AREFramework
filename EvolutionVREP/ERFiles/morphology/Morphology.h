#pragma once

#include <vector>
#include <memory>
//#include "../env/Environment.h" // impossible, environment already includes morphology
#include "../module/ER_Module.h"
#include "../control/Control.h"
#include "../control/ControlFactory.h"
#include "../../RandNum.h"
#include "../Settings.h"
#include "../../NEAT_LIB/NEAT_LIB/NeuralNetwork.h"


class Morphology // Abstract Class
{
public:
	Morphology(){};
	virtual ~Morphology();
    std::shared_ptr<Settings> settings;
    std::shared_ptr<RandNum> randomNum;
	virtual std::shared_ptr<Morphology> clone() const = 0;
    std::vector<std::shared_ptr<ER_Module> > createdModules;
	/**
		@brief This method initialize the morph
	*/
	virtual void init() = 0;
	virtual void init_noMorph() = 0;
	virtual void clearMorph() = 0;
	virtual void savePhenotype(int ind, float fitness) = 0;
	/**
		@brief This method mutate the morph
	*/
	virtual void mutate() = 0;
	/**
		@brief This method creates the morph
	*/
	virtual void create() = 0;
	virtual void createAtPosition(float x, float y, float z) = 0;

	/**
		@brief This method update the control of the morph?
	*/
	virtual void update() = 0; 


	// saving
	virtual void saveGenome(int indNum, float fitness) = 0;
	virtual const std::string generateGenome(int indNum, float fitness) const = 0;
	virtual float getFitness() = 0;

	// cloning
	virtual void saveBaseMorphology(int indNum, float fitness) = 0; // currently not used
//	virtual void saveBaseMorphology(int indNum, int sceneNum, float fitness) = 0; // currently not used
	virtual void loadBaseMorphology(int indNum, int sceneNum) = 0;
//	virtual void setMainHandlePosition(float position[3]) = 0;
//	virtual void createMorphology() = 0; // create actual morphology in init
	virtual void printSome() = 0; 

	virtual std::vector<int> getObjectHandles(int) =0;
	virtual std::vector<int> getJointHandles(int) =0;
	virtual std::vector<int> getAllHandles(int) = 0;

	virtual bool loadGenome(int individualNumber, int sceneNum) = 0;
	virtual bool loadGenome(std::istream &input, int individualNumber) = 0;
	virtual void crossover(std::shared_ptr<Morphology>, float crossoverRate) = 0;
//	virtual void checkControl(int individual, int sceneNum) =0;
	bool modular = false;
//	typedef shared_ptr<ER_Module> ModulePointer;

	// modular functions
	// This function is needed to implement fluid dynamics on modules...
	virtual std::vector <std::shared_ptr<ER_Module>> getCreatedModules() = 0;
	virtual int getAmountBrokenModules() = 0;

	float phenValue = -1.0;
	virtual void setPhenValue() = 0;


    std::vector<std::vector <int> > maxModuleTypes;
//	typedef shared_ptr<Control> ControlPointer;
	///control of part of morphology 
    std::shared_ptr<Control> control;
public:
	virtual int getMainHandle() = 0;
	float minimumHeight = 0;
	int amountIncrement = 1;
    std::shared_ptr<NEAT::NeuralNetwork> neat_net;
};
