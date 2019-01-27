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
	virtual shared_ptr<Genome> clone() const override;

	void createInitialMorphology(int individualNumber);

	int age = 0;
	int maxAge = 0;
	int minAge = 0;
	float mutationRate = 0.05f;
	float parentPhenValue;

	void savePhenotype(int indNum, int sceneNum);
	virtual void init() override;
	virtual void create() override;
	virtual void update() override; 
	virtual void mutate() override;
	virtual bool loadGenome(int indNum, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int indNum) override;
	virtual void saveGenome(int indNum) override;
	virtual const std::string generateGenome() const override;
	virtual void clearGenome() override;
	virtual void checkGenome() override;
	virtual bool loadMorphologyGenome(int indNum, int sceneNum) override;
	virtual bool loadMorphologyGenome(std::istream &input, int indNum) override;
	shared_ptr<Genome> cloneGenome();

	void init_noMorph();
	void createAtPosition(float x, float y, float z);

	//void mutate();
	float genomeFitness;  

	void loadBaseMorphology(int indNum, int sceneNum); // old


protected:
	// virtual functions to extend with simulator specific classes
	virtual std::shared_ptr<MorphologyFactory> newMorphologyFactory();
};


