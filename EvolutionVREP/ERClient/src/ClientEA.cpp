#include "ERClient/ClientEA.h"
// #define DO_NOT_USE_SHARED_MEMORY

using namespace are;
using namespace are::client;

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
            if (settings::getParameter<settings::Boolean>(parameters,"#killWhenNotConnected").value) {
				return false;
			}
			continue; // jump back to beginning loop
		}
	}
    std::function<EA::Factory> eaFactory;
    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;
    load_fct_exp_plugin<EA::Factory>(eaFactory,exp_plugin_name,"EAFactory");
    ea = eaFactory(randNum, parameters);
	ea->init();

	return true;
}

/*!
 * Only called once to initialize the EA
 * 
 */

void ClientEA::initGA()
{

    ea->init();

//    for (int i = 0; i < ea->nextGenGenomes.size(); i++) {
//        ea->nextGenGenomes[i]->init();
//        ea->nextGenGenomes[i]->morph->saveGenome(i, 0.0);
//        ea->nextGenGenomes[i]->individualNumber = i;
//    }

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

int ClientEA::finishConnections()
{
	for (auto &slave: serverInstances) {
		slave->disconnect();
	}
	extApi_sleepMs(100);
	return 0;
}

int ClientEA::openConnections()
{
	for (auto &slave: serverInstances) {
		std::cout << "Reconnecting to vrep on port " << slave->port() << std::endl;
		slave->connect(5000);
		slave->getIntegerSignalStreaming("simulationState");
	}
	extApi_sleepMs(100);
	return 0;
}

int ClientEA::reopenConnections()
{
	finishConnections();	
	openConnections();
	return 0;
}

bool ClientEA::confirmConnections()
{
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
		std::cerr << "One V-REP instance is faulty since I tried to connect to it for more than 1000 times." << std::endl;
			return false;
		}
	}

	std::cout << "Connections confirmed" << std::endl;
	return true;
}

bool ClientEA::evaluatePop()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool shouldReopenConnections = settings::getParameter<settings::Boolean>(parameters,"#shouldReopenConnections").value;
    int tries = 0;
	int pauseTime = 100; // milliseconds
	// communicate with all ports
    if (verbose) {
		std::cout << "number server instances = " << serverInstances.size() << std::endl;
	}
    if (shouldReopenConnections) {
		reopenConnections();
	}
	bool doneEvaluating = false;
	if (!confirmConnections()) {
		return false;
	}
	else {
		tries = 0;
	}

    if (verbose) {
		std::cout << "Pushing individuals" << std::endl;
	}

//	if (settings->evolutionType == settings->EA_NEAT) {
//		// use isEvaluated instead
//		for (int i = 0; i < ea->population->m_Species.size(); i++) {
//			for (int j = 0; j < ea->population->m_Species[i].m_Individuals.size(); j++) {
//				queuedInds.push_back(ea->population->m_Species[i].m_Individuals[j].GetID());
//			}
//		}
//		for (int i = 0; i < queuedInds.size(); i++) {
//            std::cout << queuedInds[i] << std::endl;
//		}
//	}
//	else {
		// following code does not work for NEAT
		// start by making adding individuals to the queue
    for(const auto & ind : ea->get_population())
    {
        queuedInds.push_back(ind->get_individual_id());
    }
//		for (int i = 0; i < ea->nextGenGenomes.size(); i++) {
//			queuedInds.push_back(i);
//		}
//	}
	int returnValue = -8; // value to see what's going on. 

    if (verbose) {
		std::cout << "Entering while with server size " << serverInstances.size() << std::endl;
	}

	while (serverInstances.size() > 0) {
        if (doneEvaluating) {
			break;
		}
		if (tries > loadingTrials) {
			std::cout << "I have told the slaves (server instances) too many times to load the genomes. If they don't want to do it, I'll kill them and myself." << std::endl;
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

			if (state == 9) { // Error loading genome
				extApi_sleepMs(20);
				std::cout << "It seems that server in port " << slave->port() << " could not load the genome. Sending it again. (" << slave->individualNum() << ")" << std::endl;
				//simxSetIntegerSignal(clientIDs[i], (simxChar*) "sceneNumber", 0, simx_opmode_blocking);
				slave->setIntegerSignal("individual", slave->individualNum());
                if (verbose) {
					std::cout << "returnV ind 9: " << returnValue << std::endl;
				}
				slave->setIntegerSignal("simulationState", 1); 
                if (verbose) {
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
                std::cout << "Trying to evaluate " << ind << std::endl;
//				if (settings->evolutionType == settings->EA_MULTINEAT) {
//					slave->setIndividualNum(ind);
//					slave->setIntegerSignal("individual", ind);
//					slave->setIntegerSignal("simulationState", 1);
//					slave->setState(SlaveConnection::State::EVALUATING);
//					std::cout << "evaluating: " << ind << ", num: " << ind << " of generation " << settings->generation << ", in port " << slave->port() << std::endl;

//				}
//				else {
					// now set the number in order to load the file properly.
//                int indNum = ea->nextGenGenomes[ind]->individualNumber;
//                slave->setIndividualNum(indNum);

                // tell simulator to start evaluating genome
//                if (settings::getParameter<settings::Boolean>(parameters,"#sendGenomeAsSignal").value) {
//                    std::cout << "sending genome as signal" << std::endl;
//                    const std::string individualGenome =
//                    sendGenomeSignal(slave, individualGenome);
//                }
                slave->setIntegerSignal("individual", ind);
                slave->setIntegerSignal("simulationState", 1);
                slave->setState(SlaveConnection::State::EVALUATING);
//                std::cout << "evaluating: " << ind << ", num: " << ind << " of generation " << settings->generation << ", in port " << slave->port() << std::endl;
//				}
			
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
//				if (settings->evolutionType == settings->EA_MULTINEAT) {
//					for (int i = 0; i < ea->population->m_Species.size(); i++) {
//						for (int j = 0;  j < ea->population->m_Species[i].m_Individuals.size(); j++) {
//							if (ea->population->m_Species[i].m_Individuals[j].GetID() == slave->individualNum()) {
//								ea->population->m_Species[i].m_Individuals[j].SetEvaluated();
//								ea->population->m_Species[i].m_Individuals[j].SetFitness(fitness);
//							}
//						}
//					}
//				}
//				else {
                    ea->get_population()[slave->individual()]->setFitness(fitness);
					// set the genome evaluation to true
                    ea->get_population()[slave->individual()]->set_isEvaluated(true);
//				}
				// set the simulation state back to 0 so this v-rep instance can receive another genome. 
				slave->setIntegerSignal("simulationState", 0);

				// Reset the individual numbers 
				slave->setIndividual(-1);
				slave->setIndividualNum(-1);
				slave->setState(SlaveConnection::State::FREE);
			}
		} /* end connections loop */

		// if all individual have been evaluated, break the while loop. 
//		if (settings->evolutionType == settings->EA_MULTINEAT) {
//			for (int i = 0; i < ea->population->m_Species.size(); i++) {
//				for (int j = 0; j < ea->population->m_Species[i].m_Individuals.size(); j++) {
//					if (ea->population->m_Species[i].m_Individuals[j].IsEvaluated() == false) {
//						doneEvaluating = false;
//						break; // should break out of both loops
//					}
//					else {
//						doneEvaluating = true;
//					}
//				}
//				if (doneEvaluating == false) {
//					break; // breaks out of the species loop as well
//				}
//			}
//		}
//		else{
            for (int z = 0; z < ea->get_population().size(); z++) {
                if (not ea->get_population()[z]->isEvaluated()) {
					doneEvaluating = false;
					break;
				}
				else {
					doneEvaluating = true;
				}
			}
//		}
	}
    if (verbose) {
		std::cout << "Out of while loop" << std::endl;
	}
	ea->replacement();
    if (verbose) {
		std::cout << "Just saved pop fitness " << std::endl;
	}
	// save settings after replacement. 
//	settings->indNumbers.clear();
//	settings->indFits.clear();
//	if (settings->evolutionType == settings->EA_MULTINEAT) {
//		for (int i = 0; i < ea->population->m_Species.size(); i++) {
//			for (int j = 0; j < ea->population->m_Species[i].m_Individuals.size(); j++) {
//				settings->indNumbers.push_back(ea->population->m_Species[i].m_Individuals[j].GetID());
//				settings->indFits.push_back(ea->population->m_Species[i].m_Individuals[j].GetFitness());
//			}
//		}
//	}
//	else {
        for (int i = 0; i < ea->get_population().size(); i++) {
            if (verbose) {
                std::cout << "nr of " << i << " is "
                          << ea->get_population()[i]->get_individual_id() << std::endl;
                std::cout << "fitness of " << i << " is "
                          << ea->get_population()[i]->getFitness() << std::endl;
			}
            properties->indNumbers.push_back(ea->get_population()[i]->get_individual_id());
            properties->indFits.push_back(ea->get_population()[i]->getFitness());
		}
		//settings->indFits = ea->popFitness;
//	}
    if (verbose) {
		std::cout << "Just before saving settings " << std::endl;
	}

//	else {
//		ea->savePopFitness(settings->generation);
//	}
//	settings->saveSettings();
//    std::cout << "saved settings " << std::endl;
	return true;
}
