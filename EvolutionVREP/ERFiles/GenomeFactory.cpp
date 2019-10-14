#include "GenomeFactory.h"

GenomeFactory::GenomeFactory()
{
}

GenomeFactory::~GenomeFactory()
{
}

std::shared_ptr<Genome> GenomeFactory::createGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
	switch (type) {
        case 1:
        {
            std::unique_ptr<Genome> m_genome(new DefaultGenome);  //create a genome??
            m_genome->randomNum = rn;
            m_genome->settings = st;
            //std::cout << m_genome->settings->COLOR_LSYSTEM << std::endl;
            return m_genome;
        }
        case 0: {
            std::cout << "ERROR: You started a client-server application. You cannot instantiate objects that depend on V-REP's libraries." << std::endl;
        }
	}
    return std::unique_ptr<Genome>();
}

std::shared_ptr<Genome> GenomeFactory::copyGenome(std::shared_ptr<Genome> parent)
{
    std::shared_ptr<Genome> cloneGenome;
	cloneGenome = parent->clone();
	return cloneGenome;
}
