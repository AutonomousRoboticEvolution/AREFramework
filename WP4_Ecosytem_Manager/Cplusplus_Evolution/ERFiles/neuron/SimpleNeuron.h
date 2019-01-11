#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

using namespace std;

class SimpleNeuron : 
	public Neuron
{
public:
	SimpleNeuron(); 
	~SimpleNeuron(); 
	void init(int neuronID); 
	void update();
	void reset();
	void mutate(float mutationRate);
	float outputWeight = 1.0; 
	shared_ptr<Neuron> clone(); 
	stringstream getNeuronParams();
	void setNeuronParams(vector<string> values);
	bool checkNeuron(vector<string>);
	void setFloatParameters(vector<float> values) {};

};



