#pragma once
#include "Neuron.h"
#include <iostream>

using namespace std;


void Neuron::printNeuron() {
	cout << "NEURON ID = " << neuronID << ", " << input << ", " << output << ", Threshold: " << threshold << endl;
//	threshold = 0.0;
}


void Neuron::init(int neuronID) {

}

void Neuron::mutate(float mr) {
	// randomly mutate connection
	for (int i = 0; i < connectionWeights.size(); i++) {
		// random mutations
		if (randomNum->randFloat(0.0, 1.0) < mr) {
			connectionWeights[i] = randomNum->randFloat(-1.0, 1.0);
		}
		// local mutations
		if (randomNum->randFloat(0.0, 1.0) < mr) {
			connectionWeights[i]+= randomNum->randFloat(-sigma, sigma);
			if (connectionWeights[i] > 1.0) {
				connectionWeights[i] = 1.0;
			}
			else if (connectionWeights[i] < -1.0) {
				connectionWeights[i] = -1.0;
			}
		}
	}
}

stringstream Neuron::getNeuronParams() {
	stringstream ss;
	ss << "\t" << ",#NeuronID," << neuronID << "," << endl;
	ss << "\t" << ",#Connections,";
	for (int i = 0; i < connections.size(); i++) {
		ss << connections[i]->neuronID << ",";
	}
	ss << endl;
	ss << "\t" << ",#ConnectionWeights,";
	for (int i = 0; i < connections.size(); i++) {
		ss << connectionWeights[i] << ",";
	}
	ss << endl;
	return ss;
}

void Neuron::setNeuronParams(vector<string> values) {
	connections.clear();
	bool addingConnections = false;
	bool addingConnectionWeights = false;
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#NeuronType") {
			it++;
			tmp = values[it];
		}
		else if (tmp == "#NeuronID") {
			it++;
			tmp = values[it];
			neuronID = atoi(tmp.c_str());
		}
		else if (tmp == "#Connections") {
			addingConnections = true;
			it++;
			tmp = values[it];
		}
		else if (tmp == "#ConnectionWeights") {
			addingConnectionWeights = true;
			it++;
			tmp = values[it];
		}
		else if (tmp == "#Threshold") {
			it++;
			tmp = values[it];
			threshold = atof(tmp.c_str());
		}

		if (tmp == "") {
			addingConnections = false;
			addingConnectionWeights = false;
		}
		if (addingConnections == true) {
			connectionsID.push_back(atoi(tmp.c_str()));
		}
		if (addingConnectionWeights == true) {
			connectionWeights.push_back(atof(tmp.c_str()));
		}
	}
}

bool Neuron::checkNeuron(vector<string> values) {
	int conCounter = 0;
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#NeuronType") {
			it++;
			tmp = values[it];
		}
		else if (tmp == "#NeuronID") {
			it++;
			tmp = values[it];
			if (neuronID != atoi(tmp.c_str())) {
				cout << "ERROR: neuronID is different" << endl;
				cout << "neuronID = " << neuronID << ", saved neuronID = " << atoi(tmp.c_str()) << endl;
				return false;
			};
		}
		else if (tmp == "#Connections") {
			for (int i = 0; i < connectionsID.size(); i++) {
				it++;
				tmp = values[it];
				if (connectionsID[i] != atoi(tmp.c_str())) {
					cout << "ERROR << connections are different" << endl;
					return false;
				}
				if (connections[i] == NULL) {
					cout << "ERROR << connection is NULL" << endl;
					return false;
				}

			}
		}
		else if (tmp == "#Threshold") {
			it++;
			tmp = values[it];
			if ((threshold + 0.001) < atof(tmp.c_str()) || (threshold - 0.001) > atof(tmp.c_str())) {
				cout << "ERROR << threshold is different" << endl;
				cout << "threshold = " << threshold << ", while file says " << atof(tmp.c_str()) << endl;
				return false;
			}
		}
	}
	return true;
}