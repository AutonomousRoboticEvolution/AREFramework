#include "FixedStructreANN.h"
#include <iostream>
#include <memory>


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
	std::unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
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
    //connection between input and recurrent layer
    for (int i = 0; i < recurrentLayer.size(); i++) {
        for (int j = 0; j < inputLayer.size(); j++) {
            inputLayer[j]->connectionsID.push_back(recurrentLayer[i]->neuronID);
            inputLayer[j]->connectionWeights.push_back(1.0);
        }
    }
    //connection between recurrent and output layer
	for (int i = 0; i < outputLayer.size(); i++) {
        for (int j = 0; j < recurrentLayer.size(); j++) {
            recurrentLayer[j]->connectionsID.push_back(outputLayer[i]->neuronID);
            recurrentLayer[j]->connectionWeights.push_back(1.0);
        }
    }
	//direct connection between input and output
    for (int i = 0; i < outputLayer.size(); i++) {
        for (int j = 0; j < inputLayer.size(); j++) {
            inputLayer[j]->connectionsID.push_back(outputLayer[i]->neuronID);
            inputLayer[j]->connectionWeights.push_back(1.0);
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
//	std::cout << "in" << inputLayer.size() << std::endl;
	for (int i = 0; i < inputLayer.size(); i++) {
//		std::cout << "conID:" << inputLayer[i]->connectionsID.size() << std::endl;
//		std::cout << "con:" << inputLayer[i]->connections.size() << std::endl;
		for (int j = 0; j < inputLayer[i]->connections.size(); j++) {
			if (inputLayer[i]->connections[j] == NULL) {
	//			std::cout << "ERROR: connection " << j << " of neuron " << i << " is NULL" << std::endl;
				for (int k = 0; k < inputLayer[i]->connections.size(); k++) {
	//				std::cout << "Con " << k << " = " << inputLayer[i]->connections[k] << std::endl;
	//				std::cout << "Con ID " << k << " = " << inputLayer[i]->connectionsID[k] << std::endl;
				}

			}
	//		std::cout << "j:" << j << std::endl;
	//		std::cout << "/" << inputLayer[i]->connectionsID[j] << std::endl;
	//		std::cout << "/" << inputLayer[i]->connections[j] << std::endl;
//			/
            // If std::shared_ptr::unique returns true, then calling std::shared_ptr::reset will delete the managed object.
            // However, if std::shared_ptr::unique returns false, it means there are more than one std::shared_ptrs sharing ownership of that object.
            // In this case a call to reset will only result in the reference count being decremented by 1,
            // actual deletion of the object will take place when the last std::shared_ptr managing that object either goes out of scope or is itself reset.
			if (inputLayer[i]->connections[j]->neuronID == -1) {
				inputLayer[i]->connections[j].reset();  //release the shared pointer
				inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + j); //remove the object from the std::vector
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
//	std::cout << "checked connections" << std::endl;
//	printNeuronValues();
}

std::vector<float> FixedStructureANN::update(const std::vector<float> &sensorValues)
{
	std::vector<float> outputValues; 
	if (sensorValues.size() != inputLayer.size()) {
		std::cout << "ERROR: sensor amount differs from input neuron amount" << std::endl;
		std::cout << "sensorSize = " << sensorValues.size() << ", amount inputNeurons = " << inputLayer.size() << std::endl;
	}
	else {

	    for (auto &input : inputLayer) {
            for (int i = 0; i < input->connections.size(); i++) {
                input->connections[i]->input = 0;
            }
        }
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
		//std::cout << "output values: " <<  outputValues[i] << std::endl;
	}
	//cf += 0.5;
	//printNeuronValues();
	return outputValues;
}


void FixedStructureANN::setFloatParameters(std::vector<float> values)
{
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


std::shared_ptr<Control> FixedStructureANN::clone() const
{
	std::shared_ptr<FixedStructureANN> newANN = std::make_shared<FixedStructureANN>(*this);
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


std::stringstream FixedStructureANN::getControlParams()
{
	return ANN::getControlParams();
}

void FixedStructureANN::setControlParams(const std::vector<std::string> &values)
{
	ANN::setControlParams(values);
}

void FixedStructureANN::changeConnectionIDToPointer()
{
	ANN::changeConnectionIDToPointer();
}

bool FixedStructureANN::checkControl(const std::vector<std::string> &values)
{
	return ANN::checkControl(values);
}

