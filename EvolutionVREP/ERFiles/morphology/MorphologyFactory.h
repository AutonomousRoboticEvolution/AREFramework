#pragma once

#include <memory>
#include "Morphology.h"


class MorphologyFactory
{
public:
	MorphologyFactory();
	~MorphologyFactory();
	virtual std::shared_ptr<Morphology> createMorphologyGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
	virtual std::shared_ptr<Morphology> copyMorphologyGenome(std::shared_ptr<Morphology> parentMorphology);
	virtual std::shared_ptr<Morphology> createBaseMorphologyGenome(); // should always be created
    std::shared_ptr<RandNum> randomNum;
};

