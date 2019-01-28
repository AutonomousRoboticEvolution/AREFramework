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
	int neuronID = 0; 
	int type = 0; 
	void checkConnections(); 
	stringstream getControlParams();
	void setFloatParameters(vector<float> values);
	void setControlParams(vector<string>);
	void changeConnectionIDToPointer(); 
	void cloneControlParameters(shared_ptr<Control> parent);
	int maxLayerSize;
	int maxCon;
	bool checkControl(vector<string> values);
	void changeConnectionPointerToID();
	void reset();
	void addInput(vector<float> input);
	void leaky(float leakRate);
	
	void mutateConnections(float mutationRate);
	void addNeurons(float mutationRate);
	void removeNeurons(float mutationRate);
	void changeNeurons(float mutationRate);
//	float tmpMutRate = 0.05; //delete!!!

	/*struct Connection {
		float weight;
		float in; 
		float out; 
	};*/

	vector<float> in;
	vector<float> out; 
	vector<shared_ptr<Neuron>> inputLayer;
	vector<shared_ptr<Neuron>> outputLayer;
	// TODO recurrentLayer -> vector<vector<shared_ptr<Neuron>>> interLayer 
	vector<shared_ptr<Neuron>> recurrentLayer;
//	vector<shared_ptr<Neuron>> allNeurons; 
	//vector<Connection> connections;
};

