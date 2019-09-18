#pragma once

#include <vector>
#include <memory>
#include "Neuron.h"


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
	std::shared_ptr<Neuron> clone();
    std::stringstream getNeuronParams();
	void setNeuronParams(std::vector<std::string>);
	bool checkNeuron(std::vector<std::string>);
	void setFloatParameters(std::vector<float> values) {};
	/// clear the input and output value of this neuron
	virtual void flush();

};



