#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

using namespace std;

class OutputNeuron : 
	public Neuron
{
public:
	OutputNeuron(); 
	~OutputNeuron(); 
	void init(int neuronID); 
	void update();
	void reset();
	void mutate(float mutationRate);
	shared_ptr<Neuron> clone(); 
	stringstream getNeuronParams();
	void setNeuronParams(vector<string>);
	bool checkNeuron(vector<string>);
	void setFloatParameters(vector<float> values) {};


};



