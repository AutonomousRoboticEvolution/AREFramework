#pragma once
#include "GenomeFactory.h"
#include "DefaultGenomeVREP.h"

class GenomeFactoryVREP :
	public GenomeFactory
{
public:
	GenomeFactoryVREP();
	~GenomeFactoryVREP();
	shared_ptr<Genome> createGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	shared_ptr<Genome> convertToGenomeVREP(shared_ptr<Genome> gn);
};

