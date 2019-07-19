#pragma once
#include "Settings.h"

/*!
 * The settings that are set in the constructor are mainly for debugging purposes. 
 * 
 */
Settings::Settings() {
	// set default module parameters
	// 1: Cube
	// 4: Servo Module
	// 13: Brain (not very useful now)
	// 14: wheel
	// 15: sensor 
	// 16: servo
	// 17: bone
	// 18: example
	moduleTypes.push_back(13);
	moduleTypes.push_back(14);
	moduleTypes.push_back(14);
	moduleTypes.push_back(17);
	moduleTypes.push_back(17);
	moduleTypes.push_back(17);
	autoDeleteSettings = true;

	for (int i = 0; i < moduleTypes.size(); i++) {
		vector <int> tmpMaxModuleTypes;
		tmpMaxModuleTypes.push_back(moduleTypes[i]);
		if (i == 0) {
            tmpMaxModuleTypes.push_back(0);
        }
		else {
            tmpMaxModuleTypes.push_back(50);
        }
		maxModuleTypes.push_back(tmpMaxModuleTypes);
	}
	//maxModuleTypes[0][1] = 100; // one base module
	maxNumberModules = 20;
	//morphologyType = MODULAR_LSYSTEM; // MODULAR_DIRECT;
	morphologyType = CAT_MORPHOLOGY;
	environmentType = DEFAULT_ENV;
	controlType = ANN_CUSTOM;
	populationSize = 20;
	energyDissipationRate = 0.0;
	lIncrements = 4; // not used, should be somewhere else?
//	controlType = ANN_DEFAULT;
	verbose = true;
	//verbose = true;
	initialInputNeurons = 1;
	initialInterNeurons = 1;
	initialOutputNeurons = 1;
	evolutionType = STEADY_STATE;
	seed = 0;
//	instanceType = INSTANCE_REGULAR;
	morphMutRate = 0.1;
	mutationRate = 0.1;
	maxGeneration = 600;
	initialAmountConnectionsNeurons = 1;
	maxAddedNeurons = 2;
	xGenerations = 50;
	savePhenotype = false;
	sendGenomeAsSignal = true;
	shouldReopenConnections = true;
	killWhenNotConnected = true;
	colorization = COLOR_NEURALNETWORK;
	createPatternNeurons = false;
	maxTorque_ForceSensor = 1000000;
	maxForce_ForceSensor = 1000000;
	consecutiveThresholdViolations = 1000;
	//repository="files";
}

Settings::~Settings() {

}

void Settings::split_line(string& line, string delim, list<string>& values)
{
	size_t pos = 0;
	while ((pos = line.find(delim, (pos + 0))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}
	while ((pos = line.find(delim, (pos + 1))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}


void Settings::readSettings() {
	bool fileExists = false;
	if (autoDeleteSettings == true) {
		// check if settings file exists and delete if present
		ofstream settingsFile;
		settingsFile.open(repository + "/settings" + to_string(sceneNum) + ".csv");
		fileExists = settingsFile.good();
		settingsFile.close();
		std::remove((repository + "/settings" + to_string(sceneNum) + ".csv").c_str());
		return;
	}
	std::cout << "sceneNum = " << sceneNum << std::endl;
	ifstream file(repository + "/settings" + to_string(sceneNum) + ".csv");
	string morphType;
	int popCounter = 0;

	std::cout << "Looking up settings in: " << repository << "/settings" << to_string(sceneNum) << ".csv" << std::endl;
	if (file.good()) {
		std::cout << "settings found" << std::endl;
		int popCounter = 0;
		string value;
		list<string> values;

		// read the settings file and store the comma seperated values
		while (file.good()) {
			getline(file, value, ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			if (value.find('\n') != string::npos) {
				split_line(value, "\n", values);
			}
			else {
				values.push_back(value);
			}
		}
		file.close();

		list<string>::const_iterator it = values.begin();
		for (it = values.begin(); it != values.end(); it++) {
			string tmp = *it;
	//		cout << tmp << endl;
			if (tmp == "#serverMode") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				if (tmpInt == 1) {
					cout << "============INSTANCE MODE============" << endl;
					cout << "-------------SERVER MODE-------------" << endl;
					cout << "=====================================" << endl;
					instanceType = INSTANCE_SERVER;
				}
				else {
					instanceType = INSTANCE_REGULAR; 
				}
			}
			if (tmp == "#evolutionType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case RANDOM_SEARCH:
					evolutionType = RANDOM_SEARCH;
					break;
				case STEADY_STATE:
					evolutionType = STEADY_STATE;
					break;
				case GENERATIONAL:
					evolutionType = GENERATIONAL;
					break;
				case EMBODIED_EVOLUTION:
					evolutionType = EMBODIED_EVOLUTION;
					break;
				}
			}
			else if (tmp == "#fitnessType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case MOVE:
					fitnessType = MOVE;
					cout << "============FITNESS TYPE=============" << endl;
					cout << "----------MOVE FITNESS TYPE----------" << endl;
					cout << "=====================================" << endl;
					break;
				case SOLAR:
					fitnessType = SOLAR;
					cout << "============FITNESS TYPE=============" << endl;
					cout << "---------SOLAR FITNESS TYPE----------" << endl;
					cout << "=====================================" << endl;

					break;
				case SWIM:
					cout << "============FITNESS TYPE=============" << endl;
					cout << "-------SWIMMING FITNESS TYPE---------" << endl;
					cout << "=====================================" << endl;
					fitnessType = SWIM;
					break;
				case FITNESS_JUMP:
					fitnessType = FITNESS_JUMP;
					cout << "============FITNESS TYPE=============" << endl;
					cout << "----------JUMP FITNESS TYPE----------" << endl;
					cout << "=====================================" << endl;
					break;
				case MOVE_AND_SOLAR:
					fitnessType = MOVE_AND_SOLAR;
					cout << "============FITNESS TYPE=============" << endl;
					cout << "-----MOVE AND SOLAR FITNESS TYPE-----" << endl;
					cout << "=====================================" << endl;
					break;
				}

			}
			else if (tmp == "#environmentType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case DEFAULT_ENV:
					environmentType = DEFAULT_ENV;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "--INITIALIZING DEFAULT ENVIRONMENT---" << endl;
					cout << "=====================================" << endl;
					break;
				case SUN_BASIC:
					environmentType = SUN_BASIC;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "--INITIALIZING SUN BASIC ENVIRONMENT-" << endl;
					cout << "=====================================" << endl;
					break;
				case SUN_CONSTRAINED:
					environmentType = SUN_CONSTRAINED;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "INITIALIZING SUN CONSTRAINED ENVIRONMENT" << endl;
					cout << "=====================================" << endl;
					break;
				case SUN_BLOCKED:
					environmentType = SUN_BLOCKED;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "------BLOCKED SUN ENVIRONMENT--------" << endl;
					cout << "=====================================" << endl;
					break;
				case SUN_CONSTRAINED_AND_MOVING:
					environmentType = SUN_CONSTRAINED_AND_MOVING;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "INITIALIZING SUN CONSTRAINED AND MOVING ENVIRONMENT" << endl;
					cout << "=====================================" << endl;
					break;
				case WATER_ENV:
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "----INITIALIZING WATER ENVIRONMENT---" << endl;
					cout << "=====================================" << endl;

					environmentType = WATER_ENV;
					break;
				case ROUGH:
					environmentType = ROUGH;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "----INITIALIZING ROUGH ENVIRONMENT---" << endl;
					cout << "=====================================" << endl;

					break;
				case SUN_MOVING:
					environmentType = SUN_MOVING;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "-INITIALIZING SUN MOVING ENVIRONMENT-" << endl;
					cout << "=====================================" << endl;

					break;
				case CONSTRAINED_MOVING_SUN:
					environmentType = CONSTRAINED_MOVING_SUN;
					cout << "===========ENVIRONMENT TYPE==========" << endl;
					cout << "--------CONSTRAINED MOVING SUN-------" << endl;
					cout << "=====================================" << endl;
					break;
				}
			}
			else if (tmp == "#controlType") {
				cout << "========INITIALIZING CONTROL=========" << endl;
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case ANN_DEFAULT:
					cout << "----------INITIALIZING ANN-----------" << endl;
					controlType = ANN_DEFAULT;
					break;
				case ANN_CUSTOM:
					cout << "-------INITIALIZING CUSTOM ANN-------" << endl;
					controlType = ANN_CUSTOM;
					break;
				case ANN_CPPN:
					cout << "----------INITIALIZING CPPN----------" << endl;
					controlType = ANN_CPPN;
					break;
				case ANN_DISTRIBUTED_UP:
					cout << "----INITIALIZING DISTRIBUTED ANN----" << endl;
					controlType = ANN_DISTRIBUTED_UP;
					break;
				case ANN_DISTRIBUTED_DOWN:
					cout << "----INITIALIZING DISTRIBUTED ANN----" << endl;
					controlType = ANN_DISTRIBUTED_DOWN;
					break;
				case ANN_DISTRIBUTED_BOTH:
					cout << "----INITIALIZING DISTRIBUTED ANN----" << endl;
					controlType = ANN_DISTRIBUTED_BOTH;
					break;
				}
				cout << "=====================================" << endl;
			}
			else if (tmp == "#morphologyType") {
				cout << "=======INITIALIZING MORPHOLOGY=======" << endl;
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case CAT_MORPHOLOGY:
					cout << "----------INITIALIZING CAT----------" << endl;
					morphologyType = CAT_MORPHOLOGY;
					break;
				case MODULAR_LSYSTEM:
					cout << "----------MODULAR LSYSTEM----------" << endl;
					morphologyType = MODULAR_LSYSTEM;
					break;
				case CUSTOM_MORPHOLOGY:
					cout << "--------CUSTOM MORPHOLOGY----------" << endl;
					morphologyType = CUSTOM_MORPHOLOGY;
					break;
				case MODULAR_CPPN:
					cout << "-----------MODULAR CPPN------------" << endl;
					morphologyType = MODULAR_CPPN;
					break;
				case MODULAR_DIRECT:
					cout << "----------MODULAR DIRECT-----------" << endl;
					morphologyType = MODULAR_DIRECT;
					break;
				case CUSTOM_MODULAR_MORPHOLOGY:
					cout << "-----CUSTOM MODULAR MORPHOLOGY-----" << endl;
					morphologyType = CUSTOM_MODULAR_MORPHOLOGY;
					break;
				case QUADRUPED_GENERATIVE:
					cout << "--QUADRUPED GENERATIVE MORPHOLOGY--" << endl;
					morphologyType = QUADRUPED_GENERATIVE;
					break;
				case QUADRUPED_DIRECT:
					cout << "----QUADRUPED DIRECT MORPHOLOGY----" << endl;
					morphologyType = QUADRUPED_DIRECT;
					break;
				case CUSTOM_SOLAR_GENERATIVE:
					cout << "----SOLAR GENERATIVE MORPHOLOGY----" << endl;
					morphologyType = CUSTOM_SOLAR_GENERATIVE;
					break;
				case TISSUE_DIRECT:
					cout << "----TISSUE_DIRECT----" << endl;
					morphologyType = TISSUE_DIRECT;
					break;
				case TISSUE_GMX:
					cout << "----TISSUE_GMX----" << endl;
					morphologyType = TISSUE_GMX;
					break;
				}
                cout << "=====================================" << endl;
            }

			//if (tmp == "#morphologyType") { // later
			//	it++;
			//	tmp = *it;
			//	int tmpInt = atoi(tmp.c_str());
			//	switch (tmpInt) { 
			//	case NO_MORPHOLOGY: 
			//		morphologyType = NO_MORPHOLOGY; // I don't know why you would want this though
			//		break;
			//	case MODULAR_LSYSTEM:
			//		morphologyType = MODULAR_LSYSTEM;
			//		break;
			//	case MODULAR_CPPN:
			//		morphologyType = MODULAR_CPPN;
			//		break;
			//	case MODULAR_DIRECT:
			//		morphologyType = MODULAR_DIRECT;
			//		break;
			//	case CAT_MORPHOLOGY:
			//		morphologyType = CAT_MORPHOLOGY;
			//		break;
			//	case CUSTOM_MORPHOLOGY:
			//		morphologyType = CUSTOM_MORPHOLOGY;
			//		break;
			//	default:
			//		morphologyType = CAT_MORPHOLOGY;
			//		break;
			//	}
			//}
			else if (tmp == "#selectionType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case RANDOM_SELECTION:
					selectionType = RANDOM_SELECTION;
					break;
				case PROPORTIONATE_SELECTION:
					selectionType = PROPORTIONATE_SELECTION;
					break;
				}
			}
			else if (tmp == "#replacementType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt) {
				case RANDOM_REPLACEMENT:
					replacementType = RANDOM_REPLACEMENT;
					break;
				case REPLACE_AGAINST_WORST:
					replacementType = REPLACE_AGAINST_WORST;
					break;
				case PARETOMORPH_REPLACEMENT:
					replacementType = PARETOMORPH_REPLACEMENT;
					break;
				}
			}
			else if (tmp == "#generation") {
				it++;
				tmp = *it;
				generation = atoi(tmp.c_str());
				cout << "generation found : " << generation << endl;
			}
	//		else if (tmp == "#repository") { // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//			it++;
	//			tmp = *it;
	//			repository = tmp;
	//		}

			else if (tmp == "#maxGeneration") {
				it++;
				tmp = *it;
				maxGeneration = atoi(tmp.c_str());
			}
			else if (tmp == "#generationInterval") {
				it++;
				tmp = *it;
				xGenerations = atoi(tmp.c_str());
			}

			else if (tmp == "#indCounter") {
				it++;
				tmp = *it;
				individualCounter = atoi(tmp.c_str());
			}

			else if (tmp == "#populationSize") {
				it++;
				tmp = *it;
				populationSize = atoi(tmp.c_str());
				popCounter = populationSize;
				cout << "populationSize found : " << populationSize << endl;
			}
			else if (tmp == "#individuals") {
				it++;
				tmp = *it;
				for (int i = 0; i < popCounter; i++) {
					indNumbers.push_back(atoi(tmp.c_str()));
					it++;
					tmp = *it;
				}
			}
			else if (tmp == "#indFits") {
				it++;
				tmp = *it;
				for (int i = 0; i < popCounter; i++) {
					indFits.push_back(atof(tmp.c_str()));
					it++;
					tmp = *it;
				}
			}
			else if (tmp == "#mutationRate") {
				it++;
				tmp = *it;
				mutationRate = atof(tmp.c_str());
			}
			else if (tmp == "#morphMutRate") {
				it++;
				tmp = *it;
				morphMutRate = atof(tmp.c_str());
			}
			else if (tmp == "#crossover") {
				it++;
				tmp = *it;
				crossover = atoi(tmp.c_str());
			}

			else if (tmp == "#crossoverRate") {
				it++;
				tmp = *it;
				crossoverRate = atof(tmp.c_str());
			}
			else if (tmp == "#agingInds") {
				it++;
				tmp = *it;
				ageInds = atoi(tmp.c_str());
			}
			else if (tmp == "#maxAge") {
				it++;
				tmp = *it;
				maxAge = atoi(tmp.c_str());
			}
			else if (tmp == "#minAge") {
				it++;
				tmp = *it;
				minAge = atoi(tmp.c_str());
			}
			else if (tmp == "#deathProbability") {
				it++;
				tmp = *it;
				deathProb = atof(tmp.c_str());
			}
			else if (tmp == "#lIncrements") {
				it++;
				tmp = *it;
				lIncrements = atoi(tmp.c_str());
			}
			else if (tmp == "#verbose") {
				it++;
				tmp = *it;
				if (atoi(tmp.c_str()) == 0) {
					verbose = false;
				}
				else {
					verbose = true;
				}
			}
			else if (tmp == "#sendGenomeAsSignal") {
				it++;
				tmp = *it;
				if (atoi(tmp.c_str()) == 0) {
					sendGenomeAsSignal = false;
				}
				else {
					sendGenomeAsSignal = true;
				}
			}
			else if (tmp == "#shouldReopenConnections") {
				it++;
				tmp = *it;
				if (atoi(tmp.c_str()) == 0) {
					shouldReopenConnections = false;
				}
				else {
					shouldReopenConnections = true;
				}
			}

			else if (tmp == "#bestIndividual") {
				cout << "found best individual" << endl;
				it++;
				tmp = *it;
				bestIndividual = atoi(tmp.c_str());
			}
			else if (tmp == "#initialSeed") {
				it++;
				tmp = *it;
				seed = atoi(tmp.c_str());
			}
			else if (tmp == "#energyDissipationRate") {
				it++;
				tmp = *it;
				energyDissipationRate = atof(tmp.c_str());
			}

			else if (tmp == "#numberOfModules") {
				it++;
				tmp = *it;
				numberOfModules = atoi(tmp.c_str());
			}
			else if (tmp == "#useVarModules") {
				it++;
				tmp = *it;
				useVarModules = atoi(tmp.c_str());
			}
			else if (tmp == "#maxNumberModules") {
				it++;
				tmp = *it;
				maxNumberModules = atoi(tmp.c_str());
			}
			else if (tmp == "#maxForceModules") {
				cout << "found max force modules" << endl;
				it++;
				tmp = *it;
				maxForce = atof(tmp.c_str());
			}
			else if (tmp == "#maxForceSensorModules") {
				cout << "found max force sensor" << endl;
				it++;
				tmp = *it;
				maxForceSensor = atof(tmp.c_str());
			}
			else if (tmp == "#moduleTypes") {
				moduleTypes.clear();
				for (int i = 0; i < numberOfModules; i++) {
					it++;
					tmp = *it;
					moduleTypes.push_back(atoi(tmp.c_str()));
				}
			}
			else if (tmp == "#maxAmountModulesTypes") {
				maxModuleTypes.clear();
				vector<int> tmpTypes;

				for (int i = 0; i < numberOfModules; i++) {
					it++;
					tmp = *it;
					vector<int> tmpMaxMods;
					tmpMaxMods.push_back(moduleTypes[i]);
					tmpMaxMods.push_back(atoi(tmp.c_str()));
					maxModuleTypes.push_back(tmpMaxMods);
					tmpMaxMods.clear();
				}
			}
			fileExists = true;
		}
	}
	cout << "Loaded Settings" << endl;
}

void Settings::saveSettings() {
	ofstream settingsFile;
	settingsFile.open(repository + "/settings" + to_string(sceneNum) + ".csv");
	settingsFile << ",#serverMode," << instanceType << "," << endl;
	settingsFile << ",#evolutionType," << evolutionType << "," << endl;
	settingsFile << ",#fitnessType," << fitnessType << "," << endl;
	settingsFile << ",#environmentType," << environmentType << "," << endl;
	settingsFile << ",#morphologyType," << morphologyType << "," << endl;
	settingsFile << ",#selectionType," << selectionType << "," << endl;
	settingsFile << ",#replacementType," << replacementType << "," << endl;
	settingsFile << ",#controlType," << controlType << "," << endl;
	settingsFile << ",#generation," << generation << "," << endl; // set when saving
	settingsFile << ",#maxGeneration," << maxGeneration << "," << endl;
	settingsFile << ",#generationInterval," << xGenerations << "," << endl;
	settingsFile << ",#indCounter," << individualCounter << "," << endl; // set when saving
	settingsFile << ",#populationSize," << populationSize << "," << endl;
	settingsFile << ",#mutationRate," << mutationRate << "," << endl;
	settingsFile << ",#morphMutRate," << morphMutRate << "," << endl;
	settingsFile << ",#crossover," << crossover << "," << endl;
	settingsFile << ",#crossoverRate," << crossoverRate << "," << endl;
	settingsFile << ",#energyDissipationRate," << energyDissipationRate << "," << endl;
	settingsFile << ",#agingInds," << ageInds << "," << endl;
	settingsFile << ",#maxAge," << maxAge << "," << endl;
	settingsFile << ",#minAge," << minAge << "," << endl;
	settingsFile << ",#deathProbability," << deathProb << "," << endl;
	settingsFile << ",#lIncrements," << lIncrements << "," << endl;
	settingsFile << ",#bestIndividual," << bestIndividual << endl; // set when saving
	settingsFile << ",#initialSeed," << seed << "," << endl;
	settingsFile << ",#verbose," << verbose << "," << endl;
	settingsFile << ",#numberOfModules," << numberOfModules << "," << endl; // not used
	settingsFile << ",#useVarModules," << useVarModules << "," << endl;
	settingsFile << ",#maxNumberModules," << maxNumberModules << "," << endl;
	settingsFile << ",#sendGenomeAsSignal," << sendGenomeAsSignal << "," << endl;
	settingsFile << ",#shouldReopenConnections," << shouldReopenConnections << "," << endl;

	settingsFile << ",#maxForceModules," << maxForce << "," << endl;
	settingsFile << ",#maxForceSensorModules," << maxForceSensor << "," << endl;
	settingsFile << ",#repository," << repository << "," << endl;

	settingsFile << ",#moduleTypes,";
	for (int i = 0; i < moduleTypes.size(); i++) {
		settingsFile << moduleTypes[i] << ",";
	} settingsFile << endl;

	settingsFile << ",#maxAmountModulesTypes,";
	for (int i = 0; i < maxModuleTypes.size(); i++) {
		settingsFile << maxModuleTypes[i][1] << ",";
	}	settingsFile << endl;

	settingsFile << ",#individuals,";
	for (int i = 0; i < populationSize; i++) {
		settingsFile << indNumbers[i] << ","; // must be set when saving
	} settingsFile << endl;
	if (indFits.size() > 0) {
		settingsFile << ",#indFits,";
		for (int i = 0; i < populationSize; i++) {
			settingsFile << indFits[i] << ","; // must be set when saving
		} 
	}
	settingsFile << endl;

	settingsFile.close();
}

void Settings::setRepository(std::string repository) {
    this->repository = repository;
    std::cout << "setting repository to " << repository << std::endl;
}
