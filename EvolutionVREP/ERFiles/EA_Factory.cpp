/**
	@file EA_Factory.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This file creates the appropriate EA (Steady state, generational, NEAT) according according to settings
    (evolution type).
*/
#include "EA_Factory.h"
#include "EA_MultiNEAT.h"

EA_Factory::EA_Factory()
{}

EA_Factory::~EA_Factory()
{}
/// Create the appropriate EA according to evolutionType.
std::shared_ptr<EA> EA_Factory::createEA(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
	int type = st->evolutionType;
	switch (type){
        case 1:
        {
            if (st->verbose) {
                std::cout << "Creating EA: Steady State" << std::endl;
            }
            std::unique_ptr<EA> m_ea(new EA_SteadyState);
            m_ea->randomNum = rn;
            m_ea->settings = st;
            return m_ea;
        }
        case 2: {
            if (st->verbose) {
                std::cout << "Creating EA: Generational" << std::endl;
            }
            std::unique_ptr<EA> m_ea(new EA_Generational);
            m_ea->randomNum = rn;
            m_ea->settings = st;
            return m_ea;
        }
        case 3: {
            if (st->verbose) {
                std::cout << "Creating EA: NEAT" << std::endl;
            }
            std::unique_ptr<EA> m_ea(new EA_NEAT);
            m_ea->randomNum = rn;
            m_ea->settings = st;
            return m_ea;
        }
        case 5: {
            if (st->verbose) {
                std::cout << "Creating EA: MULTINeat" << std::endl;
            }
            std::unique_ptr<EA> m_ea(new EA_MultiNEAT);
            m_ea->randomNum = rn;
            m_ea->settings = st;
            return m_ea;
        }
        default: {
            if (st->verbose) {
                std::cout << "Steady state EA" << std::endl;
            }
            std::unique_ptr<EA> m_ea(new EA_SteadyState);
            m_ea->randomNum = rn;
            m_ea->settings = st;
            return m_ea;
        }
	}
}
