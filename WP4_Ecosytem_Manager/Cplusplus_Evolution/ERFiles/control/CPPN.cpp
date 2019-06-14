#include "CPPN.h"

CPPN::CPPN()
{
	// init(2, 2, 3); // creates a feedforward network of five neurons
}


CPPN::~CPPN()
{
}

void CPPN::init(int input, int inter, int output) {
	unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
	// create CPPN neurons
	neuronID = 0;
	for (int i = 0; i < input; i++) {
		inputLayer.push_back(neuronFactory->createNewNeuronGenome(1, settings));
		inputLayer[i]->init(neuronID);
		neuronID++;
	} 
	for (int i = 0; i < inter; i++) {
		recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(4, settings)); // creates FunctionNeuron for CPPN
		recurrentLayer[i]->init(neuronID);
		neuronID++;
	}
	for (int i = 0; i < output; i++) {
		outputLayer.push_back(neuronFactory->createNewNeuronGenome(2, settings));
		outputLayer[i]->init(neuronID);
		neuronID++;
	}
	
	//inputLayer[0]->connectionsID.push_back(recurrentLayer[0]->neuronID);
	//inputLayer[1]->connectionsID.push_back(recurrentLayer[1]->neuronID);
	//recurrentLayer[0]->connectionsID.push_back(outputLayer[0]->neuronID);
	//recurrentLayer[1]->connectionsID.push_back(outputLayer[1]->neuronID);
	//recurrentLayer[0]->connectionsID.push_back(outputLayer[1]->neuronID);
	//recurrentLayer[1]->connectionsID.push_back(outputLayer[2]->neuronID);

	// randomized TODO
	int amountConnections = settings->initialAmountConnectionsNeurons; // the amount of connections the initial neurons have
	for (int i = 0; i < inputLayer.size(); i++) {
		for (int j = 0; j < amountConnections; j++) {
				inputLayer[i]->connectionsID.push_back(recurrentLayer[randomNum->randInt(recurrentLayer.size(), 0)]->neuronID);
				inputLayer[i]->connectionWeights.push_back(0.5);
			
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		for (int j = 0; j < amountConnections; j++) {
			if (randomNum->randFloat(0.0, 1.0) < 0.5) {
				recurrentLayer[i]->connectionsID.push_back(outputLayer[randomNum->randInt(outputLayer.size(), 0)]->neuronID);
				recurrentLayer[i]->connectionWeights.push_back(0.5);
			}
			else {
				recurrentLayer[i]->connectionsID.push_back(recurrentLayer[randomNum->randInt(recurrentLayer.size(), 0)]->neuronID);
				recurrentLayer[i]->connectionWeights.push_back(0.5);
			}
		}
	}
	
	// nonrandom: uncomment
	//for (int i = 0; i < inputLayer.size(); i++) {
	//	if (i < recurrentLayer.size()) {
	//		inputLayer[i]->connectionsID.push_back(recurrentLayer[i]->neuronID);
	//		inputLayer[i]->connectionWeights.push_back(1.0);
	//	}
	//	else {
	//		// 
	//		// recurrentLayer[i]->connectionsID.push_back(outputLayer[0]->neuronID);
	//		// recurrentLayer[i]->connectionWeights.push_back(1.0);
	//	}
	//}
	//for (int i = 0; i < recurrentLayer.size(); i++) {
	//	if (i < outputLayer.size()) {
	//		recurrentLayer[i]->connectionsID.push_back(outputLayer[i]->neuronID);
	//		recurrentLayer[i]->connectionWeights.push_back(1.0);
	//	}
	//	else {
	//		// 
	//		// recurrentLayer[i]->connectionsID.push_back(outputLayer[0]->neuronID);
	//		// recurrentLayer[i]->connectionWeights.push_back(1.0);
	//	}
	//}

	//	cout << "OUTPUTSIZE = " << outputLayer.size() << endl;
	changeConnectionIDToPointer();
	checkConnections();
	neuronFactory.reset();

	maxLayerSize = 100;
	maxCon = 20;

	mutate(0.5);
	
	changeConnectionIDToPointer();
	checkConnections();
	neuronFactory.reset();
}

void CPPN::mutate(float mutationRate) {
	for (int i = 0; i < inputLayer.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			inputLayer[i]->mutate(mutationRate);
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			recurrentLayer[i]->mutate(mutationRate);
		}
	}
	if (settings->verbose) {
		std::cout << "about to mutate cppn neurons" << std::endl;
	}
	mutateConnections(mutationRate);
	addNeurons(mutationRate);
	removeNeurons(mutationRate);

	changeConnectionIDToPointer();
	checkConnections();
}

void CPPN::addNeurons(float mutationRate) {
	for (int i = 0; i < settings->maxAddedNeurons; i++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			if (recurrentLayer.size() < maxLayerSize) {
				//		cout << "adding neuron" << endl;
				unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
				int neuronType = 4; // only create FunctionNeurons (type 4)
				recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(neuronType, settings));
				recurrentLayer[recurrentLayer.size() - 1]->init(neuronID);
				neuronID++;
				neuronFactory.reset();
				// add a single random connection from the neuron to output or recurrent layer. 
				if (randomNum->randFloat(0.0, 1.0) < 0.5) {
					recurrentLayer[recurrentLayer.size() - 1]->connectionsID.push_back(outputLayer[randomNum->randInt(outputLayer.size(), 0)]->neuronID);
				}
				else {
					recurrentLayer[recurrentLayer.size() - 1]->connectionsID.push_back(recurrentLayer[randomNum->randInt(recurrentLayer.size(), 0)]->neuronID);
				}
				recurrentLayer[recurrentLayer.size() - 1]->connectionWeights.push_back(randomNum->randFloat(-1.0, 1.0));
				// attach a random input neuron to this new recurrent neuron. 
				int randInput = randomNum->randInt(inputLayer.size(), 0);
				inputLayer[randInput]->connectionsID.push_back(recurrentLayer[recurrentLayer.size() - 1]->neuronID);
				inputLayer[randInput]->connectionWeights.push_back(randomNum->randFloat(0.0, 1.0));
				
				changeConnectionIDToPointer();
				checkConnections();
				//		cout << "added neuron" << endl;
			}
		}
	}
}

shared_ptr<Control> CPPN::clone() const {
	shared_ptr<CPPN> newANN = make_unique<CPPN>(*this);
	newANN->inputLayer.clear();
	newANN->recurrentLayer.clear();
	newANN->outputLayer.clear();
	for (int i = 0; i < this->inputLayer.size(); i++) {
		newANN->inputLayer.push_back(this->inputLayer[i]->clone());
	}
	for (int i = 0; i < this->recurrentLayer.size(); i++) {
		newANN->recurrentLayer.push_back(this->recurrentLayer[i]->clone());
	}
	for (int i = 0; i < this->outputLayer.size(); i++) {
		newANN->outputLayer.push_back(this->outputLayer[i]->clone());
	}
	//newANN->makeDependenciesUnique();
	newANN->checkConnections();
	newANN->changeConnectionIDToPointer();
	return newANN;
}

vector<float> CPPN::update(vector<float> inputs) {
	// flush

	// update
	vector<float> outputValues;
	for (int i = 0; i < inputs.size(); i++) {
		inputLayer[i]->input = 0;
		inputLayer[i]->input += inputs[i];
		inputLayer[i]->update();
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->update();
	}
	cf = 0.0;
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->update();
		outputValues.push_back(outputLayer[i]->output);
		cf += ((0.5 * outputLayer[i]->output / outputLayer.size()));
	}
	cf += 0.5;
	return outputValues;
}