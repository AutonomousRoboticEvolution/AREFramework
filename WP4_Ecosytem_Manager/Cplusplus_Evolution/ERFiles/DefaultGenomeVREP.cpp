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

void DefaultGenomeVREP::loadMorphologyGenome(int indNum, int sceneNum) {
	if (settings->verbose) {
		cout << "Loading genome " << indNum << endl;
	}
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactoryVREP> morphologyFactory(new MorphologyFactoryVREP);
	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
	morphologyFactory.reset();
	morph->loadGenome(indNum, sceneNum);
	//morph->create();
	if (settings->verbose) {
		cout << "Succesfully loaded genome " << indNum << endl;
	}
	//morph->create();
}

void DefaultGenomeVREP::loadGenome(int indNum, int sceneNum)
{
	loadMorphologyGenome(indNum, sceneNum);
	individualNumber = indNum;
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