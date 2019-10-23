#include "Settings.h"

/// The settings that are set in the constructor are mainly for debugging purposes.
Settings::Settings()
{
	// et default module parameters
	// 13: Brain (not very useful now)
	// 14: wheel
	// 15: sensor 
	// 16: servo
	// 17: bone
	// 18: example
	// This defines what components to use.
	moduleTypes.push_back(13);
	moduleTypes.push_back(14);
	moduleTypes.push_back(15);
	moduleTypes.push_back(16);
    moduleTypes.push_back(17);
    autoDeleteSettings = false;
    // Assign maximum number of components for each type
	for (size_t i = 0; i < moduleTypes.size(); i++) {
		std::vector <int> tmpMaxModuleTypes;
		tmpMaxModuleTypes.push_back(moduleTypes[i]);
		if (i == 0) {
            tmpMaxModuleTypes.push_back(0);
        }
		else {
            tmpMaxModuleTypes.push_back(50); // By default maximum number of components is 50
        }
		maxModuleTypes.push_back(tmpMaxModuleTypes);
	}

	maxNumberModules = 20;
	morphologyType = CUSTOM_MORPHOLOGY;
	environmentType = DEFAULT_ENV;
	populationSize = 100;
	energyDissipationRate = 0.0;
	lIncrements = 4; // not used, should be somewhere else?
//	controlType = ANN_DEFAULT;
	verbose = true;
	//verbose = true;
    initialInputNeurons = 1;
    initialInterNeurons = 1;
    initialOutputNeurons = 1;
	evolutionType = EA_MULTINEAT;
	seed = 0;
	morphMutRate = 0.1;
	mutationRate = 0.1;
	maxGeneration = 600;
    initialAmountConnectionsNeurons = 1;
    maxAddedNeurons = 2;
	xGenerations = 50;
	savePhenotype = true;
	sendGenomeAsSignal = true;
	shouldReopenConnections = true;
	killWhenNotConnected = true;
	colorization = COLOR_NEURALNETWORK;
	createPatternNeurons = false;
	maxTorque_ForceSensor = 1000000;
	maxForce_ForceSensor = 1000000;
	consecutiveThresholdViolations = 1000;
    bOrgansAbovePrintingBed = false;
    bCollidingOrgans = false;
    bNonprintableOrientations = false;
    bAnyOrgansNumber = false;
	//repository="files";
}

Settings::~Settings()
{}

void Settings::split_line(std::string& line, const std::string& delim, std::list<std::string>& values)
{
	size_t pos = 0;
	while ((pos = line.find(delim, (pos + 0))) != std::string::npos) {
        std::string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}
	while ((pos = line.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}

void Settings::readSettings()
{
	bool fileExists = false;
	if (autoDeleteSettings) {
		// check if settings file exists and delete if present
        std::ofstream settingsFile;
		settingsFile.open(repository + "/settings" + std::to_string(sceneNum) + ".csv");
		fileExists = settingsFile.good();
		settingsFile.close();
		std::remove((repository + "/settings" + std::to_string(sceneNum) + ".csv").c_str());
		return;
	}
	std::cout << "sceneNum = " << sceneNum << std::endl;
    std::ifstream file(repository + "/settings" + std::to_string(sceneNum) + ".csv");
    std::string morphType;
	int popCounter = 0;

	std::cout << "Looking up settings in: " << repository << "/settings" << std::to_string(sceneNum) << ".csv" << std::endl;
	if (file.good()) {
		std::cout << "settings found" << std::endl;
		int popCounter = 0;
        std::string value;
		std::list<std::string> values;

		// read the settings file and store the comma seperated values
		while (file.good()) {
			getline(file, value, ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
			if (value.find('\n') != std::string::npos) {
				split_line(value, "\n", values);
			}
			else {
				values.push_back(value);
			}
		}
		file.close();

		std::list<std::string>::const_iterator it = values.begin();
		for (it = values.begin(); it != values.end(); it++) {
			std::string tmp = *it;
	//		cout << tmp << endl;
			if (tmp == "#serverMode") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				if (tmpInt == 1) {
                    std::cout << "============INSTANCE MODE============" << std::endl;
                    std::cout << "-------------SERVER MODE-------------" << std::endl;
                    std::cout << "=====================================" << std::endl;
					instanceType = INSTANCE_SERVER;
				}
				else {
					instanceType = INSTANCE_REGULAR; 
				}
			}
            else if (tmp == "#evolutionType") {
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
            else if(tmp == "#expPluginName"){
                it++;
                tmp = *it;
                exp_plugin_name = tmp;
            }
            else if(tmp == "#extSettingsClass"){
                it++;
                tmp = *it;
                load_external_settings = atoi(tmp.c_str());
            }
			else if (tmp == "#fitnessType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt)
				{
                default:
                    throw std::runtime_error("unrecognized fitness settings");
				case MOVE:
					fitnessType = MOVE;
					std::cout << "============FITNESS TYPE=============" << std::endl;
					std::cout << "----------MOVE FITNESS TYPE----------" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case SOLAR:
					fitnessType = SOLAR;
					std::cout << "============FITNESS TYPE=============" << std::endl;
					std::cout << "---------SOLAR FITNESS TYPE----------" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case SWIM:
                    std::cout << "============FITNESS TYPE=============" << std::endl;
                    std::cout << "-------SWIMMING FITNESS TYPE---------" << std::endl;
                    std::cout << "=====================================" << std::endl;
					fitnessType = SWIM;
					break;
				case FITNESS_JUMP:
					fitnessType = FITNESS_JUMP;
					std::cout << "============FITNESS TYPE=============" << std::endl;
					std::cout << "----------JUMP FITNESS TYPE----------" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case MOVE_AND_SOLAR:
					fitnessType = MOVE_AND_SOLAR;
					std::cout << "============FITNESS TYPE=============" << std::endl;
					std::cout << "-----MOVE AND SOLAR FITNESS TYPE-----" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				}

			}
			else if (tmp == "#environmentType") {
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt)
				{
                default:
                    throw std::runtime_error("unrecognized environment settings");
				case DEFAULT_ENV:
					environmentType = DEFAULT_ENV;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "--INITIALIZING DEFAULT ENVIRONMENT---" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case SUN_BASIC:
					environmentType = SUN_BASIC;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "--INITIALIZING SUN BASIC ENVIRONMENT-" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case SUN_CONSTRAINED:
					environmentType = SUN_CONSTRAINED;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "INITIALIZING SUN CONSTRAINED ENVIRONMENT" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case SUN_BLOCKED:
					environmentType = SUN_BLOCKED;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "------BLOCKED SUN ENVIRONMENT--------" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case SUN_CONSTRAINED_AND_MOVING:
					environmentType = SUN_CONSTRAINED_AND_MOVING;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "INITIALIZING SUN CONSTRAINED AND MOVING ENVIRONMENT" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				case WATER_ENV:
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "----INITIALIZING WATER ENVIRONMENT---" << std::endl;
					std::cout << "=====================================" << std::endl;

					environmentType = WATER_ENV;
					break;
				case ROUGH:
					environmentType = ROUGH;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "----INITIALIZING ROUGH ENVIRONMENT---" << std::endl;
					std::cout << "=====================================" << std::endl;

					break;
				case SUN_MOVING:
					environmentType = SUN_MOVING;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "-INITIALIZING SUN MOVING ENVIRONMENT-" << std::endl;
					std::cout << "=====================================" << std::endl;

					break;
				case CONSTRAINED_MOVING_SUN:
					environmentType = CONSTRAINED_MOVING_SUN;
					std::cout << "===========ENVIRONMENT TYPE==========" << std::endl;
					std::cout << "--------CONSTRAINED MOVING SUN-------" << std::endl;
					std::cout << "=====================================" << std::endl;
					break;
				}
			}
			else if (tmp == "#controlType") {
				std::cout << "========INITIALIZING CONTROL=========" << std::endl;
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt)
				{
                default:
                    throw std::runtime_error("unrecognized controlType settings");
				case ANN_DEFAULT:
					std::cout << "----------INITIALIZING ANN-----------" << std::endl;
					controlType = ANN_DEFAULT;
					break;
				case ANN_CUSTOM:
					std::cout << "-------INITIALIZING CUSTOM ANN-------" << std::endl;
					controlType = ANN_CUSTOM;
					break;
				case ANN_CPPN:
					std::cout << "----------INITIALIZING CPPN----------" << std::endl;
					controlType = ANN_CPPN;
					break;
				case ANN_DISTRIBUTED_UP:
					std::cout << "----INITIALIZING DISTRIBUTED ANN----" << std::endl;
					controlType = ANN_DISTRIBUTED_UP;
					break;
				case ANN_DISTRIBUTED_DOWN:
					std::cout << "----INITIALIZING DISTRIBUTED ANN----" << std::endl;
					controlType = ANN_DISTRIBUTED_DOWN;
					break;
				case ANN_DISTRIBUTED_BOTH:
					std::cout << "----INITIALIZING DISTRIBUTED ANN----" << std::endl;
					controlType = ANN_DISTRIBUTED_BOTH;
					break;
				}
				std::cout << "=====================================" << std::endl;
			}
			else if (tmp == "#morphologyType") {
				std::cout << "=======INITIALIZING MORPHOLOGY=======" << std::endl;
				it++;
				tmp = *it;
				int tmpInt = atoi(tmp.c_str());
				switch (tmpInt)
                {
                default:
                    throw std::runtime_error("unrecognized Morphology type settings");
				case CAT_MORPHOLOGY:
					std::cout << "----------INITIALIZING CAT----------" << std::endl;
					morphologyType = CAT_MORPHOLOGY;
					break;
				case MODULAR_LSYSTEM:
					std::cout << "----------MODULAR LSYSTEM----------" << std::endl;
					morphologyType = MODULAR_LSYSTEM;
					break;
				case CUSTOM_MORPHOLOGY:
					std::cout << "--------CUSTOM MORPHOLOGY----------" << std::endl;
					morphologyType = CUSTOM_MORPHOLOGY;
					break;
				case MODULAR_CPPN:
					std::cout << "-----------MODULAR CPPN------------" << std::endl;
					morphologyType = MODULAR_CPPN;
					break;
				case MODULAR_DIRECT:
					std::cout << "----------MODULAR DIRECT-----------" << std::endl;
					morphologyType = MODULAR_DIRECT;
					break;
				}
                std::cout << "=====================================" << std::endl;
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
				switch (tmpInt)
                {
                default:
                    throw std::runtime_error("unrecognized selection type settings");
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
				switch (tmpInt)
                {
                default:
                    throw std::runtime_error("unrecognized Replacement type settings");
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
				std::cout << "generation found : " << generation << std::endl;
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
				std::cout << "populationSize found : " << populationSize << std::endl;
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
				std::cout << "found best individual" << std::endl;
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
				std::cout << "found max force modules" << std::endl;
				it++;
				tmp = *it;
				maxForce = atof(tmp.c_str());
			}
			else if (tmp == "#maxForceSensorModules") {
				std::cout << "found max force sensor" << std::endl;
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
				std::vector<int> tmpTypes;

				for (int i = 0; i < numberOfModules; i++) {
					it++;
					tmp = *it;
					std::vector<int> tmpMaxMods;
					tmpMaxMods.push_back(moduleTypes[i]);
					tmpMaxMods.push_back(atoi(tmp.c_str()));
					maxModuleTypes.push_back(tmpMaxMods);
					tmpMaxMods.clear();
				}
			}
            else if (tmp == "#collidingorgans") {
                it++;
                tmp = *it;
                if (atoi(tmp.c_str()) == 0) {
                    bCollidingOrgans = false;
                }
                else {
                    bCollidingOrgans = true;
                }
                std::cout << "Viability: Colliding organs - " << bCollidingOrgans << std::endl;
            }
            else if (tmp == "#organsbelowprintingbed") {
                it++;
                tmp = *it;
                if (atoi(tmp.c_str()) == 0) {
                    bOrgansAbovePrintingBed = false;
                }
                else {
                    bOrgansAbovePrintingBed = true;
                }
                std::cout << "Viability: Organs above printing bed - " << bOrgansAbovePrintingBed << std::endl;
            }
            else if (tmp == "#nonprintableorientations") {
                it++;
                tmp = *it;
                if (atoi(tmp.c_str()) == 0) {
                    bNonprintableOrientations = false;
                }
                else {
                    bNonprintableOrientations = true;
                }
                std::cout << "Viability: Organs above printing bed - " << bNonprintableOrientations << std::endl;
            }
            else if (tmp == "#anynumberoforgans") {
                it++;
                tmp = *it;
                if (atoi(tmp.c_str()) == 0) {
                    bAnyOrgansNumber = false;
                }
                else {
                    bAnyOrgansNumber = true;
                }
                std::cout << "Viability: Organs above printing bed - " << bNonprintableOrientations << std::endl;
            }
			fileExists = true;
		}
	}
	std::cout << "Loaded Settings" << std::endl;
}

void Settings::readSettings(const std::string& filename)
{
    repository = filename;
    readSettings();
}

void Settings::saveSettings()
{
	std::ofstream settingsFile;
	settingsFile.open(repository + "/settings" + std::to_string(sceneNum) + ".csv");
	settingsFile << ",#serverMode," << instanceType << "," << std::endl;
	settingsFile << ",#evolutionType," << evolutionType << "," << std::endl;
	settingsFile << ",#fitnessType," << fitnessType << "," << std::endl;
	settingsFile << ",#environmentType," << environmentType << "," << std::endl;
	settingsFile << ",#morphologyType," << morphologyType << "," << std::endl;
	settingsFile << ",#selectionType," << selectionType << "," << std::endl;
	settingsFile << ",#replacementType," << replacementType << "," << std::endl;
	settingsFile << ",#controlType," << controlType << "," << std::endl;
	settingsFile << ",#generation," << generation << "," << std::endl; // set when saving
	settingsFile << ",#maxGeneration," << maxGeneration << "," << std::endl;
	settingsFile << ",#generationInterval," << xGenerations << "," << std::endl;
	settingsFile << ",#indCounter," << individualCounter << "," << std::endl; // set when saving
	settingsFile << ",#populationSize," << populationSize << "," << std::endl;
	settingsFile << ",#mutationRate," << mutationRate << "," << std::endl;
	settingsFile << ",#morphMutRate," << morphMutRate << "," << std::endl;
	settingsFile << ",#crossover," << crossover << "," << std::endl;
	settingsFile << ",#crossoverRate," << crossoverRate << "," << std::endl;
	settingsFile << ",#energyDissipationRate," << energyDissipationRate << "," << std::endl;
	settingsFile << ",#agingInds," << ageInds << "," << std::endl;
	settingsFile << ",#maxAge," << maxAge << "," << std::endl;
	settingsFile << ",#minAge," << minAge << "," << std::endl;
	settingsFile << ",#deathProbability," << deathProb << "," << std::endl;
	settingsFile << ",#lIncrements," << lIncrements << "," << std::endl;
	settingsFile << ",#bestIndividual," << bestIndividual << std::endl; // set when saving
	settingsFile << ",#initialSeed," << seed << "," << std::endl;
	settingsFile << ",#verbose," << verbose << "," << std::endl;
	settingsFile << ",#useVarModules," << useVarModules << "," << std::endl;
	settingsFile << ",#maxNumberModules," << maxNumberModules << "," << std::endl;
    settingsFile << ",#expPluginName," << exp_plugin_name << "," << std::endl;
    settingsFile << ",#extSettingsClass," << load_external_settings << "," << std::endl;


	// Viability
    settingsFile << ",#collidingorgans," << bCollidingOrgans << "," << std::endl;
    settingsFile << ",#organsbelowprintingbed," << bOrgansAbovePrintingBed << "," << std::endl;
    settingsFile << ",#nonprintableorientations," << bNonprintableOrientations << "," << std::endl;
    settingsFile << ",#anynumberoforgans," << bAnyOrgansNumber << "," << std::endl;

	settingsFile << ",#sendGenomeAsSignal," << sendGenomeAsSignal << "," << std::endl;
	settingsFile << ",#shouldReopenConnections," << shouldReopenConnections << "," << std::endl;

	settingsFile << ",#maxForceModules," << maxForce << "," << std::endl;
	settingsFile << ",#maxForceSensorModules," << maxForceSensor << "," << std::endl;
	settingsFile << ",#repository," << repository << "," << std::endl;

    settingsFile << ",#numberOfModules," << numberOfModules << "," << std::endl; // not used
	settingsFile << ",#moduleTypes,";
	for (int moduleType : moduleTypes) {
		settingsFile << moduleType << ",";
	} settingsFile << std::endl;

	settingsFile << ",#maxAmountModulesTypes,";
	for (auto & maxModuleType : maxModuleTypes) {
		settingsFile << maxModuleType[1] << ",";
	}	settingsFile << std::endl;

	settingsFile << ",#individuals,";
	for (int i = 0; i < populationSize; i++) {
		settingsFile << indNumbers[i] << ","; // must be set when saving
	} settingsFile << std::endl;
	if (!indFits.empty()) {
		settingsFile << ",#indFits,";
		for (int i = 0; i < populationSize; i++) {
			settingsFile << indFits[i] << ","; // must be set when saving
		} 
	}
	settingsFile << std::endl;

	settingsFile.close();
}

void Settings::setRepository(const std::string &repository)
{
    this->repository = repository;
    if(verbose){
        std::cout << "Setting repository to ->" << repository << std::endl;
    }
}
