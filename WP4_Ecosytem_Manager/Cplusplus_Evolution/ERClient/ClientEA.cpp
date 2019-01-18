#include "ClientEA.h"
// #define DO_NOT_USE_SHARED_MEMORY

ClientEA::ClientEA()
{

}

ClientEA::~ClientEA()
{
}

void ClientEA::init(int amountPorts)
{
	for (int i = 0; i < amountPorts; i++) {
		ports.push_back(i + 104000 +1);
	}
	int amPorts = ports.size();
	simxFinish(-1);
	for (int i = 0; i < ports.size(); i++) {
		clientIDs.push_back(simxStart("127.0.0.1", ports[i], true, true, 5000, 5));
		if (clientIDs[i] == -1) {
			cout << "Port " << ports[i] << " could not be opened" << endl;
			ports.erase(ports.begin() + i);
			clientIDs.erase(clientIDs.begin() + i);
			i--;
		}
		else {
			cout << "Connected to port " << ports[i] << endl;
			portState.push_back(0);
			portIndividual.push_back(-1); // this checks which individual the specific port is evaluating
		}
	}
	randNum = shared_ptr<RandNum>(new RandNum(settings->seed));
	// TODO: use factory
	// ea_fac = new EA_Factory();

	ea = shared_ptr<EA>(new EA_SteadyState());
	ea->setSettings(settings, randNum);
	ea->init();
}

void ClientEA::initGA() {
	// init GA
	queuedInds;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		// ea->initializePopulation();
		// ea->initializeIndividual(i);
		ea->populationGenomes[i]->init();
		ea->populationGenomes[i]->morph->saveGenome(indCounter, 0, -1);
		ea->populationGenomes[i]->individualNumber = i;
		ea->popIndNumbers.push_back(i);
		shared_ptr<IND> nIND = shared_ptr<IND>(new IND);
		nIND->nr = i;
		nIND->sceneNr = 0;
		queuedInds.push_back(nIND);
		indCounter++;
	}
	extApi_sleepMs(500);
}

void ClientEA::evaluateNextGen()
{
	// first connect to port again
	int amPorts = ports.size();
	for (int i = 0; i < ports.size(); i++) {
		clientIDs.push_back(simxStart(ipNum, ports[i], true, true, 2000, 5));
		if (clientIDs[i] == -1) {
			cout << "Port " << ports[i] << " could not be opened" << endl;
			ports.erase(ports.begin() + i);
			clientIDs.erase(clientIDs.begin() + i);
			i--;
		}
		else {
			cout << "Connected to port " << ports[i] << endl;
			portState.push_back(0);
			portIndividual.push_back(-1); // this checks which individual the specific port is evaluating
		}
	}
	if (ports.size() == 0) {
		cout << "ERROR, PORT SIZE WAS ZERO, QUITTING EA" << endl;
		exit(0);
	}

	// create new genomes
	ea->settings->indCounter = indCounter;
	ea->selection();
	// assign number to new genomes 
	indCounter += ea->populationGenomes.size();
	ea->settings->indCounter = indCounter;
	//indCounter += ea->populationGenomes.size();
	extApi_sleepMs(100);
	// communicate with all ports
	int currentEv = 0;
	bool doneEvaluating = false;
	while (ports.size() > 0) {
		if (doneEvaluating == true) {
			break;
		}
		for (int i = 0; i < ports.size(); i++) {
			if (simxGetConnectionId(clientIDs[i]) != -1)
			{
				if (clientIDs[i] != -1)
				{
					int state[1];
					simxGetIntegerSignal(clientIDs[i], "simulationState", state, simx_opmode_oneshot);
					//	cout << state[0] << endl;
					if (state[0] == 0 && portState[i] == 0 && currentEv < ea->populationGenomes.size()) {
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "sceneNumber", 0, simx_opmode_oneshot);
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "individual", ea->nextGenGenomes[currentEv]->individualNumber, simx_opmode_oneshot);
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "simulationState", 1, simx_opmode_oneshot);
						cout << "evaluating:  " << currentEv << " in port " << i + 1040000 <<" num: " << ea->nextGenGenomes[currentEv]->individualNumber <<  endl;
						portIndividual[i] = currentEv;
						// tells the simulator to start evaluating the genome
						//	cout << "setting integer signal" << endl;
						currentEv++;
						portState[i] = 1;
					}
					else if (state[0] == 2 && portState[i] == 1)
					{
						portState[i] = 0;
						float fitness[1] = { 0.0 };
						float phenValue[1] = { 0.0 };
						simxGetFloatSignal(clientIDs[i], "phenValue", phenValue, simx_opmode_blocking);
						simxGetFloatSignal(clientIDs[i], "fitness", fitness, simx_opmode_blocking);
						cout << "fitness of individual " << portIndividual[i] << " was " << fitness[0] << endl;
			//			cout << "phenValue = " << phenValue[0] << endl;
						// ea->nextGenFitness[portIndividual[i]] = fitness[0];
						ea->nextGenGenomes[portIndividual[i]]->fitness = fitness[0];
						ea->nextGenGenomes[portIndividual[i]]->morph->phenValue = phenValue[0];
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "simulationState", 0, simx_opmode_oneshot);
						ea->nextGenGenomes[portIndividual[i]]->isEvaluated = true;
						portIndividual[i] = -1;
						for (int z = 0; z < ea->nextGenGenomes.size(); z++) {
							if (ea->nextGenGenomes[z]->isEvaluated != true) {
								doneEvaluating = false;
								break;
							}
							else {
								doneEvaluating = true; /* if all individuals have been evaluated,
													   * set doneEvaluating to true
													   */
							}
						}
					}
					//	simxStopSimulation(clientIDs[i], simx_opmode_oneshot);
					extApi_sleepMs(2);
				}
				else {
					cout << "huh, lost connection with " << clientIDs[i] << endl;
				}
			}
			else {
				cout << "Connection lost with API " << ports[i] << ", terminating remote API" << endl;
				ports.erase(ports.begin() + i);
				simxFinish(clientIDs[i]);
				clientIDs.erase(clientIDs.begin() + i);
				portIndividual.erase(portIndividual.begin() + i);
				portState.erase(portState.begin() + i);
				i--;
				extApi_sleepMs(2000);
				break;
			}
		}
		extApi_sleepMs(5);
	}
	if (settings->replacementType == settings->RANDOM_REPLACEMENT) {
		// I don't think this is used?
		ea->replacement();
		ea->savePopFitness(settings->generation);
	}
	//else if (settings->replacementType == settings->PARETOMORPH_REPLACEMENT) {
	//	ea->replaceNewPopPareto(indCounter, sceneNum, 5);
	//}

}

void ClientEA::quitSimulators()
{
	for (int i = 0; i < clientIDs.size(); i++) {
		cout << "closing simulator " << ports[i] << endl;
		simxSetIntegerSignal(clientIDs[i], (simxChar*) "simulationState", 99, simx_opmode_oneshot);
	}
}

void ClientEA::evaluateInitialPop()
{
	// communicate with all ports
	int currentEv = 0;
	bool doneEvaluating = false;
	while (ports.size() > 0) {
		if (doneEvaluating == true) {
			break;
		}
		for (int i = 0; i < ports.size(); i++) {
			if (simxGetConnectionId(clientIDs[i]) != -1)
			{
				if (clientIDs[i] != -1)
				{
					int state[1];
					simxGetIntegerSignal(clientIDs[i], "simulationState", state, simx_opmode_oneshot);
					//	cout << state[0] << endl;
					if (state[0] == 0 && portState[i] == 0 && currentEv < ea->populationGenomes.size()) {
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "sceneNumber", 0, simx_opmode_oneshot);
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "individual", currentEv, simx_opmode_blocking);
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "simulationState", 1, simx_opmode_oneshot);
						cout << "evaluating:  " << currentEv << " in port " << i << endl;
						portIndividual[i] = currentEv;
						// tells the simulator to start evaluating the genome
						//	cout << "setting integer signal" << endl;
						currentEv++;
						portState[i] = 1;
					}
					else if (state[0] == 2 && portState[i] == 1)
					{
						portState[i] = 0;
						float fitness[1];
						simxGetFloatSignal(clientIDs[i], "fitness", fitness, simx_opmode_blocking);
						cout << "fitness of individual " << portIndividual[i] << " was " << fitness[0] << endl;
						// ea->popFitness[portIndividual[i]] = fitness[0];
						ea->populationGenomes[portIndividual[i]]->fitness = fitness[0];
						ea->populationGenomes[portIndividual[i]]->morph->saveGenome(portIndividual[i], sceneNum, fitness[0]);
						ea->populationGenomes[portIndividual[i]]->isEvaluated = true;
						portIndividual[i] = -1;
						simxSetIntegerSignal(clientIDs[i], (simxChar*) "simulationState", 0, simx_opmode_oneshot);
						for (int z = 0; z < ea->populationGenomes.size(); z++) {
							if (ea->populationGenomes[z]->isEvaluated == false) {
								doneEvaluating = false;
								break;
							}
							else {
								doneEvaluating = true; /* if all individuals have been evaluated, 
														* set doneEvaluating to true
														*/
							}
						}
					}
					//	simxStopSimulation(clientIDs[i], simx_opmode_oneshot);
					extApi_sleepMs(5);
				}
			}
			else {
				cout << "Connection lost with API " << ports[i] << ", terminating remote API" << endl;
				ports.erase(ports.begin() + i);
				simxFinish(clientIDs[i]);
				clientIDs.erase(clientIDs.begin() + i);
				portIndividual.erase(portIndividual.begin() + i);
				portState.erase(portState.begin() + i);
				i--;
				extApi_sleepMs(2000);
				break;
			}

		}
		extApi_sleepMs(5);
	}
	ea->savePopFitness(0);
	settings->indNumbers = ea->popIndNumbers;
	//settings->indFits = ea->popFitness;
	settings->saveSettings();
}

void ClientEA::createNextGenGenomes()
{
	ea->selection(); 
	for (int i = 0; i < ea->nextGenGenomes.size(); i++) {
		ea->nextGenGenomes[i]->morph->saveGenome(indCounter, 0, -1);
		indCounter++;
	}
}


