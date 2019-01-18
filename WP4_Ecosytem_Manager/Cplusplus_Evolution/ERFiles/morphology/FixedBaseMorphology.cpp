#include "FixedBaseMorphology.h"
#include <iostream>
using namespace std;

FixedBaseMorphology::FixedBaseMorphology()
{
}

FixedBaseMorphology::~FixedBaseMorphology()
{
//	cout << "should delete FixedBaseMorphology" << endl; 
}

void FixedBaseMorphology::init() {
	cout << "FixedBaseMorphology.init() is called" << endl; 
	addDefaultMorphology();
	BaseMorphology::init();
}

void FixedBaseMorphology::addDefaultMorphology() {
	cout << "addDefaultMorphology is called but the function is deprived" << endl; 
}

void FixedBaseMorphology::saveGenome(int indNum, int sceneNum, float fitness) {
	cout << "saving Fixed Genome " << endl << "-------------------------------- " << endl;
	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository << "/morphologies" << sceneNum << "/genome" << indNum << ".csv";
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." std::endl;
	}
	if (settings->morphologyType == settings->CAT_MORPHOLOGY) {
		genomeFile << ",#GenomeType,CatGenome," << endl;
	}
	else {
		genomeFile << ",#GenomeType,unknown," << endl;
	}
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	genomeFile << "#ControlParams:," << endl;
	genomeFile << "	#ControlType,0," << endl;
	if (control) {
		genomeFile << control->getControlParams().str() << endl;
	}
	genomeFile << "#EndControlParams" << endl;
	genomeFile << "end of fixed Morphology" << endl;
	cout << "saved cat" << endl;
	genomeFile.close();
}