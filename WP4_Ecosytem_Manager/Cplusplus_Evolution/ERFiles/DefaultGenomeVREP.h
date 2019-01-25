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
	virtual bool loadGenome(int indNum, int sceneNum) override;
	virtual bool loadGenome(std::istream &input, int indNum) override;

	// deepcopy
	shared_ptr<Genome> clone() const;
	
	typedef shared_ptr<Morphology> MorphologyPointer;
	typedef shared_ptr<Control> ControlPointer;
	void init();

protected:
	virtual std::shared_ptr<MorphologyFactory> newMorphologyFactory() override;
};


