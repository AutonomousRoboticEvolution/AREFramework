#include "ER_LSystem.h"
#include <iostream>
#include <sstream>


ER_LSystem::ER_LSystem()
{
	modular = true;
}


ER_LSystem::~ER_LSystem()
{
	
}


void ER_LSystem::crossover(shared_ptr<Morphology> partnerMorph, float cr) {
//	cout << "crossing over" << endl;
	shared_ptr<ER_LSystem>morpho(static_pointer_cast<ER_LSystem>(partnerMorph));   //*partnerMorph(ER_LSystem()));
	shared_ptr<LGENOME> tempLGenome;
//	tempLGenome = morpho->lGenome;
	tempLGenome = morpho->lGenome->clone(); 

	bool cross = false;
	for (int i = 0; i < lGenome->amountStates; i++) {
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
			lGenome->lParameters[i] = tempLGenome->lParameters[i]->clone();// unique_ptr<LPARAMETERS>(new tempLGenome->lParameters[i]);
		}
	}
}

void ER_LSystem::printSome() {
	BaseMorphology::printSome();
	cout << "printing some from ER_LSystem" << endl;
}


void ER_LSystem::initializeGenomeCustom(int type)
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

	for (int i = 0; i < lGenome->amountStates; i++) {
		lGenome->lParameters.push_back(shared_ptr<LPARAMETERS>(new LPARAMETERS));
		lGenome->lParameters[i]->currentState = i;
		lGenome->lParameters[i]->type = settings->moduleTypes[i];
		lGenome->lParameters[i]->amountChilds = 0; 
		vector<float>tempVector;

		lGenome->lParameters[i]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
		lGenome->lParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
		//	cout << "Initializing state modules" << endl; 
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = green[j];
			}
		case 5:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = orange[j];
			}
		default:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = white[j];
			}
			break;

		}

		if (i == 0) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
	//		lGenome->lParameters[i]->childConfigurations.push_back(1);
	//		lGenome->lParameters[i]->childSites.push_back(2);
	//		lGenome->lParameters[i]->childSiteStates.push_back(1);
	//		lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
	//		lGenome->lParameters[i]->childConfigurations.push_back(2);
	//		lGenome->lParameters[i]->childSites.push_back(3);
	//		lGenome->lParameters[i]->childSiteStates.push_back(1);
	//		lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//			lGenome->lParameters[i]->childConfigurations.push_back(1);
			//			lGenome->lParameters[i]->childSites.push_back(2);
			//			lGenome->lParameters[i]->childSiteStates.push_back(1);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			/*	lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(2);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(3);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(4);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();*/
		}
		else if (i == 1) {
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(3);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//			lGenome->lParameters[i]->module = lGenome->moduleFactory->createModuleGenome(4, randomNum, settings);
			//			lGenome->lParameters[i]->childConfigurations.push_back(1);
			//			lGenome->lParameters[i]->childSites.push_back(1);
			//			lGenome->lParameters[i]->childSiteStates.push_back(1);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//			lGenome->lParameters[i]->module = lGenome->moduleFactory->createModuleGenome(4, randomNum, settings);
			//			lGenome->lParameters[i]->childConfigurations.push_back(1);
			//			lGenome->lParameters[i]->childSites.push_back(2);
			//			lGenome->lParameters[i]->childSiteStates.push_back(1);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 2) {
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(3);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 3) {
		}
		else if (i == 4) {
		}
		else {
		}
	}
	cf.reset();
}

void ER_LSystem::initializeQuadruped(int type)
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

	cout << "initializing Quadruped L-System Genome" << endl;

	morphFitness = 0;
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);

	for (int i = 0; i < lGenome->amountStates; i++) {
		lGenome->lParameters.push_back(shared_ptr<LPARAMETERS>(new LPARAMETERS));
		lGenome->lParameters[i]->currentState = i;
		if (i == 0) {
			lGenome->lParameters[i]->type = 1;
		}
		else if (i == 4) {
			lGenome->lParameters[i]->type = 12;
		}
		else {
			lGenome->lParameters[i]->type = 4;
		}
		lGenome->lParameters[i]->amountChilds = 0;
		vector<float>tempVector;

		lGenome->lParameters[i]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
		lGenome->lParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
		//	cout << "Initializing state modules" << endl; 
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = green[j];
			}
		case 5:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = orange[j];
			}
		default:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = white[j];
			}
			break;
		}

		if (i == 0) {
			// head
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(4);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			
			// servos
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(4);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(1);
			lGenome->lParameters[i]->childSiteStates.push_back(2);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(2);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(3);
			lGenome->lParameters[i]->childSiteStates.push_back(2);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			
			//		lGenome->lParameters[i]->childConfigurations.push_back(1);
			//		lGenome->lParameters[i]->childSites.push_back(2);
			//		lGenome->lParameters[i]->childSiteStates.push_back(1);
			//		lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//		lGenome->lParameters[i]->childConfigurations.push_back(2);
			//		lGenome->lParameters[i]->childSites.push_back(3);
			//		lGenome->lParameters[i]->childSiteStates.push_back(1);
			//		lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//			lGenome->lParameters[i]->childConfigurations.push_back(1);
			//			lGenome->lParameters[i]->childSites.push_back(2);
			//			lGenome->lParameters[i]->childSiteStates.push_back(1);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			/*	lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(2);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(3);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(4);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();*/
		}
		else if (i == 1) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(3);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//			lGenome->lParameters[i]->module = lGenome->moduleFactory->createModuleGenome(4, randomNum, settings);
			//			lGenome->lParameters[i]->childConfigurations.push_back(1);
			//			lGenome->lParameters[i]->childSites.push_back(1);
			//			lGenome->lParameters[i]->childSiteStates.push_back(1);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			//			lGenome->lParameters[i]->module = lGenome->moduleFactory->createModuleGenome(4, randomNum, settings);
			//			lGenome->lParameters[i]->childConfigurations.push_back(1);
			//			lGenome->lParameters[i]->childSites.push_back(2);
			//			lGenome->lParameters[i]->childSiteStates.push_back(1);
			//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 2) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(3);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 3) {
		}
		else if (i == 4) {
		}
		else {
		}
	}
	cf.reset();
}

void ER_LSystem::initializeSolar(int type)
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

	cout << "initializing Quadruped L-System Genome" << endl;

	morphFitness = 0;
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);

	for (int i = 0; i < lGenome->amountStates; i++) {
		lGenome->lParameters.push_back(shared_ptr<LPARAMETERS>(new LPARAMETERS));
		lGenome->lParameters[i]->currentState = i;
		if (i == 0) {
			lGenome->lParameters[i]->type = 8;
		}
		else if (i == 4) {
			lGenome->lParameters[i]->type = 11;
		}
		else {
			lGenome->lParameters[i]->type = 4;
		}
		lGenome->lParameters[i]->amountChilds = 0;
		vector<float>tempVector;

		lGenome->lParameters[i]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
		lGenome->lParameters[i]->control->init(6, 1, 2);
		//	cout << "Initializing state modules" << endl; 
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = green[j];
			}
		case 5:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = orange[j];
			}
		default:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = white[j];
			}
			break;
		}

		if (i == 0) {
			// head
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(1);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 1) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(2);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 2) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(4);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}
		else if (i == 3) {
		}
		else if (i == 4) {
		}
		else {
		}
	}
	settings->morphMutRate = -0.1;
	cf.reset();
}


const std::string ER_LSystem::generateGenome(int indNum, float fitness) const
{
//	cout << "saving lGenome " << indNum << " in folder " << sceneNum << ", that had a fitness of " << fitness << endl << "-------------------------------- "<< endl;
//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	int amountCreatedModules = createdModules.size(); 
	int amountStates = lGenome->lParameters.size(); 

	ostringstream genomeText;
	genomeText << "#Individual:" << indNum << endl; 
	genomeText << "#Fitness:," << fitness << endl;
	genomeText << "#phenValue;," << phenValue << endl;
	genomeText << "#AmountStates:," << amountStates << "," << endl << endl;
//	cout << "#AmountStates:," << amountStates << "," << endl << endl;

	genomeText << "Module Parameters Start Here: ," << endl << endl;
	for (int i = 0; i < amountStates; i++) {
		genomeText << "#Module:," << i << endl;
		genomeText << "#ModuleType:," << lGenome->lParameters[i]->type << endl;
		genomeText << "#ModuleState:," << lGenome->lParameters[i]->currentState << endl; 
		genomeText << "#AmountChilds:," << lGenome->lParameters[i]->amountChilds << endl;
		genomeText << "#ChildSites:,";
		for (int j = 0; j < lGenome->lParameters[i]->childSites.size(); j++) {
			genomeText << lGenome->lParameters[i]->childSites[j] << ",";
		} genomeText << endl; 
		genomeText << "#ChildConfigurations:,";
		for (int j = 0; j < lGenome->lParameters[i]->childConfigurations.size(); j++) {
			genomeText << lGenome->lParameters[i]->childConfigurations[j] << ",";
		} genomeText << endl;
		genomeText << "#ChildSiteStates:,";
		for (int j = 0; j < lGenome->lParameters[i]->childSiteStates.size(); j++) {
			genomeText << lGenome->lParameters[i]->childSiteStates[j] << ",";
		} genomeText << endl;
//		genomeText << "," << lGenome->lParameters[i]->moduleType << ",";
		genomeText << "#ControlParams:," << endl;
		genomeText << lGenome->lParameters[i]->control->getControlParams().str(); 

		genomeText << "#EndOfModule," << endl << endl;
	}

	return genomeText.str();
}

float ER_LSystem::getFitness() {
	return fitness;
}

bool ER_LSystem::loadGenome(std::istream &genomeInput, int individualNumber)
{
	if (settings->verbose) {
		cout << "loading genome " << individualNumber << "(ER_LSystem)" << endl;
	}

//	cout << "loading genome " << individualNumber << endl;
	lGenome = std::shared_ptr<LGENOME>(new LGENOME);
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
			lGenome->amountStates = atoi(tmp.c_str());
			for (int i = 0; i < lGenome->amountStates; i++) {
				lGenome->lParameters.push_back(shared_ptr<LPARAMETERS>(new LPARAMETERS));
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
			lGenome->lParameters[moduleNum]->amountChilds = ac;
			lGenome->lParameters[moduleNum]->childSites.resize(ac);
			lGenome->lParameters[moduleNum]->childConfigurations.resize(ac);
			lGenome->lParameters[moduleNum]->childSiteStates.resize(ac);
		}
		else if (tmp == "#ChildSites:") {
			for (int i = 0; i < lGenome->lParameters[moduleNum]->amountChilds; i++) {
				it++;
				tmp = *it;
				lGenome->lParameters[moduleNum]->childSites[i] = atoi(tmp.c_str());
			}
		}
		else if (tmp == "#ChildConfigurations:") {
			for (int i = 0; i < lGenome->lParameters[moduleNum]->amountChilds; i++) {
				it++;
				tmp = *it;
				lGenome->lParameters[moduleNum]->childConfigurations[i] = atoi(tmp.c_str());
			}
		}
		else if (tmp == "#ChildSiteStates:") {
	//		cout << "loading childSiteStates" << endl;
			for (int i = 0; i < lGenome->lParameters[moduleNum]->amountChilds; i++) {
				it++;
				tmp = *it;
				lGenome->lParameters[moduleNum]->childSiteStates[i] = atoi(tmp.c_str());
			}
		}
		else if (tmp == "#ModuleType:") {
			it++;
			tmp = *it;
	//		cout << "creating module of type: " << atoi(tmp.c_str()) << endl; 
			lGenome->lParameters[moduleNum]->type = atoi(tmp.c_str());
			lGenome->lParameters[moduleNum]->currentState = moduleNum;
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
				lGenome->lParameters[moduleNum]->control = controlFactory->createNewControlGenome(atoi(controlValues[2].c_str()), randomNum, settings); // 0 is ANN
			//	lGenome->lParameters[moduleNum]->control->init(1, 2, 1);
				lGenome->lParameters[moduleNum]->control->setControlParams(controlValues);
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
	for (int i = 0; i < lGenome->amountStates; i++) {
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = red[j];
			}
			break;
		case 5:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = yellow[j];
			}
			break;
		default:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = grey[j];
			}
			break;
		}
		lGenome->lParameters[i]->moduleColor[0] = lGenome->lParameters[i]->color[0];
		lGenome->lParameters[i]->moduleColor[1] = lGenome->lParameters[i]->color[1];
		lGenome->lParameters[i]->moduleColor[2] = lGenome->lParameters[i]->color[2];
	}
	return true;
//	cout << "loaded L-System genome" << endl;
}



void ER_LSystem::init() {
	lGenome = shared_ptr<LGENOME>(new LGENOME);
	maxModuleTypes = settings->maxModuleTypes;
	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->morphologyType != settings->QUADRUPED_GENERATIVE 
		&& settings->morphologyType != settings->CUSTOM_SOLAR_GENERATIVE) {
//		initializeGenomeCustom(0);
		initializeLGenome(0);
	}
	if (settings->morphologyType == settings->QUADRUPED_GENERATIVE) {
		initializeQuadruped(0);
	}
	else if (settings->morphologyType == settings->CUSTOM_SOLAR_GENERATIVE) {
		initializeSolar(0);
	}
	else {
//		initializeLRobot(0);
	}
}



int ER_LSystem::initializeLGenome(int type) {
	//first read settings
	lGenome->amountIncrement = settings->lIncrements;
	lGenome->amountStates = settings->amountModules;
	lGenome->maxAmountStates = settings->maxAmountModules; // states *

	float red[3] = { 1.0, 0, 0 };
	float blue[3] = { 0.0, 0.0, 1.0 };
	float yellow[3] = { 1.0, 1.0, 0.0 };
	float green[3] = { 0.0, 0.3, 0.0 };
	float orange[3] = { 1.0, 0.5, 0.0 };
	float orangePlus[3] = { 1.0, 0.6, 0.2 };
	float black[3] = { 0.0, 0.0, 0 };
	float pink[3] = { 1.0, 0.5, 0.5 };
	float white[3] = { 1.0, 1.0, 1.0 };
	
//	cout << "initializing L-System Genome" << endl; 
	//	lGenome->lParameters.resize(lGenome->amountStates);
	//	cout << "amount of states = " << lGenome->amountStates << endl; 
	morphFitness = 0;
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);
	for (int i = 0; i < lGenome->amountStates; i++) {
		lGenome->lParameters.push_back(shared_ptr<LPARAMETERS>(new LPARAMETERS));
		lGenome->lParameters[i]->type = settings->moduleTypes[i];
		vector<float>tempVector;
	//	cout << "Initializing state modules" << endl; 
		// currently everything has a neural network...
		lGenome->lParameters[i]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
		if (settings->controlType == settings->ANN_DISTRIBUTED_BOTH) {
			lGenome->lParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
		}
		else if (settings->controlType == settings->ANN_DISTRIBUTED_UP) {
			lGenome->lParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
		}
		else if (settings->controlType == settings->ANN_DISTRIBUTED_DOWN) {
			lGenome->lParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // 2 additional inputs and outputs used fotr 
		}
		else {
			lGenome->lParameters[i]->control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons); // still two additional outputs
		}
	
		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = pink[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = green[j];
			}
		case 5:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = orange[j];
			}
		default:
			for (int j = 0; j < 3; j++) {
				lGenome->lParameters[i]->color[j] = white[j];
			}
			break;
		}

		lGenome->lParameters[i]->currentState = i;

	}
	mutateERLGenome(0.5);
	mutateControlERLGenome(0.5);
	cf.reset();

	return 1;
}

shared_ptr<Morphology> ER_LSystem::clone() const {
	BaseMorphology::clone();
	shared_ptr<ER_LSystem> ur = make_unique<ER_LSystem>(*this);
	ur->lGenome = this->lGenome->clone();
	//for (int i = 0; i < ur->lGenome->lParameters.size(); i++) {
	//	ur->lGenome->lParameters[i] = this->lGenome->lParameters[i]->clone();
	//}
	return ur;
}

void ER_LSystem::update() {	
	cout << "cannot update Genome:: no phenotype" << endl;
}

void ER_LSystem::symmetryMutation(float mutationRate) {
	cout << "This version does not support symmetry mutation, check code" << endl;
}

int ER_LSystem::getNewSite(int maxCh, int currentSite, vector<int> sites) {
	if (maxCh == sites.size()) {
		// all other sites are occupied, so the site cannot be changed. 
		return currentSite;
	}
	int newSite = -1;
	bool uniqueSite = false;
	while (uniqueSite == false) {
		// will never exit the loop until a unique site is found. 
		newSite = randomNum->randInt(maxCh, 0);
		uniqueSite = true; // if the site is not unique, it will be set by the next for loop.
		for (int i = 0; i < sites.size(); i++) {
			if (sites[i] == newSite) {
				// this site is already taken so try again
				uniqueSite = false;
				break;
			}
		}

	}
	return newSite;
}

int ER_LSystem::mutateERLGenome(float mutationRate) {
	if (settings->verbose) {
		std::cout << "Mutating L-Genome" << std::endl;
	}
	for (int i = 0; i < lGenome->lParameters.size(); i++) {
		// mutate the controller. 
		if (!lGenome->lParameters[i]->control) {
			std::cerr << "control = null? Check ER_LSystem.cpp" << std::endl;
		}
		lGenome->lParameters[i]->control->mutate(settings->mutationRate);

		// Resize the rules (Potentially very destructive) 
		int childSize = lGenome->lParameters[i]->childSites.size();
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) { // randomly change amount childs, very destructive mutation
			int maxCh = getMaxChilds(lGenome->lParameters[i]->type);
			if (maxCh != 0) {
				int newChildSize = randomNum->randInt(maxCh, 0);
				if (settings->verbose) {
					std::cout << "newChildSize = " << newChildSize << std::endl;
					std::cout << "site vector size = " << lGenome->lParameters[i]->childSites.size() << std::endl;
				}
				while (lGenome->lParameters[i]->childSites.size() < newChildSize) {
					if (settings->verbose) {
						std::cout << "while" << std::endl;
					}
					// TODO make another object containing child site, state and orientation. Struct?
					vector<int> sitebuffer = lGenome->lParameters[i]->childSites;
					lGenome->lParameters[i]->childSites.push_back(getNewSite(maxCh, -1, sitebuffer));
					lGenome->lParameters[i]->childSiteStates.push_back(randomNum->randInt(lGenome->amountStates, 0));
					lGenome->lParameters[i]->childConfigurations.push_back(randomNum->randInt(4,0));
					// old code, not working
					//lGenome->lParameters[i]->childSites.resize(newChildSize);
					//lGenome->lParameters[i]->childSiteStates.resize(newChildSize);
					//lGenome->lParameters[i]->childConfigurations.resize(newChildSize);
					//for (int j = childSize; j < newChildSize; j++) {
						//		cout << "j = " << j << endl; 
					//	lGenome->lParameters[i]->childSites[j] = getNewSite(maxCh, -1, lGenome->lParameters[i]->childSites); // -1 because there is no site yet.
					//	lGenome->lParameters[i]->childSiteStates[j] = randomNum->randInt(lGenome->amountStates, 0); // random state
					//	lGenome->lParameters[i]->childConfigurations[j] = randomNum->randInt(4, 0); // 4 sites
					//}
				}
				if (settings->verbose) {
					std::cout << "size = " << lGenome->lParameters[i]->childSites.size() << std::endl;
				}
				else if (lGenome->lParameters[i]->childSites.size() > newChildSize) {
					if (settings->verbose) {
						std::cout << "erasing" << std::endl;
					}
					while (lGenome->lParameters[i]->childSites.size() > newChildSize) { // now removes random site
						int rSite = randomNum->randInt(lGenome->lParameters[i]->childSites.size(), 0);
						lGenome->lParameters[i]->childSites.erase(lGenome->lParameters[i]->childSites.begin() + rSite);
						lGenome->lParameters[i]->childSiteStates.erase(lGenome->lParameters[i]->childSiteStates.begin() + rSite);
						lGenome->lParameters[i]->childConfigurations.erase(lGenome->lParameters[i]->childConfigurations.begin() + rSite);
					}
				}
			}
			if (settings->verbose) {
				std::cout << "size = " << lGenome->lParameters[i]->childSites.size() << std::endl;
			}
			// mutate the sites
			for (int j = 0; j < lGenome->lParameters[i]->childSites.size(); j++) {
				if (settings->verbose) {
					std::cout << "j = " << j << std::endl;
				}
				if (randomNum->randFloat(0.0, 1.0) < mutationRate) { // change child configuration and state at random
					if (maxCh != 0) {
						if (settings->verbose) {
							std::cout << "randomizing" << std::endl;
						}
						lGenome->lParameters[i]->childSites[j] = getNewSite(maxCh, lGenome->lParameters[i]->childSites[j], lGenome->lParameters[i]->childSites);
						lGenome->lParameters[i]->childSiteStates[j] = randomNum->randInt(lGenome->amountStates,0);
						lGenome->lParameters[i]->childConfigurations[j] = randomNum->randInt(4, 0);
					}
				}
				if (randomNum->randFloat(0.0, 1.0) < mutationRate) { // swap sites
					if (maxCh >= 2) {
						int swapCandidate = randomNum->randInt(lGenome->lParameters[i]->childSites.size(), 0);
						if (j != swapCandidate) { 
							if (settings->verbose) {
								std::cout << "swapping" << std::endl;
							}
							// Guys, I know, there is a way more efficient way to do this. Trust me, I know, it will be done soon. s
							int siteSwapCandidateBuffer = lGenome->lParameters[i]->childSites[swapCandidate];
							int stateSwapCandidateBuffer = lGenome->lParameters[i]->childSiteStates[swapCandidate];
							int configurationSwapCandidateBuffer = lGenome->lParameters[i]->childConfigurations[swapCandidate];
							if (settings->verbose) {
								std::cout << "changing candidate" << std::endl;
							}
							// change swapCandidate
							lGenome->lParameters[i]->childSites[swapCandidate] = lGenome->lParameters[i]->childSites[j];
							lGenome->lParameters[i]->childSiteStates[swapCandidate] = lGenome->lParameters[i]->childSiteStates[j];
							lGenome->lParameters[i]->childConfigurations[swapCandidate] = lGenome->lParameters[i]->childConfigurations[j];
							if (settings->verbose) {
								std::cout << "changing j" << std::endl;
							}
							// change j for swapCandidate using buffer. 
							lGenome->lParameters[i]->childSites[j] = siteSwapCandidateBuffer;
							lGenome->lParameters[i]->childSiteStates[j] = stateSwapCandidateBuffer;
							lGenome->lParameters[i]->childConfigurations[j] = configurationSwapCandidateBuffer;
						}
					}
				}
			}
		}
		lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		if (settings->verbose) {
			std::cout << "Done mutating L-Genome parameters of " << i << std::endl;
		}

	//	cout << "size = " << lGenome->lParameters[i]->childSites.size() << endl;

	}
	if (settings->verbose) {
		std::cout << "Mutated L-Genome "<< std::endl;
	}
//	symmetryMutation(settings->morphMutRate);
	return 1;
}

int ER_LSystem::mutateControlERLGenome(float mutationRate) {
	cout << "mutating l-genome" << endl;
	for (int i = 0; i < lGenome->lParameters.size(); i++) {
		//		cout << "i = " << i << endl; 
		lGenome->lParameters[i]->control->mutate(mutationRate);
	}
	return 1;
}

void ER_LSystem::mutate() {
	//cout << "mutating lmorph" << endl;
	mutateERLGenome(settings->morphMutRate);
}

void ER_LSystem::checkGenome(int individualNumber, int sceneNum) {
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

void ER_LSystem::checkControl(int individual, int sceneNum) {
	checkGenome(individual, sceneNum);
}
