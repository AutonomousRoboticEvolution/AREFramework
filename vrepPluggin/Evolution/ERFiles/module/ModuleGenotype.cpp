#include "ModuleGenotype.h"



ModuleGenotype::ModuleGenotype()
{
}


ModuleGenotype::~ModuleGenotype()
{
}

int ModuleGenotype::init()
{
	return 0;
}

int ModuleGenotype::mutate(float mutationRate)
{
	return 0;
}

int ModuleGenotype::createModule(vector<float> configuration, int relativePosHandle, int parentHandle)
{
	return 0;
}

void ModuleGenotype::createControl()
{
}

stringstream ModuleGenotype::getModuleParams()
{
	return stringstream();
}

stringstream ModuleGenotype::getControlParams()
{
	return stringstream();
}

void ModuleGenotype::setModuleParams(vector<string>)
{
}

void ModuleGenotype::removeModule()
{
}

vector<float> ModuleGenotype::getPosition()
{
	return vector<float>();
}

shared_ptr<ER_Module> ModuleGenotype::clone()
{
	return shared_ptr<ER_Module>(new ModuleGenotype(*this));
}

vector<int> ModuleGenotype::getFreeSites(vector<int>)
{
	return vector<int>();
}

vector<float> ModuleGenotype::updateModule(vector<float> input)
{
	return vector<float>();
}

vector<int> ModuleGenotype::getObjectHandles()
{
	return vector<int>();
}

void ModuleGenotype::setModuleColor()
{
}

