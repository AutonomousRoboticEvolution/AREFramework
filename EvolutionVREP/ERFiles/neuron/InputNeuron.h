#pragma once

#include <vector>
#include <memory>
#include "Neuron.h"


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
	std::shared_ptr<Neuron> clone();
	std::stringstream getNeuronParams();
	void setNeuronParams(std::vector<std::string>);
	bool checkNeuron(std::vector<std::string>);
	void setFloatParameters(std::vector<float> values) {};
	virtual void flush();
};



