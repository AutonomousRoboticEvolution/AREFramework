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
	/// assign ID to the neuron
	void init(int neuronID);
	/// update the weights of this neuron
	void update();
	/// remove the neuron
	void reset();
	/// mutate the neuron
	void mutate(float mutationRate); 
	float transformValue = 0; 
	shared_ptr<Neuron> clone(); 
	stringstream getNeuronParams();
	void setNeuronParams(vector<string>);
	bool checkNeuron(vector<string>);
	void setFloatParameters(vector<float> values) {};
	virtual void flush();
};



