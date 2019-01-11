#include "OutputNeuron.h"
#include <iostream>


OutputNeuron::OutputNeuron() {
	threshold = 0.0;
	input = 0.0;
	output = 0.0;
}

OutputNeuron::~OutputNeuron() {
	//	cout << "DELETED CONTROL" << endl; 
}

void OutputNeuron::init(int id) {
	neuronID = id;
}
void OutputNeuron::update() {
	output = input; 
	if (output > 1.0) {
		output = 1.0;
	}
	else if (output < -1.0) {
		output = -1.0;
	}
//	cout << "output neuron input = " << input << endl;
	for (int i = 0; i < connections.size(); i++) {
		connections[i]->input += output * connectionWeights[i];
	}
//	input = 0;
}
void OutputNeuron::reset() {
	input = 0.0;
	output = 0.0;
	// output neuron has nothing to reset
}

shared_ptr<Neuron> OutputNeuron::clone() {
	shared_ptr<OutputNeuron> thisNeuron = make_unique<OutputNeuron>(*this);
	thisNeuron->input = 0;
	thisNeuron->output = 0;
	return thisNeuron;
}

void OutputNeuron::mutate(float mutationRate) {
	threshold = randomNum->randFloat(-1.0, 1.0);
	Neuron::mutate(mutationRate);
}

stringstream OutputNeuron::getNeuronParams() {
	stringstream ss;
	ss << "2," << endl;
	ss << Neuron::getNeuronParams().str();
	return ss;
}

void OutputNeuron::setNeuronParams(vector<string> values) {
	Neuron::setNeuronParams(values);
}

bool OutputNeuron::checkNeuron(vector<string> values) {
	return Neuron::checkNeuron(values);
}