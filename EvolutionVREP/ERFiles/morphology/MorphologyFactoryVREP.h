#pragma once

#include <memory>
#include "CATVREP.h"
#include "v_repLib.h"
#include "MorphologyFactory.h"


class MorphologyFactoryVREP : public MorphologyFactory
{
public:
	MorphologyFactoryVREP();
	~MorphologyFactoryVREP();
	virtual std::shared_ptr<Morphology> createMorphologyGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st) override;
	virtual std::shared_ptr<Morphology> copyMorphologyGenome(std::shared_ptr<Morphology> parentMorphology) override;
	virtual std::shared_ptr<Morphology> createBaseMorphologyGenome() override; // should always be created
    std::shared_ptr<RandNum> randomNum;
    std::shared_ptr<Morphology> convertMorph(std::shared_ptr<Morphology> morph);
};
