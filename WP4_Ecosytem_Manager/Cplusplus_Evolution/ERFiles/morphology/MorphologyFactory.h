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
#include "EdgarsAmazingMorphologyClass.h"
#include "ER_CPPN_Encoding.h"


class MorphologyFactory
{
public:
	MorphologyFactory();
	~MorphologyFactory();
	/**
		@brief This method creates different morph types
	*/
	shared_ptr<Morphology> createMorphologyGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	shared_ptr<Morphology> copyMorphologyGenome(shared_ptr<Morphology> parentMorphology);
	shared_ptr<Morphology> createBaseMorphologyGenome(); // should always be created
	shared_ptr<RandNum> randomNum;
};




