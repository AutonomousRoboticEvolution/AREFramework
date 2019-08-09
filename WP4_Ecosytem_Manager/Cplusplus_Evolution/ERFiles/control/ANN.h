#pragma once
#include "Control.h"
#include "../neuron/NeuronFactory.h"
#include "../neuron/Neuron.h"
#include <vector>

class ANN :
	public Control
{
public:
	ANN();
	~ANN();
	void init(int input, int inter, int output);
	vector<float> update(vector<float>);
	void mutate(float mutationRate); 
	void printNeuronValues(); 
	shared_ptr<Control> clone() const;
	//void makeDependenciesUnique();
	/// neuron ID
	int neuronID = 0;
	/// neuron type
	int type = 0;
	/// check the connection between neurons and remove unconnected connection
	void checkConnections(); 
	stringstream getControlParams();
	void setFloatParameters(vector<float> values);
	void setControlParams(vector<string>);
	/// map the connection ID into reference
	void changeConnectionIDToPointer();
	/// clone the parent's neuron parameter
	void cloneControlParameters(shared_ptr<Control> parent);
	/// maximum number size of neuron for each layer
	int maxLayerSize;
	///
	int maxCon;
	/// read the topology from CSV file and check if it is correct
	bool checkControl(vector<string> values);
	void changeConnectionPointerToID();
	/// destroy the neuron of the ANN
	void reset();
	/// add the inputs into the ANN
	void addInput(vector<float> input);
	/// clear value of the neuron
	void flush();
	/// change connection through adding, removing or switching
	void mutateConnections(float mutationRate);
	/// add a neuron in the recurrent layer
	void addNeurons(float mutationRate);
	/// delete a neuron from the recurrent layer
	void removeNeurons(float mutationRate);
	void changeNeurons(float mutationRate);

	/// a vector of input neurons
	vector<shared_ptr<Neuron>> inputLayer;
    /// a vector of output neurons
	vector<shared_ptr<Neuron>> outputLayer;
    /// a vector of hidden neurons
	vector<shared_ptr<Neuron>> recurrentLayer;
};




