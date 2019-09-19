
#include "Neuron.h"
#include <iostream>

void Neuron::printNeuron() {
	std::cout << "NEURON ID = " << neuronID << ", " << input << ", " << output << ", Threshold: " << threshold << std::endl;
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
			if (connectionWeights[i] > 1.0) {   //normalize the weights
				connectionWeights[i] = 1.0;
			}
			else if (connectionWeights[i] < -1.0) {
				connectionWeights[i] = -1.0;
			}
		}
	}
}

std::stringstream Neuron::getNeuronParams() {
    std::stringstream ss;
	ss << "\t" << ",#NeuronID," << neuronID << "," << std::endl;
	ss << "\t" << ",#Connections,";
	for (int i = 0; i < connections.size(); i++) {
		ss << connections[i]->neuronID << ",";  //the ID of the neuron this neuron connects to
	}
	ss << std::endl;
	ss << "\t" << ",#ConnectionWeights,";
	for (int i = 0; i < connections.size(); i++) {
		ss << connectionWeights[i] << ",";
	}
	ss << std::endl;
	return ss;
}

void Neuron::setNeuronParams(std::vector<std::string> values) {
	connections.clear();
	bool addingConnections = false;
	bool addingConnectionWeights = false;
	for (int it = 0; it < values.size(); it++) {
        std::string tmp = values[it];
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

bool Neuron::checkNeuron(std::vector<std::string> values) {
	for (int it = 0; it < values.size(); it++) {
        std::string tmp = values[it];
		if (tmp == "#NeuronType") {
			it++;
			tmp = values[it];
		}
		else if (tmp == "#NeuronID") {
			it++;
			tmp = values[it];
			if (neuronID != atoi(tmp.c_str())) {
                std::cout << "ERROR: neuronID is different" << std::endl;
                std::cout << "neuronID = " << neuronID << ", saved neuronID = " << atoi(tmp.c_str()) << std::endl;
				return false;
			}
		}
		else if (tmp == "#Connections") {
			for (int i = 0; i < connectionsID.size(); i++) {
				it++;
				tmp = values[it];
				if (connectionsID[i] != atoi(tmp.c_str())) {
                    std::cout << "ERROR << connections are different" << std::endl;
					return false;
				}
				if (connections[i] == NULL) {
                    std::cout << "ERROR << connection is NULL" << std::endl;
					return false;
				}

			}
		}
		else if (tmp == "#Threshold") {
			it++;
			tmp = values[it];
			if ((threshold + 0.001) < atof(tmp.c_str()) || (threshold - 0.001) > atof(tmp.c_str())) {
                std::cout << "ERROR << threshold is different" << std::endl;
                std::cout << "threshold = " << threshold << ", while file says " << atof(tmp.c_str()) << std::endl;
				return false;
			}
		}
	}
	return true;
}