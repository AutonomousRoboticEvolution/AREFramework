#include "SimpleNeuron.h"
#include <iostream>



SimpleNeuron::SimpleNeuron() {
	threshold = 0.0;
	input = 0.0;
	output = 0.0;
}

SimpleNeuron::~SimpleNeuron() {
	//	cout << "DELETED CONTROL" << endl; 
}

void SimpleNeuron::init(int id) {
	neuronID = id;
}
void SimpleNeuron::update() {
//	if (input > threshold) {
//		output = outputWeight; 
//	}
//	else {
//		output = 0; 
//	}
//	if (input != 0.0) {
//		cout << "input = " << input << endl;
//	}
	if (input > threshold) {
		for (int i = 0; i < connections.size(); i++) {
			connections[i]->input = connectionWeights[i]; // not output anymore 12-05-2017
		}
	}
}
void SimpleNeuron::reset() {
	input = 0; 
	output = 0;
}

shared_ptr<Neuron> SimpleNeuron::clone() {
	shared_ptr<SimpleNeuron> thisNeuron = make_unique<SimpleNeuron>(*this);
	thisNeuron->input = 0; 
	thisNeuron->output = 0;
	return thisNeuron;
}

void SimpleNeuron::mutate(float mutationRate) {
	if (mutationRate > randomNum->randFloat(0, 1)) {
		threshold = randomNum->randFloat(-1.0, 1.0);
		outputWeight = randomNum->randFloat(-1.0, 1.0);
	}
	Neuron::mutate(mutationRate);
//	input = randFloat(-1.0, 1.0);
}

stringstream SimpleNeuron::getNeuronParams() {
	stringstream ss;
	ss << "0," << endl;
	ss << Neuron::getNeuronParams().str(); 
	ss << "\t" << ",#Threshold," << to_string(threshold) << "," << endl;
	ss << "\t" << ",#outputWeight," << to_string(outputWeight) << "," << endl;
	return ss;
}

void SimpleNeuron::setNeuronParams(vector<string> values) {
	Neuron::setNeuronParams(values);
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#outputWeight") {
			it++;
			tmp = values[it];
			outputWeight = atof(tmp.c_str());
		}
	}
}

bool SimpleNeuron::checkNeuron(vector<string> values) {
	return Neuron::checkNeuron(values);
}