#include "ER_CPPN_Encoding.h"
#include <iostream>
#include <sstream>
#include <cmath>


ER_CPPN_Encoding::ER_CPPN_Encoding()
{
	// cout << "CREATED CPPN ENCODING" << endl;
	modular = true;
}


ER_CPPN_Encoding::~ER_CPPN_Encoding()
{
	//if (genome) {
	//	for (int i = 0; i < genome->moduleParameters.size(); i++) {
	//		if (genome->moduleParameters[i]->control) {
	//			genome->moduleParameters[i]->control->~Control();
	//		}
	//		genome->moduleParameters[i].reset();
	//	}
	//	genome->moduleParameters.clear();
	//	genome.reset();
	//}
}

void ER_CPPN_Encoding::init() {
	genome = shared_ptr<GENOTYPE>(new GENOTYPE);
	maxModuleTypes = settings->maxModuleTypes;
	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->morphologyType != settings->QUADRUPED_DIRECT) {
		initializeGenome(0);
	}
	else if (settings->morphologyType == settings->QUADRUPED_DIRECT) {
		// not used in CPPN
		cout << "CPPN encoding cannot yet create a quadruped" << endl;
	}
	
}

int ER_CPPN_Encoding::initializeGenome(int type) {
	//first read settings

	if (settings->verbose) {
		cout << "initializing CPPN Encoding Genome" << endl;
	}
	morphFitness = 0;
	


	// CPPN currently takes the increment and position as inputs
	cppn = shared_ptr<CPPN>(new CPPN);
	cppn->settings = settings;
	cppn->init(8, 8, 6); // output is type, orientation and activation of sin 
	// cppn->mutate(settings->morphMutRate);
	cppn->mutate(0.5);

	// make a temporary storage for phenotype based on genotypic information
	//for (int i = 0; i < genome->amountModules; i++) {
	//	genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
	//	genome->moduleParameters[i]->type = settings->initialModuleType;
	//	vector<float>tempVector;
	//	// currently everything has a neural network...
	//	genome->moduleParameters[i]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
	//	if (settings->controlType == settings->ANN_DISTRIBUTED_BOTH) {
	//		genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
	//		cout << "initializing both!!!" << endl;
	//		//			cout << "OUTPUTSIZE = " << genome->moduleParameters[i]->control-> outputLayer.size() << endl;

	//	}
	//	else if (settings->controlType == settings->ANN_DISTRIBUTED_UP) {
	//		genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
	//	}
	//	else if (settings->controlType == settings->ANN_DISTRIBUTED_DOWN) {
	//		genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
	//	}
	//	else {
	//		genome->moduleParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
	//	}


	//}

	// mutateCPPN(0.5);
	// mutateERGenome(0.5);
	return 1;
}


void ER_CPPN_Encoding::mutate() {
	mutateCPPN(settings->morphMutRate);
}


int ER_CPPN_Encoding::mutateCPPN(float mutationRate) {
	if (settings->verbose) {
		cout << "mutating CPPN" << endl;
	}
	cppn->mutate(settings->morphMutRate);

	//cout << "mutating direct" << endl;
	//// 1) add module, 
	//// 2) mutate morphology
	//// 3) mutate control
	//// 4) prune 
	//// 5) symmetry

	//for (int am = 0; am < 5; am++) {
	//	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
	//		if (settings->verbose) {
	//			cout << "adding module" << endl;
	//		}
	//		// add module
	//		// select where module should be attached to
	//		if (genome->moduleParameters.size() < 50) { // absolute max amount modules
	//			int attachModule = randomNum->randInt(genome->moduleParameters.size(), 0);
	//			int attachType = genome->moduleParameters[attachModule]->type;
	//			if (genome->moduleParameters[attachModule]->childSiteStates.size() < genome->moduleParameters[attachModule]->maxChilds) {
	//				int newModuleType = 4;//settings->moduleTypes[randomNum->randInt(settings->moduleTypes.size(), 0)];
	//				//	genome->moduleParameters.size(); // type of module to be attached // change
	//				bool collideSite = true;
	//				int newModuleLocation = 0;
	//				newModuleLocation = randomNum->randInt(genome->moduleParameters[attachModule]->maxChilds, 0);

	//				int moduleAmount = 0;
	//				int typeLoc = -1; // should always be assigned
	//				for (int j = 0; j < settings->moduleTypes.size(); j++) {
	//					if (newModuleType == settings->moduleTypes[j]) {
	//						typeLoc = j;
	//						for (int i = 0; i < genome->moduleParameters.size(); i++) {
	//							if (genome->moduleParameters[i]->type == settings->moduleTypes[j]) {
	//								moduleAmount++;
	//								break;
	//							}
	//						}
	//					}
	//				}

	//				if (moduleAmount > settings->maxModuleTypes[typeLoc][1]) {

	//				}
	//				else {

	//					genome->moduleParameters[attachModule]->childSiteStates.push_back(newModuleType);
	//					genome->moduleParameters[attachModule]->childSites.push_back(newModuleLocation);
	//					genome->moduleParameters[attachModule]->childStates.push_back(newModuleType);
	//					int newOr = randomNum->randInt(4, 0);
	//					genome->moduleParameters[attachModule]->childConfigurations.push_back(newOr);
	//					unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);
	//					genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
	//					int theNewModule = genome->moduleParameters.size() - 1;
	//					int n = randomNum->randInt(settings->moduleTypes.size() - 1, 1);
	//					genome->moduleParameters[theNewModule]->type = settings->moduleTypes[n];
	//					vector<float>tempVector;
	//					genome->moduleParameters[theNewModule]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
	//					genome->moduleParameters[theNewModule]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
	//					genome->moduleParameters[theNewModule]->control->mutate(0.5);
	//					genome->moduleParameters[theNewModule]->control->mutate(0.5);
	//					cf.reset();
	//					genome->moduleParameters[theNewModule]->control->mutate(0.8);
	//					if (genome->moduleParameters[theNewModule]->type == 1) {
	//						genome->moduleParameters[theNewModule]->maxChilds = 5;
	//					}
	//					else if (genome->moduleParameters[theNewModule]->type == 4) {
	//						genome->moduleParameters[theNewModule]->maxChilds = 3;
	//					}
	//					else {
	//						genome->moduleParameters[theNewModule]->maxChilds = 0;
	//					}
	//					genome->moduleParameters[theNewModule]->amountChilds = genome->moduleParameters[theNewModule]->childSites.size();
	//					genome->moduleParameters[theNewModule]->parent = attachModule;
	//					genome->moduleParameters[theNewModule]->parentSite = newModuleLocation;
	//					genome->moduleParameters[theNewModule]->orientation = newOr;
	//					genome->amountModules = genome->moduleParameters.size();
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int i = 0; i < genome->moduleParameters.size(); i++) {
	//	if (randomNum->randFloat(0.0, 1.0) < settings->morphMutRate) {
	//		for (int j = 0; j < genome->moduleParameters[i]->childConfigurations.size(); j++) {
	//			int newOr = randomNum->randInt(4, 0);
	//			genome->moduleParameters[i]->childConfigurations[j] = newOr;
	//		}
	//	}
	//}
	//for (int i = 0; i < genome->moduleParameters.size(); i++) {
	//	if (genome->moduleParameters[i]->control) { // to check if not nullpointer
	//		genome->moduleParameters[i]->control->mutate(settings->mutationRate);
	//	}
	//}
	//if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
	//	if (settings->verbose) {
	//		cout << "pruning" << endl;
	//	}
	//	if (genome->moduleParameters.size() > 1) {
	//		int randomModule = randomNum->randInt(genome->moduleParameters.size() - 1, 1);
	//		vector<int> deleteModules;
	//		deleteModules.push_back(randomModule);
	//		cout << "deleting: " << randomModule << endl;
	//		for (int j = 0; j < deleteModules.size(); j++) {
	//			cout << "dm[j]: " << deleteModules[j] << endl;
	//			for (int i = 0; i < genome->moduleParameters.size(); i++) {
	//				if (genome->moduleParameters[i]->parent == deleteModules[j])
	//				{
	//					deleteModules.push_back(i);
	//					cout << "deleting: " << i << ", " << genome->moduleParameters[i]->parent << ", dm: " << deleteModules[j] << endl;
	//					//						genome->moduleParameters.erase(genome->moduleParameters.begin() + i);
	//				}
	//			}
	//		}
	//		cout << "actual deletion" << endl;
	//		int dCounter = 0;
	//		std::sort(deleteModules.begin(), deleteModules.end());
	//		for (int i = deleteModules.size() - 1; i >= 0; i--) {
	//			cout << "i: " << i << endl;
	//			cout << "module params size = " << genome->moduleParameters.size() << endl;
	//			genome->moduleParameters.erase(genome->moduleParameters.begin() + (deleteModules[i]));
	//			cout << "removed one from module parameters" << endl;
	//			genome->amountModules -= 1;
	//			for (int j = 0; j < genome->moduleParameters.size(); j++) {
	//				cout << "j: " << j << endl;
	//				if (genome->moduleParameters[j]->parent >= deleteModules[i] - dCounter) {
	//					genome->moduleParameters[j]->parent -= 1;
	//				}
	//			}
	//			cout << "deleted" << endl;
	//		}
	//	}
	//}
	//if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
	//	symmetryMutation(settings->morphMutRate);
	//}

	if (settings->verbose) {
		cout << "mutated CPPN" << endl;
	}
	return 1;
}


void ER_CPPN_Encoding::crossover(shared_ptr<Morphology> partnerMorph, float cr) {
		
	shared_ptr<ER_CPPN_Encoding>morpho(static_pointer_cast<ER_CPPN_Encoding>(partnerMorph));   //*partnerMorph(ER_CPPN_Encoding()));
	shared_ptr<GENOTYPE> tempGenome;
	tempGenome = morpho->genome->clone(); 

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
			// do some sort of crossover here
		}
	}
}

void ER_CPPN_Encoding::printSome() {
	BaseMorphology::printSome();
	cout << "printing some from ER_CPPN_Encoding : Use this function for debugging" << endl;
}


/*!
 * Temporary function for future use
 * 
 * \param type
 */
void ER_CPPN_Encoding::initializeQuadruped(int type)
{
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

/*!
 * Generating the genome
 * 
 * \param indNum
 * \param fitness
 */
const std::string ER_CPPN_Encoding::generateGenome(int indNum, float fitness) const {
	if (settings->verbose) {
		cout << "generating CPPN genome " << endl << "-------------------------------- " << endl;
	}

	int amountStates = genome->moduleParameters.size();
	stringstream cppndata = cppn->getControlParams();
	
	ostringstream genomeText;
	genomeText << "#CPPN Genome" << endl;
	genomeText << "#Individual:" << indNum << "," << endl;
	genomeText << "#Fitness:," << fitness << "," << endl;
	genomeText << "#phenValue;," << phenValue << endl;
	genomeText << "#AmountStates:," << amountStates << "," << endl << endl;
	
	genomeText << "#CPPN_Start," << endl;
	genomeText << cppndata.str() << endl;
	genomeText << "#CPPN_End," << endl;
	genomeText << endl;

	return genomeText.str();
}

float ER_CPPN_Encoding::getFitness() {
	return fitness;
}

bool ER_CPPN_Encoding::loadGenome(std::istream &genomeInput, int individualNumber)
{
	if (settings->verbose) {
		cout << "loading genome " << individualNumber << "(ER_CPPN_Encoding)" << endl;
	}
	genome = std::shared_ptr<GENOTYPE>(new GENOTYPE);
	//	lGenome->lParameters.clear();
	//	cout << "lGenome cleared" << endl;
	
	string value;
	list<string> values;
	while (genomeInput.good())
	{
		getline(genomeInput, value, ',');
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
	vector<string> cppnValues;
	bool checkingModule = false;
	bool checkingControl = false;
	bool checkingCPPN = false;


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
		if (checkingCPPN == true) {
			cppnValues.push_back(tmp);
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
		else if (tmp == "#ModuleType:") {
			it++;
			tmp = *it;
			//		cout << "creating module of type: " << atoi(tmp.c_str()) << endl; 
			genome->moduleParameters[moduleNum]->type = atoi(tmp.c_str());
			//		cout << "state = " << lGenome->lParameters[moduleNum].module->state << endl;
		}
		if (tmp == "#CPPN_Start") {
			checkingCPPN = true;
		}
		else if (tmp == "#CPPN_End") {
			checkingCPPN = false;
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

	cppn = shared_ptr<Control>(new CPPN);
	cppn->settings = settings;
	cppn->setControlParams(cppnValues);

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

	if (settings->verbose) {
		cout << "loaded cppn genome" << endl;
	}
	return true;
}



shared_ptr<Morphology> ER_CPPN_Encoding::clone() const {
	BaseMorphology::clone();
	shared_ptr<ER_CPPN_Encoding> ur = make_unique<ER_CPPN_Encoding>(*this);
	ur->genome = this->genome->clone();
	for (int i = 0; i < ur->genome->moduleParameters.size(); i++) {
		ur->genome->moduleParameters[i] = this->genome->moduleParameters[i]->clone();
	}
	ur->cppn = this->cppn->clone();
	//make_unique<CPPN>(this)
	//shared_ptr<CPPN> cppnClone = make_unique<CPPN>(*cppn);
//	cppnClone->makeDependenciesUnique();
//	ur->cppn = cppnClone->clone();
	return ur;
}

void ER_CPPN_Encoding::update() {
	cout << "cannot update Genome:: no phenotype" << endl;
}

void ER_CPPN_Encoding::symmetryMutation(float mutationRate) {
	cout << "This version does not support symmetry mutation, check code" << endl;
}


int ER_CPPN_Encoding::mutateControlERGenome(float mutationRate) {
	cout << "mutating l-genome" << endl;
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		//		cout << "i = " << i << endl; 
		genome->moduleParameters[i]->control->mutate(mutationRate);
	}
	return 1;
}

void ER_CPPN_Encoding::deleteModuleFromGenome(int num)
{
	while (genome->moduleParameters[num]->childSiteStates.size() > 0) {
		deleteModuleFromGenome(genome->moduleParameters[num]->childSiteStates.size() - 1);
		genome->moduleParameters[num]->childSiteStates.pop_back();
	}
}



void ER_CPPN_Encoding::checkGenome(int individualNumber, int sceneNum) {
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

void ER_CPPN_Encoding::checkControl(int individual, int sceneNum) {
	checkGenome(individual, sceneNum);
}
