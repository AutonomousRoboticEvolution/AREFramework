#include "ClientEA.h"
// #define DO_NOT_USE_SHARED_MEMORY

void sendGenomeSignal(std::unique_ptr<SlaveConnection> &slave, const std::string &individualGenome)
{
	simxInt individualGenomeLength = individualGenome.size();
	slave->setStringSignal("individualGenome", individualGenome);
	slave->setIntegerSignal("individualGenomeLenght", individualGenomeLength);
}

ClientEA::ClientEA()
{}

ClientEA::~ClientEA()
{}

bool ClientEA::init(int amountPorts, int startPort)
{
	// initialize serverInstances
	for (int i = 0; i < amountPorts; i++) {
		auto new_slave = std::unique_ptr<SlaveConnection>(new SlaveConnection("127.0.0.1", i + startPort));
		std::cout << "Connecting to vrep on port " << new_slave->port() << std::endl;
		if (new_slave->connect(5000)) {
			// new_slave->setState(SlaveConnection::State::FREE);
			new_slave->getIntegerSignalStreaming("simulationState");
			serverInstances.push_back(std::move(new_slave));
		} else {
			std::cerr << "Could not connect to V-REP on port " << new_slave->port() << std::endl;
			if (settings->killWhenNotConnected) {
				return false;
			}
			continue; // jump back to beginning loop
		}
	}
	ea = shared_ptr<EA>(new EA_SteadyState());
	ea->setSettings(settings, randNum);
	ea->init();
	return true;
}

/*!
 * Only called once to initialize the EA
 * 
 */

void ClientEA::initGA() {
	// init GA
	for (int i = 0; i < ea->nextGenGenomes.size(); i++) {
		ea->nextGenGenomes[i]->init();
		ea->nextGenGenomes[i]->morph->saveGenome(i, 0.0);
		ea->nextGenGenomes[i]->individualNumber = i;
	}
	extApi_sleepMs(500);
}

void ClientEA::quitSimulators()
{
	for (auto &slave: serverInstances) {
		std::cout << "closing simulator " << slave->port() << std::endl;
		slave->setIntegerSignal("simulationState", 99);
		slave->disconnect();
	}
}

int ClientEA::finishConnections() {
	for (auto &slave: serverInstances) {
		slave->disconnect();
	}
	extApi_sleepMs(100);
	return 0;
}

int ClientEA::openConnections() {
	for (auto &slave: serverInstances) {
		std::cout << "Reconnecting to vrep on port " << slave->port() << std::endl;
		slave->connect(5000);
		slave->getIntegerSignalStreaming("simulationState");
	}
	extApi_sleepMs(100);
	return 0;
}

int ClientEA::reopenConnections() {
	finishConnections();	
	openConnections();
	return 0;
}

bool ClientEA::confirmConnections() {
	int tries = 0;
	for (auto &slave: serverInstances) {
		if (!slave->status()) {
			std::cerr << "Client could not connect to server in port, trying again " << slave->port() << std::endl;
			tries++;
			extApi_sleepMs(20);
		}
		else {
			continue;
		}
		if (tries > loadingTrials) {
		std:cerr << "One V-REP instance is faulty since I tried to connect to it for more than 1000 times." << std::endl;
			return false;
		}
	}

	std::cout << "Connections confirmed" << std::endl;
	return true;
}

bool ClientEA::evaluatePop() {
	int tries = 0;
	int pauseTime = 100; // milliseconds
	// communicate with all ports
	if (settings->verbose) {
		std::cout << "number server instances = " << serverInstances.size() << std::endl;
	}
	if (settings->shouldReopenConnections) {
		reopenConnections();
	}
	bool doneEvaluating = false;
	if (!confirmConnections()) {
		return false;
	}
	else {
		tries = 0;
	}

	if (settings->verbose) {
		std::cout << "Pushing individuals" << endl;
	}

	// start by making adding individuals to the queue
	for (int i = 0; i < ea->nextGenGenomes.size(); i++) {
		queuedInds.push_back(i);
	}
	int returnValue = -8; // value to see what's going on. 

	if (settings->verbose) {
		std::cout << "Entering while with server size " << serverInstances.size() << endl;
	}

	while (serverInstances.size() > 0) {
		if (doneEvaluating == true) {
			break;
		}
		if (tries > loadingTrials) {
			std::cout << "I have told the slaves (server instances) too many times to load the genomes. If they don't want to do it, I'll kill them and myself." << endl;
			quitSimulators();
			return false;
		}
		// loop through all servers and send them queued individuals
		for (auto &slave: serverInstances) {
			SlaveConnection::State slave_client_state = slave->state();
			if (slave_client_state == SlaveConnection::State::DELAY) {
				// try again in 100 ms. 
				extApi_sleepMs(pauseTime);
				continue;
			}

			int state;
			state = slave->getIntegerSignalBuffer("simulationState");

			//try {
			// state = slave->getIntegerSignalBuffer("simulationState");
			//} catch (const VrepRemoteException &e) {
			//	if (e.returnValue & simx_return_novalue_flag) {
			//		// buffer not yet ready, continue
			//		// continue;
			//	} else {
			//		// something else happened, re-throw.
			//		throw;
			//	}
			//}

			if (state == 9) {
				extApi_sleepMs(20);
				std::cout << "It seems that server in port " << slave->port() << " could not load the genome. Sending it again. (" << slave->individualNum() << ")" << std::endl;
				//simxSetIntegerSignal(clientIDs[i], (simxChar*) "sceneNumber", 0, simx_opmode_blocking);
				slave->setIntegerSignal("individual", slave->individualNum());
				if (settings->verbose) {
					std::cout << "returnV ind 9: " << returnValue << std::endl;
				}
				slave->setIntegerSignal("simulationState", 1); 
				if (settings->verbose) {
					std::cout << "returnV state 9: " << returnValue << std::endl;
				}
				tries++;
			}
			if (state == 0 && slave->state() == SlaveConnection::State::FREE && queuedInds.size() > 0) {
				// state 0 means that the client can send a new individual to the server. 
				// the individual number of the serverInstance is set to the last queued individual and the vector is popped. 
				
				int ind = queuedInds.back();
				queuedInds.pop_back(); 
				slave->setIndividual(ind);

				// now set the number in order to load the file properly.
				int indNum = ea->nextGenGenomes[ind]->individualNumber;
				slave->setIndividualNum(indNum);

				// tell simulator to start evaluating genome
				if (settings->sendGenomeAsSignal) {
					const std::string individualGenome = ea->nextGenGenomes[ind]->generateGenome();
					sendGenomeSignal(slave, individualGenome);
				}
				slave->setIntegerSignal("individual", indNum);
				slave->setIntegerSignal("simulationState", 1);
				slave->setState(SlaveConnection::State::EVALUATING);
				std::cout << "evaluating: " << ind << ", num: " << indNum << " of generation " << settings->generation << ", in port " << slave->port() << std::endl;
			}
			else if (state == 2 && slave->state() == SlaveConnection::State::EVALUATING)
			{
				// This combination signifies that an individual has been evaluated
				// First retrieve values. (phenValue is not used in this case but will be used in near future experiments
				float phenValue = slave->getFloatSignal("phenValue");
				float fitness = slave->getFloatSignal("fitness");
				std::cout << "The fitness of individual " << slave->individualNum() << " is " << fitness << std::endl;

				// set the individual fitness in the ea :: TODO: set phenotype value as well
				// Make a buffer vector for nextGenGenome and don't switch between the two like I do now. 
				ea->nextGenGenomes[slave->individual()]->fitness = fitness;
				
				// set the simulation state back to 0 so this v-rep instance can receive another genome. 
				slave->setIntegerSignal("simulationState", 0);

				// set the genome evaluation to true
				ea->nextGenGenomes[slave->individual()]->isEvaluated = true;

				// Reset the individual numbers 
				slave->setIndividual(-1);
				slave->setIndividualNum(-1);
				slave->setState(SlaveConnection::State::FREE);
			}
		} /* end connections loop */

		// if all individual have been evaluated, break the while loop. 
		for (int z = 0; z < ea->nextGenGenomes.size(); z++) {
			if (ea->nextGenGenomes[z]->isEvaluated == false) {
				doneEvaluating = false;
				break;
			}
			else {
				doneEvaluating = true; 
			}
		}
	}
	if (settings->verbose) {
		std::cout << "Out of while loop" << std::endl;
	}
	ea->replacement();
	ea->savePopFitness(settings->generation);
	if (settings->verbose) {
		std::cout << "Just saved pop fitness " << std::endl;
	}
	// save settings after replacement. 
	if (settings->indNumbers.size() < 1) {
		settings->indNumbers.resize(ea->populationGenomes.size());
		settings->indFits.resize(ea->populationGenomes.size());
	}
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		settings->indNumbers[i] = ea->populationGenomes[i]->individualNumber;
		settings->indFits[i] = ea->populationGenomes[i]->fitness;
	}
	//settings->indFits = ea->popFitness;
	if (settings->verbose) {
		std::cout << "Just before saving settings " << std::endl;
	}
	settings->saveSettings();
	return true;
}
