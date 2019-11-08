#pragma once
#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <iostream>
#include <vector>
#include <memory>
#include <multineat/Substrate.h>
//#include "ARE/ER_Module.h"
#include "misc/RandNum.h"
#include "ARE/Settings.h"
#include "ARE/Phenotype.h"


namespace are {

struct ObjectParameters {
    float size[3];
    float position[3];
    float orientation[3];
    float mass;
    int parent; // points to custom ID
    int type;
    int state; // only used for LSystems?
};


struct JointParameters {
    float size[3];
    float position[3];
    float orientation[3];
    float angleLimits[2];
    float maxForce;
    float maxVelocity;
    int parent; // points to custom ID
    int type;
    int state;
};


//Abstract Class
class Morphology
{
public:

    typedef std::shared_ptr<Morphology> Ptr;
    typedef std::shared_ptr<const Morphology> ConstPtr;
    typedef Morphology::Ptr (Factory)(int,misc::RandNum::Ptr, Settings::Ptr);

    Morphology(){}
    Morphology(const Settings& settings);
    Morphology(const Morphology& morph) :
        modular(morph.modular),
        phenValue(morph.phenValue),
        maxModuleTypes(morph.maxModuleTypes),
        minimumHeight(morph.minimumHeight),
        amountIncrement(morph.amountIncrement),
        settings(morph.settings),
        randomNum(morph.randomNum),
//        createdModules(morph.createdModules),
        substrate(morph.substrate)
    {}
    virtual ~Morphology(){}

    virtual Morphology::Ptr clone() const = 0;

    /// This method initialize the morph
    virtual void init_noMorph(){}
    virtual void clearMorph(){}
    virtual void savePhenotype(int ind, float fitness){}

	/// This method creates the morphology
	virtual void create() = 0;
	virtual void createAtPosition(float x, float y, float z) = 0;

	/// This method updates the control of the morphology
    virtual std::vector<double> update() = 0;

    // operators of the evolutionary algorithm
    virtual void crossover(std::shared_ptr<Morphology>, float cr){}
    void grow(){}
    virtual void mutate();
    virtual bool loadGenome(int individualNumber, int sceneNum);
    virtual bool loadGenome(std::istream &input, int individualNumber);
    virtual void saveGenome(int indNum, float fitness);
    virtual const std::string generateGenome(int indNum, float fitness) const;

	// cloning
	virtual void saveBaseMorphology(int indNum, float fitness) = 0; // currently not used
//	virtual void saveBaseMorphology(int indNum, int sceneNum, float fitness) = 0; // currently not used
	virtual void loadBaseMorphology(int indNum, int sceneNum) = 0;
//	virtual void setMainHandlePosition(float position[3]) = 0;
//	virtual void createMorphology() = 0; // create actual morphology in init
    virtual void printSome(){}

	bool modular = false;
//	typedef shared_ptr<ER_Module> ModulePointer;

	// modular functions
	// This function is needed to implement fluid dynamics on modules...
//    virtual std::vector <std::shared_ptr<ER_Module>> getCreatedModules();
    virtual int getAmountBrokenModules(){}

    virtual void setPhenValue();
    float getPhenValue(){return phenValue;}


    std::vector<std::vector<int>> maxModuleTypes;

    virtual int getMainHandle(){return mainHandle;}

    const NEAT::Substrate &get_substrate(){return substrate;}

protected:


    /**
     * @brief Use this method to get the handles of the robot's components
     * @param parentHandle
     */
    virtual void getObjectHandles() = 0;

    float phenValue = -1.0;

    int mainHandle;
    float morphFitness;
	float minimumHeight = 0;
	int amountIncrement = 1;

    Settings::Ptr settings;
    misc::RandNum::Ptr randomNum;
//    std::vector<std::shared_ptr<ER_Module> > createdModules;
    NEAT::Substrate substrate;
};

}//are

#endif //MORPHOLOGY_H
