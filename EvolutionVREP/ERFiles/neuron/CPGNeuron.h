#pragma once

#include <vector>
#include <memory>
#include "Neuron.h"


class CPGNeuron : 
	public Neuron
{
public:
	CPGNeuron(); 
	~CPGNeuron(); 
	void init(int neuronID); 
	void update();
	void reset();
	void mutate(float mutationRate);
	float cycleSpeed = 0.1f;
	float cycleRange = 1.0f; 
	virtual void flush();
	bool dir = true;
	std::stringstream getNeuronParams();
//	void setNeuronParams(vector<string>);
    std::shared_ptr<Neuron> clone();
	void setNeuronParams(std::vector<std::string> values);
	bool checkNeuron(std::vector<std::string>);
	void setFloatParameters(std::vector<float> values) {};
private:
	float innerValue = 0; 
};



