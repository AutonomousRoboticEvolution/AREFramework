/**
	@file v_repExtER.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This file creates the appropriate EA (Steady state, generational, NEAT) according according to settings
    (evolution type).
*/
#pragma once
#include <memory>
#include "EA.h"

class EA_Factory 
{
public:
	EA_Factory();
	~EA_Factory();
    /// Create the appropriate EA according to evolutionType.
	std::shared_ptr<EA> createEA(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
};
