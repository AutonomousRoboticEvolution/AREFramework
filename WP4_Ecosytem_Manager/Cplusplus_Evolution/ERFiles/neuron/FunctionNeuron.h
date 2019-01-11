#pragma once
#include <vector>
#include <memory>
#include "Neuron.h"

using namespace std;

class FunctionNeuron :
	public Neuron
{
public:
	enum FunctionType
	{
		AF_SIGMOID_UNSIGNED = 0,
		AF_SIGMOID_SIGNED = 1,
		AF_TANH = 2,
		AF_TANH_CUBIC = 3,
		AF_STEP_SIGNED = 4,
		AF_STEP_UNSIGNED = 5,
		AF_GAUSS_SIGNED = 6,
		AF_GAUSS_UNSIGNED = 7,
		AF_ABS = 8,
		AF_SINE_SIGNED = 9,
		AF_SINE_UNSIGNED = 10,
		AF_LINEAR = 11,
		AF_RELU = 12,
		AF_SOFTPLUS = 13, 
		UNSIGNED_SIGMOID_DERIVATIVE = 14,
		TANH_DERIVATIVE = 15
	};
	
	FunctionNeuron();
	~FunctionNeuron();
	void init(int neuronID);
	void update();
	void reset();
	void mutate(float mutationRate);
	void setFloatParameters(vector<float> values);
	float m_a = 1;
	float m_b = 0;
	float m_activation = 0;
	float sigma = 0.2;
	int function_type = AF_SIGMOID_UNSIGNED;
	float transformFunction(float x , float a, float b);
//	void setFloatParameters(vector<float> values) {};

	int functionRange = 14; // between 0 and 13 are used

	bool dir = true;
	stringstream getNeuronParams();
	//	void setNeuronParams(vector<string>);
	shared_ptr<Neuron> clone();
	void setNeuronParams(vector<string> values);
	bool checkNeuron(vector<string>);
private:
	
};



