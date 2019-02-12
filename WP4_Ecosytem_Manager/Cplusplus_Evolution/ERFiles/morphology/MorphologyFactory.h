#pragma once

#include "BaseMorphology.h"
#include "Morphology.h"
#include "FixedBaseMorphology.h"
#include "ER_LSystem.h"
#include <memory>
#include "ER_CPPN.h"
#include "ER_LSystemInterpreter.h"
#include "CAT.h"
#include "ER_DirectEncoding.h"
#include "CustomMorphology.h"
#include "Tissue_DirectBars.h"
#include "Tissue_GMX.h"
#include "ER_CPPN_Encoding.h"


class MorphologyFactory
{
public:
	MorphologyFactory();
	~MorphologyFactory();
	virtual shared_ptr<Morphology> createMorphologyGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	virtual shared_ptr<Morphology> copyMorphologyGenome(shared_ptr<Morphology> parentMorphology);
	virtual shared_ptr<Morphology> createBaseMorphologyGenome(); // should always be created
	shared_ptr<RandNum> randomNum;
};

