#include "ARE/DefaultGenome.h"
#include <iostream>

DefaultGenome::DefaultGenome(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
	randomNum = rn;
	settings = st;
	genomeFitness = 0;

    if(!load_fct_exp_plugin<Morphology::Factory>
            (morphologyFactory,settings->exp_plugin_name,"morphologyFactory"))
        exit(1);
}




DefaultGenome::~DefaultGenome() {
	/*if (morph) {
		morph->~Morphology();
	}
	if (control) {
		control->~Control();
	}*/
}

std::shared_ptr<Genome> DefaultGenome::clone() const
{
    std::shared_ptr<DefaultGenome> genome = std::make_unique<DefaultGenome>(*this);
	genome->morph = this->morph->clone();
    return std::move(genome);
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
    std::cout << "mainHandle = " << morph->getMainHandle() << std::endl;
	if (morph == NULL) {
        std::cout << "morph is null, cannot run evolution without a morphology being loaded" << std::endl;
	}
	if (settings->verbose) {
		// prints some debugging information in the terminal
		morph->printSome();
	}
}

std::shared_ptr<Genome> DefaultGenome::cloneGenome()
{
    std::shared_ptr<DefaultGenome> cloned = std::make_unique<DefaultGenome>(*this);
	cloned->morph = morph->clone();
	return cloned;
}


//std::shared_ptr<MorphologyFactory> DefaultGenome::newMorphologyFactory()
//{
//    std::shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
//	return morphologyFactory;
//}

bool DefaultGenome::loadMorphologyGenome(int indNum, int sceneNum) {
	if (settings->verbose) {
        std::cout << "Loading genome " << indNum << std::endl;
	}
	int m_type = settings->morphologyType;
//    std::shared_ptr<MorphologyFactory> morphologyFactory = this->newMorphologyFactory();
//	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
//    morphologyFactory.reset();
    morph = morphologyFactory(m_type, randomNum, settings);


	bool load = morph->loadGenome(indNum, sceneNum);
	if (settings->verbose && load == true) {
        std::cout << "Succesfully loaded genome " << indNum << std::endl;
	}
	return load;
}

bool DefaultGenome::loadMorphologyGenome(std::istream &input, int indNum) {
	if (settings->verbose) {
        std::cout << "Loading genome " << indNum << std::endl;
	}
	int m_type = settings->morphologyType;
//    std::shared_ptr<MorphologyFactory> morphologyFactory = this->newMorphologyFactory();
//	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
//	morphologyFactory.reset();

    morph = morphologyFactory(m_type, randomNum, settings);


	bool load = morph->loadGenome(input, indNum);
	if (settings->verbose && load == true) {
        std::cout << "Succesfully loaded genome " << indNum << std::endl;
	}
	return load;
}

void DefaultGenome::loadBaseMorphology(int indNum, int sceneNum) {
	// This is an old piece of code that I didn't want to delete yet.
    std::cout << "about to load base morphology" << std::endl;
	morph->loadBaseMorphology(indNum, sceneNum);
}

void DefaultGenome::init_noMorph() {
	// For debugging. Shouldn't be used otherwise.
	int m_type = settings->morphologyType;
//    std::shared_ptr<MorphologyFactory> morphologyFactory(new MorphologyFactory);
//	morph = morphologyFactory->createMorphologyGenome(m_type, randomNum, settings);
//	morphologyFactory.reset();

    morph = morphologyFactory(m_type, randomNum, settings);

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
//	MorphologyFactory morphologyFactory;
//	morph = morphologyFactory.createMorphologyGenome(m_type, randomNum, settings);

    morph = morphologyFactory(m_type, randomNum, settings);

	morph->init();
//	if (m_type == -1) { // not used
//		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//		morph->control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
//		controlFactory.reset();
//	}
}

void DefaultGenome::mutate() {
	if (morph == NULL) {
        std::cout << "ERROR: morph == NULL" << std::endl;
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
