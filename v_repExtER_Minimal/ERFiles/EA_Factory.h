#pragma once
#include "EA.h"
#include "EA_SteadyState.h"
#include "EA_Generational.h"

class EA_Factory :
	public GenomeFactory
{
public:
	EA_Factory();
	~EA_Factory();
	shared_ptr<EA> createEA(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st);
};

