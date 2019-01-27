#pragma once
#include "DefaultGenome.h"
#include <iostream>

using namespace std;

DefaultGenome::DefaultGenome(shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	randomNum = rn;
	settings = st;
	genomeFitness = 0;
	maxAge = settings->maxAge;
	age = 0;
}

DefaultGenome::~DefaultGenome() {
	/*if (morph) {
		morph->~Morphology();
	}
	if (control) {
		control->~Control();
	}*/
}

shared_ptr<Genome> DefaultGenome::clone() const
{
	return make_unique<DefaultGenome>(*this);
}

void DefaultGenome::createInitialMorphology(int individualNumber) {
}

void DefaultGenome::update() {
	// This can only be called if the phenotype is created
	morph->update();
}

bool DefaultGenome::loadGenome(int indNum, int sceneNum)
{
	return loadMorphologyGenome(indNum, sceneNum);
}

bool DefaultGenome::loadGenome(std::istream &input, int indNum)
{
	return loadMorphologyGenome(input, indNum);
}

void DefaultGenome::saveGenome(int indNum) {
	morph->saveGenome(indNum, 0);
}

const std::string DefaultGenome::generateGenome() const
{
	return morph->generateGenome(individualNumber, 0);
}

void DefaultGenome::checkGenome() {
	cout << "mainHandle = " << morph->getMainHandle() << endl;
	if (morph == NULL) {
		cout << "morph is null, cannot run evolution without a morphology being loaded" << endl;
	}
	if (settings->verbose) {
		// prints some debugging information in the terminal
		morph->printSome();
	}
}

shared_ptr<Genome> DefaultGenome::cloneGenome()
{
	shared_ptr<DefaultGenome> cloned = make_unique<DefaultGenome>(*this);
	cloned->morph = morph->clone();
	return cloned;
}


std::shared_ptr<MorphologyFactory> DefaultGenome::newMorphologyFactory()
{
	shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
	return morphologyFactory;
}

bool DefaultGenome::loadMorphologyGenome(int indNum, int sceneNum) {
	if (settings->verbose) {
		cout << "Loading genome " << indNum << endl;
	}
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactory> morphologyFactory = this->newMorphologyFactory();
	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
	morphologyFactory.reset();

	bool load = morph->loadGenome(indNum, sceneNum);
	if (settings->verbose && load == true) {
		cout << "Succesfully loaded genome " << indNum << endl;
	}
	return load;
}

bool DefaultGenome::loadMorphologyGenome(std::istream &input, int indNum) {
	if (settings->verbose) {
		cout << "Loading genome " << indNum << endl;
	}
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactory> morphologyFactory = this->newMorphologyFactory();
	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
	morphologyFactory.reset();

	bool load = morph->loadGenome(input, indNum);
	if (settings->verbose && load == true) {
		cout << "Succesfully loaded genome " << indNum << endl;
	}
	return load;
}

void DefaultGenome::loadBaseMorphology(int indNum, int sceneNum) {
	// This is an old piece of code that I didn't want to delete yet. 
	cout << "about to load base morphology" << endl;
	morph->loadBaseMorphology(indNum, sceneNum);
}

void DefaultGenome::init_noMorph() {
	// For debugging. Shouldn't be used otherwise. 
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
	morphologyFactory.reset();
	//morph->init_noMorph();
	//if (m_type == -1) {
	//	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	//	control = controlFactory->createNewControlGenome(0, randomNum, settings);
	//	controlFactory.reset();
	//}
}

void DefaultGenome::savePhenotype(int indNum, int sceneNum)
{
	morph->savePhenotype(indNum, fitness);
}

void DefaultGenome::init() {
	// To initialize a genome
	int m_type = settings->morphologyType;
	shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
	morphologyFactory.reset();
	morph->init();
//	if (m_type == -1) { // not used
//		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//		morph->control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
//		controlFactory.reset();
//	}
}

void DefaultGenome::mutate() {
	if (morph == NULL) {
		cout << "ERROR: morph == NULL" << endl;
	}	
	// TODO: The morphology mutation rate should be gathered from the settings file in the mutate function?
	morph->mutate(); 
}

void DefaultGenome::create() {
	morph->create();
}

void DefaultGenome::createAtPosition(float x, float y, float z){
	morph->createAtPosition(x, y, z);
}

void DefaultGenome::clearGenome() {
	morph->clearMorph(); 
}
