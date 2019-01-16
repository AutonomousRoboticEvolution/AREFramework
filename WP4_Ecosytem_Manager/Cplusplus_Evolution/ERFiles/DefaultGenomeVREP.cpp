#pragma once
#include "DefaultGenomeVREP.h"
#include <iostream>


DefaultGenomeVREP::DefaultGenomeVREP(shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	randNum = rn;
	settings = st;
	genomeFitness = 0;
	maxAge = settings->maxAge;
	age = 0;
}

DefaultGenomeVREP::~DefaultGenomeVREP() {
}

void DefaultGenomeVREP::createInitialMorphology(int individualNumber) {
}

void DefaultGenomeVREP::update() {
	morph->update(); // this can only be called if the phenotype is created
					 //	cout << "The update function should be removed from 'DefaultGenomeVREP.cpp'" << endl;
					 //	environment->updateEnv(morph);
}

void DefaultGenomeVREP::saveGenome(int indNum, int sceneNum) {
	morph->saveGenome(indNum, sceneNum, 0);
}

void DefaultGenomeVREP::checkGenome() {
	cout << "mainHandle = " << morph->getMainHandle() << endl;
	//	cout << "morph == " << morph << endl;
	if (morph == NULL) {
		cout << "morph is null" << endl;
	}
	cout << "morph. " << endl;
	morph->printSome();
}

void DefaultGenomeVREP::loadMorphologyGenome(int indNum, int sceneNum) {
	cout << "about to load genome" << endl;
	morph->loadGenome(indNum, sceneNum);
	cout << "LOADED (defaultGenomeVREP) " << endl;
}

void DefaultGenomeVREP::loadBaseMorphology(int indNum, int sceneNum) {
//	cout << "about to load base morphology" << endl;
	morph->loadBaseMorphology(indNum, sceneNum);
}

void DefaultGenomeVREP::init_noMorph() {
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactoryVREP> morphologyFactory(new MorphologyFactoryVREP);
	morph = morphologyFactory->createMorphologyGenome(m_type, randNum, settings);
	morphologyFactory.reset();
	morph->init_noMorph();
	if (m_type == -1) {
		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
		control = controlFactory->createNewControlGenome(0, randNum, settings);
		controlFactory.reset();
	}
}

void DefaultGenomeVREP::init() {
	//	cout << "DefaultGenomeVREP.init called" << endl; 
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactoryVREP> morphologyFactory(new MorphologyFactoryVREP);
	morph = morphologyFactory->createMorphologyGenome(m_type, randNum, settings);
	morphologyFactory.reset();
	morph->init();
	//	cout << "creating control" << endl;
	if (m_type == -1) { // not used
		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
		morph->control = controlFactory->createNewControlGenome(0, randNum, settings); // ann
		controlFactory.reset();
//		morph->control->init(1, 1, morph->outputHandles.size());//morph->morph_jointHandles);
	}
}

void DefaultGenomeVREP::mutate() {
	if (morph == NULL) {
		cout << "morph == NULL" << endl;
	}
	else {
		//	cout << "morph is not NULL" << endl; 
		//		cout << morph << endl; 
	}
	/*if (morph->modular == true) {
	if (randNum->randFloat(0.0, 1.0) < crossLGenome.mutationRate) {
	crossLGenome.mutationRate += randNum->randFloat(-0.01, 0.01);
	if (crossLGenome.mutationRate > 1.0) {
	crossLGenome.mutationRate = 1.0;
	}
	else if (crossLGenome.mutationRate < 0.0) {
	crossLGenome.mutationRate = 0.0;
	}
	}
	}*/
	morph->mutate(settings->mutationRate);
}

void DefaultGenomeVREP::create() {
	//	cout << "trying to create morph" << endl; 
	morph->create();
}

void DefaultGenomeVREP::createAtPosition(float x, float y, float z) {
	morph->createAtPosition(x, y, z);
}

void DefaultGenomeVREP::deleteCreated() {
	// function not used anymore
}

void DefaultGenomeVREP::clearGenome() {
	morph->clearMorph();
}
