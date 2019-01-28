#include "CustomANN.h"
#include <iostream>


CustomANN::CustomANN()
{

}


CustomANN::~CustomANN()
{

}

void CustomANN::init(int input, int inter, int output) {
	unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
	neuronID = 0;
	for (int i = 0; i < input; i++) {
		inputLayer.push_back(neuronFactory->createNewNeuronGenome(1, settings));
		inputLayer[i]->init(neuronID);
		inputLayer[i]->connectionWeights.push_back(1.0);
		neuronID++;
	}
	for (int i = 0; i < inter; i++) {
		recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(5, settings)); // creates pattern neuron 
		recurrentLayer[i]->init(neuronID);
		recurrentLayer[i]->connectionWeights.push_back(1.0);
		neuronID++;
	}
	for (int i = 0; i < output; i++) {
		outputLayer.push_back(neuronFactory->createNewNeuronGenome(2, settings));
		outputLayer[i]->init(neuronID);
		outputLayer[i]->connectionWeights.push_back(1.0);
		neuronID++;
	}
	inputLayer[0]->connectionsID.push_back(recurrentLayer[0]->neuronID);
	recurrentLayer[0]->connectionsID.push_back(outputLayer[0]->neuronID);
	checkConnections();
	changeConnectionIDToPointer();
	neuronFactory.reset();
}

void CustomANN::reset() {
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i].reset();
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i].reset();
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i].reset();
	}
}

void CustomANN::checkConnections() { // this function deletes the connections to deleted neurons and in turn the pointer to these neurons should go out of scope. 
//	cout << "in" << inputLayer.size() << endl;
	for (int i = 0; i < inputLayer.size(); i++) {
//		cout << "conID:" << inputLayer[i]->connectionsID.size() << endl;
//		cout << "con:" << inputLayer[i]->connections.size() << endl;
		for (int j = 0; j < inputLayer[i]->connections.size(); j++) {
			if (inputLayer[i]->connections[j] == NULL) {
	//			cout << "ERROR: connection " << j << " of neuron " << i << " is NULL" << endl;
				for (int k = 0; k < inputLayer[i]->connections.size(); k++) {
	//				cout << "Con " << k << " = " << inputLayer[i]->connections[k] << endl;
	//				cout << "Con ID " << k << " = " << inputLayer[i]->connectionsID[k] << endl;
				}

			}
	//		cout << "j:" << j << endl;
	//		cout << "/" << inputLayer[i]->connectionsID[j] << endl;
	//		cout << "/" << inputLayer[i]->connections[j] << endl;
			
			if (inputLayer[i]->connections[j]->neuronID == -1) {
				inputLayer[i]->connections[j].reset();
				inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + j);
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
//	cout << "checked connections" << endl;
//	printNeuronValues();
}

vector<float> CustomANN::update(vector<float> sensorValues) {
	vector<float> outputValues; 
	if (sensorValues.size() != inputLayer.size()) {
		//std::cout << "ERROR: sensor amount differs from input neuron amount" << endl;
		//cout << "sensorSize = " << sensorValues.size() << ", amount inputNeurons = " << inputLayer.size() << endl;
	}
	else {
		for (int i = 0; i < sensorValues.size(); i++) {
			inputLayer[i]->input = sensorValues[i];
			inputLayer[i]->update();
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->update();
	}
	cf = 0.0;
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->update();
		outputValues.push_back(outputLayer[i]->output);
		cf += ((0.5 * outputLayer[i]->output / outputLayer.size()));
		// cout << "outputvalues " <<  outputValues[0] << endl;
	}
	cf += 0.5;
	//	printNeuronValues();
		//leaky(0.8);
	return outputValues;
}

void CustomANN::mutateConnections(float mutationRate) {
	ANN::mutateConnections(mutationRate);
}

void CustomANN::addNeurons(float mutationRate) {
	ANN::addNeurons(mutationRate);
}

void CustomANN::setFloatParameters(vector<float> values) {
	// function can be used to manually set specific parameters
	recurrentLayer[0]->setFloatParameters(values);
}

void CustomANN::removeNeurons(float mutationRate) {
	ANN::removeNeurons(mutationRate);
}

void CustomANN::changeNeurons(float mutationRate) { // debugging required
	ANN::changeNeurons(mutationRate);
}

void CustomANN::mutate(float mutationRate) {
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->mutate(mutationRate);
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->mutate(mutationRate);
	}
//	mutateConnections(mutationRate);
//	addNeurons(mutationRate);
//	removeNeurons(mutationRate);
//	changeNeurons(mutationRate);	
//	checkConnections(); 
//	changeConnectionIDToPointer();
}

shared_ptr<Control> CustomANN::clone() const {
	shared_ptr<CustomANN> newANN = make_unique<CustomANN>(*this);
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
	return newANN;
}

void CustomANN::printNeuronValues() {
	ANN::printNeuronValues();
}

stringstream CustomANN::getControlParams() {
	return ANN::getControlParams();
}

void CustomANN::setControlParams(vector<string> values) {
	ANN::setControlParams(values);
}

void CustomANN::cloneControlParameters(shared_ptr<Control> parent) {
	ANN::cloneControlParameters(parent);
}

void CustomANN::changeConnectionIDToPointer() {
	ANN::changeConnectionIDToPointer();
}

void CustomANN::changeConnectionPointerToID() {
	ANN::changeConnectionPointerToID();
}

bool CustomANN::checkControl(vector<string> values) {
	return ANN::checkControl(values);
}

