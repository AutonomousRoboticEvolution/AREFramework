#pragma once
#include <vector>
//#include "Morphology.h"
//#include "LMorphology.h"
//#include "Control.h"
#include "morphology/MorphologyFactoryVREP.h"
#include "control/ControlFactory.h"
#include <memory>
#include "../RandNum.h"
#include "Settings.h"
#include "DefaultGenome.h"

using namespace std;

class DefaultGenomeVREP : public DefaultGenome
{
public:
	DefaultGenomeVREP(shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	DefaultGenomeVREP() {};

	~DefaultGenomeVREP();
	unique_ptr<DefaultGenomeVREP> clone() const;
	//GenomeClone* clone() const { return new DefaultGenomeVREP(*this); }
	int morphologyType = 1;
	//	vector<Morphology> genomeMorphologies;
	void saveGenome(int indNum, int sceneNum);

	void createInitialMorphology(int individualNumber);
	void controlHandle();

	shared_ptr<Settings> settings;
	shared_ptr<RandNum> randNum;
	typedef shared_ptr<Morphology> MorphologyPointer;
	MorphologyPointer morph;
	typedef shared_ptr<Control> ControlPointer;
	ControlPointer control;

	//	unique_ptr<Control> control;

	int age = 0;
	int maxAge = 0;
	int minAge = 0;
	float mutationRate = 0.05f;
	
	void init();
	void init_noMorph();
	void mutate();
	void create();
	void createAtPosition(float x, float y, float z);
	void update();

	//	DefaultGenomeVREP * clone(DefaultGenomeVREP *parent);

	void mutate(float);
	void loadGenome() {};
	float evaluateGenome(int sceneNum);
	float evaluateGenomeStep(int sceneNum);
	float genomeFitness;

	void loadMorphologyGenome(int indNum, int sceneNum);

	void duplicate() {};
	void deleteCreated();

	void loadBaseMorphology(int indNum, int sceneNum);

	void clearGenome();
	void checkGenome();

	void currentGenomeChecker();
	int individualNumber;
	void printIndividualInfo();

	// for crossing over the lSystem
protected:

};


