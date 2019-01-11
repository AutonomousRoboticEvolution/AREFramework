#include "CPGNeuron.h"
#include <iostream>


CPGNeuron::CPGNeuron() {
	cycleSpeed = 0.01;
	cycleRange = 0.5;
	threshold = 0.0;
	input = 0.0;
	output = 0.0;
	innerValue = 0.0;
}

CPGNeuron::~CPGNeuron() {
	//	cout << "DELETED CONTROL" << endl; 
}

void CPGNeuron::init(int id) {
	neuronID = id;
}
void CPGNeuron::update() {
//	cout << "Speed and Range : " << cycleSpeed << ", " << cycleRange << endl;
	// CPG doesn't actually take an input value, it uses a stable limit cycle instead
	// note that the input value is used as an x value for the stable limit cycle
	if (dir == true) {
		if ((innerValue + cycleSpeed) > cycleRange) {
			innerValue -= cycleSpeed;
			dir = false; // input decreases from here on
		}
		else {
			innerValue += cycleSpeed;
		}
	}
	else if (dir == false) {
		if ((innerValue - cycleSpeed) < -cycleRange) {
			innerValue += cycleSpeed;
			dir = true; // input increases
		}
		else {
			innerValue -= cycleSpeed;
		}
	}
	output = innerValue; 
//	cout << "innerValue = " << innerValue << endl; 
//	output = sqrt((cycleRange * cycleRange) - (innerValue * innerValue));
	for (int i = 0; i < connections.size(); i++) {
		connections[i]->input += output;
	}
	input = 0;

	//	cout << "innerValue " << neuronID << " = " << innerValue << endl; 
}

void CPGNeuron::reset() {
	input = 0.0;
	output = 0.0;
	innerValue = 0.0;
}

shared_ptr<Neuron> CPGNeuron::clone() {
	shared_ptr<CPGNeuron> thisNeuron = make_unique<CPGNeuron>(*this);
	thisNeuron->input = 0;
	thisNeuron->output = 0;
	return thisNeuron;
}

void CPGNeuron::mutate(float mutationRate) {
	if (mutationRate < randomNum->randFloat(0, 1)) {
		threshold = randomNum->randFloat(-1.0, 1.0);
	}
	if (mutationRate < randomNum->randFloat(0, 1)) {
		cycleSpeed = randomNum->randFloat(-0.2, 0.2);
	}
	if (mutationRate < randomNum->randFloat(0, 1)) {
		cycleRange = randomNum->randFloat(0.2, 1.0);
	}
	Neuron::mutate(mutationRate);
}

stringstream CPGNeuron::getNeuronParams() {
	stringstream ss; 
	ss << "3," << endl; 
	ss << Neuron::getNeuronParams().str(); 
	ss << "\t" << ",#Threshold," << to_string(threshold) << "," << endl;
	ss << "\t" << ",#CycleSpeed," << to_string(cycleSpeed) << "," << endl;
	ss << "\t" << ",#CycleRange," << to_string(cycleRange) << "," << endl;
	return ss; 
}

//void CPGNeuron::setNeuronParams(vector<string> values) {
//	Neuron::setNeuronParams(values);
//	for (int it = 0; it < values.size(); it++) {
//		string tmp = values[it];
//		if (tmp == "#CycleSpeed") {
//			it++;
//			tmp = values[it];
//			cycleSpeed = atof(tmp.c_str());
//		}
//		else if (tmp == "#CycleRange") {
//			it++;
//			tmp = values[it];
//			cycleRange = atof(tmp.c_str());
//		}		
//	}
//}

void CPGNeuron::setNeuronParams(vector<string> values) {
	Neuron::setNeuronParams(values);
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#CycleSpeed") {
			it++;
			tmp = values[it];
			cycleSpeed = atof(tmp.c_str());
		}
		else if (tmp == "#CycleRange") {
			it++;
			tmp = values[it];
			cycleRange = atof(tmp.c_str());
		}
	}
}

bool CPGNeuron::checkNeuron(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#CycleSpeed") {
			it++;
			tmp = values[it];
			if ((cycleSpeed + 0.001) < atof(tmp.c_str()) || (cycleSpeed - 0.001) > atof(tmp.c_str())) {
				cout << "ERROR: cyclespeed differs" << endl;
				cout << "cyclespeed = " << cycleSpeed << ", while file says: " << atof(tmp.c_str()) << endl;
			}
		}
		else if (tmp == "#CycleRange") {
			it++;
			tmp = values[it];
			if ((cycleRange + 0.001) < atof(tmp.c_str()) || (cycleRange - 0.001) > atof(tmp.c_str())) {
				cout << "ERROR: CycleRange differs " << endl;
				cout <<  "cycleRange = " << cycleRange << ", while file says: " << atof(tmp.c_str()) << endl;
				return false;
			}
		}
	}
	return Neuron::checkNeuron(values);
}