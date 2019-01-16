#include "ER_DirectEncoding.h"
#include <iostream>
#include <math.h>


ER_DirectEncoding::ER_DirectEncoding()
{
	cout << "CREATED DIRECT ENCODING" << endl;
	modular = true;
}


ER_DirectEncoding::~ER_DirectEncoding()
{

}


void ER_DirectEncoding::crossover(shared_ptr<Morphology> partnerMorph, float cr) {
//	cout << "crossing over" << endl;
	shared_ptr<ER_DirectEncoding>morpho(static_pointer_cast<ER_DirectEncoding>(partnerMorph));   //*partnerMorph(ER_DirectEncoding()));
	shared_ptr<GENOTYPE> tempLGenome;
//	tempLGenome = morpho->lGenome;
	tempLGenome = morpho->genome->clone(); 

	bool cross = false;
	for (int i = 0; i < genome->amountModules; i++) {
		if (randomNum->randFloat(0, 1) < cr) {
			if (cross == false) {
				cross = true;
			}
			else {
				cross = false;
			}
		}
		if (cross == true) {
//			std::unique_ptr<my_type> ptr2{ new my_type{ *ptr1 } }; // Make deep copy using copy ctor.
//			lGenome->lParameters[i] = tempLGenome->lParameters[i];
			genome->moduleParameters[i] = tempLGenome->moduleParameters[i]->clone();// unique_ptr<LPARAMETERS>(new tempLGenome->lParameters[i]);
		}
	}
}

void ER_DirectEncoding::printSome() {
	BaseMorphology::printSome();
	cout << "printing some from ER_DirectEncoding" << endl;
}


void ER_DirectEncoding::initializeGenomeCustom(int type)
{
	float PI = 3.14159265;
	float red[3] = { 1.0, 0, 0 };
	float blue[3] = { 0.0, 0.0, 1.0 };
	float yellow[3] = { 1.0, 1.0, 0.0 };
	float green[3] = { 0.0, 0.3, 0.0 };
	float orange[3] = { 1.0, 0.5, 0.0 };
	float orangePlus[3] = { 1.0, 0.6, 0.2 };
	float black[3] = { 0.0, 0.0, 0 };
	float pink[3] = { 1.0, 0.5, 0.5 };
	float white[3] = { 1.0, 1.0, 1.0 };

	cout << "initializing custom L-System Genome" << endl;

	morphFitness = 0;
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);

	cf.reset();
}


void ER_DirectEncoding::initializeQuadruped(int type)
{
	float PI = 3.14159265;
	float red[3] = { 1.0, 0, 0 };
	float blue[3] = { 0.0, 0.0, 1.0 };
	float yellow[3] = { 1.0, 1.0, 0.0 };
	float green[3] = { 0.0, 0.3, 0.0 };
	float orange[3] = { 1.0, 0.5, 0.0 };
	float orangePlus[3] = { 1.0, 0.6, 0.2 };
	float black[3] = { 0.0, 0.0, 0 };
	float pink[3] = { 1.0, 0.5, 0.5 };
	float white[3] = { 1.0, 1.0, 1.0 };

	cout << "initializing custom L-System Genome" << endl;

	morphFitness = 0;
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);

	cf.reset();
}

void ER_DirectEncoding::saveGenome(int indNum, int sceneNum, float fitness) {
	cout << "saving direct genome " << endl << "-------------------------------- "<< endl;
//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	int amountStates = genome->moduleParameters.size(); 

	ofstream genomeFile; 
	ostringstream genomeFileName; 
	genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << indNum << ".csv";
//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	genomeFile << "#Individual:" << indNum << endl; 
	genomeFile << "#Fitness:," << fitness << endl;
	genomeFile << "#phenValue;," << phenValue << endl;
	genomeFile << "#AmountStates:," << amountStates << "," << endl << endl;
//	cout << "#AmountStates:," << amountStates << "," << endl << endl;

	genomeFile << "Module Parameters Start Here: ," << endl << endl;
	for (int i = 0; i < amountStates; i++) {
		if (genome->moduleParameters[i]->expressed == true) {
			genomeFile << "#Module:," << i << endl;
			genomeFile << "#ModuleType:," << genome->moduleParameters[i]->type << endl;
			//		genomeFile << "#ModuleState:," << genome->moduleParameters[i]->currentState << endl; 
			genomeFile << "#ModuleParent:," << genome->moduleParameters[i]->parent << endl;
			genomeFile << "#ParentSite:," << genome->moduleParameters[i]->parentSite << endl;
			genomeFile << "#ParentSiteOrientation:," << genome->moduleParameters[i]->orientation << endl;
			genomeFile << "#AmountChilds:," << "0," << endl;
			//		genomeFile << "#AmountChilds:," << genome->moduleParameters[i]->amountChilds << endl;
			genomeFile << "#ChildSites:,";
			for (int j = 0; j < genome->moduleParameters[i]->childSites.size(); j++) {
				genomeFile << genome->moduleParameters[i]->childSites[j] << ",";
			} genomeFile << endl;
			genomeFile << "#ChildConfigurations:,";
			for (int j = 0; j < genome->moduleParameters[i]->childConfigurations.size(); j++) {
				genomeFile << genome->moduleParameters[i]->childConfigurations[j] << ",";
			} genomeFile << endl;
			genomeFile << "#ChildSiteStates:,";
			for (int j = 0; j < genome->moduleParameters[i]->childSiteStates.size(); j++) {
				genomeFile << genome->moduleParameters[i]->childSiteStates[j] << ",";
			} genomeFile << endl;
			//		genomeFile << "," << lGenome->lParameters[i]->moduleType << ",";
			genomeFile << "#ControlParams:," << endl;
			genomeFile << genome->moduleParameters[i]->control->getControlParams().str();

			genomeFile << "#EndOfModule," << endl << endl;
		}
	}
	genomeFile.close();
}

float ER_DirectEncoding::getFitness() {
	return fitness;
}

void ER_DirectEncoding::loadGenome(int individualNumber, int sceneNum) {
	if (settings->verbose) {
		cout << "loading genome " << individualNumber << "(ER_Direct)" << endl;
	}
	genome = shared_ptr<GENOTYPE>(new GENOTYPE);
//	lGenome->lParameters.clear();
//	cout << "lGenome cleared" << endl; 
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
//	genomeFileName << "files/morphologies0/genome9137.csv";
	cout << genomeFileName.str() << endl;
	ifstream genomeFile(genomeFileName.str());
	string value;
	list<string> values;
	while (genomeFile.good()) {
		getline(genomeFile, value, ',');
//		cout << value << ",";
		if (value.find('\n') != string::npos) {
			split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
	int moduleNum;
	vector<string> moduleValues;
	vector<string> controlValues;
	bool checkingModule = false;
	bool checkingControl = false; 

	list<string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		if (settings->verbose) {
			cout << "," << tmp;
		}
		if (checkingModule == true) {
			moduleValues.push_back(tmp);
		}
		if (checkingControl == true) {
			controlValues.push_back(tmp);
		}
		if (tmp == "#Fitness:") {
			it++;
			tmp = *it;
			fitness = atof(tmp.c_str());
	//		cout << "Fitness was " << fitness << endl; 
		}
		if (tmp == "#phenValue;") {
			it++;
			tmp = *it;
			phenValue = atof(tmp.c_str());
		}
		else if (tmp == "#AmountStates:") {
			it++;
			tmp = *it;
			genome->amountModules = atoi(tmp.c_str());
			for (int i = 0; i < genome->amountModules; i++) {
				genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
			}
		}
		if (tmp == "#Module:") {
			it++;
			tmp = *it;
			moduleNum = atoi(tmp.c_str());
			checkingModule = true; 
	//		cout << "moduleNum set to " << moduleNum << endl; 
		}
		else if (tmp == "#AmountChilds:") {
			it++;
			tmp = *it;
			int ac = atoi(tmp.c_str());
			genome->moduleParameters[moduleNum]->amountChilds = ac;
			genome->moduleParameters[moduleNum]->childSites.resize(ac);
			genome->moduleParameters[moduleNum]->childConfigurations.resize(ac);
			genome->moduleParameters[moduleNum]->childSiteStates.resize(ac);
		}
		else if (tmp == "#ModuleParent:")
		{
			it++;
			tmp = *it;
			genome->moduleParameters[moduleNum]->parent = atoi(tmp.c_str());
		}
		else if (tmp == "#ModuleType:")
		{
			it++;
			tmp = *it;
			genome->moduleParameters[moduleNum]->type = atoi(tmp.c_str());
		}
		else if (tmp == "#ParentSite:")
		{
			it++;
			tmp = *it;
			genome->moduleParameters[moduleNum]->parentSite = atoi(tmp.c_str());
		}
		else if (tmp == "#ParentSiteOrientation:") 
		{
			genome->moduleParameters[moduleNum]->orientation = atoi(tmp.c_str());
		}
		else if (tmp == "#ChildSites:") {
			genome->moduleParameters[moduleNum]->maxChilds = 0;
			for (int i = 0; i < genome->moduleParameters[moduleNum]->amountChilds; i++) {
				it++;
				tmp = *it;
				genome->moduleParameters[moduleNum]->childSites[i] = atoi(tmp.c_str());
			}
		}
		else if (tmp == "#ChildConfigurations:") {
			for (int i = 0; i < genome->moduleParameters[moduleNum]->amountChilds; i++) {
				it++;
				tmp = *it;
				genome->moduleParameters[moduleNum]->childConfigurations[i] = atoi(tmp.c_str());
			}
		}
		else if (tmp == "#ChildSiteStates:") {
	//		cout << "loading childSiteStates" << endl;
			for (int i = 0; i < genome->moduleParameters[moduleNum]->amountChilds; i++) {
				it++;
				tmp = *it;
				genome->moduleParameters[moduleNum]->childSiteStates[i] = atoi(tmp.c_str());
			}
		}
		else if (tmp == "#ModuleType:") {
			it++;
			tmp = *it;
	//		cout << "creating module of type: " << atoi(tmp.c_str()) << endl; 
			genome->moduleParameters[moduleNum]->type = atoi(tmp.c_str());
			genome->moduleParameters[moduleNum]->currentState = moduleNum;
	//		cout << "state = " << lGenome->lParameters[moduleNum].module->state << endl;
		}
		if (tmp == "#ControlParams:") {
			checkingControl = true; 
		}
		else if (tmp == "#EndOfModule") {
			// this is not right: change it 
//			lGenome->lParameters[moduleNum]->module->setModuleParams(moduleValues);
			moduleValues.clear(); 
			if (checkingControl == true) {
				unique_ptr<ControlFactory> controlFactory(new ControlFactory);
				genome->moduleParameters[moduleNum]->control = controlFactory->createNewControlGenome(atoi(controlValues[2].c_str()), randomNum, settings); // 0 is ANN
			//	lGenome->lParameters[moduleNum]->control->init(1, 2, 1);
				genome->moduleParameters[moduleNum]->control->setControlParams(controlValues);
				checkingControl = false;
				controlValues.clear(); 
				controlFactory.reset();
			}
			moduleNum++;
			checkingModule = false; 
		}
	}

	// setting color
	float red[3] = { 1.0, 0, 0 };
	float blue[3] = { 0.0, 0.0, 1.0 };
	float yellow[3] = { 1.0, 1.0, 0.0 };
	float green[3] = { 0.0, 0.3, 0.0 };
	float orange[3] = { 1.0, 0.5, 0.0 };
	float orangePlus[3] = { 1.0, 0.6, 0.2 };
	float black[3] = { 0.0, 0.0, 0 };
	float pink[3] = { 1.0, 0.5, 0.5 };
	float white[3] = { 0.95,0.95,0.95 };
	float grey[3] = { 0.45,0.45,0.45 };

	//	lGenome->lParameters.resize(lGenome->amountStates);
	//	cout << "amount of states = " << lGenome->amountStates << endl; 
	morphFitness = 0;
	for (int i = 0; i < genome->amountModules; i++) {
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = red[j];
			}
			break;
		case 5:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = yellow[j];
			}
			break;
		default:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = grey[j];
			}
			break;
		}
		genome->moduleParameters[i]->moduleColor[0] = genome->moduleParameters[i]->color[0];
		genome->moduleParameters[i]->moduleColor[1] = genome->moduleParameters[i]->color[1];
		genome->moduleParameters[i]->moduleColor[2] = genome->moduleParameters[i]->color[2];
	}

	cout << "loaded direct genome" << endl;
}

void ER_DirectEncoding::init() {
	genome = shared_ptr<GENOTYPE>(new GENOTYPE);
	maxModuleTypes = settings->maxModuleTypes;
	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->morphologyType != settings->QUADRUPED_DIRECT) {
//		initializeGenomeCustom(0);
		initializeGenome(0);
	}
	else if (settings->morphologyType == settings->QUADRUPED_DIRECT) {
		initializeQuadruped(0);
	}
	else {
//		initializeLRobot(0);
	}
}


int ER_DirectEncoding::initializeGenome(int type) {
	//first read settings
	float PI = 3.14159265;
	float red[3] = { 1.0, 0, 0 };
	float blue[3] = { 0.0, 0.0, 1.0 };
	float yellow[3] = { 1.0, 1.0, 0.0 };
	float green[3] = { 0.0, 0.3, 0.0 };
	float orange[3] = { 1.0, 0.5, 0.0 };
	float orangePlus[3] = { 1.0, 0.6, 0.2 };
	float black[3] = { 0.0, 0.0, 0 };
	float pink[3] = { 1.0, 0.5, 0.5 };
	float white[3] = { 1.0, 1.0, 1.0 };
	
	cout << "initializing Direct Encoding Genome" << endl; 
	//	lGenome->lParameters.resize(lGenome->amountStates);
	//	cout << "amount of states = " << lGenome->amountStates << endl; 
	morphFitness = 0;
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);
	for (int i = 0; i < genome->amountModules; i++) {
		genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
		genome->moduleParameters[i]->type = settings->initialModuleType;
		vector<float>tempVector;
		// currently everything has a neural network...
		genome->moduleParameters[i]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
		if (settings->controlType == settings->ANN_DISTRIBUTED_BOTH) {
			genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
			cout << "initializing both!!!" << endl;
//			cout << "OUTPUTSIZE = " << genome->moduleParameters[i]->control-> outputLayer.size() << endl;

		}
		else if (settings->controlType == settings->ANN_DISTRIBUTED_UP) {
			genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
		}
		else if (settings->controlType == settings->ANN_DISTRIBUTED_DOWN) {
			genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
		}
		else {
			genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
		}

//		genome->moduleParameters[i]->control->init(1, 1, 1);
	
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = green[j];
			}
		case 5:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = orange[j];
			}
		default:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->color[j] = white[j];
			}
			break;
		}

		if (i == 0) {
			//			lGenome->lParameters[i]->childConfigurations.push_back(0);
			//			lGenome->lParameters[i]->childSites.push_back(0);
			//			lGenome->lParameters[i]->childSiteStates.push_back(2);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			if (settings->moduleTypes[i] == 4 || settings->moduleTypes[i] == 9) {
				genome->moduleParameters[i]->maxChilds = 3;
			}
			else if (settings->moduleTypes[i] == 6 || settings->moduleTypes[i] == 3) {
				genome->moduleParameters[i]->maxChilds = 0;
			}
			else if (settings->moduleTypes[i] == 1) {
				genome->moduleParameters[i]->maxChilds = 5;
			}
			else {
				genome->moduleParameters[i]->maxChilds = 3;
			}
		}
		else {
			if (settings->moduleTypes[i] == 4 || settings->moduleTypes[i] == 9) {
				genome->moduleParameters[i]->maxChilds = 3;
			}
			else if (settings->moduleTypes[i] == 6 || settings->moduleTypes[i] == 3) {
				genome->moduleParameters[i]->maxChilds = 0;
			}
			else if (settings->moduleTypes[i] == 1) {
				genome->moduleParameters[i]->maxChilds = 5;
			}
			else {
				genome->moduleParameters[i]->maxChilds = 3;
			}
		}
		genome->moduleParameters[i]->currentState = i;
		genome->moduleParameters[i]->parent = i - 1;
		genome->moduleParameters[i]->parentSite = 0;
		genome->moduleParameters[i]->orientation = 0;

		/*	if (i == 2) {
				lGenome->lParameters[i]->module->state = i;
				lGenome->lParameters[i]->childConfigurations.push_back(0);
				lGenome->lParameters[i]->childSites.push_back(1);
				lGenome->lParameters[i]->childSiteStates.push_back(0);
				lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			}*/
	}
	cf.reset();
	mutateERGenome(0.5);
	mutateERGenome(0.5);
	return 1;
}

shared_ptr<Morphology> ER_DirectEncoding::clone() const {
	BaseMorphology::clone();
	shared_ptr<ER_DirectEncoding> ur = make_unique<ER_DirectEncoding>(*this);
	for (int i = 0; i < ur->genome->moduleParameters.size(); i++) {
		ur->genome->moduleParameters[i] = ur->genome->moduleParameters[i]->clone();
	}
	ur->genome = ur->genome->clone();
	return ur;
}

void ER_DirectEncoding::update() {
	cout << "cannot update Genome:: no phenotype" << endl;
}

void ER_DirectEncoding::symmetryMutation(float mutationRate) {
	cout << "This version does not support symmetry mutation, check code" << endl;
}

int ER_DirectEncoding::mutateERGenome(float mutationRate) {
	cout << "mutating direct" << endl;
	// 1) add module, 
	// 2) mutate morphology
	// 3) mutate control
	// 4) prune 
	// 5) symmetry

//	int typeOfMutation = randomNum->randInt(5, 0);
//	typeOfMutation = 0;
//	switch (typeOfMutation) {
//	case 0: {
	for (int am = 0; am < 5; am++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			if (settings->verbose) {
				cout << "adding module" << endl;
			}		// add module
			// select where module should be attached to
			if (genome->moduleParameters.size() < 50) { // absolute max amount modules
				int attachModule = randomNum->randInt(genome->moduleParameters.size(), 0);
				int attachType = genome->moduleParameters[attachModule]->type;
				if (genome->moduleParameters[attachModule]->childSiteStates.size() < genome->moduleParameters[attachModule]->maxChilds) {
					int newModuleType = 4;//settings->moduleTypes[randomNum->randInt(settings->moduleTypes.size(), 0)];
					//	genome->moduleParameters.size(); // type of module to be attached // change
					bool collideSite = true;
					int newModuleLocation = 0;
					newModuleLocation = randomNum->randInt(genome->moduleParameters[attachModule]->maxChilds, 0);

					//while (collideSite == true) {
					//	newModuleLocation = randomNum->randInt(genome->moduleParameters[attachModule]->maxChilds, 0);
					//	collideSite = false;
					//	for (int i = 0; i < genome->moduleParameters[attachModule]->childSites.size(); i++) {
					//		if (genome->moduleParameters[attachModule]->childSites[i] == newModuleLocation) {
					//			collideSite = true;
					//		}
					//	}
					//}

					int moduleAmount = 0;
					int typeLoc = -1; // should always be assigned
					for (int j = 0; j < settings->moduleTypes.size(); j++) {
						if (newModuleType == settings->moduleTypes[j]) {
							typeLoc = j;
							for (int i = 0; i < genome->moduleParameters.size(); i++) {
								if (genome->moduleParameters[i]->type == settings->moduleTypes[j]) {
									moduleAmount++;
									break;
								}
							}
						}
					}

					if (moduleAmount > settings->maxModuleTypes[typeLoc][1]) {

					}
					else {

						genome->moduleParameters[attachModule]->childSiteStates.push_back(newModuleType);
						genome->moduleParameters[attachModule]->childSites.push_back(newModuleLocation);
						genome->moduleParameters[attachModule]->childStates.push_back(newModuleType);
						int newOr = randomNum->randInt(4, 0);
						genome->moduleParameters[attachModule]->childConfigurations.push_back(newOr);
						// add a new moduleParameter struct for the new genome
						unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);
						genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
						int theNewModule = genome->moduleParameters.size() - 1;
						int n = randomNum->randInt(settings->moduleTypes.size() - 1, 1);
						genome->moduleParameters[theNewModule]->type = settings->moduleTypes[n];
						vector<float>tempVector;
						// currently everything has a neural network...
						genome->moduleParameters[theNewModule]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
						genome->moduleParameters[theNewModule]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
						genome->moduleParameters[theNewModule]->control->mutate(0.5);
						genome->moduleParameters[theNewModule]->control->mutate(0.5);
						cf.reset();
						genome->moduleParameters[theNewModule]->control->mutate(0.8);
						if (genome->moduleParameters[theNewModule]->type == 1) {
							genome->moduleParameters[theNewModule]->maxChilds = 5;
						}
						else if (genome->moduleParameters[theNewModule]->type == 4) {
							genome->moduleParameters[theNewModule]->maxChilds = 3;
						}
						else {
							genome->moduleParameters[theNewModule]->maxChilds = 0;
						}
						genome->moduleParameters[theNewModule]->amountChilds = genome->moduleParameters[theNewModule]->childSites.size();
						genome->moduleParameters[theNewModule]->parent = attachModule;
						genome->moduleParameters[theNewModule]->parentSite = newModuleLocation;
						genome->moduleParameters[theNewModule]->orientation = newOr;
						genome->amountModules = genome->moduleParameters.size();
					}
				}
			}
		}
	}
	
//		break;
//	}
//	case 1: { // mutate orientation stored in childConfigurations
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) <settings->morphMutRate) {
			for (int j = 0; j < genome->moduleParameters[i]->childConfigurations.size(); j++) {
//				if (randomNum->randFloat(0.0, 1.0) < settings->morphMutRate) {
					int newOr = randomNum->randInt(4, 0);
					genome->moduleParameters[i]->childConfigurations[j] = newOr;
//				}
			}
		}
	}
//		break;
//	}
//	case 2: { // mutate control
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
			if (genome->moduleParameters[i]->control) { // to check if not nullpointer
				genome->moduleParameters[i]->control->mutate(settings->mutationRate);
			}
	}
//		break;
//	}
//	case 3: // prune
//	{
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		if (settings->verbose) {
			cout << "pruning" << endl;
		}
		if (genome->moduleParameters.size() > 1) {
			int randomModule = randomNum->randInt(genome->moduleParameters.size() - 1, 1);
			// get childs to be deleted
			vector<int> deleteModules;
			deleteModules.push_back(randomModule);
//			genome->moduleParameters.erase(genome->moduleParameters.begin() + randomModule);
			cout << "deleting: " << randomModule << endl;
			for (int j = 0; j < deleteModules.size(); j++) {
				cout << "dm[j]: " << deleteModules[j] << endl;
				for (int i = 0; i < genome->moduleParameters.size(); i++) {
					if (genome->moduleParameters[i]->parent == deleteModules[j])
					{
						deleteModules.push_back(i);
						cout << "deleting: " << i << ", " << genome->moduleParameters[i]->parent << ", dm: " << deleteModules[j] << endl;
//						genome->moduleParameters.erase(genome->moduleParameters.begin() + i);
					}
				}
			}
			cout << "actual deletion" << endl;
			int dCounter = 0;
			/*for (int i = 0; i < deleteModules.size(); i++) {
				cout << "i: " << i << endl;
				cout << "deleting " << deleteModules[i] - dCounter << endl;
				cout << "module params size = " << genome->moduleParameters.size() << endl;
				cout << "dcounter = " << dCounter; cout << ", deleteModules[i] = " << deleteModules[i];
				genome->moduleParameters.erase(genome->moduleParameters.begin() + (deleteModules[i] - dCounter));
				cout << "removed one from module parameters" << endl;
				genome->amountModules -= 1;
				for (int j = 0; j < genome->moduleParameters.size(); j++) {
					cout << "j: " << j << endl;
					if (genome->moduleParameters[j]->parent >= deleteModules[i] - dCounter) {
						genome->moduleParameters[j]->parent -= 1;
					}
				}
				cout << "deleted" << endl;
				dCounter++;
			}*/
			std::sort(deleteModules.begin(), deleteModules.end());
			for (int i = deleteModules.size() -1; i >= 0; i--) {
				cout << "i: " << i << endl;
//				cout << "deleting " << deleteModules[i] - dCounter << endl;
				cout << "module params size = " << genome->moduleParameters.size() << endl;
//				cout << "dcounter = " << dCounter; cout << ", deleteModules[i] = " << deleteModules[i];
				genome->moduleParameters.erase(genome->moduleParameters.begin() + (deleteModules[i]));
				cout << "removed one from module parameters" << endl;
				genome->amountModules -= 1;
				for (int j = 0; j < genome->moduleParameters.size(); j++) {
					cout << "j: " << j << endl;
					if (genome->moduleParameters[j]->parent >= deleteModules[i] - dCounter) {
						genome->moduleParameters[j]->parent -= 1;
					}
				}
				cout << "deleted" << endl;
	//			dCounter++;
			}
			
//			genome->moduleParameters[randomModule];
		//	deleteModuleFromGenome(randomModule); // deletes module branch (includes all child modules)
		}
	}
//		break;
//	}
//	case 4: // symmetryMutation
//	{
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			symmetryMutation(settings->morphMutRate);
		}
//		break;
//	}
//	} // end of switch

	if (settings->verbose) {
		cout << "mutated direct" << endl;
	} 
	return 1;
}

int ER_DirectEncoding::mutateControlERGenome(float mutationRate) {
	cout << "mutating l-genome" << endl;
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		//		cout << "i = " << i << endl; 
		genome->moduleParameters[i]->control->mutate(mutationRate);
	}
	return 1;
}

void ER_DirectEncoding::deleteModuleFromGenome(int num)
{
	while (genome->moduleParameters[num]->childStates.size() > 0) {
		deleteModuleFromGenome(genome->moduleParameters[num]->childStates.size() - 1);
		genome->moduleParameters[num]->childSites.pop_back();
		genome->moduleParameters[num]->childConfigurations.pop_back();
		genome->moduleParameters[num]->childSiteStates.pop_back();
		genome->moduleParameters[num]->childStates.pop_back();
	}
}

void ER_DirectEncoding::mutate(float mutationRate) {
	//cout << "mutating lmorph" << endl;
	mutateERGenome(mutationRate);
}

void ER_DirectEncoding::checkGenome(int individualNumber, int sceneNum) {
	cout << "checkingGenome" << endl;
	ostringstream genomeFileName;
	genomeFileName << "interfaceFiles\\morphologies" << sceneNum << "\\genome" << individualNumber << ".csv";
	ifstream genomeFile(genomeFileName.str());
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
	int moduleNum;
	vector<string> moduleValues;
	vector<string> controlValues;
	bool checkingModule = false;
	bool checkingControl = false;

	list<string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		//	cout << "tmp = " << tmp << endl;
		if (checkingModule == true) {
			moduleValues.push_back(tmp);
		}
		if (checkingControl == true) {
			controlValues.push_back(tmp);
		}
		
		if (tmp == "#Module:") {
			it++;
			tmp = *it;
			moduleNum = atoi(tmp.c_str());
			checkingModule = true;
			//		cout << "moduleNum set to " << moduleNum << endl; 
		}
		if (tmp == "#ControlParams:") {
			checkingControl = true;
		}
		else if (tmp == "#EndOfModule") {
		//	lGenome->lParameters[moduleNum].module->setModuleParams(moduleValues);
			moduleValues.clear();
			// cannot check control in this version
			moduleNum++;
			checkingModule = false;
		}
	}
	cout << "checked L-System genome" << endl;
}

void ER_DirectEncoding::checkControl(int individual, int sceneNum) {
	checkGenome(individual, sceneNum);
}
