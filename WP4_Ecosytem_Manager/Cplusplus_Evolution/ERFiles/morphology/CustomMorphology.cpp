#include "CustomMorphology.h"



CustomMorphology::CustomMorphology()
{
}


CustomMorphology::~CustomMorphology()
{
}

vector<int> CustomMorphology::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> CustomMorphology::getJointHandles(int parentHandle)
{
	return vector<int>();
}

void CustomMorphology::update() {

}

int CustomMorphology::getMainHandle()
{
	return mainHandle;
}

void CustomMorphology::create()
{

}

void CustomMorphology::mutate() {
	if (control) {
		control->mutate(settings->mutationRate);
	}
}

void CustomMorphology::saveGenome(int indNum, float fitness) {
	cout << "saving Custom Morphology Genome " << endl << "-------------------------------- " << endl;

	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository << "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
	}
	genomeFile << ",#GenomeType,CatGenome," << endl;
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	genomeFile << "#ControlParams:," << endl;
	genomeFile << "	#ControlType,0," << endl;

	if (control) {
		genomeFile << control->getControlParams().str() << endl;
	}
	genomeFile << "#EndControlParams" << endl;
	genomeFile << "end of custom morphology" << endl;
	cout << "saved Custom morphology" << endl;
	
	genomeFile.close();
}

void CustomMorphology::init() {
	create();   //create the morphology
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	control->init(1, 1, 2);
	control->mutate(0.5);
}

shared_ptr<Morphology> CustomMorphology::clone() const {
	// shared_ptr<Morphology>()
	shared_ptr<CustomMorphology> cat = make_unique<CustomMorphology>(*this);
	cat->randomNum = randomNum;
	cat->settings = settings;
	//	cat->va = va->clone();
	cat->control = control->clone();
	return cat;
}