#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

using namespace std;

class InputNeuron : 
	public Neuron
{
public:
	InputNeuron(); 
	~InputNeuron(); 
	void init(int neuronID); 
	void update();
	void reset();
	void mutate(float mutationRate); 
	float transformValue = 0; 
	shared_ptr<Neuron> clone(); 
	stringstream getNeuronParams();
	void setNeuronParams(vector<string>);
	bool checkNeuron(vector<string>);
	void setFloatParameters(vector<float> values) {};

};



