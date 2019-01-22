#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

using namespace std;

class PatternNeuron : 
	public Neuron
{
public:
	PatternNeuron(); 
	~PatternNeuron();
	void setFloatParameters(vector<float> values);

	void init(int neuronID); 
	void update();
	void reset();
	void mutate(float mutationRate);
	float frequency = 0.5;
	float amplitude = 0.0;
	float phase = 0.0;
	stringstream getNeuronParams(); 
//	void setNeuronParams(vector<string>);
	shared_ptr<Neuron> clone(); 
	void setNeuronParams(vector<string> values);
	bool checkNeuron(vector<string>);
private:
	float time = 0; // initial
	float timeStep = 0.05f;
	float maxFreq = 2.5f;// 1.0f;// 2.5f;
};



