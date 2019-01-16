#pragma once
#include "DefaultGenome.h"
#include <iostream>


DefaultGenome::DefaultGenome(shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	randNum = rn;
	settings = st;
	genomeFitness = 0;
	maxAge = settings->maxAge;
	age = 0;
}

DefaultGenome::~DefaultGenome() {
}

unique_ptr<DefaultGenome> DefaultGenome::clone() const
{
	return make_unique<DefaultGenome>(*this);
}

void DefaultGenome::createInitialMorphology(int individualNumber) {
}

void DefaultGenome::update() {
	morph->update(); // this can only be called if the phenotype is created
//	cout << "The update function should be removed from 'DefaultGenome.cpp'" << endl;
//	environment->updateEnv(morph);
}

void DefaultGenome::saveGenome(int indNum, int sceneNum) {
	morph->saveGenome(indNum, sceneNum, 0);
}

void DefaultGenome::checkGenome() {
	cout << "mainHandle = " << morph->getMainHandle() << endl;
	//	cout << "morph == " << morph << endl;
	if (morph == NULL) {
		cout << "morph is null" << endl;
	}
	cout << "morph. " << endl;
	morph->printSome();
}

void DefaultGenome::loadMorphologyGenome(int indNum, int sceneNum) {
//	cout << "about to load genome" << endl;
	morph->loadGenome(indNum, sceneNum);
//	morph->init();
}

void DefaultGenome::loadBaseMorphology(int indNum, int sceneNum) {
	cout << "about to load base morphology" << endl;
	morph->loadBaseMorphology(indNum, sceneNum);
}

void DefaultGenome::init_noMorph() {
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
	morph = morphologyFactory->createMorphologyGenome(m_type, randNum, settings);
	morphologyFactory.reset();
	morph->init_noMorph();
	if (m_type == -1) {
		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
		control = controlFactory->createNewControlGenome(0, randNum, settings);
		controlFactory.reset();
	}
}

void DefaultGenome::init() {
	//	cout << "DefaultGenome.init called" << endl; 
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
	morph = morphologyFactory->createMorphologyGenome(m_type, randNum, settings);
	morphologyFactory.reset();
	morph->init();
	//	cout << "creating control" << endl;
	if (m_type == -1) { // not used
		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
		morph->control = controlFactory->createNewControlGenome(0, randNum, settings); // ann
		controlFactory.reset();
//		morph->control->init(1, 1, 1);//morph->morph_jointHandles);
	}
}

void DefaultGenome::mutate() {
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
	
	morph->mutate(settings->morphMutRate);

}

void DefaultGenome::create() {
//	cout << "trying to create morph" << endl; 
	morph->create();
}

void DefaultGenome::createAtPosition(float x, float y, float z){
	morph->createAtPosition(x, y, z);
}

void DefaultGenome::clearGenome() {
	morph->clearMorph(); 
}
