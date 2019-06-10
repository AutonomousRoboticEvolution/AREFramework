#pragma once
#include <vector>
#include <memory>
#include "../RandNum.h"
#include "Settings.h"
#include "morphology/Morphology.h"


class Genome
{
public:
	Genome();
	virtual ~Genome();
    	/// simulation setting shared by genome and EA
	shared_ptr<Settings> settings;     
	/// random number generator
	shared_ptr<RandNum> randomNum;
	/// morph is part of genome
	shared_ptr<Morphology> morph;
	shared_ptr<Control> control;

	/// fitness value; make vector for multi-objective
	float fitness;
	/// mark the end of evaluation for a generation
	bool isEvaluated = false; 

	///mark the ID of inidividual
	int individualNumber;
	/**
	@brief Initialize the morph and its control 
	*/
	virtual void init() = 0;

	virtual void create() = 0;

	virtual void update() = 0;
	/**
		@brief This method clone the genome
	*/
	virtual shared_ptr<Genome> clone() const = 0;
	virtual void mutate() = 0;

	virtual void savePhenotype(int indNum, int sceneNum) = 0;

	virtual bool loadGenome(int indNum, int sceneNum) = 0;
	virtual bool loadGenome(std::istream &input, int indNum) = 0;
	virtual void saveGenome(int indNum) = 0;
	virtual const std::string generateGenome() const = 0; 
	virtual void clearGenome() = 0;
	virtual void checkGenome() = 0; // for debugging
	virtual bool loadMorphologyGenome(int indNum, int sceneNum) = 0;
	virtual bool loadMorphologyGenome(std::istream &input, int indNum) = 0;

};

