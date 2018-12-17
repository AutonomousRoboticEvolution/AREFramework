
#include "ER_VREP.h"

//#include <afxwin.h> ;
//***************************************************************************************//
ER_VREP::ER_VREP(){
}
ER_VREP::~ER_VREP(){
}

// for reading text files
void ER_VREP::initializeServer() {
	// create the environment
	unique_ptr<EnvironmentFactory> environmentFactory(new EnvironmentFactory);
	environment = environmentFactory->createNewEnvironment(settings);
	environmentFactory.reset();
	ea = unique_ptr<EA_VREP>(new EA_VREP); // TODO Should not be the EA class
	ea->randomNum = randNum;
	ea->settings = settings;
	ea->init();
	ea->initNewGenome(settings, 0);
	//ea->initializePopulation(settings, false);
	environment->init();
}


void ER_VREP::initializeSimulation() {
	// set env
	unique_ptr<EnvironmentFactory> environmentFactory(new EnvironmentFactory);
	environment = environmentFactory->createNewEnvironment(settings);
	environmentFactory.reset();
	ea = unique_ptr<EA_VREP>(new EA_VREP);
	ea->randomNum = randNum;
	ea->init();
	ea->initializePopulation(settings, false);
	environment->init();
}

void ER_VREP::initialize() {
	/* initialize the settings class; it will read a settings file or it 
	 * will use default parameters if it cannot read a settings file. 
	 * A random number class will also be created and all other files
	 * refer to this class. 
	 */
	settings->indCounter = 0;
	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->instanceType == settings->INSTANCE_REGULAR) {
		cout << "Regular Evolution" << endl;
		settings->client = true;
		initializeSimulation();
	}
	else if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->instanceType == settings->INSTANCE_SERVER && simSet != RECALLBEST) {
		cout << "Initializing Server" << endl;
		settings->client = false;
		initializeServer();
	}
	else {
		// used to initialize robot connected to V-REP
		// initializeSimulation();
		// initializeRobot();
	}
	//simSet = RECALLBEST;
}

void ER_VREP::startOfSimulation(){
	
	/* When V-REP starts, this function is called. Depending on the settings,
	* it initializes the properties of the individual of the optimization
	* strategy chosen.
	*/
	if (settings->verbose) {
		cout << "Starting" << endl;
	}
	randNum->setSeed(settings->seed + settings->indCounter * settings->indCounter);
	// environment->initialPos.clear();
	// environment->init();
	
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		// If the simulation is a server. It just holds information for one genome for now. 
		ea->newGenome->create(); 
		// new genome should be initialized through api command. 
		currentMorphology = ea->newGenome->morph;
	}
	else {
		if (settings->evolutionType == settings->STEADY_STATE && simSet != RECALLBEST) { // by default do an evolutionary run. 
			if (settings->verbose) {
				cout << "Creating Individual " << settings->indCounter << endl;
			}
			if (settings->indCounter < ea->populationGenomes.size()) {		
				currentInd = settings->indCounter;
				//ea->createIndividual(currentInd);
				ea->populationGenomes[currentInd]->init();
				
				currentMorphology = ea->populationGenomes[currentInd]->morph; // essential function
				ea->popIndNumbers.push_back(settings->indCounter);
				if (settings->verbose) {
					cout << "creating individual" << endl;
				}
				// ea->createIndividual(settings->indCounter);
				if (settings->verbose) {
					cout << "created individual" << endl;
				}
				ea->newGenome = ea->populationGenomes[settings->indCounter];
			}
			else if (settings->indCounter >= ea->populationGenomes.size()) {
				// TODO: replace by selection function
				ea->selection(); 
				// randNum->randInt(ea->populationGenomes.size(), 0);
				// ea->newGenome = ea->populationGenomes[currentInd]->clone(); // should deep copy
				// ea->newGenome->mutate();
				// TODO check init
				ea->newGenome->init();
				currentMorphology = ea->newGenome->morph; // essential function... But for what? I forgot...
			}
		}
		else if (settings->evolutionType == settings->GENERATIONAL) { // NOTE: STILL HAS TO BE FIXED
			// TODO
		}

		else if (simSet == RECALLBEST) {
			ea->loadBestIndividualGenome(sceneNum);
			ea->newGenome->create();
			currentMorphology = ea->newGenome->morph;
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
		ea->newGenome->update();
		if (simGetSimulationTime() > environment->maxTime) {
			simStopSimulation();
		}
	}
	else {
		ea->newGenome->update();
		if (simGetSimulationTime() > environment->maxTime) {
			simStopSimulation();
		}
	}
}

float ER_VREP::fitnessFunction(MorphologyPointer morph) {
	vector <float> pStart;
	vector <float> pOne;
	vector <float> pEnd;
	float fitness = 0;

		//	int mainHandle = morph->getMainHandle();
		//	float pos[3];
		//	simGetObjectPosition(mainHandle, -1, pos);
		//	return pos[0];
		if (settings->moveDirection == settings->FORWARD_Y) {
			if (morph->modular == false) {
				int mainHandle = morph->getMainHandle();
				float pos[3];
				simGetObjectPosition(mainHandle, -1, pos);
				fitness = -pos[1];
				pEnd.push_back(pos[1]);
				if (pOne.size() < 1) {
					//			cout << "Note, pOne never set" << endl;
				}
				else {
					fitness = fitness + pOne[1];
				}
				pOne.clear();
				pEnd.clear();
			}
			else {
				int mainHandle = morph->getMainHandle();
				float pos[3];
				simGetObjectPosition(mainHandle, -1, pos);
				fitness = -pos[1];
				pEnd.push_back(-pos[1]);
				if (pOne.size() < 1) {
					//			cout << "Note, pOne never set" << endl;
				}
				else {
					fitness = fitness + pOne[1];
				}
				int brokenModules = morph->getAmountBrokenModules();
				fitness = fitness * pow(0.8, brokenModules);
				pOne.clear();
				pEnd.clear();
			}
		}
		else {
			if (morph->modular == false) {
				//		cout << "getting main handle" << endl;
				int mainHandle = morph->getMainHandle();
				float pos[3];
				simGetObjectPosition(mainHandle, -1, pos);
				fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
				pEnd.push_back(pos[0]);
				pEnd.push_back(pos[1]);
				if (pOne.size() < 1) {
					//			cout << "Note, pOne never set" << endl;
					fitness = sqrtf((pEnd[0] * pEnd[0]) + (pEnd[1] * pEnd[1]));
				}
				else {
					fitness = sqrtf(((pEnd[0] - pOne[0]) * (pEnd[0] - pOne[0])) + ((pEnd[1] - pOne[1]) * (pEnd[1] - pOne[1])));
				}
				pOne.clear();
				pEnd.clear();
				//	fitness = 0; // no fixed morphology that can absorb light
			}
			else {
				int mainHandle = morph->getMainHandle();
				float pos[3];
				simGetObjectPosition(mainHandle, -1, pos);

				pEnd.push_back(pos[0]);
				pEnd.push_back(pos[1]);
				if (pOne.size() < 1) {
					//			cout << "Note, pOne never set" << endl;
					fitness = sqrtf((pEnd[0] * pEnd[0]) + (pEnd[1] * pEnd[1]));
				}
				else {
					fitness = sqrtf(((pEnd[0] - pOne[0]) * (pEnd[0] - pOne[0])) + ((pEnd[1] - pOne[1]) * (pEnd[1] - pOne[1])));
				}
				int brokenModules = morph->getAmountBrokenModules();

				fitness = fitness * pow(0.8, brokenModules);
				pOne.clear();
				pEnd.clear();
				// to do: pEnd - pOne

			//	vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
			//	vector<float> pos = createdModules[0]->getPosition();
			//	fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
			}
		}

		return fitness;
	

}

void ER_VREP::endOfSimulation(){
	/* At the end of the simulation the fitness value of the simulated individual
	* is retrieved and stored in the appropriate files. 
	*/
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		float fitness = environment->fitnessFunction(currentMorphology);
		// Environment independent fitness function:
		// float fitness = fit->fitnessFunction(currentMorphology);
		float phenValue = ea->newGenome->morph->phenValue; // phenValue is used for morphological protection algorithm
		cout << "fitness = " << fitness << endl;
		simSetFloatSignal((simChar*) "fitness", fitness); // set fitness value to be received by client
		simSetFloatSignal((simChar*) "phenValue", phenValue); // set phenValue, for morphological protection
		int signal[1] = { 2 };
		simSetIntegerSignal((simChar*) "simulationState", signal[0]);
	}
	else {
		cout << "settings->indCounter = " << settings->indCounter << endl;
		if (settings->evolutionType == settings->STEADY_STATE && simSet != RECALLBEST) {

			if (settings->indCounter < ea->populationGenomes.size()) {
				float fitness = environment->fitnessFunction(currentMorphology);
				ea->populationGenomes[currentInd]->fitness = fitness;
				cout << "fitness = " << ea->populationGenomes[currentInd]->fitness << endl;
				ea->populationGenomes[currentInd]->morph->saveGenome(settings->indCounter, sceneNum, fitness);
				ea->popIndNumbers[currentInd] = settings->indCounter;
				settings->indCounter++;
			}
			else if (settings->indCounter >= ea->populationGenomes.size()) {
				float fitness = environment->fitnessFunction(currentMorphology);
				ea->newGenome->fitness = fitness;
				ea->newGenome->morph->saveGenome(settings->indCounter, sceneNum, fitness);
				cout << "FITNESS = " << fitness << endl;
				settings->indCounter++;
			}
			if (settings->indCounter % ea->populationGenomes.size() == 0 && settings->indCounter != 0) {
				ea->replacement();// replaceNewIndividual(settings->indCounter, sceneNum, fitness);
				ea->savePopFitness(generation);
				generation++;
				saveSettings();
				newGenerations++;
			}
		}
		if (simSet == RECALLBEST) {
			float fitness = environment->fitnessFunction(currentMorphology);
			ea->newGenome.reset();
			cout << "-----------------------------------" << endl;
			cout << "fitness = " << fitness << endl;
			cout << "-----------------------------------" << endl;
		}
		
		// following uncommented code was necessary when V-REP itself contained a memory leak.
		// if (newGenerations == settings->xGenerations) { // close v-rep every x generations
		//	string lightName = "Light"; // +to_string(sceneNum);
		//	int light_handle = simGetObjectHandle(lightName.c_str());
		//	simRemoveObject(light_handle);
		//	simQuitSimulator(false);
		// }
	}
}

shared_ptr<Morphology> ER_VREP::getMorphology(Genome* g)
{
	return shared_ptr<Morphology>();
}

void ER_VREP::saveSettings(){
	cout << "saving" << endl;
	settings->generation = generation;
	settings->individualCounter = settings->indCounter;
	vector<int> indNums;
	for (int i = 0; i < ea->popIndNumbers.size(); i++) {
		indNums.push_back(ea->popIndNumbers[i]); // must be set when saving
	}
	settings->indNumbers = indNums;

	int bestInd = 0;
	int bestIndividual = 0;
	float bestFitness = 0;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		if (bestFitness < ea->populationGenomes[i]->fitness) {
			bestFitness = ea->populationGenomes[i]->fitness;
			bestInd = i;
			bestIndividual = ea->popIndNumbers[bestInd];
			cout << "Best individual has number " << ea->popIndNumbers[bestInd] << endl;
		}
	}
	settings->bestIndividual = bestIndividual;
	settings->saveSettings();
	cout << "Settings saved" << endl;
}