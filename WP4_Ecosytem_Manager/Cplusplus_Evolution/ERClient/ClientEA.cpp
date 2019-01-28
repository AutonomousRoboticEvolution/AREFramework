#include "ClientEA.h"
// #define DO_NOT_USE_SHARED_MEMORY

void sendGenomeSignal(simxInt clientID, const std::string &individualGenome)
{
	simxInt individualGenomeLength = individualGenome.size();
	simxSetStringSignal(clientID, (simxChar*) "individualGenome", (simxUChar*) individualGenome.c_str(), individualGenomeLength, simx_opmode_blocking);
	simxSetIntegerSignal(clientID, (simxChar*) "individualGenomeLenght", individualGenomeLength, simx_opmode_blocking);
}

ClientEA::ClientEA()
{}

ClientEA::~ClientEA()
{}

bool ClientEA::init(int amountPorts)
{
	// initialize serverInstances
	for (int i = 0; i < amountPorts; i++) {
		serverInstances.push_back(unique_ptr<ServerInstance>(new ServerInstance({i + 104000, -1,-1,-1 })));
		std::cout << "Connecting to vrep on port " << serverInstances.back()->portNum << std::endl;
		int new_connection = simxStart("127.0.0.1", serverInstances.back()->portNum, true, true, 5000, 5);
		if (new_connection == -1) {
			std::cerr << "Could not connect to V-REP on port " << serverInstances[serverInstances.size() - 1]->portNum << std::endl;
			serverInstances.pop_back();// erase(serverInstances.begin() + i);
			if (settings->killWhenNotConnected) {
				return false;
			}
			continue; // jump back to beginning loop
		}
		serverInstances.back()->port = new_connection;
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
	// std::cout << "closing " << clientIDs.size() << " simulators" << std::endl;
	// std::cout << "closing " << ports.size() << " simulators" << std::endl;
	for (int i = 0; i < serverInstances.size(); i++) {
		std::cout << "closing simulator " << serverInstances[i]->port << std::endl;
		simxSetIntegerSignal(serverInstances[i]->port, (simxChar*) "simulationState", 99, simx_opmode_blocking);
		simxFinish(serverInstances[i]->port);
	}
}

int ClientEA::finishConnections() {
	for (int i = 0; i < serverInstances.size(); i++) {
		simxFinish(serverInstances[i]->port);
	}
	extApi_sleepMs(100);
	return 0;
}

int ClientEA::openConnections() {
	for (int i = 0; i < serverInstances.size(); i++) {
		std::cout << "Reconnecting to vrep on port " << serverInstances[i]->portNum << std::endl;
		int new_connection = simxStart("127.0.0.1", serverInstances[i]->portNum, true, true, 5000, 5);
		serverInstances[i]->port = new_connection;
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
	for (int i = 0; i < serverInstances.size(); i++) {
		if (simxGetConnectionId(serverInstances[i]->port) == -1) {
			std::cerr << "Client could not connect to server in port, trying again " << serverInstances[i]->port << std::endl;
			tries++;
			i--;
			extApi_sleepMs(10);
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
		for (int i = 0; i < serverInstances.size(); i++) {
			if (settings->verbose) {
				std::cout << "i:" << i << ",";
			}
			int state;
			if (serverInstances[i]->state == 10 || serverInstances[i]->state == 11) { // 10 is now the delay state 
				if (serverInstances[i]->state == 10) {
					// try again in 100 ms. 
					extApi_sleepMs(pauseTime);
					serverInstances[i]->state = 0;
					continue;
				}
				else if (serverInstances[i]->state == 11) { // 10 is now the delay state 
					// try again in 25 ms. 
					extApi_sleepMs(25);
					serverInstances[i]->state = 0;
					continue;
				}
			}
			else {
				returnValue = simxGetIntegerSignal(serverInstances[i]->port, "simulationState", &state, simx_opmode_oneshot);
			}
			if (settings->verbose) {
				// Yeah sorry, if you have verbose on, this we be spammed. 
				std::cout << "rs: " << returnValue << "," << std::endl;
			}
			// Some debugging code
			if (returnValue != 0) { // 0 is ok, 1 is fine
				// TODO: the returnValues can actually be a combination of all of the following. Not sure how to encode this. 
				if (settings->verbose) {
					std::cout << "Return value of simxGetIntegerSignal was " << returnValue << std::endl;
				}
				if (returnValue >= 3) {
					std::cerr << "Note, the simxGetIntegerSignal function returned " << returnValue << std::endl;
					std::cerr << "If this message persists, the client is unable to get the simulation state from vrep" << std::endl;
					serverInstances[i]->state = 10;
				}
				else if (returnValue != 0) {
					// short pause because this return value is not as bad as the others. 
					serverInstances[i]->state = 11;
				}
				if (returnValue >= 8) {
					tries++;
				}
				if (returnValue == 64) {
					std::cerr << "Return value of simxGetIntegerSignal was 64, meaning that simxStart was not yet called. This was on port: " << serverInstances[i]->portNum << std::endl;
				}
				else if (returnValue == 32) {
					std::cerr << "Return value of simxGetIntegerSignal was 32, meaning that there is an error no the client side???. This was on port: " << serverInstances[i]->portNum << std::endl;
				}
				else if (returnValue == 16) {
					std::cerr << "The server is still processing a previous command of the same type. Don't send it again please. " << serverInstances[i]->portNum << std::endl;
					serverInstances[i]->state = 10;
					// It will loop through all the other server instances first and then start communicating with the faulty one again. 
				}
				else if (returnValue == 8) {
					std::cerr << "The simxGetIntegerSignal cause an error on the server side... I guess I should kill this damn server?" << std::endl;
					std::cerr << "For now, I'll try to get a message again after a brief pause." << std::endl;
					serverInstances[i]->state = 10;
				}
				else if (returnValue == 4) {
					std::cerr << "The operation mode for receiving the signal is not supported??? simxGetIntegerSignal is the culprit. Probably linux related issue " << std::endl;
				}
				else if (returnValue == 2) {
					std::cerr << "simxGetIntegerSignal was 8, time out of function at node " << serverInstances[i]->portNum << std::endl;
					std::cerr << "Trying to receive the signal again in 100 ms" << std::endl;
					serverInstances[i]->state = 10;
				}
				if (tries > 100) {
					pauseTime = 250;
				}
				else {
					pauseTime = 25;
				}
			}

			if (state == 9) {
				extApi_sleepMs(20);
				std::cout << "It seems that server in port " << serverInstances[i]->port << " could not load the genome. Sending it again. (" << serverInstances[i]->individualNum << ")" << std::endl;
				//simxSetIntegerSignal(clientIDs[i], (simxChar*) "sceneNumber", 0, simx_opmode_oneshot);
				returnValue = simxSetIntegerSignal(serverInstances[i]->port, (simxChar*) "individual", serverInstances[i]->individualNum, simx_opmode_oneshot);
				if (settings->verbose) {
					std::cout << "returnV ind 9: " << returnValue << std::endl;
				}
				returnValue = simxSetIntegerSignal(serverInstances[i]->port, (simxChar*) "simulationState", 1, simx_opmode_blocking); 
				if (settings->verbose) {
					std::cout << "returnV state 9: " << returnValue << std::endl;
				}
				tries++;
			}
			if (state == 0 && serverInstances[i]->state == 0 && queuedInds.size() > 0) {
				// state 0 means that the client can send a new individual to the server. 
				// the individual number of the serverInstance is set to the last queued individual and the vector is popped. 
				
				int ind = queuedInds.back();
				queuedInds.pop_back(); 
				serverInstances[i]->individual = ind;

				// now set the number in order to load the file properly.
				int indNum = -1;
				indNum = ea->nextGenGenomes[ind]->individualNumber;
				serverInstances[i]->individualNum = indNum;

				// tell simulator to start evaluating genome
				if (settings->sendGenomeAsSignal) {
					const std::string individualGenome = ea->nextGenGenomes[ind]->generateGenome();
					sendGenomeSignal(serverInstances[i]->port, individualGenome);
				}
				returnValue = simxSetIntegerSignal(serverInstances[i]->port, (simxChar*) "individual", indNum, simx_opmode_oneshot);
				if (settings->verbose) {
					std::cout << "returnV ind: " << returnValue << std::endl;
				}

				returnValue = simxSetIntegerSignal(serverInstances[i]->port, (simxChar*) "simulationState", 1, simx_opmode_blocking);
				if (settings->verbose) {
					std::cout << "returnV state set: " << returnValue << std::endl;
				}

				serverInstances[i]->state = 1;
				std::cout << "evaluating: " << ind << ", num: " << indNum << " of generation " << settings->generation << ", in port " << i << std::endl;
			}
			else if (state == 2 && serverInstances[i]->state == 1)
			{
				// This combination signifies that an individual has been evaluated
				// First retrieve values. (phenValue is not used in this case but will be used in near future experiments
				float fitness;
				float phenValue;
				returnValue = simxGetFloatSignal(serverInstances[i]->port, "phenValue", &phenValue, simx_opmode_oneshot);
				if (settings->verbose) {
					std::cout << "returnV phen: " << returnValue << std::endl;
				}

				returnValue = simxGetFloatSignal(serverInstances[i]->port, "fitness", &fitness, simx_opmode_blocking);
				std::cout << "The fitness of individual " << serverInstances[i]->individualNum << " is " << fitness << std::endl;
				if (settings->verbose) {
					std::cout << "returnV fit: " << returnValue << std::endl;
				}


				// set the individual fitness in the ea :: TODO: set phenotype value as well
				// Make a buffer vector for nextGenGenome and don't switch between the two like I do now. 
				ea->nextGenGenomes[serverInstances[i]->individual]->fitness = fitness;
				
				// set the simulation state back to 0 so this v-rep instance can receive another genome. 
				returnValue = simxSetIntegerSignal(serverInstances[i]->port, (simxChar*) "simulationState", 0, simx_opmode_blocking);
				if (settings->verbose) {
					std::cout << "returnV state done : " << returnValue << std::endl;
				}

				// set the genome evaluation to true
				ea->nextGenGenomes[serverInstances[i]->individual]->isEvaluated = true;

				// Reset the individual numbers 
				serverInstances[i]->individual = -1;
				serverInstances[i]->individualNum = -1;
				serverInstances[i]->state = 0;
			}
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
	}
	if (settings->verbose) {
		std::cout << "Out of while loop" << std::endl;
	}
	ea->replacement();
	ea->savePopFitness(settings->generation);
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
	settings->saveSettings();
	return true;
}



