#include "GenomeFactoryVREP.h"


GenomeFactoryVREP::GenomeFactoryVREP()
{
}


GenomeFactoryVREP::~GenomeFactoryVREP()
{
}


shared_ptr<Genome> GenomeFactoryVREP::createGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	switch (type) {
	case 1:
	{
		if (st->verbose) {
			cout << "Creating default genome" << endl;
		}
		unique_ptr<Genome> m_genome(new DefaultGenome);
		m_genome->randomNum = rn;
		m_genome->settings = st;
		cout << m_genome->settings->COLOR_LSYSTEM << endl;
		return m_genome;
	}
	case 0: {
		if (st->verbose) {
			cout << "Creating default V-REP genome" << endl;
		}
		unique_ptr<Genome> m_genome(new DefaultGenomeVREP);
		m_genome->randomNum = rn;
		m_genome->settings = st;
		return m_genome;
	}
	}
	return unique_ptr<Genome>();
}