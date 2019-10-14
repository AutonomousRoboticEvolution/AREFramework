#include "CAT.h"
#include "v_repLib.h"
#include <iostream>
#include <fstream>

CAT::CAT()
{
    va = std::shared_ptr<VestibularAttributes>(new VestibularAttributes);
}


CAT::~CAT()
{
//	cout << "delete CAT class" << endl; 
}

std::shared_ptr<Morphology> CAT::clone() const {
	// shared_ptr<Morphology>()
    std::shared_ptr<CAT> cat = std::make_unique<CAT>(*this);
//	cat->va = va->clone();
	cat->control = control->clone();
	return cat;
}

void CAT::init_noMorph() {
//    std::unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
//	controlFactory.reset();
    control = controlFactory(0, randomNum, settings);
//	control->init(1, 1, 1);//morph->morph_jointHandles);
}

void CAT::init(){
	create();
//    std::unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
//	controlFactory.reset();
    control = controlFactory(0, randomNum, settings);

	control->init(50, 28, 28);
	control->mutate(0.5);
}

void CAT::grow(int i) {
    std::cout << "Cat cannot grow; therefore, the call to grow(int) will be dismissed." << std::endl;
}

int CAT::getMainHandle() {
//	catHandle = simGetObjectHandle("Cat_Head");
	return catHandle;
}

//save the genome into a file
void CAT::saveGenome(int indNum, float fitness) {
    std::cout << "saving Cat Genome " << std::endl << "-------------------------------- " << std::endl;
    std::ofstream genomeFile;
    std::ostringstream genomeFileName;
	genomeFileName << settings->repository << "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
	}
    genomeFile << ",#GenomeType,CatGenome," << std::endl;
    genomeFile << "#Individual:" << indNum << std::endl;
    genomeFile << "#Fitness:," << fitness << std::endl;
    genomeFile << "#ControlParams:," << std::endl;
    genomeFile << "	#ControlType,0," << std::endl;
	if (control) {
        genomeFile << control->getControlParams().str() << std::endl;
	}
    genomeFile << "#EndControlParams" << std::endl;
    genomeFile << "end of cat" << std::endl;
    std::cout << "saved cat" << std::endl;
	genomeFile.close();
}

//load the control parameters
bool CAT::loadGenome(int individualNumber, int sceneNum) {
    std::cout << "loading cat genome " << individualNumber << std::endl;
//    std::unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
//	controlFactory.reset();
    control = controlFactory(0, randomNum, settings);
    std::ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
    std::ifstream genomeFile(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Could not load " << genomeFileName.str() << std::endl;
		return false;
		//		std::exit(1);
	}
    std::string value;
    std::list<std::string> values;
	//read the control parameters from file
	while (genomeFile.good()) {
        std::getline(genomeFile, value, ',');
	//	cout << value << ",";
        if (value.find('\n') != std::string::npos) {
            split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
    std::vector<std::string> controlValues;
	bool checkingControl = false;

    std::list<std::string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
        std::string tmp = *it;
		if (checkingControl) {
			controlValues.push_back(tmp);
		}
		if (tmp == "#Fitness:") {
			it++;
			tmp = *it;
			fitness = atof(tmp.c_str());
		}
		if (tmp == "#ControlParams:") {
			checkingControl = true;
		}
		if (tmp == "#EndControlParams") {
			//cout << "setting control params" << endl;
			control->setControlParams(controlValues);
			controlValues.clear();
			checkingControl = false;
		}
	}
	return true;
}

void CAT::mutate() {
	if (control) {
		control->mutate(settings->mutationRate);
	}
}

