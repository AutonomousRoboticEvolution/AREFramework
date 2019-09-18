#pragma once

#include <memory>
#include <vector>
#include "Settings.h"
#include "DefaultGenome.h"
#include "control/ControlFactory.h"
#include "morphology/MorphologyFactoryVREP.h"
#include "../RandNum.h"


class DefaultGenomeVREP : public DefaultGenome
{
public:
	DefaultGenomeVREP(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
	DefaultGenomeVREP() {};

	~DefaultGenomeVREP();
	virtual bool loadGenome(int indNum, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int indNum) override;

	// deepcopy
    std::shared_ptr<Genome> clone() const;
	
	typedef std::shared_ptr<Morphology> MorphologyPointer;
	typedef std::shared_ptr<Control> ControlPointer;
	void init();

protected:
	virtual std::shared_ptr<MorphologyFactory> newMorphologyFactory() override;
};


