#include "GenomeFactoryVREP.h"

#include <memory>

GenomeFactoryVREP::GenomeFactoryVREP()
{}

GenomeFactoryVREP::~GenomeFactoryVREP()
{}

std::shared_ptr<Genome> GenomeFactoryVREP::createGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
	switch (type) {
	case 1:
	{
		if (st->verbose) {
            std::cout << "Creating default genome" << std::endl;
		}
        std::unique_ptr<Genome> m_genome(new DefaultGenome);
		m_genome->randomNum = rn;
		m_genome->settings = st;
        std::cout << m_genome->settings->COLOR_LSYSTEM << std::endl;
		return m_genome;
	}
	case 0: {
		if (st->verbose) {
            std::cout << "Creating default V-REP genome" << std::endl;
		}
        std::unique_ptr<Genome> m_genome(new DefaultGenomeVREP);
		m_genome->randomNum = rn;
		m_genome->settings = st;
		return m_genome;
	}
	}
    return std::unique_ptr<Genome>();
}

std::shared_ptr<Genome> GenomeFactoryVREP::convertToGenomeVREP(std::shared_ptr<Genome> gn)
{
    std::shared_ptr<Genome> vrepGenome(new DefaultGenomeVREP);
	vrepGenome->randomNum = gn->randomNum;
	vrepGenome->settings = gn->settings;
    std::unique_ptr<MorphologyFactoryVREP> mf = std::make_unique<MorphologyFactoryVREP>();
	// vrepGenome->morph.reset();
    // std::shared_ptr<Morphology> m = mf->convertMorph(gn->morph);
	vrepGenome->morph = mf->convertMorph(gn->morph);
	if (vrepGenome->morph->control) {
		vrepGenome->morph->control = gn->morph->control->clone();
	}
	// gn->clone();
	mf.reset();
    return vrepGenome;// std::shared_ptr<Genome>();
}
