#pragma once

#include <vector>
#include <memory>
#include "Neuron.h"


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
    std::shared_ptr<Neuron> clone();
    std::stringstream getNeuronParams();
	void setNeuronParams(std::vector<std::string> values);
	bool checkNeuron(std::vector<std::string>);
	void setFloatParameters(std::vector<float> values) {};
	virtual void flush();
};



