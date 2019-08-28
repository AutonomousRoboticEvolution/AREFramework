#include "FixedStructreANN.h"
#include <iostream>

FixedStructureANN::FixedStructureANN()
{

}


FixedStructureANN::~FixedStructureANN()
{

}

void FixedStructureANN::flush() {
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->flush();
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->flush();
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->flush();
	}
}

void FixedStructureANN::init(int input, int inter, int output) {
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
    //Connection is extended for the entire layer
    for (int i = 0; i < recurrentLayer.size(); i++) {
        for (int j = 0; j < inputLayer.size(); j++) {
            inputLayer[j]->connectionsID.push_back(recurrentLayer[i]->neuronID);
            inputLayer[j]->connectionWeights.push_back(1.0);
        }
    }
	for (int i = 0; i < outputLayer.size(); i++) {
        for (int j = 0; j < recurrentLayer.size(); j++) {
            recurrentLayer[j]->connectionsID.push_back(outputLayer[i]->neuronID);
            recurrentLayer[j]->connectionWeights.push_back(1.0);
        }
    }
	checkConnections();
	changeConnectionIDToPointer();
	neuronFactory.reset();  //destroy the unique point
}

void FixedStructureANN::reset() {
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

void FixedStructureANN::checkConnections() {
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
//			/
            // If shared_ptr::unique returns true, then calling shared_ptr::reset will delete the managed object.
            // However, if shared_ptr::unique returns false, it means there are more than one shared_ptrs sharing ownership of that object.
            // In this case a call to reset will only result in the reference count being decremented by 1,
            // actual deletion of the object will take place when the last shared_ptr managing that object either goes out of scope or is itself reset.
			if (inputLayer[i]->connections[j]->neuronID == -1) {
				inputLayer[i]->connections[j].reset();  //release the shared pointer
				inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + j); //remove the object from the vector
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

vector<float> FixedStructureANN::update(vector<float> sensorValues) {
	vector<float> outputValues; 
	if (sensorValues.size() != inputLayer.size()) {
		std::cout << "ERROR: sensor amount differs from input neuron amount" << endl;
		cout << "sensorSize = " << sensorValues.size() << ", amount inputNeurons = " << inputLayer.size() << endl;
	}
	else {
		for (int i = 0; i < sensorValues.size(); i++) {
			inputLayer[i]->input = sensorValues[i];  //overwrite the input of the input layer
			inputLayer[i]->update();  //update the neuron the input layer is connected to
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->update();
	}
	//cf = 0.0;
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->update();
		outputValues.push_back(outputLayer[i]->output);
		//cf += ((0.5 * outputLayer[i]->output / outputLayer.size()));
		cout << "output values: " <<  outputValues[i] << endl;
	}
	//cf += 0.5;
	//printNeuronValues();
	return outputValues;
}


void FixedStructureANN::setFloatParameters(vector<float> values) {
	// function can be used to manually set specific parameters
	recurrentLayer[0]->setFloatParameters(values);
}

void FixedStructureANN::mutate(float mutationRate) {
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->mutate(mutationRate);
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->mutate(mutationRate);
	}
}

shared_ptr<Control> FixedStructureANN::clone() const {
	shared_ptr<FixedStructureANN> newANN = make_unique<FixedStructureANN>(*this);  //make_unique
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
	newANN->checkConnections();
	newANN->changeConnectionIDToPointer();
	newANN->flush(); // flush
	return newANN;
}

stringstream FixedStructureANN::getControlParams() {
	return ANN::getControlParams();
}

void FixedStructureANN::setControlParams(vector<string> values) {
	ANN::setControlParams(values);
}

void FixedStructureANN::changeConnectionIDToPointer() {
	ANN::changeConnectionIDToPointer();
}

bool FixedStructureANN::checkControl(vector<string> values) {
	return ANN::checkControl(values);
}

