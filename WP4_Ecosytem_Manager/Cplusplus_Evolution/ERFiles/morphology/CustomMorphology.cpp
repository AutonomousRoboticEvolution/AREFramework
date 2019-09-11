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

//load the control parameters
bool CustomMorphology::loadGenome(int individualNumber, int sceneNum) {
    cout << "loading custom genome " << individualNumber << endl;
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
    //read the control parameters from file
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
    //list<string>::const_iterator it = values.begin();
    for (list<string>::const_iterator it = values.begin(); it != values.end(); it++) {
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
    cout << "loaded custom morphology" << endl;
    return true;
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
	control->init(2, 0, 2);  //2: two inputs; 1: one hidden neuron; 2: two outputs
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


