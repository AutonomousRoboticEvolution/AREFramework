#include "CustomANN.h"
#include <iostream>

using namespace std;

CustomANN::CustomANN()
{}

CustomANN::~CustomANN()
{}

void CustomANN::flush()
{
	for (auto & neuron : inputLayer) {
		neuron->flush();
	}
	for (auto & neuron : recurrentLayer) {
		neuron->flush();
	}
	for (auto & neuron : outputLayer) {
		neuron->flush();
	}
}

void CustomANN::init(int input, int inter, int output)
{
	unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
	neuronID = 0;
	for (int i = 0; i < input; i++) {
		inputLayer.push_back(neuronFactory->createNewNeuronGenome(1, settings));
		inputLayer[i]->init(neuronID);  //assign the ID for the neuron
		inputLayer[i]->connectionWeights.push_back(1.0);  //initialize the weights into 1.0
		neuronID++;  //make sure the ID is unique for each neuron
	}
	for (int i = 0; i < inter; i++) {
		recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(0, settings));
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
	inputLayer[0]->connectionsID.push_back(recurrentLayer[0]->neuronID);  //TODO:??
	recurrentLayer[0]->connectionsID.push_back(outputLayer[0]->neuronID); //TODO:??
	checkConnections();
	changeConnectionIDToPointer();
	neuronFactory.reset();  //destroy the unique point
}

void CustomANN::reset()
{
	for (auto & neuron : inputLayer) {
		neuron.reset();
	}
	for (auto & neuron : outputLayer) {
		neuron.reset();
	}
	for (auto & neuron : recurrentLayer) {
		neuron.reset();
	}
}

void CustomANN::checkConnections()
{
//	cout << "in" << inputLayer.size() << endl;
	for (auto & neuron : inputLayer) {
//		cout << "conID:" << inputLayer[neuron]->connectionsID.size() << endl;
//		cout << "con:" << inputLayer[neuron]->connections.size() << endl;
		for (int j = 0; j < neuron->connections.size(); j++) {
			if (neuron->connections[j] == nullptr) {
	//			cout << "ERROR: connection " << j << " of neuron " << neuron << " is NULL" << endl;
				for (int k = 0; k < neuron->connections.size(); k++) {
	//				cout << "Con " << k << " = " << inputLayer[neuron]->connections[k] << endl;
	//				cout << "Con ID " << k << " = " << inputLayer[neuron]->connectionsID[k] << endl;
				}

			}
	//		cout << "j:" << j << endl;
	//		cout << "/" << inputLayer[neuron]->connectionsID[j] << endl;
	//		cout << "/" << inputLayer[neuron]->connections[j] << endl;
//			/
            // If shared_ptr::unique returns true, then calling shared_ptr::reset will delete the managed object.
            // However, if shared_ptr::unique returns false, it means there are more than one shared_ptrs sharing ownership of that object.
            // In this case a call to reset will only result in the reference count being decremented by 1,
            // actual deletion of the object will take place when the last shared_ptr managing that object either goes out of scope or is itself reset.
			if (neuron->connections[j]->neuronID == -1) {
				neuron->connections[j].reset();  //release the shared pointer
				neuron->connections.erase(neuron->connections.begin() + j); //remove the object from the vector
			}
		}
	}
	for (auto & neuron : recurrentLayer) {
		for (int j = 0; j < neuron->connections.size(); j++) {
			if (neuron->connections[j]->neuronID == -1) {
				neuron->connections[j].reset();
				neuron->connections.erase(neuron->connections.begin() + j);
			}
		}
	}
	for (auto & neuron : outputLayer) {
		for (int j = 0; j < neuron->connections.size(); j++) {
			if (neuron->connections[j]->neuronID == -1) {
				neuron->connections[j].reset();
				neuron->connections.erase(neuron->connections.begin() + j);
			}
		}
	}
//	cout << "checked connections" << endl;
//	printNeuronValues();
}

vector<float> CustomANN::update(vector<float> sensorValues)
{
	vector<float> outputValues; 
	if (sensorValues.size() != inputLayer.size()) {
		std::cout << "ERROR: sensor amount differs from input neuron amount" << endl;
		cout << "sensorSize = " << sensorValues.size() << ", amount inputNeurons = " << inputLayer.size() << endl;
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
		cout << "outputvalues " <<  outputValues[i] << endl;
	}
	cf += 0.5;
	//printNeuronValues();
		//leaky(0.8);
	return outputValues;
}

void CustomANN::setFloatParameters(vector<float> values)
{
	// function can be used to manually set specific parameters
	recurrentLayer[0]->setFloatParameters(values);
}

void CustomANN::mutate(float mutationRate)
{
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->mutate(mutationRate);
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->mutate(mutationRate);
	}
}

shared_ptr<Control> CustomANN::clone() const
{
	shared_ptr<CustomANN> newANN = make_shared<CustomANN>(*this);  //make_unique
	newANN->inputLayer.clear();
	newANN->recurrentLayer.clear();
	newANN->outputLayer.clear();
	for (const auto & neuron : this->inputLayer) {
		newANN->inputLayer.push_back(neuron->clone());
	}
	for (const auto & neuron : this->recurrentLayer) {
		newANN->recurrentLayer.push_back(neuron->clone());
	}
	for (const auto & neuron : this->outputLayer) {
		newANN->outputLayer.push_back(neuron->clone());
	}
	newANN->checkConnections();
	newANN->changeConnectionIDToPointer();
	newANN->flush(); // flush
	return newANN;
}

stringstream CustomANN::getControlParams()
{
	return ANN::getControlParams();
}

void CustomANN::setControlParams(vector<string> values)
{
	ANN::setControlParams(values);
}

void CustomANN::changeConnectionIDToPointer()
{
	ANN::changeConnectionIDToPointer();
}

bool CustomANN::checkControl(const std::vector<std::string> &values)
{
	return ANN::checkControl(values);
}
