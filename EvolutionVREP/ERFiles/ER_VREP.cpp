#include "ER_VREP.h"

using namespace std;

ER_VREP::ER_VREP()
{}

ER_VREP::~ER_VREP()
{}

/*!
 * Initializes ER as a server to accept genomes from client. 
 * 
 */
void ER_VREP::initializeServer()
{
	// create the environment
	unique_ptr<EnvironmentFactory> environmentFactory(new EnvironmentFactory);
	environment = environmentFactory->createNewEnvironment(settings);
	environmentFactory.reset();
	// initialize the environment
	environment->init();
	unique_ptr<EA_Factory> eaf(new EA_Factory);
	ea = eaf->createEA(randNum, settings); // unique_ptr<EA>(new EA_VREP);
	ea->randomNum = randNum;
	ea->init();
	eaf.reset();
}

void ER_VREP::initializeSimulation()
{
    // Initialize a genome factory to create genomes when the simulation is running
	genomeFactory = unique_ptr<GenomeFactoryVREP>(new GenomeFactoryVREP);
	genomeFactory->randomNum = randNum;
	// Environment factory is used to create the environment
	unique_ptr<EnvironmentFactory> environmentFactory(new EnvironmentFactory);
	environment = environmentFactory->createNewEnvironment(settings);
	environmentFactory.reset();
	unique_ptr<EA_Factory> eaf(new EA_Factory);
	ea = eaf->createEA(randNum, settings); // unique_ptr<EA>(new EA_VREP);
	ea->randomNum = randNum;
	ea->init();
	environment->init();
	eaf.reset();
}

void ER_VREP::initialize()
{
	/* initialize the settings class; it will read a settings file or it 
	 * will use default parameters if it cannot read a settings file. 
	 * A random number class will also be created and all other files
	 * refer to this class. 
	 */
	settings->indCounter = 0;
	if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->instanceType == settings->INSTANCE_REGULAR) {
		if (settings->verbose) {
		    std::cout << "Regular Evolution" << std::endl;
		}
		settings->client = true; // V-REP opens in client mode itself, this means there is no client-server relationship
		if (settings->verbose) {
			std::cout << "initializing evolution" << std::endl;
		}
		// initialize simulation
		initializeSimulation();
	}
	else if (settings->evolutionType != settings->EMBODIED_EVOLUTION && settings->instanceType == settings->INSTANCE_SERVER && settings->startingCondition != settings->COND_LOAD_BEST) {
		if (settings->verbose){
		    std::cout << "Initializing Server" << std::endl;
		}
		settings->client = false; // V-REP plugin will receive genomes from ea client
		initializeServer();
	}
}

void ER_VREP::startOfSimulation()
{
	
	/* When V-REP starts, this function is called. Depending on the settings,
	* it initializes the properties of the individual of the optimization
	* strategy chosen.
	*/
	if (settings->instanceType == settings->INSTANCE_DEBUGGING) {
		return;
	}
	if (settings->verbose) {
		std::cout << "Starting" << std::endl;
	}

	// set the random seed
	randNum->setSeed(settings->seed + settings->indCounter * settings->indCounter);

	if (settings->instanceType == settings->INSTANCE_SERVER) {
		if (settings->evolutionType == settings->EA_NEAT) {
			ea->createIndividual(individualToBeLoaded); // this actually sets the NEAT genome
			// ea->createIndividual(-1);
			currentMorphology = ea->getMorph();
			currentMorphology->create();
		}
		else {
			// If the simulation is a server. It just holds information for one genome for now.
			// currentGenome should be created, double check
			currentGenome->create();
			currentMorphology = currentGenome->morph;
		}
	}
	else {
		if (settings->startingCondition != settings->COND_LOAD_BEST) {
			if (settings->verbose) {
				cout << "Creating Individual " << settings->indCounter << endl;
			}
			if (settings->indCounter < ea->nextGenGenomes.size()) {
				// First generation:
				currentInd = settings->indCounter;
				ea->nextGenGenomes[currentInd]->init(); //create the genome for morphology and control

				if (settings->verbose) {
					cout << "creating individual" << endl;
				}
				if (settings->evolutionType == settings->EA_NEAT) {
					ea->createIndividual(-1);
					currentMorphology = ea->getMorph();
					currentMorphology->create();
				}
				else {
					currentGenome = genomeFactory->convertToGenomeVREP(ea->nextGenGenomes[settings->indCounter]);
					currentGenome->create();//create the morphology in vrep
					currentMorphology = currentGenome->morph->clone();
				}

			}
			else if (settings->indCounter >= ea->populationGenomes.size()) {
				if (settings->evolutionType == settings->EA_NEAT) {
					ea->createIndividual(-1);
					currentMorphology = ea->getMorph();
				}
				else {
					currentInd = settings->indCounter % settings->populationSize;
					currentGenome = genomeFactory->convertToGenomeVREP(ea->nextGenGenomes[currentInd]);
					currentGenome->create();
					currentMorphology = currentGenome->morph->clone(); // two different classes sharing the same parameter; essential function... But for what? I forgot...
				}
			}
		}

		else if (settings->startingCondition == settings->COND_LOAD_BEST) {

			if (settings->evolutionType == settings->EA_NEAT) {
				ea->loadBestIndividualGenome(settings->sceneNum); // loads from ea
				ea->createIndividual(-2);
				currentMorphology = ea->getMorph();

			}
			else {
				loadBestIndividualGenome(settings->sceneNum);
				currentGenome = genomeFactory->convertToGenomeVREP(currentGenome);
				currentGenome->create();
				currentMorphology = currentGenome->morph;
			}
		}

		else if (settings->startingCondition == settings->COND_LOAD_SPECIFIC_INDIVIDUAL) {
			// TODO
		}
	}
	if (settings->evolutionType != settings->EA_NEAT) {
		currentMorphology->setPhenValue();
	}
}

void ER_VREP::handleSimulation()
{
	
	/* This function is called every simulation step. Note that the behavior of
	* the robot drastically changes when slowing down the simulation since this
	* function will be called more often. All simulated individuals will be
	* updated until the maximum simulation time, as specified in the environment
	* class, is reached.
	*/
	if (settings->instanceType == settings->INSTANCE_DEBUGGING) {
		simStopSimulation();
		return;
	}
	simulationTime += simGetSimulationTimeStep();
	if (settings->evolutionType == settings->EA_NEAT) {
		// ea->end(); // needs the position of the robot.
		environment->updateEnv(currentMorphology);
	}
	else {
		environment->updateEnv(currentMorphology);
	}
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		if (settings->evolutionType == settings->EA_NEAT) {
			ea->update();
		}
		else {
			currentGenome->update();
		}
		if (simGetSimulationTime() > environment->maxTime) {
			simStopSimulation();
		}
	}
	else {
		if (settings->evolutionType == settings->EA_NEAT) {
			ea->update();
		}
		else {
			currentGenome->update();
		}
		if (simGetSimulationTime() > environment->maxTime) {
			simStopSimulation();
		}
	}
}

float ER_VREP::fitnessFunction(MorphologyPointer morph)
{
	vector <float> pStart; // start position of the robot
	vector <float> pOne; // position after x time
	vector <float> pEnd; // end position of the robot
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
				//	cout << "Note, pOne never set" << endl;
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
				//	cout << "Note, pOne never set" << endl;
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

void ER_VREP::endOfSimulation()
{
	/* At the end of the simulation the fitness value of the simulated individual
	* is retrieved and stored in the appropriate files. 
	*/
	if (settings->verbose) {
		std::cout << "End of simulation" << endl;
	}
	if (settings->instanceType == settings->INSTANCE_DEBUGGING) {
		return;
	}
	if (settings->instanceType == settings->INSTANCE_SERVER) {
		if (settings->evolutionType == settings->EA_NEAT) {
			currentMorphology = ea->getMorph();
			float fitness = environment->fitnessFunction(currentMorphology);
			ea->setFitness(-1, fitness);

			// Environment independent fitness function:
			// float fitness = fit->fitnessFunction(currentMorphology);
			float phenValue = 0.0; // not used in CPPN-NEAT
			cout << "fitness = " << fitness << endl;
			simSetFloatSignal((simChar*) "fitness", fitness); // set fitness value to be received by client
			simSetFloatSignal((simChar*) "phenValue", phenValue); // set phenValue, for morphological protection
			int signal[1] = { 2 };
			simSetIntegerSignal((simChar*) "simulationState", signal[0]);
		}


		else {
			float fitness = environment->fitnessFunction(currentMorphology);
			// Environment independent fitness function:
			// float fitness = fit->fitnessFunction(currentMorphology);
			float phenValue = currentGenome->morph->phenValue; // phenValue is used for morphological protection algorithm
			cout << "fitness = " << fitness << endl;
			simSetFloatSignal((simChar*) "fitness", fitness); // set fitness value to be received by client
			simSetFloatSignal((simChar*) "phenValue", phenValue); // set phenValue, for morphological protection
			int signal[1] = { 2 };
			simSetIntegerSignal((simChar*) "simulationState", signal[0]);
			if (settings->savePhenotype) {
				currentGenome->fitness = fitness;
				currentGenome->savePhenotype(currentGenome->individualNumber, settings->sceneNum);
			}
		}
	}
	else {
		cout << "settings->indCounter = " << settings->indCounter << endl;
		if (settings->startingCondition != settings->COND_LOAD_BEST) {
			if (settings->indCounter < ea->populationGenomes.size()) {
				float fitness = environment->fitnessFunction(currentMorphology);
				ea->populationGenomes[currentInd]->fitness = fitness;
				cout << "fitness = " << ea->populationGenomes[currentInd]->fitness << endl;
				if (settings->savePhenotype) {
					currentGenome->fitness = fitness;
					currentGenome->savePhenotype(settings->indCounter, settings->sceneNum);
				}
				ea->populationGenomes[currentInd]->morph->saveGenome(settings->indCounter, fitness);
				ea->populationGenomes[currentInd]->individualNumber = settings->indCounter;
				settings->indCounter++;
			}
			else if (settings->indCounter >= ea->populationGenomes.size()) {
				if (settings->evolutionType == settings->EA_NEAT) { // Exception // TODO proper integration
					currentMorphology = ea->getMorph();
				}
				float fitness = environment->fitnessFunction(currentMorphology);
				if (settings->evolutionType == settings->EA_NEAT) { // Exception // TODO proper integration
					ea->setFitness(-1,fitness);
				}
				else {
					currentGenome->fitness = fitness;
					if (settings->savePhenotype) {
						currentGenome->fitness = fitness;
						currentGenome->savePhenotype(settings->indCounter, settings->sceneNum);
					}
					// TODO set fitness
					ea->setFitness(settings->indCounter % ea->nextGenGenomes.size(), fitness);
					currentGenome->morph->saveGenome(settings->indCounter, fitness);
					cout << "FITNESS = " << fitness << endl;
					settings->indCounter++;
				}
			}
			if (settings->evolutionType != settings->EA_NEAT && settings->indCounter % ea->nextGenGenomes.size() == 0 && settings->indCounter != 0) {
				ea->replacement();// replaceNewIndividual(settings->indCounter, sceneNum, fitness);
				ea->selection();
				ea->savePopFitness(generation);
				generation++;
				saveSettings();
				newGenerations++;
			}
		}
		if (settings->startingCondition == settings->COND_LOAD_BEST) {
			float fitness = environment->fitnessFunction(currentMorphology);
			currentGenome.reset();
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

bool ER_VREP::loadIndividual(int individualNum)
{
	std::cout << "loading individual " << individualNum << ", sceneNum " << settings->sceneNum << endl;
	currentGenome = genomeFactory->createGenome(0, randNum, settings);
	// try to load from signal
	simInt signalLength = -1;
	simInt signalLengthVerify = -1;
	simChar* signal = simGetStringSignal("individualGenome", &signalLength);
	simInt retValue = simGetIntegerSignal("individualGenomeLenght", &signalLengthVerify);

	if (settings->verbose) {
		if (signal != nullptr && signalLength != signalLengthVerify) {
			std::cout << "genome received by signal, but length got corrupted, using file." << std::endl;
			std::cout << signalLength << " != " << signalLengthVerify << std::endl;
		}
		std::cout << "loading genome " << individualNum << " from ";
	}
	
	bool load = false;
	if (signal != nullptr && signalLength == signalLengthVerify)
	{
		// load from signal
		if (settings->verbose) {
			std::cout << " signal." << std::endl;
		}
		
		const std::string individualGenome((char*)signal, signalLength);
		std::istringstream individualGenomeStream(individualGenome);
		load = currentGenome->loadGenome(individualGenomeStream, individualNum);
	} 
	else
	{
		// load from file
		if (settings->verbose) {
			std::cout << " file." << std::endl;
		}
		if (settings->evolutionType == settings->EA_NEAT) {
			ea->loadPopulationGenomes();
			// ea->createIndividual(individualNum); // this actually sets the NEAT genome
			individualToBeLoaded = individualNum;
			load = true;
		}
		else {
			load = currentGenome->loadGenome(individualNum, settings->sceneNum);
		}
	}

	if (signal != nullptr) {
		simReleaseBuffer(signal);
	}
	
	currentGenome->individualNumber = individualNum;
	cout << "loaded" << endl;
	return load;
}


void ER_VREP::saveSettings()
{
	cout << "saving" << endl;
	settings->generation = generation;
	settings->individualCounter = settings->indCounter;
	vector<int> indNums;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		indNums.push_back(ea->populationGenomes[i]->individualNumber); // must be set when saving
	}
	settings->indNumbers = indNums;

	int bestInd = 0;
	int bestIndividual = 0;
	float bestFitness = 0;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		if (bestFitness < ea->populationGenomes[i]->fitness) {
			bestFitness = ea->populationGenomes[i]->fitness;
			bestInd = i;
			bestIndividual = ea->populationGenomes[bestInd]->individualNumber;
			cout << "Best individual has number " << ea->populationGenomes[bestInd]->individualNumber << endl;
		}
	}
	settings->bestIndividual = bestIndividual;
	settings->saveSettings();
	cout << "Settings saved" << endl;
}

void ER_VREP::loadBestIndividualGenome(int sceneNum)
{
	vector<int> individuals;
	vector<float> fitnessValues;

	ifstream file(settings->repository + "/SavedGenerations" + to_string(settings->sceneNum) + ".csv");
	if (file.good()) {
		cout << "saved generations file found" << endl;
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
			if (tmp == "ind: ") {
				it++;
				tmp = *it;
				individuals.push_back(atoi(tmp.c_str()));
			}
			else if (tmp == "fitness: ") {
				it++;
				tmp = *it;
				fitnessValues.push_back(atof(tmp.c_str()));
			}
		}
	}
	int bestInd = 0;
	float bestFit = 0.0;
	cout << "individuals size = " << individuals.size() << endl;
	for (int i = 0; i < individuals.size(); i++) {
		if (fitnessValues[i] >= bestFit) {
			bestFit = fitnessValues[i];
			bestInd = individuals[i];
		}
	}
	cout << "loading individual " << bestInd << ", sceneNum " << settings->sceneNum << endl;
	randNum->setSeed(settings->seed + bestInd * bestInd);

	currentGenome.reset();
	currentGenome = genomeFactory->createGenome(1, randNum, settings);
	currentGenome->init();	//	cout << "loading" << endl;
	currentGenome->loadGenome(bestInd, settings->sceneNum);
}
