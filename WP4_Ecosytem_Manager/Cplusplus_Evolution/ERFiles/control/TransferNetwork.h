#pragma once
#include "Control.h"
#include "..\\neuron\NeuronFactory.h"
#include "..\\neuron\Neuron.h"
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
	shared_ptr<Control> clone();
	int neuronID = 0; 
	int type = 0; 
	void checkConnections(); 
	stringstream getControlParams();
	void setControlParams(vector<string>);
	void changeConnectionIDToPointer(); 
	void cloneControlParameters(shared_ptr<Control> parent);

	/*struct Connection {
		float weight;
		float in; 
		float out; 
	};*/

	vector<float> in;
	vector<float> out; 
	vector<shared_ptr<Neuron>> inputLayer;
	vector<shared_ptr<Neuron>> outputLayer;
	vector<shared_ptr<Neuron>> recurrentLayer;
//	vector<shared_ptr<Neuron>> allNeurons; 
	//vector<Connection> connections;
};

