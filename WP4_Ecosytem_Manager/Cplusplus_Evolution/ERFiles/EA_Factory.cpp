#include "EA_Factory.h"


EA_Factory::EA_Factory()
{
}


EA_Factory::~EA_Factory()
{
}


shared_ptr<EA> EA_Factory::createEA(shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	int type = st->evolutionType;
	switch (type) {
	case 1:
	{
		if (st->verbose) {
			cout << "Steady state EA" << endl;
		}
		unique_ptr<EA> m_ea(new EA_SteadyState);
		m_ea->randomNum = rn;
		m_ea->settings = st;
		return m_ea;
	}
	case 2: {
		if (st->verbose) {
			cout << "Generational EA" << endl;
		}
		unique_ptr<EA> m_ea(new EA_Generational);
		m_ea->randomNum = rn;
		m_ea->settings = st;
		return m_ea;
	}
	}
	return unique_ptr<EA>();
}