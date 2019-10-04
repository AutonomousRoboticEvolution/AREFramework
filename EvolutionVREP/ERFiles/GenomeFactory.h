#pragma once

#include <memory>
#include "Genome.h"
#include "DefaultGenome.h"


class GenomeFactory
{
public:
	GenomeFactory();
	~GenomeFactory();
    std::shared_ptr<Genome> createGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
    std::shared_ptr<Genome> copyGenome(std::shared_ptr<Genome> parentMorphology);
    std::shared_ptr<RandNum> randomNum;
};

