#pragma once

#include <vector>
#include "Control.h"
#include "../neuron/NeuronFactory.h"
#include "../neuron/Neuron.h"


class ANN :
	public Control
{
public:
	ANN();
	~ANN();
	void init(int input, int inter, int output);
    std::vector<float> update(std::vector<float>);
	void mutate(float mutationRate); 
	void printNeuronValues();
    std::shared_ptr<Control> clone() const;
	//void makeDependenciesUnique();
	/// neuron ID
	int neuronID = 0;
	/// neuron type
	int type = 0;
	/// check the connection between neurons and remove unconnected connection
	void checkConnections();
    std::stringstream getControlParams();
	void setFloatParameters(std::vector<float> values);
	void setControlParams(std::vector<std::string>);
	/// map the connection ID into reference
	void changeConnectionIDToPointer();
	/// clone the parent's neuron parameter
	void cloneControlParameters(std::shared_ptr<Control> parent);
	/// maximum number size of neuron for each layer
	int maxLayerSize;
    /// maximum number of connection
	int maxCon = 8;
	bool checkControl(std::vector<std::string> values);
	/// map the neuron reference connection pointer into neuron ID
	void changeConnectionPointerToID();
	/// destroy the neuron of the ANN
	void reset();
	/// add the inputs into the ANN
	void addInput(std::vector<float> input);
	/// clear value of the neuron
	void flush();
	/// change connection through adding, removing or switching
	void mutateConnections(float mutationRate);
	/// add a neuron in the recurrent layer
	void addNeurons(float mutationRate);
	/// delete a neuron from the recurrent layer
	void removeNeurons(float mutationRate);   //another operator: replace neuron

	/// a vector of input neurons
    std::vector<std::shared_ptr<Neuron>> inputLayer;
    /// a vector of output neurons
    std::vector<std::shared_ptr<Neuron>> outputLayer;
    /// a vector of hidden neurons
    std::vector<std::shared_ptr<Neuron>> recurrentLayer;
};




