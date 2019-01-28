#include "ER_DirectEncoding.h"
#include <iostream>
#include <sstream>

ER_DirectEncoding::ER_DirectEncoding()
{
	modular = true;
}

ER_DirectEncoding::~ER_DirectEncoding()
{
	//if (genome) {
	//	for (int i = 0; i < genome->moduleParameters.size(); i++)
	//	{
	//		if (genome->moduleParameters[i]->control) {
	//			genome->moduleParameters[i]->control->~Control();
	//		}
	//		genome->moduleParameters[i].reset();
	//	}
	//	genome->moduleParameters.clear();
	//	genome.reset();
	//}
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

const std::string ER_DirectEncoding::generateGenome(int indNum, float fitness) const
{
	if (settings->verbose) {
		std::cout << "generating direct genome " << std::endl << "-------------------------------- " << std::endl;
	}
//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	int amountStates = genome->moduleParameters.size(); 

	std::ostringstream genomeText;

	genomeText << "#Individual:" << indNum << std::endl; 
	genomeText << "#Fitness:," << fitness << std::endl;
	genomeText << "#phenValue;," << phenValue << std::endl;
	genomeText << "#AmountStates:," << amountStates << "," << std::endl << std::endl;
//	cout << "#AmountStates:," << amountStates << "," << std::endl << std::endl;

	genomeText << "Module Parameters Start Here: ," << std::endl << std::endl;
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		//if (genome->moduleParameters[i]->expressed == true) {
			genomeText << "#Module:," << i << std::endl;
			genomeText << "#ModuleType:," << genome->moduleParameters[i]->type << std::endl;
			
			genomeText << "#ModuleParent:," << genome->moduleParameters[i]->parent << std::endl;
			genomeText << "#ParentSite:," << genome->moduleParameters[i]->parentSite << std::endl;
			genomeText << "#Orientation:," << genome->moduleParameters[i]->orientation << std::endl;
			
			genomeText << "#ControlParams:," << std::endl;
			genomeText << genome->moduleParameters[i]->control->getControlParams().str();
			genomeText << "#EndControlParams" << std::endl;
			genomeText << "#EndOfModule," << std::endl;
		//}
	}
	if (settings->verbose) {
		cout << "this is what I save" << endl;
		cout << genomeText.str() << endl;
	}

	//genomeFile.close();
	//if (settings->verbose) {
	//	cout << "Direct Genome Saved" << endl;
	//}


	return genomeText.str();

}

float ER_DirectEncoding::getFitness() {
	return fitness;
}

void ER_DirectEncoding::loadPhenotype(int ind)
{
	// load the phenotype
	vector<shared_ptr<BASEMODULEPARAMETERS>> bmp;
	bmp = Development::loadBasePhenotype(ind);
	// change the BASEMODULEPARAMETERS to actual parameters for the create function to use. (they are actually the same right now)
	genome = shared_ptr<GENOTYPE>(new GENOTYPE);
	for (int i = 0; i < bmp.size(); i++) {
		genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
		genome->moduleParameters[i]->parent = bmp[i]->parent;
		genome->moduleParameters[i]->type = bmp[i]->type;
		genome->moduleParameters[i]->parentSite = bmp[i]->parentSite;
		genome->moduleParameters[i]->orientation = bmp[i]->orientation;
		genome->moduleParameters[i]->control = bmp[i]->control;
	}
}

bool ER_DirectEncoding::loadGenome(std::istream &genomeInput, int individualNumber)
{
	if (settings->morphologyType == settings->MODULAR_PHENOTYPE) {
		loadPhenotype(individualNumber);
		genome->amountModules = genome->moduleParameters.size();
		morphFitness = 0;
		setGenomeColors();
		return true; // TODO CHANGE
	}
	if (settings->verbose) {
		cout << "loading genome " << individualNumber << "(ER_Direct)" << endl;
	}
	genome = std::shared_ptr<GENOTYPE>(new GENOTYPE);
//	lGenome->lParameters.clear();
//	cout << "lGenome cleared" << endl; 

	string value;
	list<string> values;
	while (genomeInput.good()) {
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
			//for (int i = 0; i < genome->amountModules; i++) {
			//	genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
			//}
		}
		if (tmp == "#Module:") {
			it++;
			tmp = *it;
			genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
			moduleNum = genome->moduleParameters.size() - 1;// atoi(tmp.c_str());
			checkingModule = true; 
	//		cout << "moduleNum set to " << moduleNum << endl; 
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
			//genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
			genome->moduleParameters[moduleNum]->type = atoi(tmp.c_str());
		}
		else if (tmp == "#ParentSite:")
		{
			it++;
			tmp = *it;
			genome->moduleParameters[moduleNum]->parentSite = atoi(tmp.c_str());
		}
		else if (tmp == "#Orientation:") 
		{
			it++;
			tmp = *it;
			genome->moduleParameters[moduleNum]->orientation = atoi(tmp.c_str());
		}

		if (tmp == "#ControlParams:") {
			checkingControl = true; 
		}
		else if (tmp == "#EndOfModule") {
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

	//	lGenome->lParameters.resize(lGenome->amountStates);
	//	cout << "amount of states = " << lGenome->amountStates << endl; 
	genome->amountModules = genome->moduleParameters.size();
	morphFitness = 0;
	setGenomeColors();

	if (settings->verbose) {
		cout << "loaded direct genome" << endl;
	}
	return true;
}

void ER_DirectEncoding::setGenomeColors() {
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
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
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
}

void ER_DirectEncoding::init() {
	genome = shared_ptr<GENOTYPE>(new GENOTYPE);
	maxModuleTypes = settings->maxModuleTypes;
	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->morphologyType != settings->QUADRUPED_DIRECT) {
		initializeGenome(0);
	}
	else {
//		initializeLRobot(0);
	}
}


int ER_DirectEncoding::initializeGenome(int type) {
	//first read settings
	float red[3] = { 1.0, 0, 0 };
	float blue[3] = { 0.0, 0.0, 1.0 };
	float yellow[3] = { 1.0, 1.0, 0.0 };
	float green[3] = { 0.0, 0.3, 0.0 };
	float orange[3] = { 1.0, 0.5, 0.0 };
	float orangePlus[3] = { 1.0, 0.6, 0.2 };
	float black[3] = { 0.0, 0.0, 0 };
	float pink[3] = { 1.0, 0.5, 0.5 };
	float white[3] = { 1.0, 1.0, 1.0 };

	if (settings->verbose) {
		cout << "initializing Direct Encoding Genome" << endl;
	}
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

		genome->moduleParameters[i]->parent = i - 1;
		genome->moduleParameters[i]->parentSite = 0;
		genome->moduleParameters[i]->orientation = 0;
	}
	cf.reset();
	mutateERGenome(0.5);
	mutateERGenome(0.5);
	return 1;
}

shared_ptr<Morphology> ER_DirectEncoding::clone() const {
	BaseMorphology::clone();
	shared_ptr<ER_DirectEncoding> ur = make_unique<ER_DirectEncoding>(*this);
	ur->genome = this->genome->clone();
	return ur;
}

void ER_DirectEncoding::update() {
	cout << "cannot update Genome:: no phenotype" << endl;
}

void ER_DirectEncoding::symmetryMutation(float mutationRate) {
	if (settings->verbose) {
		cout << "This version does not support symmetry mutation, check code" << endl;
	}
}

bool ER_DirectEncoding::checkIfLocationIsOccupied(vector<shared_ptr<MODULEPARAMETERS>> mps, int parentSite, int parent) {
	for (int i = 0; i < mps.size(); i++) {
		if (mps[i]->parent == parent && mps[i]->parentSite == parentSite) {
			return true;
		}
	}
	return false;
}

int ER_DirectEncoding::mutateERGenome(float mutationRate) {
	if (settings->verbose) {
		cout << "mutating direct" << endl;
	}
	// 1) add module, 
	// 2) mutate morphology
	// 3) mutate control
	// 4) prune 
	// 5) symmetry

//	int typeOfMutation = randomNum->randInt(5, 0);
//	typeOfMutation = 0;
//	switch (typeOfMutation) {
//	case 0: {
	// modularParameters.size() influences the chance of adding modules. 
	int maxModuleAddition = 5; // up to five modules can be added each mutation step...
	for (int am = 0; am < maxModuleAddition; am++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			if (settings->verbose) {
				cout << "adding module" << endl;
			}		
			// add module
			// select where module should be attached to
			if (genome->moduleParameters.size() < 50) { // absolute max amount modules
				int attachModule = randomNum->randInt(genome->moduleParameters.size(), 0);
				int treeDepth = checkTreeDepth(attachModule, 0);
				// cout << "Tree depth = " << treeDepth << endl;
				if (treeDepth > settings->lIncrements -1 ) {
					if (settings->verbose) {
						cout << "Tree too deep" << endl;
					}
				}
				else {
					int attachType = genome->moduleParameters[attachModule]->type;
					int maxCh = getMaxChilds(genome->moduleParameters[attachModule]->type); // placeholder for max childs of parent module
					if (genome->moduleParameters[attachModule]->childSiteStates.size() < maxCh) {
						int newModuleType = settings->moduleTypes[randomNum->randInt(settings->moduleTypes.size(), 0)];

						//bool collideSite = true;
						int newModuleLocation = 0;
						newModuleLocation = randomNum->randInt(getMaxChilds(genome->moduleParameters[attachModule]->type), 0);
						if (checkIfLocationIsOccupied(genome->moduleParameters, newModuleLocation, attachModule)) {
							if (settings->verbose) {
								cout << "Module location is occupied, not creating new module. " << endl;
							}
						}
						else {

							// Check module to be created
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
								int newOr = randomNum->randInt(4, 0);
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
								/*if (genome->moduleParameters[theNewModule]->type == 1) {
									genome->moduleParameters[theNewModule]->maxChilds = 5;
								}
								else if (genome->moduleParameters[theNewModule]->type == 4) {
									genome->moduleParameters[theNewModule]->maxChilds = 3;
								}
								else {
									genome->moduleParameters[theNewModule]->maxChilds = 0;
								}*/
								genome->moduleParameters[theNewModule]->parent = attachModule;
								genome->moduleParameters[theNewModule]->parentSite = newModuleLocation;
								genome->moduleParameters[theNewModule]->orientation = newOr;
								genome->amountModules = genome->moduleParameters.size();
							}
						}
					}
				}
			}
		}
	}
	
	// mutate orientation stored in childConfigurations
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) <settings->morphMutRate) {
			genome->moduleParameters[i]->orientation = randomNum->randInt(4, 0);
		}
	}
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
			if (genome->moduleParameters[i]->control) { // to check if not nullpointer
				genome->moduleParameters[i]->control->mutate(settings->mutationRate);
			}
	}
	if (randomNum->randFloat(0.0, 1.0) < mutationRate / 2) { // less chance to delete
		if (settings->verbose) {
			cout << "pruning" << endl;
		}
		if (genome->moduleParameters.size() > 1) {
			int randomModule = randomNum->randInt(genome->moduleParameters.size() - 1, 1);
			// get children to be deleted
			vector<int> deleteModules;
			deleteModules.push_back(randomModule);
//			genome->moduleParameters.erase(genome->moduleParameters.begin() + randomModule);
			if (settings->verbose) {
				cout << "deleting: " << randomModule << endl;
			}
			for (int j = 0; j < deleteModules.size(); j++) {
				if (settings->verbose) {
					cout << "dm[j]: " << deleteModules[j] << endl;
				}
				for (int i = 0; i < genome->moduleParameters.size(); i++) {
					if (genome->moduleParameters[i]->parent == deleteModules[j])
					{
						deleteModules.push_back(i);
						if (settings->verbose) {
							cout << "deleting: " << i << ", " << genome->moduleParameters[i]->parent << ", dm: " << deleteModules[j] << endl;
						}	
						//genome->moduleParameters.erase(genome->moduleParameters.begin() + i);
					}
				}
			}
			if (settings->verbose) {
				cout << "actual deletion" << endl;
			}			
			int dCounter = 0;
			std::sort(deleteModules.begin(), deleteModules.end());
			for (int i = deleteModules.size() -1; i >= 0; i--) {
				if (settings->verbose) {
					cout << "i: " << i << endl;
					//				cout << "deleting " << deleteModules[i] - dCounter << endl;
					cout << "module params size = " << genome->moduleParameters.size() << endl;
					//				cout << "dcounter = " << dCounter; cout << ", deleteModules[i] = " << deleteModules[i];
				}
				genome->moduleParameters.erase(genome->moduleParameters.begin() + (deleteModules[i]));
				if (settings->verbose) {
					cout << "removed one from module parameters" << endl;
				}
				genome->amountModules -= 1;
				for (int j = 0; j < genome->moduleParameters.size(); j++) {
					if (settings->verbose) {
						cout << "j: " << j << endl;
					}
					if (genome->moduleParameters[j]->parent >= deleteModules[i] - dCounter) {
						genome->moduleParameters[j]->parent -= 1;
					}
				}
				if (settings->verbose) {
					cout << "deleted" << endl;
				}
				//			dCounter++;
			}
			
//			genome->moduleParameters[randomModule];
		//	deleteModuleFromGenome(randomModule); // deletes module branch (includes all child modules)
		}
	}
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		symmetryMutation(settings->morphMutRate);
	}

	if (settings->verbose) {
		cout << "mutated direct" << endl;
	} 
	return 1;
}

int ER_DirectEncoding::checkTreeDepth(int attachModule, int increment) {
	if (genome->moduleParameters[attachModule]->parent > -1) {
		increment++;
		return checkTreeDepth(genome->moduleParameters[attachModule]->parent, increment);
	}
	else {
		return increment;
	}
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
	// deletes module and submodules
	while (genome->moduleParameters[num]->childSiteStates.size() > 0) {
		deleteModuleFromGenome(genome->moduleParameters[num]->childSiteStates.size() - 1);
		genome->moduleParameters[num]->childSiteStates.pop_back();
	}
}

void ER_DirectEncoding::mutate() {
	//cout << "mutating lmorph" << endl;
	mutateERGenome(settings->morphMutRate);
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
