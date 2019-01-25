#pragma once
#include "DefaultGenomeVREP.h"
#include <iostream>


DefaultGenomeVREP::DefaultGenomeVREP(shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	randomNum = rn;
	settings = st;
	genomeFitness = 0;
	maxAge = settings->maxAge;
	age = 0;
}

DefaultGenomeVREP::~DefaultGenomeVREP() {
}

std::shared_ptr<MorphologyFactory> DefaultGenomeVREP::newMorphologyFactory()
{
	shared_ptr<MorphologyFactoryVREP> morphologyFactory(new MorphologyFactoryVREP);
	return morphologyFactory;
}

bool DefaultGenomeVREP::loadGenome(int indNum, int sceneNum)
{
	bool load = DefaultGenome::loadGenome(indNum, sceneNum);
	individualNumber = indNum;
	return load;
}

bool DefaultGenomeVREP::loadGenome(std::istream &input, int indNum)
{
	bool load = DefaultGenome::loadGenome(input, indNum);
	individualNumber = indNum;
	return load;
}

void DefaultGenomeVREP::init() {
	// This function calls a VREP based morphology factory which contains VREP specific functions. 
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactoryVREP> morphologyFactory(new MorphologyFactoryVREP);
	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
	morphologyFactory.reset();
	morph->init();
	// The control creation is the same as the one in DefaultGenome::init(). Code can be improved.
	if (m_type == -1) { // not used
		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
		morph->control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
		controlFactory.reset();
	}
	//control = new control();
	//vector<float> output = control->update();
	//morph->update(output);

	//if (morph->changed == true) {
	//	control->change(morph->morphoparameters);
	//}
	//vector<Module> morphoparameters;
	//moduleID
}

shared_ptr<Genome> DefaultGenomeVREP::clone() const
{
	return make_unique<DefaultGenomeVREP>(*this);
}