#include "CAT.h"
#include "v_repLib.h"
#include <iostream>
#include <fstream>

using namespace std;

CAT::CAT()
{
	float position[3] = { 0.0, 0.0, 1.0 };
	va = shared_ptr<VestibularAttributes>(new VestibularAttributes);
}


CAT::~CAT()
{
//	cout << "delete CAT class" << endl; 
}

shared_ptr<Morphology> CAT::clone() const {
	// shared_ptr<Morphology>()
	shared_ptr<CAT> cat = make_unique<CAT>(*this);
//	cat->va = va->clone();
	cat->control = control->clone();
	return cat;
}

void CAT::init_noMorph() {
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
//	control->init(1, 1, 1);//morph->morph_jointHandles);
}

void CAT::init(){
	create();
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	control->init(50, 28, 28);
	control->mutate(0.5);
}

void CAT::grow(int i) {
	cout << "Cat cannot grow; therefore, the call to grow(int) will be dismissed." << endl;
}

int CAT::getMainHandle() {
//	catHandle = simGetObjectHandle("Cat_Head");
	return catHandle;
}

void CAT::saveGenome(int indNum, float fitness) {
	cout << "saving Cat Genome " << endl << "-------------------------------- " << endl;
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
	genomeFile << "end of cat" << endl;
	cout << "saved cat" << endl;
	genomeFile.close();
}

bool CAT::loadGenome(int individualNumber, int sceneNum) {
	cout << "loading cat genome " << individualNumber << endl;
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
	ifstream genomeFile(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Could not load " << genomeFileName.str() << std::endl;
		return false;
		//		std::exit(1);
	}
	string value;
	list<string> values;
	while (genomeFile.good()) {
		getline(genomeFile, value, ',');
	//	cout << value << ",";
		if (value.find('\n') != string::npos) {
			split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
	vector<string> controlValues;
	bool checkingControl = false;

	list<string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		if (checkingControl == true) {
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

