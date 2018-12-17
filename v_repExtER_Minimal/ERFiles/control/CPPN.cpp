#include "CPPN.h"

CPPN::CPPN()
{
	init(2, 2, 3); // creates a feedforward network of five neurons
}


CPPN::~CPPN()
{
}

void CPPN::init(int input, int inter, int output) {\
	unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
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
	inputLayer[0]->connectionsID.push_back(recurrentLayer[0]->neuronID);
	inputLayer[1]->connectionsID.push_back(recurrentLayer[1]->neuronID);
	recurrentLayer[0]->connectionsID.push_back(outputLayer[0]->neuronID);
	recurrentLayer[1]->connectionsID.push_back(outputLayer[1]->neuronID);
	recurrentLayer[0]->connectionsID.push_back(outputLayer[1]->neuronID);
	recurrentLayer[1]->connectionsID.push_back(outputLayer[2]->neuronID);
	checkConnections();
	changeConnectionIDToPointer();
	neuronFactory.reset();
}

void CPPN::addNeurons(float mutationRate) {
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		if (recurrentLayer.size() < maxLayerSize) {
			//		cout << "adding neuron" << endl;
			unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
			int neuronType = 4; // only create FunctionNeurons (type 4)
			recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(neuronType, settings));
			recurrentLayer[recurrentLayer.size() - 1]->init(neuronID);
			neuronID++;
			neuronFactory.reset();
			recurrentLayer[recurrentLayer.size() - 1]->connectionsID.push_back(outputLayer[0]->neuronID);
			checkConnections();
			changeConnectionIDToPointer();
			//		cout << "added neuron" << endl;
		}
	}
}