#include <ARE/Genome.h>

using namespace are;

Genome::Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param)
{
    randomNum = rn;
    parameters = param;
    genomeFitness = 0;
}


Genome::~Genome() {
    parameters.reset();
    randomNum.reset();
}


//bool Genome::loadGenome(int indNum, int sceneNum)
//{
//    return loadMorphologyGenome(indNum, sceneNum);
//}

//bool Genome::loadGenome(std::istream &input, int indNum)
//{
//    return loadMorphologyGenome(input, indNum);
//}




//void Genome::checkGenome() {
//    std::cout << "mainHandle = " << morph->getMainHandle() << std::endl;
//    if (morph == NULL) {
//        std::cout << "morph is null, cannot run evolution without a morphology being loaded" << std::endl;
//    }
//    if (settings->verbose) {
//        // prints some debugging information in the terminal
//        morph->printSome();
//    }
//}






//void Genome::loadBaseMorphology(int indNum, int sceneNum) {
//    // This is an old piece of code that I didn't want to delete yet.
//    std::cout << "about to load base morphology" << std::endl;
//    morph->loadBaseMorphology(indNum, sceneNum);
//}

//void Genome::init_noMorph() {
//    // For debugging. Shouldn't be used otherwise.
//    int m_type = settings->morphologyType;
//    morph = morphologyFactory(m_type, randomNum, settings);
//}

//void Genome::savePhenotype(int indNum, int sceneNum)
//{
//    morph->savePhenotype(indNum, fitness);
//}

//void Genome::init() {
//    // To initialize a genome
//    int m_type = settings->morphologyType;
////	MorphologyFactory morphologyFactory;
////	morph = morphologyFactory.createMorphologyGenome(m_type, randomNum, settings);

//    morph = morphologyFactory(m_type, randomNum, settings);

//    morph->init();
////	if (m_type == -1) { // not used
////		unique_ptr<ControlFactory> controlFactory(new ControlFactory);
////		morph->control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
////		controlFactory.reset();
////	}
//}

//void Genome::mutate() {
//    if (morph == NULL) {
//        std::cout << "ERROR: morph == NULL" << std::endl;
//    }
//    // TODO: The morphology mutation rate should be gathered from the settings file in the mutate function?
//    morph->mutate();
//}

//void Genome::create() {
//    morph->create();
//}

//void Genome::createAtPosition(float x, float y, float z){
//    morph->createAtPosition(x, y, z);
//}

//void Genome::clearGenome() {
//    morph->clearMorph();
//}
