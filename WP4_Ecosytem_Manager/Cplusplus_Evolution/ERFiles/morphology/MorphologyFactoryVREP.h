#pragma once

#include "CATVREP.h"
#include "CustomMorphologyVREP.h"
#include <memory>
#include "ER_CPPN.h"
// #include "Manual_ModularRobot.h"
#include "ER_LSystemInterpreter.h"
#include "v_repLib.h"
#include "MorphologyFactory.h"
#include "ER_DirectEncodingInterpreter.h"
#include "Tissue_DirectBarsVREP.h"
#include "Tissue_GMX_VREP.h"
#include "ER_CPPN_Interpreter.h"

class MorphologyFactoryVREP : public MorphologyFactory
{
public:
	MorphologyFactoryVREP();
	~MorphologyFactoryVREP();
	virtual shared_ptr<Morphology> createMorphologyGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st) override;
	virtual shared_ptr<Morphology> copyMorphologyGenome(shared_ptr<Morphology> parentMorphology) override;
	virtual shared_ptr<Morphology> createBaseMorphologyGenome() override; // should always be created
	shared_ptr<RandNum> randomNum;
	shared_ptr<Morphology> convertMorph(shared_ptr<Morphology> morph);
};

