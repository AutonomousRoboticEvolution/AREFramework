#pragma once
#ifndef DEFAULTGENOMEVREP_H
#define DEFAULTGENOMEVREP_H

#include <memory>
#include <vector>
#include "ARE/Settings.h"
#include "ARE/DefaultGenome.h"
#include "misc/RandNum.h"


class DefaultGenomeVREP : public DefaultGenome
{
public:
    DefaultGenomeVREP(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
        : DefaultGenome(rn,st){};

	DefaultGenomeVREP() {};

	~DefaultGenomeVREP();
	virtual bool loadGenome(int indNum, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int indNum) override;

	// deepcopy
    std::shared_ptr<Genome> clone() const override;
	
	typedef std::shared_ptr<Morphology> MorphologyPointer;
	typedef std::shared_ptr<Control> ControlPointer;
	void init() override;

//protected:
//	virtual std::shared_ptr<MorphologyFactory> newMorphologyFactory() override;
};


#endif //DEFAULTGENOMEVREP_H
