#include "ANN.h"
#include <iostream>


ANN::ANN()
{
	std::cout << "creating ANN" << endl; 
	init(1,1,1); 
	printNeuronValues();
}


ANN::~ANN()
{
	std::cout << "deleting ANN" << endl; 
}

void ANN::init(int input, int inter, int output) {
	NeuronFactory *neuronFactory = new NeuronFactory;
	neuronID = 0;
	for (int i = 0; i < input; i++) {
		inputLayer.push_back(neuronFactory->createNewNeuronGenome(1));
		inputLayer[i]->init(neuronID);
		neuronID++;
	}
	if (type == 0) { // change this 
		for (int i = 0; i < inter; i++) {
			recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(3)); // creates CPG
			recurrentLayer[i]->init(neuronID);
			neuronID++;
		}
	}
	else {
		for (int i = 0; i < inter; i++) {
			recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(0)); // creates simpleNeuron
			recurrentLayer[i]->init(neuronID);
			neuronID++;
		}
	}
	for (int i = 0; i < output; i++) {
		outputLayer.push_back(neuronFactory->createNewNeuronGenome(2));
		outputLayer[i]->init(neuronID);
		neuronID++;
	}
	//allNeurons.insert(allNeurons.end(), inputLayer.begin(), inputLayer.end());
	//allNeurons.insert(allNeurons.end(), recurrentLayer.begin(), recurrentLayer.end());
	//allNeurons.insert(allNeurons.end(), outputLayer.begin(), outputLayer.end());
//	inputLayer[0]->connections.push_back(recurrentLayer[0]);
	inputLayer[0]->connectionsID.push_back(recurrentLayer[0]->neuronID);
//	recurrentLayer[0]->connections.push_back(outputLayer[0]);
	recurrentLayer[0]->connectionsID.push_back(outputLayer[0]->neuronID);
	checkConnections();
	changeConnectionIDToPointer();
	delete neuronFactory;
}

void ANN::checkConnections() { // this function deletes the connections to deleted neurons and in turn the pointer to these neurons should go out of scope. 
	for (int i = 0; i < inputLayer.size(); i++) {
		for (int j = 0; j < inputLayer[i]->connections.size(); j++) {
			if (inputLayer[i]->connections[j]->neuronID == -1) {
				inputLayer[i]->connections[j].reset();
			//	inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + j);
			}
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		for (int j = 0; j < recurrentLayer[i]->connections.size(); j++) {
			if (recurrentLayer[i]->connections[j]->neuronID == -1) {
				recurrentLayer[i]->connections[j].reset();
				recurrentLayer[i]->connections.erase(recurrentLayer[i]->connections.begin() + j);
			}
		}
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		for (int j = 0; j < outputLayer[i]->connections.size(); j++) {
			if (outputLayer[i]->connections[j]->neuronID == -1) {
				outputLayer[i]->connections[j].reset();
				outputLayer[i]->connections.erase(outputLayer[i]->connections.begin() + j);
			}
		}
	}

}

vector<float> ANN::update(vector<float> sensorValues) {
	vector<float> outputValues; 
	if (sensorValues.size() != inputLayer.size()) {
		std::cout << "ERROR: sensor amount differs from input neuron amount" << endl;
	}
	else {
		for (int i = 0; i < sensorValues.size(); i++) {
			inputLayer[i]->input = sensorValues[i];
			inputLayer[i]->update();
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->update();
	//	cout << "recurrent neuron output = " << recurrentLayer[i]->output << endl;
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->update();
		outputValues.push_back(outputLayer[i]->output);
	//	cout << "output neuron output = " << outputLayer[i]->output << endl;
	}
	return outputValues; 
}

void ANN::mutate(float mutationRate) {
	// mutate input layer
	for (int i = 0; i < inputLayer.size(); i++) {
		for (int j = 0; j < inputLayer[i]->connections.size(); j++) {
			// add connection
			// - to recurrent layer
			if (randFloat(0, 1.0 < mutationRate)) {
				inputLayer[i]->connections.push_back(recurrentLayer[rand() % recurrentLayer.size()]);
			}
			// - to output layer
			if (randFloat(0, 1.0 < mutationRate)) {
				inputLayer[i]->connections.push_back(outputLayer[rand() % outputLayer.size()]);
			}
			// remove connection
			if (randFloat(0, 1.0 < mutationRate)) {
				if (inputLayer[i]->connections.size() != 0) {
					inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + rand() % inputLayer[i]->connections.size());
				}
			}
			// change connected neuron
			// - to recurrent layer
			if (randFloat(0, 1.0 < mutationRate)) {
				if (inputLayer[i]->connections.size() != 0) {
					inputLayer[i]->connections[rand() % inputLayer[i]->connections.size()] = recurrentLayer[rand() % recurrentLayer.size()];
				}
			}
			// - to output layer
			if (randFloat(0, 1.0 < mutationRate)) {
				if (inputLayer[i]->connections.size() != 0) {
					inputLayer[i]->connections[rand() % inputLayer[i]->connections.size()] = outputLayer[rand() % outputLayer.size()];
				}
			}
			if (randFloat(0, 1.0 < mutationRate)) {
				inputLayer[i]->mutate();
			}
		}
	}

	// mutate recurrent layer
	for (int i = 0; i < recurrentLayer.size(); i++) {
		for (int j = 0; j < recurrentLayer[i]->connections.size(); j++) {
			// add connection
			// - to recurrent layer
			if (randFloat(0, 1.0 < mutationRate)) {
				recurrentLayer[i]->connections.push_back(recurrentLayer[rand() % recurrentLayer.size()]);
			}
			// - to output layer
			if (randFloat(0, 1.0 < mutationRate)) {
				recurrentLayer[i]->connections.push_back(outputLayer[rand() % outputLayer.size()]);
			}
			// remove connection
			if (randFloat(0, 1.0 < mutationRate)) {
				if (recurrentLayer[i]->connections.size() != 0) {
					recurrentLayer[i]->connections.erase(recurrentLayer[i]->connections.begin() + rand() % recurrentLayer[i]->connections.size());
				}
			}
			// change connected neuron
			// - to recurrent layer
			if (randFloat(0, 1.0 < mutationRate)) {
				if (recurrentLayer[i]->connections.size() != 0) {
					recurrentLayer[i]->connections[rand() % recurrentLayer[i]->connections.size()] = recurrentLayer[rand() % recurrentLayer.size()];
				}
			}
			// - to output layer
			if (randFloat(0, 1.0 < mutationRate)) {
				if (recurrentLayer[i]->connections.size() != 0) {
					recurrentLayer[i]->connections[rand() % recurrentLayer[i]->connections.size()] = outputLayer[rand() % outputLayer.size()];
				}
			}
			if (randFloat(0, 1.0) < mutationRate) {
				recurrentLayer[i]->mutate();
			}

		}
		if (randFloat(0.0, 1.0) < mutationRate) {
			recurrentLayer[i].reset();
			NeuronFactory *neuronFactory = new NeuronFactory;
			int neuronType = rand() % 2;
			if (neuronType == 1) {
				neuronType = 3; 
			}
			recurrentLayer[i] = neuronFactory->createNewNeuronGenome(neuronType);
			recurrentLayer[i]->init(neuronID);
			neuronID++;
			delete neuronFactory; 
			recurrentLayer[i]->connectionsID.push_back(outputLayer[0]->neuronID);
			checkConnections();
			changeConnectionIDToPointer();
		}
	}
	//for (int i = 0; i < connections.size(); i++) {
	//	if (randFloat(0, 1.0 < mutationRate)) {
	//		if (randFloat(0, 1.0) < 0.5) {
	//			connections[i].in = rand() % (allNeurons.size() - outputLayer.size()) + 0; 
	//		}
	//		if (randFloat(0, 1.0) < 0.5) {
	//			connections[i].out = rand() % allNeurons.size() + inputLayer.size();
	//		}
	//		if (randFloat(0, 1.0) < 0.5) {
	//			connections[i].weight = randFloat(-1.0, 1.0);
	//		}
	//	}
	//}
	checkConnections(); 
}

shared_ptr<Control> ANN::clone() {
	return shared_ptr<Control>(new ANN(*this));
}

void ANN::printNeuronValues() {
	std::cout << "printingNeuronValues: " << endl;
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->printNeuron(); 
	}
	for (int i = 0; i < inputLayer.size(); i++) {
		recurrentLayer[i]->printNeuron();
	}
	for (int i = 0; i < inputLayer.size(); i++) {
		outputLayer[i]->printNeuron();
	}
	std::cout << "done printing neuron values" << endl; 
}

stringstream ANN::getControlParams() {
	stringstream ss;
	ss << "\t" << ",#AmountInputNeurons," << inputLayer.size() << ","<< endl;
	for (int i = 0; i < inputLayer.size(); i++) {
		ss << "\t\t" << ",#StartNeuron,";
		ss << inputLayer[i]->getNeuronParams().str(); 
		ss << "\t\t" << ",#EndNeuron," << endl;
	}
	ss << "\t" << ",#AmountInterNeurons," << recurrentLayer.size() << "," << endl;
	for (int i = 0; i < recurrentLayer.size(); i++) {
		ss << "\t\t" << ",#StartNeuron,";
		ss << recurrentLayer[i]->getNeuronParams().str();
		ss << "\t\t" << ",#EndNeuron," << endl;
	}
	ss << "\t" << ",#AmountOutputNeurons," << outputLayer.size() << "," << endl;
	for (int i = 0; i < outputLayer.size(); i++) {
		ss << "\t\t" << ",#StartNeuron,";
		ss << outputLayer[i]->getNeuronParams().str();
		ss << "\t\t" << ",#EndNeuron," << endl;
	}
	ss << endl;
	return ss;
}

void ANN::setControlParams(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		bool checkingNeuron = false; 
		vector<string> neuronValues; 
		if (tmp == "#StartNeuron") {
			checkingNeuron = true;
			it++;
			tmp = values[it];
		}
		if (checkingNeuron == true) {
			neuronValues.push_back(tmp);
		}
		if (tmp == "#EndNeuron") {
			NeuronFactory *neuronFactory = new NeuronFactory;
			int neuronType = atoi(neuronValues[0].c_str());
			shared_ptr<Neuron> neuron = neuronFactory->createNewNeuronGenome(atoi(neuronValues[0].c_str()));
			neuron->setNeuronParams(neuronValues);
			if (neuronType == 1) {
				inputLayer.push_back(neuron);
			}
			else if (neuronType == 2) {
				outputLayer.push_back(neuron);
			}
			else {
				recurrentLayer.push_back(neuron);
			}		
			
			neuronValues.clear();
		}
	}
}

void ANN::cloneControlParameters(shared_ptr<Control> parent) { 
	NeuronFactory *neuronFactory = new NeuronFactory;
	//inputLayer.push_back();
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i] = inputLayer[i]->clone();
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i] = recurrentLayer[i]->clone();
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i] = outputLayer[i]->clone();
	}
	delete neuronFactory;
	changeConnectionIDToPointer();
}

void ANN::changeConnectionIDToPointer() {

	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->connections.resize(inputLayer[i]->connectionsID.size()); 
		for (int j = 0; j < inputLayer[i]->connectionsID.size(); j++) {
			for (int k = 0; k < recurrentLayer.size(); k++) {
				if (inputLayer[i]->connectionsID[j] == recurrentLayer[k]->neuronID) {
					inputLayer[i]->connections[j] = recurrentLayer[k];
				}
			}
			for (int k = 0; k < outputLayer.size(); k++) {
				if (inputLayer[i]->connectionsID[j] == outputLayer[k]->neuronID) {
					inputLayer[i]->connections[j] = outputLayer[k];
				}
			}
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->connections.resize(recurrentLayer[i]->connectionsID.size());
		for (int j = 0; j < recurrentLayer[i]->connectionsID.size(); j++) {
			for (int k = 0; k < recurrentLayer.size(); k++) {
				if (recurrentLayer[i]->connectionsID[j] == recurrentLayer[k]->neuronID) {
					recurrentLayer[i]->connections[j] = recurrentLayer[k];
				}
			}
			for (int k = 0; k < outputLayer.size(); k++) {
				if (recurrentLayer[i]->connectionsID[j] == outputLayer[k]->neuronID) {
					recurrentLayer[i]->connections[j] = outputLayer[k];
				}
			}
		}
	}

}