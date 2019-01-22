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
#include "Genome.h"

class DefaultGenome : public Genome
{
public:
	DefaultGenome(shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	DefaultGenome() {};
	
	~DefaultGenome();
	shared_ptr<Genome> clone() const;

	void createInitialMorphology(int individualNumber);

	int age = 0;
	int maxAge = 0;
	int minAge = 0;
	float mutationRate = 0.05f;
	float parentPhenValue;

	void savePhenotype(int indNum, int sceneNum);
	void init();
	void create();
	void update(); 
	bool loadGenome(int indNum, int sceneNum);
	void saveGenome(int indNum, int sceneNum);
	void clearGenome();
	void mutate();
	void checkGenome();
	shared_ptr<Genome> cloneGenome();

	void init_noMorph();
	void createAtPosition(float x, float y, float z);

	//void mutate();
	float genomeFitness;  

	bool loadMorphologyGenome(int indNum, int sceneNum); 
	void loadBaseMorphology(int indNum, int sceneNum); // old


protected:

};


