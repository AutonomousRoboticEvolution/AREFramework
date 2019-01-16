#include "ER_LSystem.h"
#include <iostream>


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
			lGenome->lParameters[i]->maxChilds = 3;
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
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 2) {
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(3);
			lGenome->lParameters[i]->childSiteStates.push_back(1);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 3) {
			lGenome->lParameters[i]->maxChilds = 5;
		}
		else if (i == 4) {
			lGenome->lParameters[i]->maxChilds = 5;
		}
		else {
			lGenome->lParameters[i]->maxChilds = 5;
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
			lGenome->lParameters[i]->maxChilds = 5;
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
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 2) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(3);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 3) {
			lGenome->lParameters[i]->maxChilds = 5;
		}
		else if (i == 4) {
			lGenome->lParameters[i]->maxChilds = 5;
		}
		else {
			lGenome->lParameters[i]->maxChilds = 5;
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
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 1) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(2);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 2) {
			lGenome->lParameters[i]->childConfigurations.push_back(1);
			lGenome->lParameters[i]->childSites.push_back(0);
			lGenome->lParameters[i]->childSiteStates.push_back(4);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			lGenome->lParameters[i]->maxChilds = 3;
		}
		else if (i == 3) {
			lGenome->lParameters[i]->maxChilds = 5;
		}
		else if (i == 4) {
			lGenome->lParameters[i]->maxChilds = 5;
		}
		else {
			lGenome->lParameters[i]->maxChilds = 5;
		}
	}
	settings->morphMutRate = -0.1;
	cf.reset();
}


void ER_LSystem::saveGenome(int indNum, int sceneNum, float fitness) {
//	cout << "saving lGenome " << indNum << " in folder " << sceneNum << ", that had a fitness of " << fitness << endl << "-------------------------------- "<< endl;
//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	int amountCreatedModules = createdModules.size(); 
	int amountStates = lGenome->lParameters.size(); 

	ofstream genomeFile; 
	ostringstream genomeFileName; 
//	cout << "destination: " << settings->repository + "/morphologies" << sceneNum << "/genome" << indNum << ".csv";
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
		genomeFile << "#Module:," << i << endl;
		genomeFile << "#ModuleType:," << lGenome->lParameters[i]->type << endl;
		genomeFile << "#ModuleState:," << lGenome->lParameters[i]->currentState << endl; 
		genomeFile << "#AmountChilds:," << lGenome->lParameters[i]->amountChilds << endl;
		genomeFile << "#ChildSites:,";
		for (int j = 0; j < lGenome->lParameters[i]->childSites.size(); j++) {
			genomeFile << lGenome->lParameters[i]->childSites[j] << ",";
		} genomeFile << endl; 
		genomeFile << "#ChildConfigurations:,";
		for (int j = 0; j < lGenome->lParameters[i]->childConfigurations.size(); j++) {
			genomeFile << lGenome->lParameters[i]->childConfigurations[j] << ",";
		} genomeFile << endl;
		genomeFile << "#ChildSiteStates:,";
		for (int j = 0; j < lGenome->lParameters[i]->childSiteStates.size(); j++) {
			genomeFile << lGenome->lParameters[i]->childSiteStates[j] << ",";
		} genomeFile << endl;
//		genomeFile << "," << lGenome->lParameters[i]->moduleType << ",";
		genomeFile << "#ControlParams:," << endl;
		genomeFile << lGenome->lParameters[i]->control->getControlParams().str(); 

		genomeFile << "#EndOfModule," << endl << endl;
	}
	genomeFile.close();
}

float ER_LSystem::getFitness() {
	return fitness;
}

void ER_LSystem::loadGenome(int individualNumber, int sceneNum) {
//	cout << "loading genome " << individualNumber << endl;
	lGenome = shared_ptr<LGENOME>(new LGENOME);
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
			lGenome->lParameters[moduleNum]->maxChilds = 5;
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

		if (i == 0) {
//			lGenome->lParameters[i]->childConfigurations.push_back(0);
//			lGenome->lParameters[i]->childSites.push_back(0);
//			lGenome->lParameters[i]->childSiteStates.push_back(2);
//			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
			if (settings->moduleTypes[i] == 4 || settings->moduleTypes[i] == 9 || settings->moduleTypes[i] == 8) {
				lGenome->lParameters[i]->maxChilds = 3;
			}
			else if (settings->moduleTypes[i] == 6 || settings->moduleTypes[i] == 3) {
				lGenome->lParameters[i]->maxChilds = 0;
			}
			else {
				lGenome->lParameters[i]->maxChilds = 5;
			}
		}
		else {
			if (settings->moduleTypes[i] == 4 || settings->moduleTypes[i] == 9) {
				lGenome->lParameters[i]->maxChilds = 3;
			}
			else if (settings->moduleTypes[i] == 6 || settings->moduleTypes[i] == 3) {
				lGenome->lParameters[i]->maxChilds = 0;
			}
			else {
				lGenome->lParameters[i]->maxChilds = 5;
			}
		}
		lGenome->lParameters[i]->currentState = i;

	/*	if (i == 2) {
			lGenome->lParameters[i]->module->state = i;
			lGenome->lParameters[i]->childConfigurations.push_back(0);
			lGenome->lParameters[i]->childSites.push_back(1);
			lGenome->lParameters[i]->childSiteStates.push_back(0);
			lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();
		}*/
	}
	mutateERLGenome(0.5);
	mutateControlERLGenome(0.5);
	mutateERLGenome(0.5);
	// mutateERLGenome(0.5);
	cf.reset();

	return 1;
}

shared_ptr<Morphology> ER_LSystem::clone() const {
	BaseMorphology::clone();
	shared_ptr<ER_LSystem> ur = make_unique<ER_LSystem>(*this);
	for (int i = 0; i < ur->lGenome->lParameters.size(); i++) {
		ur->lGenome->lParameters[i] = ur->lGenome->lParameters[i]->clone();
	}
	ur->lGenome = ur->lGenome->clone();

	return ur;
}

void ER_LSystem::update() {	
	cout << "cannot update Genome:: no phenotype" << endl;
}

void ER_LSystem::symmetryMutation(float mutationRate) {
	cout << "This version does not support symmetry mutation, check code" << endl;
}

int ER_LSystem::mutateERLGenome(float mutationRate) {
//	cout << "mutating l-genome" << endl;
	for (int i = 0; i < lGenome->lParameters.size(); i++) {
	//	cout << "i = " << i << endl; 
		lGenome->lParameters[i]->control->mutate(settings->mutationRate);
		int childSize = lGenome->lParameters[i]->childSites.size();
	//	cout << "childSize = " << childSize << endl; 
		// change amount children
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) { 
			if (lGenome->lParameters[i]->maxChilds != 0) {
	//			cout << "maxChilds = " << lGenome->lParameters[i]->maxChilds << endl;
				int newChildSize = randomNum->randInt(lGenome->lParameters[i]->maxChilds, 0);
				//	cout << "newChildSize = " << newChildSize << endl ;
				if (childSize < newChildSize) {
					lGenome->lParameters[i]->childSites.resize(newChildSize);
					lGenome->lParameters[i]->childSiteStates.resize(newChildSize);
					lGenome->lParameters[i]->childConfigurations.resize(newChildSize);
					for (int j = childSize; j < newChildSize; j++) {
						//		cout << "j = " << j << endl; 
						lGenome->lParameters[i]->childSites[j] = randomNum->randInt(lGenome->lParameters[i]->maxChilds, 0);
						lGenome->lParameters[i]->childSiteStates[j] = randomNum->randInt(lGenome->amountStates, 0);
						lGenome->lParameters[i]->childConfigurations[j] = randomNum->randInt(4, 0);
					}
				}
				//	cout << "size = " << lGenome->lParameters[i]->childSites.size() << endl; 
				if (childSize > newChildSize) {
					lGenome->lParameters[i]->childSites.resize(newChildSize);
					lGenome->lParameters[i]->childSiteStates.resize(newChildSize);
					lGenome->lParameters[i]->childConfigurations.resize(newChildSize);
				}
			}

			//	cout << "size = " << lGenome->lParameters[i]->childSites.size() << endl;

			for (int j = 0; j < lGenome->lParameters[i]->childSites.size(); j++) {
				//		cout << "j = " << j << endl; 
				if (randomNum->randFloat(0.0, 1.0) < settings->morphMutRate) { // change child configuration and state
					if (lGenome->lParameters[i]->maxChilds != 0) {
						lGenome->lParameters[i]->childSites[j] = (randomNum->randInt(lGenome->lParameters[i]->maxChilds,0));
						lGenome->lParameters[i]->childSiteStates[j] = (randomNum->randInt(lGenome->amountStates,0));
						lGenome->lParameters[i]->childConfigurations[j] = (randomNum->randInt(4, 0));
					}
				}
			}
		}
		lGenome->lParameters[i]->amountChilds = lGenome->lParameters[i]->childSites.size();

	//	cout << "size = " << lGenome->lParameters[i]->childSites.size() << endl;

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
