#include "InputNeuron.h"
#include <iostream>

InputNeuron::InputNeuron() {
	threshold = 0.0;
	input = 0.0;
	output = 0.0;
}

InputNeuron::~InputNeuron() {
//	cout << "Deleting input Neuron" << endl; 
}

void InputNeuron::init(int id) {
	neuronID = id;
}
void InputNeuron::update() {
//	cout << "input = " << input << endl, 
	output = input;// * transformValue; 
//	cout << "output = " << output << endl; 
	for (int i = 0; i < connections.size(); i++) {
		connections[i]->input += (output * connectionWeights[i]);
	}
//	input = 0;
}
void InputNeuron::reset() {
	input = 0.0;
	output = 0.0;
}

shared_ptr<Neuron> InputNeuron::clone() {
	shared_ptr<InputNeuron> thisNeuron = make_unique<InputNeuron>(*this);
	thisNeuron->input = 0;
	thisNeuron->output = 0;
	return thisNeuron;
}

void InputNeuron::mutate(float mutationRate) {
	if (mutationRate < randomNum->randFloat(0, 1)) {
		threshold = randomNum->randFloat(-1.0, 1.0);
	}
	if (mutationRate < randomNum->randFloat(0, 1)) {
		transformValue = randomNum->randFloat(-1.0, 1.0);
	}
	Neuron::mutate(mutationRate);

}

stringstream InputNeuron::getNeuronParams() {
	stringstream ss;
	ss << "1," << endl; 
	ss << Neuron::getNeuronParams().str();
	return ss;
}

void InputNeuron::setNeuronParams(vector<string> values) {
	Neuron::setNeuronParams(values);
}

bool InputNeuron::checkNeuron(vector<string> values) {
	return Neuron::checkNeuron(values);
}