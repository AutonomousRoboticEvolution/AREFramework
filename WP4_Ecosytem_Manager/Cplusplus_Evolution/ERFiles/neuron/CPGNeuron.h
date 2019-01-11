#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

using namespace std;

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
	bool dir = true; 
	stringstream getNeuronParams(); 
//	void setNeuronParams(vector<string>);
	shared_ptr<Neuron> clone(); 
	void setNeuronParams(vector<string> values);
	bool checkNeuron(vector<string>);
	void setFloatParameters(vector<float> values) {};
private:
	float innerValue = 0; 
};



