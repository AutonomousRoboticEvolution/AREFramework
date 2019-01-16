
#include "ER_VREP.h"
//#include <afxwin.h> ;

//***************************************************************************************//
ER_VREP::ER_VREP(){
	// to initialize ER
}
ER_VREP::~ER_VREP(){

}

// for reading text files

void ER_VREP::initializeServer() {
	populationsVREP.clear();
	unique_ptr<EnvironmentFactory> environmentFactory(new EnvironmentFactory);
	//	EnvironmentFactory *environmentFactory = new EnvironmentFactory;
	int envType = settings->environmentType;
	int fitnessType = settings->fitnessType;
	environment = environmentFactory->createNewEnvironment(settings);
	environment->init();
	environmentFactory.reset();
	// server has a populationSize of 0 since the populationGenomes are never used, only newgenome
	populationsVREP.push_back(unique_ptr<PopulationVREP>(new PopulationVREP(0, randNum, settings)));
}

void ER_VREP::initializeSimulation() {
	// set env
	unique_ptr<EnvironmentFactory> environmentFactory(new EnvironmentFactory);
//	EnvironmentFactory *environmentFactory = new EnvironmentFactory;
	int envType = settings->environmentType;
	int fitnessType = settings->fitnessType;
	environment = environmentFactory->createNewEnvironment(settings);
	environment->init();
	
	environmentFactory.reset();

	if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 9 || atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 8) {
		simSet = RECALLBEST;
	}

	if ((settings->evolutionType == settings->STEADY_STATE || settings->evolutionType == settings->GENERATIONAL) && simSet != RECALLBEST) {
		// note that the environment pointer is empty in this class, the VREP relevant files assign a pointer to it, thus accessing it when
		// running the client will give a null pointer error.. Could I fix this?
		populationsVREP.push_back(unique_ptr<PopulationVREP>(new PopulationVREP(settings->populationSize, randNum, settings)));
		cout << "Population created" << endl;
		populationsVREP[0]->popIndNumbers = settings->indNumbers;
		indCounter = settings->individualCounter;
		for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
			populationsVREP[0]->populationGenomes[i]->mutationRate = settings->mutationRate;
		}
		for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
			populationsVREP[0]->populationGenomes[i]->maxAge = settings->maxAge;
		}

		if (settings->morphologyType == 0) {
			for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
				populationsVREP[0]->populationGenomes[i]->morphologyType = 2; // should be changed to 0 at some point
			}
		}
		if (settings->morphologyType == 1) {
			for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
				populationsVREP[0]->populationGenomes[i]->morphologyType = 1; // should be changed to 0 at some point
			}
		}
		if (settings->morphologyType == 3) {
			for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
				populationsVREP[0]->populationGenomes[i]->morphologyType = 3; // should be changed to 0 at some point
			}
		}
		if (settings->evolutionType == settings->GENERATIONAL) {
			populationsVREP[0]->nextGenFitness.resize(settings->populationSize);
		}
	}
	else if (settings->evolutionType == settings->EMBODIED_EVOLUTION && simSet != RECALLBEST) {
		populationsVREP.push_back(unique_ptr<PopulationVREP>(new PopulationVREP(1, randNum, settings)));
	}

	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && simSet == RECALLBEST) {
		populationsVREP.push_back(unique_ptr<PopulationVREP>(new PopulationVREP(1, randNum, settings)));
	}
	else if (settings->evolutionType == settings->EMBODIED_EVOLUTION && simSet == RECALLBEST) {
		populationsVREP.push_back(unique_ptr<PopulationVREP>(new PopulationVREP(1, randNum, settings)));
	}
	else if (settings->generation != 0) {
		cout << "loading genomes" << endl;
		populationsVREP[0]->loadPopulationGenomes(sceneNum);
		indCounter = settings->individualCounter;
		indCounter++;
		generation = settings->generation;
		cout << "population loaded" << endl;
	}
	newGenerations = 0; /**/
}

void ER_VREP::initializeRobot() {
	settings->openPort();
//	populationsVREP.push_back(unique_ptr<PopulationVREP>(new PopulationVREP(settings->populationSize, randNum, settings)));
//	populationsVREP[0]->initializeIndividual(currentInd);
	
}

void ER_VREP::initialize() {
	/* initialize the settings class; it will read a settings file or it 
	 * will use default parameters if it cannot read a settings file. 
	 * A random number class will also be created and all other files
	 * refer to this class. 
	 */

	//NEAT neat = new NEAT();
	settings = shared_ptr<Settings>(new Settings);
//	cout << "set seed to " << sceneNum << endl;
	shared_ptr<RandNum> newRandNum(new RandNum(settings->seed));
	randNum = newRandNum;
	newRandNum.reset();
	settings->repository = simGetStringParameter(sim_stringparam_app_arg3);
	settings->readSettings(sceneNum);
	if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 9) {
		cout << "Recalling best" << endl;
		simSet = RECALLBEST;
		settings->instanceType = settings->INSTANCE_REGULAR;
	}
	
	/*indicate that a robot should be loaded
	*/
	if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 8) {
		cout << "recalling best and coupling it to robot" << endl;
		simSet = RECALLBEST;
		settings->instanceType = settings->INSTANCE_REGULAR;
		settings->evolutionType = settings->EMBODIED_EVOLUTION;
	}


	/* Start by determining whether the simulation is connected to
	 * to an actual robot morphology as specified in the settings file.
	 */

	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->instanceType == settings->INSTANCE_REGULAR) {
		initializeSimulation();
	}
	else if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->instanceType == settings->INSTANCE_SERVER && simSet != RECALLBEST) {
		initializeServer();
	}
	else {
		initializeSimulation();
		initializeRobot();
	}
}

void ER_VREP::startOfSimulation(){
	
	/* When V-REP starts, this function is called. Depending on the settings,
	* it initializes the properties of the individual of the optimization
	* strategy chosen.
	*/
	if (settings->verbose) {
		cout << "starting" << endl;
	}
	randNum->setSeed(settings->seed + indCounter * indCounter);
	environment->initialPos.clear();
	environment->init();
	// cout << "WHAT!" << endl;
	
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		// create newGenome
		cout << "creating genome" << endl; 
		currentMorphology = populationsVREP[0]->newGenome->morph; // essential function
		populationsVREP[0]->createNewIndividual();
		cout << "individual created" << endl;
	}
	else {
		if (settings->evolutionType == settings->STEADY_STATE && simSet != RECALLBEST) { // by default do an evolutionary run. 
			if (indCounter < populationsVREP[0]->populationGenomes.size()) {
		
				// cout << "creating initial individual : " << indCounter << endl; 
				//	indCounter = 100;
				currentInd = indCounter;
				populationsVREP[0]->initializeIndividual(currentInd);
				currentMorphology = populationsVREP[0]->populationGenomes[currentInd]->morph; // essential function
			//	populationsVREP[0]->loadIndividualGenome(1, 2);
			//	populationsVREP[0]->growInd(2, indCounter);
			//	populationsVREP[0]->growInd(amountIncrements, currentInd);
				populationsVREP[0]->popIndNumbers.push_back(indCounter);
				if (settings->verbose) {
					cout << "creating individual" << endl;
				}
				populationsVREP[0]->createIndividual(indCounter);
				if (settings->verbose) {
					cout << "created individual" << endl;
				}

			}
			else if (indCounter >= populationsVREP[0]->populationGenomes.size()) {
				currentInd = randNum->randInt(populationsVREP[0]->populationGenomes.size(), 0);
				if (settings->selectionType != settings->PROPORTIONATE_SELECTION) {
					cout << "selecting new individual" << endl;
					populationsVREP[0]->selectNewIndividual(currentInd); // before this, the old stored class of newGenome needs to be deleted, else memory will leak
					cout << "setting morph" << endl;
					currentMorphology = populationsVREP[0]->newGenome->morph; // essential function
					cout << "done selecting new individual" << endl;
				}
				else {
					populationsVREP[0]->selectNewIndividualProportional(currentInd, sceneNum);
					currentMorphology = populationsVREP[0]->newGenome->morph; // essential function
				}
				cout << "Muatating new individual!" << endl;
				populationsVREP[0]->mutateNewIndividual();
				cout << "Creating new individual" << endl;
				populationsVREP[0]->createNewIndividual();
			}
		}
		else if (settings->evolutionType == settings->EMBODIED_EVOLUTION && simSet != RECALLBEST) {
			currentInd = indCounter;
			//		populationsVREP[0]->initializeIndividual(currentInd);
			//		cout << "creating new individual" << endl;
			populationsVREP[0]->initializeNewIndividual();
			currentMorphology = populationsVREP[0]->newGenome->morph; // essential function
	//		populationsVREP[0]->createNewIndividual();
			if (settings->verbose) {
				cout << "created new individual" << endl;
			}
		}
		else if (settings->evolutionType == settings->GENERATIONAL) {
			if (generation == 0) {
				populationsVREP[0]->initializeIndividual(currentInd);
				populationsVREP[0]->popIndNumbers.push_back(indCounter);
			}
			else {
				if (populationsVREP[0]->nextGenGenomes.size() < populationsVREP[0]->populationGenomes.size()) {
		//			populationsVREP[0]->createNewGenRandomlySelect(1,1);
					for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
						populationsVREP[0]->createNewIndividual();
						populationsVREP[0]->nextGenGenomes.push_back(std::move(populationsVREP[0]->newGenome));
						populationsVREP[0]->popNextIndNumbers.push_back(indCounter);
					}
					currentInd = 0;
				}
				//	populationsVREP[0]->evaluateNextGen(currentInd);
				//	currentInd++;
			}
		}
		else if (settings->evolutionType == settings->AFPO) {
			if (generation == 0) {
				populationsVREP[0]->initializeIndividual(currentInd);
				populationsVREP[0]->popIndNumbers.push_back(indCounter);
			}
			else {
				vector<int> paretoIndividuals;
				populationsVREP[0]->getParetoIndividuals();
//				for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
//				}

				if (populationsVREP[0]->nextGenGenomes.size() < populationsVREP[0]->populationGenomes.size()) {
					for (int i = 0; i < populationsVREP[0]->populationGenomes.size(); i++) {
						populationsVREP[0]->createNewIndividual();
						populationsVREP[0]->nextGenGenomes.push_back(std::move(populationsVREP[0]->newGenome));
						populationsVREP[0]->popNextIndNumbers.push_back(indCounter);
					}
					currentInd = 0;
				}
				//	populationsVREP[0]->evaluateNextGen(currentInd);
				//	currentInd++;
			}
		}

		//else if (simType == COEVOLUTION) {
		//	if (indCounter < populationsVREP[0]->populationGenomes.size()) {
		//		currentInd = indCounter;
		//		populationsVREP[0]->initializeIndividual(currentInd);
		//		populationsVREP[0]->popIndNumbers.push_back(indCounter);
		//	}
		//	else if (indCounter >= populationsVREP[0]->populationGenomes.size()) {
		//		currentInd = randNum->randInt(populationsVREP[0]->populationGenomes.size(),0);
		//		populationsVREP[0]->selectNewIndividual(currentInd); 
		//		populationsVREP[0]->mutateNewIndividual();
		//		populationsVREP[0]->createNewIndividual();
		//		populationsVREP[0]->selectNewCoPop();
		//		populationsVREP[0]->createCoPopulation(1.5);
		//	}
		//}
		else if (simSet == RECALLBEST) {
			//	populationsVREP[0]->initializeIndividual(0);
		//		populationsVREP[0]->loadIndividualGenome(settings->bestIndividual, sceneNum);
			populationsVREP[0]->loadBestIndividualGenome(sceneNum);
			currentMorphology = populationsVREP[0]->newGenome->morph;
			//		populationsVREP[0]->loadIndividualGenome(0, sceneNum);
		}
	}
	currentMorphology->setPhenValue();
}

void ER_VREP::handleSimulation() {
	
	/* This function is called every simulation step. Note that the behavior of
	* the robot drastically changes when slowing down the simulation since this
	* function will be called more often. All simulated individuals will be
	* updated until the maximum simulation time, as specified in the environment
	* class, is reached.
	*/
	simulationTime += simGetSimulationTimeStep();
	environment->updateEnv(currentMorphology);
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		populationsVREP[0]->update();
		if (simGetSimulationTime() > environment->maxTime) {
			simStopSimulation();
		}
	}
	else {
		if (settings->evolutionType == settings->EMBODIED_EVOLUTION && simSet != RECALLBEST) {
		//	cout << "UPDATING EMBODIED" << endl;
			populationsVREP[0]->update();
		}
		else if (settings->evolutionType == settings->STEADY_STATE && simSet != RECALLBEST) {
			if (indCounter < populationsVREP[0]->populationGenomes.size()) {
				populationsVREP[0]->update(indCounter);
			}
			else {
				populationsVREP[0]->update();
			}
			if (simGetSimulationTime() > environment->maxTime) {
				simStopSimulation();
			}
		}
		else if (settings->evolutionType == settings->GENERATIONAL) {
			populationsVREP[0]->updateNextGen(currentInd);
			if (simGetSimulationTime() > environment->maxTime) {
				simStopSimulation();
			}
		}
		if (simSet == RECALLBEST && settings->evolutionType != settings->EMBODIED_EVOLUTION) {
			populationsVREP[0]->update();
			if (simGetSimulationTime() > environment->maxTime) {
				simStopSimulation();
			}
		}
		if (simSet == RECALLBEST && settings->evolutionType == settings->EMBODIED_EVOLUTION) {
			populationsVREP[0]->update();
		
//			if (simGetSimulationTime() > environment->maxTime) {
//				simStopSimulation();
//			}
		}
		if (settings->evolutionType == settings->EMPTY_RUN) {
			simStopSimulation();
		}
		//	case (COEVOLUTION) :
		//		break; 
		//	}
	}
}

void ER_VREP::endOfSimulation(){
	/* At the end of the simulation the fitness value of the simulated individual
	* is retrieved and stored in the appropriate files. 
	*/
	cout << "end" << endl;
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		float fitness = environment->fitnessFunction(currentMorphology);
		float phenValue = populationsVREP[0]->newGenome->morph->phenValue;
		cout << "fitness = " << fitness << endl;
	//	cout << "phenValue = " << phenValue << endl;
		simSetFloatSignal((simChar*) "fitness", fitness); // set fitness value to be received by client
		simSetFloatSignal((simChar*) "phenValue", phenValue); // set phenValue

		int signal[1] = { 2 };
		simSetIntegerSignal((simChar*) "simulationState", signal[0]);
//		int signal[1] = { 0 };
//		simSetIntegerSignal((simChar*) "simulationState", signal[0]); // this means that the client can get the fitness value
	}
	else {
		cout << "indCounter = " << indCounter << endl;
		if (settings->evolutionType == settings->STEADY_STATE && simSet != RECALLBEST) {
			if (indCounter - 1 >= populationsVREP[0]->populationGenomes.size()) {
			}
			else {
				populationsVREP[0]->populationGenomes[indCounter - 1]->deleteCreated();
			}
			if (indCounter % populationsVREP[0]->populationGenomes.size() == 0 && indCounter != 0) {
				populationsVREP[0]->savePopFitness(generation, populationsVREP[0]->populationFitness, sceneNum);
				if (settings->ageInds == 1) {
					populationsVREP[0]->agePop();
				}
				generation++;
				saveSettings();
				newGenerations++;
			}
			if (indCounter < populationsVREP[0]->populationGenomes.size()) {
				float fitness = environment->fitnessFunction(currentMorphology);
				cout << "fitness = " << fitness << endl;
				populationsVREP[0]->populationGenomes[currentInd]->morph->saveGenome(indCounter, sceneNum, fitness);
				populationsVREP[0]->populationFitness[currentInd] = fitness;
				populationsVREP[0]->popIndNumbers[currentInd] = indCounter;
				// populationsVREP[0]->evaluateIndividual(currentInd, indCounter, sceneNum);
				indCounter++;
			}
			else if (indCounter >= populationsVREP[0]->populationGenomes.size()) {
				float fitness = environment->fitnessFunction(currentMorphology);
				if (settings->replacementType == 0) {
					populationsVREP[0]->replaceNewIndividual(indCounter, sceneNum, fitness);
				}
				else if (settings->replacementType == 1) {
					populationsVREP[0]->replaceNewIndividualAgainstWorst(indCounter, sceneNum, fitness); //
				}
				cout << "FITNESS = " << fitness << endl;
				indCounter++;
			}
		}
		if (settings->evolutionType == settings->GENERATIONAL) {
			float fitness = environment->fitnessFunction(currentMorphology);
			populationsVREP[0]->nextGenFitness[currentInd] = fitness;
			//		populationsVREP[0]->evaluateIndividual(currentInd, indCounter, sceneNum);
			indCounter++;
			currentInd++;

			if (indCounter % populationsVREP[0]->populationGenomes.size() == 0 && indCounter != 0) {
				populationsVREP[0]->savePopFitness(generation, populationsVREP[0]->populationFitness, sceneNum);
				if (settings->ageInds == 1) {
					populationsVREP[0]->agePop();
				}
				generation++;
				saveSettings();
				newGenerations++;
				populationsVREP[0]->replacePopulation();
				//			currentInd = 0; in this case currentInd is set to zero at the start of the simulation
			}
		}
		if (settings->evolutionType == settings->RANDOM_SEARCH) { // note, this is not functioning properly yet
			populationsVREP[0]->populationGenomes[currentInd]->deleteCreated();
			float fitness = environment->fitnessFunction(currentMorphology);
			populationsVREP[0]->replaceNewIndividual(indCounter, sceneNum, fitness);


			//	float fitness = populationGenomes[currentInd]->evaluateGenome(sceneNum);
			//	populationGenomes[currentInd]->morph->saveGenome(indCounter, sceneNum, fitness);
			//	populationFitness[currentInd] = fitness;
			//	popIndNumbers[currentInd] = indCounter;
			//	return fitness;


			indCounter++;
			currentInd++;

			if (indCounter % populationsVREP[0]->populationGenomes.size() == 0 && indCounter != 0) {
				populationsVREP[0]->savePopFitness(generation, populationsVREP[0]->populationFitness, sceneNum);
				if (settings->ageInds == 1) {
					populationsVREP[0]->agePop();
				}
				generation++;
				saveSettings();
				newGenerations++;
				populationsVREP[0]->replacePopulation();
				currentInd = 0;
			}
		}
		if (simSet == RECALLBEST) {
			float fitness = environment->fitnessFunction(currentMorphology);
			populationsVREP[0]->newGenome.reset();
			cout << "-----------------------------------" << endl;
			cout << "fitness = " << fitness << endl;
			cout << "-----------------------------------" << endl;
		}
		/*case (COEVOLUTION) : {
			if (indCounter - 1 >= populationsVREP[0]->populationGenomes.size()) {
			}
			else {
				populationsVREP[0]->populationGenomes[indCounter - 1]->deleteCreated();
			}
			if (indCounter % populationsVREP[0]->populationGenomes.size() == 0 && indCounter != 0) {
				populationsVREP[0]->savePopFitness(generation, populationsVREP[0]->populationFitness, sceneNum);
				generation++;
				newGenerations++;
			}
			if (indCounter < populationsVREP[0]->populationGenomes.size()) {
				populationsVREP[0]->evaluateIndividual(currentInd, indCounter, sceneNum);
				indCounter++;
			}
			else if (indCounter >= populationsVREP[0]->populationGenomes.size()) {
				float fitness = populationsVREP[0]->evaluateNewIndividual(indCounter, sceneNum);
				cout << "FITNESS = " << fitness << endl;
				indCounter++;
			}
			break;
		}
		}*/
		//	simCloseScene();

		if (newGenerations == settings->xGenerations) { // close v-rep every x generations
			string lightName = "Light"; // +to_string(sceneNum);
			int light_handle = simGetObjectHandle(lightName.c_str());
			simRemoveObject(light_handle);
			//		cout << "removed lightHandle" << endl;
			simQuitSimulator(false);
		}
	}
}

void ER_VREP::saveSettings(){ 
	cout << "saving" << endl;
	settings->generation = generation;
	settings->individualCounter = indCounter;
	vector<int> indNums;
	for (int i = 0; i < populationsVREP[0]->popIndNumbers.size(); i++) {
		indNums.push_back(populationsVREP[0]->popIndNumbers[i]); // must be set when saving
	}
	settings->indNumbers = indNums;

	int bestInd = 0;
	int bestIndividual = 0;
	float bestFitness = 0;
	for (int i = 0; i < populationsVREP[0]->populationFitness.size(); i++) {
		if (bestFitness < populationsVREP[0]->populationFitness[i]) {
			bestFitness = populationsVREP[0]->populationFitness[i];
			bestInd = i;
			bestIndividual = populationsVREP[0]->popIndNumbers[bestInd];
			cout << "Best individual has number " << populationsVREP[0]->popIndNumbers[bestInd] << endl;
		}
	}
	settings->bestIndividual = bestIndividual;
	settings->saveSettings(sceneNum);
	cout << "Settings saved" << endl;
}