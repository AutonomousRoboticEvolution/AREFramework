#pragma once
#include <vector>
//#include "Morphology.h"
//#include "LMorphology.h"
//#include "Control.h"
#include "morphology/MorphologyFactory.h"
#include "control/ControlFactory.h"
#include <memory>
#include "../RandNum.h"
#include "Settings.h"

using namespace std;

class DefaultGenome 
{
public:
	DefaultGenome(shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	DefaultGenome() {};
	
	~DefaultGenome();
	unique_ptr<DefaultGenome> clone() const;
	//GenomeClone* clone() const { return new DefaultGenome(*this); }
	int morphologyType = 1;
//	vector<Morphology> genomeMorphologies;
	void saveGenome(int indNum, int sceneNum);
	bool isEvaluated = false;

	void createInitialMorphology(int individualNumber);
	void controlHandle();

	shared_ptr<Settings> settings;
	shared_ptr<RandNum> randNum;
	shared_ptr<Morphology> morph;
	shared_ptr<Control> control;

//	unique_ptr<Control> control;

	int age = 0;
	int maxAge = 0;
	int minAge = 0;
	float mutationRate = 0.05f;
	float parentPhenValue;

	void init();
	void init_noMorph();
	void mutate();
	void create();
	void createAtPosition(float x, float y, float z);
	void update(); 

//	DefaultGenome * clone(DefaultGenome *parent);
	
	void mutate(float);
	void loadGenome(){};
	float evaluateGenome(int sceneNum);
	float evaluateGenomeStep(int sceneNum);
	float genomeFitness;  

	void loadMorphologyGenome(int indNum, int sceneNum); 

	void duplicate(){};
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


