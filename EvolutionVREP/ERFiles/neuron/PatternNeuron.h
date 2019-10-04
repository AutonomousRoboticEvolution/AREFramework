#pragma once

#include <vector>
#include <memory>
#include "Neuron.h"


class PatternNeuron : 
	public Neuron
{
public:
	PatternNeuron(); 
	~PatternNeuron();
	void setFloatParameters(std::vector<float> values);

	void init(int neuronID); 
	void update();
	void reset();
	void mutate(float mutationRate);
	float frequency = 0.5;
	float amplitude = 0.0;
	float phase = 0.0;
    std::stringstream getNeuronParams();
//	void setNeuronParams(vector<string>);
    std::shared_ptr<Neuron> clone();
	void setNeuronParams(std::vector<std::string> values);
	bool checkNeuron(std::vector<std::string>);
	virtual void flush();
private:
	float time = 0.0; // initial
	float timeStep = 0.05f;
	float maxFreq = 2.5f;// 1.0f;// 2.5f;
};



