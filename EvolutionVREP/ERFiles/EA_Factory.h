#pragma once

#include <memory>
#include "EA.h"
#include "EA_SteadyState.h"
#include "EA_Generational.h"
#include "EA_NEAT.h"


class EA_Factory 
{
public:
	EA_Factory();
	~EA_Factory();
	std::shared_ptr<EA> createEA(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
};
