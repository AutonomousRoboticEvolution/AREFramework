#pragma once

#include "GenomeFactory.h"
#include "DefaultGenomeVREP.h"


class GenomeFactoryVREP :
	public GenomeFactory
{
public:
	GenomeFactoryVREP();
	~GenomeFactoryVREP();
    std::shared_ptr<Genome> createGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
    std::shared_ptr<Genome> convertToGenomeVREP(std::shared_ptr<Genome> gn);
};

