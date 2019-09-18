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
	/// Can be used for a adding a centralized controller
	shared_ptr<Control> control;
	bool initialized = false;

	/// fitness value; make vector for multi-objective
	float fitness;
	/// mark the end of evaluation for a generation
	bool isEvaluated = false; 

	/// mark the ID of individual
	int individualNumber;
	/// Initialize the morph and its control
	virtual void init() = 0;
    /// Creates the phenotype
	virtual void create() = 0;
    /// This function is called every frame (updates the phenotype)
	virtual void update() = 0;
	/// This method deep copies the genome
	virtual shared_ptr<Genome> clone() const = 0;
	virtual void mutate() = 0;
	virtual void savePhenotype(int indNum, int sceneNum) = 0;
    /// loads genome from .csv
	virtual bool loadGenome(int indNum, int sceneNum) = 0;
    /// loads genome from signal : TODO BUG????
    virtual bool loadGenome(std::istream &input, int indNum) = 0;
    virtual void saveGenome(int indNum) = 0;
    /// Returns a genome string from existing genome
	virtual const std::string generateGenome() const = 0;
	/// prints out a few genome debugging statements
	virtual void checkGenome() = 0; // for debugging
};

