#include "ANN.h"
#include <iostream>

using namespace std;

ANN::ANN()
{
//	std::cout << "creating ANN" << endl; 
//	init(1,1,1); 
//	printNeuronValues();
}


ANN::~ANN()
{
//	std::cout << "deleting ANN" << endl; 
}

void ANN::init(int input, int inter, int output) {
//	cout << "initializing control" << endl;
	unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
	neuronID = 0;
	for (int i = 0; i < input; i++) {
		inputLayer.push_back(neuronFactory->createNewNeuronGenome(1, settings));
		inputLayer[i]->init(neuronID);
		neuronID++;
	}
	for (int i = 0; i < inter; i++) {
		if (settings->createPatternNeurons) {
			if (randomNum->randFloat(0, 1) < 0.5) { // change this 
				recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(5, settings)); // creates CPG
				recurrentLayer[i]->init(neuronID);
				neuronID++;
			}
			else {
				recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(0, settings)); // creates simpleNeuron
				recurrentLayer[i]->init(neuronID);
				neuronID++;
			}
		}
		else {
			recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(0, settings)); // creates simpleNeuron
			recurrentLayer[i]->init(neuronID);
			neuronID++;
		}

	}
	for (int i = 0; i < output; i++) {
		outputLayer.push_back(neuronFactory->createNewNeuronGenome(2, settings));
		outputLayer[i]->init(neuronID);
		neuronID++;
	}
	//allNeurons.insert(allNeurons.end(), inputLayer.begin(), inputLayer.end());
	//allNeurons.insert(allNeurons.end(), recurrentLayer.begin(), recurrentLayer.end());
	//allNeurons.insert(allNeurons.end(), outputLayer.begin(), outputLayer.end());
//	inputLayer[0]->connections.push_back(recurrentLayer[0]);
	inputLayer[0]->connectionsID.push_back(recurrentLayer[0]->neuronID);
	inputLayer[0]->connectionWeights.push_back(1.0);
//	recurrentLayer[0]->connections.push_back(outputLayer[0]);
	for (int i = 0; i < recurrentLayer.size(); i++) {
		if (i < outputLayer.size()) {
			recurrentLayer[i]->connectionsID.push_back(outputLayer[i]->neuronID);
			recurrentLayer[i]->connectionWeights.push_back(1.0);
		}
		else {
			// now, additional recurrent neurons will not be connected to an output neuron
			// recurrentLayer[i]->connectionsID.push_back(outputLayer[0]->neuronID);
			// recurrentLayer[i]->connectionWeights.push_back(1.0);
		}
	}
//	cout << "OUTPUTSIZE = " << outputLayer.size() << endl;
	changeConnectionIDToPointer();
	checkConnections();
	neuronFactory.reset();
//	cout << "OUTPUTSIZE = " << outputLayer.size() << endl;
}

void ANN::reset() {
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->reset();
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->reset();
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->reset();
	}
}

void ANN::checkConnections() { // this function deletes the connections to deleted neurons and in turn the pointer to these neurons should go out of scope. 
//	cout << "in" << inputLayer.size() << endl;
	for (int i = 0; i < inputLayer.size(); i++) {
//		cout << "conID:" << inputLayer[i]->connectionsID.size() << endl;
//		cout << "con:" << inputLayer[i]->connections.size() << endl;
		for (int j = 0; j < inputLayer[i]->connections.size(); j++) {
			if (inputLayer[i]->connections[j] == NULL) {
	//			cout << "ERROR: connection " << j << " of neuron " << i << " is NULL" << endl;
	//			for (int k = 0; k < inputLayer[i]->connections.size(); k++) {
	//				cout << "Con " << k << " = " << inputLayer[i]->connections[k] << endl;
	//				cout << "Con ID " << k << " = " << inputLayer[i]->connectionsID[k] << endl;
	//			}

			}
	//		cout << "j:" << j << endl;
	//		cout << "/" << inputLayer[i]->connectionsID[j] << endl;
	//		cout << "/" << inputLayer[i]->connections[j] << endl;
			
			if (inputLayer[i]->connections[j]->neuronID == -1) {
				inputLayer[i]->connections[j].reset();
				inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + j);
				inputLayer[i]->connectionWeights.erase(inputLayer[i]->connectionWeights.begin() + j);
			}
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		for (int j = 0; j < recurrentLayer[i]->connections.size(); j++) {
			if (recurrentLayer[i]->connections[j]->neuronID == -1) {
				recurrentLayer[i]->connections[j].reset();
				recurrentLayer[i]->connections.erase(recurrentLayer[i]->connections.begin() + j);
				recurrentLayer[i]->connectionWeights.erase(recurrentLayer[i]->connectionWeights.begin() + j);
			}
		}
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		for (int j = 0; j < outputLayer[i]->connections.size(); j++) {
			if (outputLayer[i]->connections[j]->neuronID == -1) {
				outputLayer[i]->connections[j].reset();  //remove the connection
				outputLayer[i]->connections.erase(outputLayer[i]->connections.begin() + j);  //remove the connection
				outputLayer[i]->connectionWeights.erase(outputLayer[i]->connectionWeights.begin() + j); //remove he connection weight
			}
		}
	}
//	cout << "checked connections" << endl;
//	printNeuronValues();
}

//may not be needed
void ANN::addInput(vector<float> input) {
	int controlSize = inputLayer.size();
	int inputSize = input.size();
	if (inputSize > controlSize) {
		for (int i = 0; i < controlSize; i++) {
			inputLayer[i]->input += input[i];
		}
	}
	else {
		for (int i = 0; i < inputSize; i++) {
			inputLayer[i]->input += input[i];
		}
	}
}

vector<float> ANN::update(vector<float> sensorValues) {
	vector<float> outputValues; 
	//cout << "updating ANN" << endl;
//	if (sensorValues.size() != inputLayer.size()) {
//		std::cout << "ERROR: sensor amount differs from input neuron amount" << endl;
//		cout << "sensorSize = " << sensorValues.size() << ", amount inputNeurons = " << inputLayer.size() << endl;
//	}
//	else {
	//	cout << "sensorValues.size = " << sensorValues.size() << endl;
		for (int i = 0; i < sensorValues.size(); i++) {
			inputLayer[i]->input += sensorValues[i];
	//		cout << "sv[i] = " << sensorValues[i] << endl;
			inputLayer[i]->update();
	//		cout << "input neuron output = " << inputLayer[i]->output << endl;
		}
//	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->update();
	//	cout << "recurrent neuron output = " << recurrentLayer[i]->output << endl;
	}
	cf = 0.0;
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->update();
		outputValues.push_back(outputLayer[i]->output);
		cf += ((0.5 * outputLayer[i]->output/outputLayer.size()));
//		cout << "cf = " << cf << endl;
//		cout << "outputLayer[i]->output" << outputLayer[i]->output << endl;
//		cout << "outputLyerSize = " << outputLayer.size() << endl;
	//	cout << "output neuron output = " << outputLayer[i]->output << endl;
	}
	cf += 0.5;
//	printNeuronValues();
	return outputValues; 
}

void ANN::flush()
{
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

void ANN::mutateConnections(float mutationRate) {
	for (int i = 0; i < inputLayer.size(); i++) {
		int amountCon = inputLayer[i]->connections.size();
		for (int j = 0; j < amountCon; j++) {
			// add connection
			// - to recurrent layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (amountCon < maxCon) {
					if (recurrentLayer.size() > 0) {
						//						cout << "add connection to rec" << endl;
						int newCon = randomNum->randInt(recurrentLayer.size(), 0);
						//						cout << "newCon : " << newCon << endl;
						inputLayer[i]->connections.push_back(recurrentLayer[newCon]);
						inputLayer[i]->connectionWeights.push_back(randomNum->randFloat(0.0, 1.0));
						//						cout << "con success" << endl;
						inputLayer[i]->connectionsID.push_back(recurrentLayer[newCon]->neuronID);
						//						cout << "added connection to rec" << endl;
					}
				}
			}
			// - to output layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (amountCon < maxCon) {
					//					cout << "add connection output" << endl;
					int newCon = randomNum->randInt(outputLayer.size(), 0);
					inputLayer[i]->connections.push_back(outputLayer[newCon]);
					inputLayer[i]->connectionWeights.push_back(randomNum->randFloat(0.0, 1.0));
					inputLayer[i]->connectionsID.push_back(outputLayer[newCon]->neuronID);
					//					cout << "added connection to output" << endl;

				}
			}
			// remove connection
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (inputLayer[i]->connections.size() > 0) {
					//					cout << "remove connection" << endl;
					int deleteCon = randomNum->randInt(inputLayer[i]->connections.size(), 0);
					inputLayer[i]->connections.erase(inputLayer[i]->connections.begin() + deleteCon);
					inputLayer[i]->connectionWeights.erase(inputLayer[i]->connectionWeights.begin() + deleteCon);
					inputLayer[i]->connectionsID.erase(inputLayer[i]->connectionsID.begin() + deleteCon);
					//					cout << "removed connection" << endl;
				}
			}
			// change connected neuron
			// - to recurrent layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (inputLayer[i]->connections.size() > 0 && recurrentLayer.size() > 0) {
					//					cout << "change connection to rec" << endl;
					int newCon = randomNum->randInt(recurrentLayer.size(), 0);
					int con = randomNum->randInt(inputLayer[i]->connections.size(), 0);
					inputLayer[i]->connections[con] = recurrentLayer[newCon];
					inputLayer[i]->connectionsID[con] = recurrentLayer[newCon]->neuronID;
					//					cout << "changed connection to rec" << endl;
				}
			}
			// - to output layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (inputLayer[i]->connections.size() > 0 && outputLayer.size() > 0) {
					//					cout << "change connection to out" << endl;
					int newCon = randomNum->randInt(outputLayer.size(), 0);
					int con = randomNum->randInt(inputLayer[i]->connections.size(), 0);
					inputLayer[i]->connections[con] = outputLayer[newCon];
					inputLayer[i]->connectionsID[con] = outputLayer[newCon]->neuronID;
					//					cout << "changed connection to out" << endl;
				}
			}
			
		}
	}
	//	cout << "input connections mutated" << endl;

	// mutate recurrent layer
	for (int i = 0; i < recurrentLayer.size(); i++) {
		int amountCon = recurrentLayer[i]->connections.size();
		for (int j = 0; j < amountCon; j++) {
			// add connection
			// - to recurrent layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (amountCon < maxCon && recurrentLayer.size() > 0) {
					//					cout << "add rec con to rec" << endl;
					int con = randomNum->randInt(recurrentLayer.size(), 0);
					recurrentLayer[i]->connections.push_back(recurrentLayer[con]);
					recurrentLayer[i]->connectionWeights.push_back(randomNum->randFloat(0.0, 1.0));
					recurrentLayer[i]->connectionsID.push_back(recurrentLayer[con]->neuronID);
					//					cout << "added rec con to rec " << endl;
				}
			}
			// - to output layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (amountCon < maxCon && outputLayer.size() > 0) {
					//					cout << "adding rec to out" << endl;
					int con = randomNum->randInt(outputLayer.size(), 0);
					recurrentLayer[i]->connections.push_back(outputLayer[con]);
					recurrentLayer[i]->connectionWeights.push_back(randomNum->randFloat(0.0, 1.0));
					recurrentLayer[i]->connectionsID.push_back(outputLayer[con]->neuronID);
					//					cout << "added rec to out" << endl;
				}
			}
			// remove connection
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (recurrentLayer[i]->connections.size() > 0) {
					//					cout << "removing rec con" << endl;
					int con = randomNum->randInt(recurrentLayer[i]->connections.size(), 0);
					recurrentLayer[i]->connections.erase(recurrentLayer[i]->connections.begin() + con);
					recurrentLayer[i]->connectionWeights.erase(recurrentLayer[i]->connectionWeights.begin() + con);
					recurrentLayer[i]->connectionsID.erase(recurrentLayer[i]->connectionsID.begin() + con);
					//					cout << "removed rec con" << endl;
				}
			}
			// change connected neuron
			// - to recurrent layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (recurrentLayer[i]->connections.size() > 0) {
					//					cout << "mutated to rec" << endl;
					int conIn = randomNum->randInt(recurrentLayer[i]->connections.size(), 0);
					int conOut = randomNum->randInt(recurrentLayer.size(), 0);
					recurrentLayer[i]->connections[conIn] = recurrentLayer[conOut];
					recurrentLayer[i]->connectionsID[conIn] = recurrentLayer[conOut]->neuronID;
					//					cout << "to rec mutated" << endl;
				}
			}
			// - to output layer
			if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
				if (recurrentLayer[i]->connections.size() > 0 && outputLayer.size() > 0) {
					//					cout << "mutating to output" << endl;
					int conIn = randomNum->randInt(recurrentLayer[i]->connections.size(), 0);
					int conOut = randomNum->randInt(outputLayer.size(), 0);
					recurrentLayer[i]->connections[conIn] = outputLayer[conOut];
					recurrentLayer[i]->connectionsID[conIn] = outputLayer[conOut]->neuronID;
					//					cout << "mutated to output" << endl;
				}
			}
		
		}
	}
}

void ANN::addNeurons(float mutationRate) {
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		if (recurrentLayer.size() < maxLayerSize) {
			if (settings->verbose) {
				cout << "adding neuron" << endl;
			}
			unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
			int neuronType = randomNum->randInt(2, 0);
			if (neuronType == 1) {
				neuronType = 5; // to be changed in next version
			}
			recurrentLayer.push_back(neuronFactory->createNewNeuronGenome(neuronType, settings));
			recurrentLayer[recurrentLayer.size() - 1]->init(neuronID);
			neuronID++;
			neuronFactory.reset();
			if (randomNum->randFloat(0, 1) < 0.5) {
				int outputConnection = randomNum->randInt(outputLayer.size(), 0);
				recurrentLayer[recurrentLayer.size() - 1]->connectionsID.push_back(outputLayer[outputConnection]->neuronID);
				recurrentLayer[recurrentLayer.size() - 1]->connectionWeights.push_back(randomNum->randFloat(-1.0,1.0));
			}
			else {
				int recConnection = randomNum->randInt(recurrentLayer.size(), 0);
				recurrentLayer[recurrentLayer.size() - 1]->connectionsID.push_back(recurrentLayer[recConnection]->neuronID);
				recurrentLayer[recurrentLayer.size() - 1]->connectionWeights.push_back(randomNum->randFloat(-1.0, 1.0));
			}
			changeConnectionIDToPointer();
			checkConnections();
			//		cout << "added neuron" << endl;
		}
	}
}

void ANN::removeNeurons(float mutationRate) {
	if (randomNum->randFloat(0.0, 1.0) < mutationRate / 2) { // it is less likely that neurons are removed
		if (recurrentLayer.size() > 0) {
			if (settings->verbose) {
				cout << "removing neuron" << endl;
			}
			int randNeur = randomNum->randInt(recurrentLayer.size(), 0);
	//		cout << "Removing neuron nr: " << randNeur << endl;
			int deletedID = recurrentLayer[randNeur]->neuronID;
	//		cout << "deleted neuron ID = " << deletedID << endl;
			recurrentLayer.erase(recurrentLayer.begin() + randNeur);
			for (int j = 0; j < inputLayer.size(); j++) {
				for (int k = 0; k < inputLayer[j]->connectionsID.size(); k++) {
					if (inputLayer[j]->connectionsID[k] == deletedID) {
					//	cout << "Erasing" << endl;
						inputLayer[j]->connectionsID.erase(inputLayer[j]->connectionsID.begin() + k);
						inputLayer[j]->connectionWeights.erase(inputLayer[j]->connectionWeights.begin() + k);
						//	inputLayer[j]->connections.erase(inputLayer[j]->connections.begin() + k);
						k--; // array gets one shorter;
					}
				}
			}
	//		cout << "Neuron Removed" << endl;
			for (int j = 0; j < recurrentLayer.size(); j++) {
				for (int k = 0; k < recurrentLayer[j]->connectionsID.size(); k++) {
					if (recurrentLayer[j]->connectionsID[k] == deletedID) {
						recurrentLayer[j]->connectionsID.erase(recurrentLayer[j]->connectionsID.begin() + k);
						recurrentLayer[j]->connectionWeights.erase(recurrentLayer[j]->connectionWeights.begin() + k);

				//		recurrentLayer[j]->connections.erase(recurrentLayer[j]->connections.begin() + k);
						k--; // array gets one shorter;
							 //	for (int m = 0; m < inputLayer.size(); m++) { 
							 //		for (int n = 0; n < inputLayer[m]->connectionsID.size(); n++) {
							 //			if (inputLayer[m]->connectionsID[n] == deletedID) {
							 //				inputLayer[m]->connectionsID.erase(inputLayer[m]->connectionsID.begin() + n);
							 //				inputLayer[m]->connections.erase(inputLayer[m]->connections.begin() + n);
							 //				j = 0;
							 //			}
							 //		}
							 //	}
					}
				}
			}
		}
	}
}

void ANN::mutate(float mutationRate) {
//	cout << "Mutating ANN" << endl;
	for (int i = 0; i < inputLayer.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			inputLayer[i]->mutate(mutationRate);
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			recurrentLayer[i]->mutate(mutationRate);
		}
	}
	if (settings->verbose) {
		cout << "about to mutate neurons" << endl;
	}
	mutateConnections(mutationRate);
	addNeurons(mutationRate);
	removeNeurons(mutationRate);
//	changeNeurons(mutationRate);//	NOT WORKING?

	changeConnectionIDToPointer();
	checkConnections();
//	cout << "ANN MUTATED" << endl;
}


shared_ptr<Control> ANN::clone() const
{
	shared_ptr<ANN> newANN = make_shared<ANN>(*this); //make a copy of the ANN class with different address/pointer
	newANN->inputLayer.clear();
	newANN->recurrentLayer.clear();
	newANN->outputLayer.clear();
	for (int i = 0; i < this->inputLayer.size(); i++) {
		newANN->inputLayer.push_back(this->inputLayer[i]->clone()); //make a copy of the neuron with the same parameter; clear the input and output
	}
	for (int i = 0; i < this->recurrentLayer.size(); i++) {
		newANN->recurrentLayer.push_back(this->recurrentLayer[i]->clone());
	}
	for (int i = 0; i < this->outputLayer.size(); i++) {
		newANN->outputLayer.push_back(this->outputLayer[i]->clone());
	}
	newANN->checkConnections();
	newANN->changeConnectionIDToPointer();
	newANN->flush();
	return newANN;
}


void ANN::printNeuronValues() {
	std::cout << "printingNeuronValues: " << endl;
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->printNeuron(); 
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->printNeuron();
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i]->printNeuron();
	}
	std::cout << "done printing neuron values" << endl; 
}

stringstream ANN::getControlParams() {
	stringstream ss;
	ss << ",#controlType,0," << endl;
	ss << "\t" << ",#AmountInputNeurons," << inputLayer.size() << ","<< endl;
	for (auto & neuron : inputLayer) {
		ss << "\t\t" << ",#StartNeuron,";
		ss << neuron->getNeuronParams().str();
		ss << "\t\t" << ",#EndNeuron," << endl;
	}
	ss << "\t" << ",#AmountInterNeurons," << recurrentLayer.size() << "," << endl;
	for (auto & neuron : recurrentLayer) {
		ss << "\t\t" << ",#StartNeuron,";
		ss << neuron->getNeuronParams().str();
		ss << "\t\t" << ",#EndNeuron," << endl;
	}
	ss << "\t" << ",#AmountOutputNeurons," << outputLayer.size() << "," << endl;
	for (auto & neuron : outputLayer) {
		ss << "\t\t" << ",#StartNeuron,";
		ss << neuron->getNeuronParams().str();
		ss << "\t\t" << ",#EndNeuron," << endl;
	}
	ss << endl;
//	printNeuronValues();
	return ss;
}

void ANN::setFloatParameters(vector<float> values) {
	// function can be used to manually set specific parameters
	recurrentLayer[0]->setFloatParameters(values);
}

void ANN::setControlParams(vector<string> values) {
	bool checkingNeuron = false;
	vector<string> neuronValues;
	inputLayer.clear();
	recurrentLayer.clear();
	outputLayer.clear();

	for (unsigned long it = 0; it < values.size(); it++) {
		std::string tmp = values[it];
		if (tmp == "#StartNeuron") {
			checkingNeuron = true;
			it++;
			tmp = values[it];
		}
		if (checkingNeuron) {
			neuronValues.push_back(tmp);
		}
		if (tmp == "#EndNeuron") {
			unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
			int neuronType = atoi(neuronValues[0].c_str());
			shared_ptr<Neuron> neuron = neuronFactory->createNewNeuronGenome(neuronType, settings);
			neuron->setNeuronParams(neuronValues);
			if (neuronType == 1) {
				inputLayer.push_back(neuron);
			}
			else if (neuronType == 2) {
				outputLayer.push_back(neuron);
			}
			else {
				recurrentLayer.push_back(neuron);
			}		
			
			neuronValues.clear();
			checkingNeuron = false; 
		}
	}
	changeConnectionIDToPointer();
//	printNeuronValues();
}

void ANN::cloneControlParameters(shared_ptr<Control> parent) {
	unique_ptr<NeuronFactory> neuronFactory(new NeuronFactory);
	//inputLayer.push_back();
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i] = inputLayer[i]->clone();
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i] = recurrentLayer[i]->clone();
	}
	for (int i = 0; i < outputLayer.size(); i++) {
		outputLayer[i] = outputLayer[i]->clone();
	}
	neuronFactory.reset();
	changeConnectionIDToPointer();
	checkConnections();
}

void ANN::changeConnectionIDToPointer() {
	//	cout << "changing connection ID to pointer" << endl;
	// input layer can connect to recurrent and output layer
	for (int i = 0; i < inputLayer.size(); i++) {
		if (inputLayer[i]->connections.size() != inputLayer[i]->connectionsID.size()) {
			if (settings->verbose) {
                cout << "ERROR: connections" << i << ".size() = " << inputLayer[i]->connections.size();
                cout << ", while: connectionsID" << i << ".size() = " << inputLayer[i]->connectionsID.size() << endl;
            }
		}
		inputLayer[i]->connections.resize(inputLayer[i]->connectionsID.size());  //TODO:??
		for (int j = 0; j < inputLayer[i]->connectionsID.size(); j++) {
			for (int k = 0; k < recurrentLayer.size(); k++) {
				if (inputLayer[i]->connectionsID[j] == recurrentLayer[k]->neuronID) {
					inputLayer[i]->connections[j] = recurrentLayer[k];
					if (recurrentLayer[k] == nullptr) {
                        if (settings->verbose) {
                            cout << "ERROR: recurrent layer " << k << " = NULL" << endl;
                        }
					}
				}
			}
			for (int k = 0; k < outputLayer.size(); k++) {
				if (inputLayer[i]->connectionsID[j] == outputLayer[k]->neuronID) {
					inputLayer[i]->connections[j] = outputLayer[k];
					if (outputLayer[k] == nullptr) {
                        if (settings->verbose) {
                            cout << "ERROR: output layer " << k << " = NULL" << endl;
                        }
					}
				}
			}
			if (inputLayer[i]->connections[j] == nullptr) {
                if (settings->verbose) {
                    cout << "ERROR: still inputLayer[" << i << "]->connections[" << j << "]" << " = NULL" << endl;
                }
			}
		}
	}
	// recurrent layer only connects to recurrent layer
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->connections.resize(recurrentLayer[i]->connectionsID.size());
		for (int j = 0; j < recurrentLayer[i]->connectionsID.size(); j++) {
			for (int k = 0; k < recurrentLayer.size(); k++) {
				if (recurrentLayer[i]->connectionsID[j] == recurrentLayer[k]->neuronID) {
					recurrentLayer[i]->connections[j] = recurrentLayer[k];
					if (recurrentLayer[k] == nullptr) {
                        if (settings->verbose) {
                            cout << "ERROR: recurrent layer " << k << " = NULL" << endl;
                        }
					}
				}
			}
			for (int k = 0; k < outputLayer.size(); k++) {
				if (recurrentLayer[i]->connectionsID[j] == outputLayer[k]->neuronID) {
					recurrentLayer[i]->connections[j] = outputLayer[k];
					if (outputLayer[k] == nullptr) {
                        if (settings->verbose) {
                            cout << "ERROR: output layer " << k << " = NULL" << endl;
                        }
					}
				}
			}
			if (recurrentLayer[i]->connections[j] == nullptr) {
                if (settings->verbose) {
                    cout << "ERROR: recurrentLayer[" << i << "]->connections[" << j << "]" << " = NULL" << endl;
                }
			}
		}
	}
	//	cout << "changed connection id to pointer" << endl;
}

void ANN::changeConnectionPointerToID()
{
	//	cout << "changing connection ID to pointer" << endl;
	for (int i = 0; i < inputLayer.size(); i++) {
		inputLayer[i]->connections.resize(inputLayer[i]->connectionsID.size());
		for (int j = 0; j < inputLayer[i]->connectionsID.size(); j++) {
			for (int k = 0; k < recurrentLayer.size(); k++) {
				if (inputLayer[i]->connections[j] == recurrentLayer[k]) {
					inputLayer[i]->connectionsID[j] = recurrentLayer[k]->neuronID;
					
				}
			}
			for (int k = 0; k < outputLayer.size(); k++) {
				if (inputLayer[i]->connections[j] == outputLayer[k]) {
					inputLayer[i]->connectionsID[j] = outputLayer[k]->neuronID;
					
				}
			}
		}
	}
	for (int i = 0; i < recurrentLayer.size(); i++) {
		recurrentLayer[i]->connections.resize(recurrentLayer[i]->connectionsID.size());
		for (int j = 0; j < recurrentLayer[i]->connectionsID.size(); j++) {
			for (int k = 0; k < recurrentLayer.size(); k++) {
				if (recurrentLayer[i]->connections[j] == recurrentLayer[k]) {
					recurrentLayer[i]->connectionsID[j] = recurrentLayer[k]->neuronID;
				}
			}
			for (int k = 0; k < outputLayer.size(); k++) {
				if (recurrentLayer[i]->connections[j] == outputLayer[k]) {
					recurrentLayer[i]->connectionsID[j] = outputLayer[k]->neuronID;
				}
			}
		}
	}
	//	cout << "changed connection id to pointer" << endl;
}

bool ANN::checkControl(const std::vector<std::string> &values)
{
	bool checkingNeuron = false;
	vector<string> neuronValues;

	int inputCheck = 0;
	int interCheck = 0;
	int outputCheck = 0;
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#StartNeuron") {
			checkingNeuron = true;
			it++;
			tmp = values[it];
		}
		if (checkingNeuron) {
			neuronValues.push_back(tmp);
		}
		if (tmp == "#EndNeuron") {
			int neuronType = atoi(neuronValues[0].c_str());
			if (neuronType == 0 || neuronType == 3 || neuronType == 5) {
				if (recurrentLayer[interCheck]->checkNeuron(neuronValues) == false) {
					return false;
				}
				interCheck++;
			}
			else if (neuronType == 1) {
				if (inputLayer[inputCheck]->checkNeuron(neuronValues) == false) {
					return false;
				}
				inputCheck++;
			}
			else if (neuronType == 2) {
				if (outputLayer[outputCheck]->checkNeuron(neuronValues) == false) {
					return false;
				}
				outputCheck++;
			}
			checkingNeuron = false;
			neuronValues.clear();
		}
	}
	return true;
}