#include "FunctionNeuron.h"
#include <iostream>
#include <cmath>


inline double af_sigmoid_unsigned(double aX, double aSlope, double aShift)
{
	return 1.0 / (1.0 + exp(-aSlope * aX - aShift));
}

inline double af_sigmoid_signed(double aX, double aSlope, double aShift)
{
	double tY = af_sigmoid_unsigned(aX, aSlope, aShift);
	return (tY - 0.5) * 2.0;
}

inline double af_tanh(double aX, double aSlope, double aShift)
{
	return tanh(aX * aSlope);
}

inline double af_tanh_cubic(double aX, double aSlope, double aShift)
{
	return tanh(aX * aX * aX * aSlope);
}

inline double af_step_signed(double aX, double aShift)
{
	double tY;
	if (aX > aShift)
	{
		tY = 1.0;
	}
	else
	{
		tY = -1.0;
	}

	return tY;
}

inline double af_step_unsigned(double aX, double aShift)
{
	if (aX > (0.5 + aShift))
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

inline double af_gauss_signed(double aX, double aSlope, double aShift)
{
	double tY = exp(-aSlope * aX * aX + aShift); // TODO: Need separate a, b per activation function
	return (tY - 0.5)*2.0;
}

inline double af_gauss_unsigned(double aX, double aSlope, double aShift)
{
	return exp(-aSlope * aX * aX + aShift);
}

inline double af_abs(double aX, double aShift)
{
	return ((aX + aShift)< 0.0) ? -(aX + aShift) : (aX + aShift);
}

inline double af_sine_signed(double aX, double aFreq, double aShift)
{
	return sin(aX * aFreq + aShift);
}

inline double af_sine_unsigned(double aX, double aFreq, double aShift)
{
	double tY = sin((aX * aFreq + aShift));
	return (tY + 1.0) / 2.0;
}


inline double af_linear(double aX, double aShift)
{
	return (aX + aShift);
}


inline double af_relu(double aX)
{
	return (aX > 0) ? aX : 0;
}


inline double af_softplus(double aX)
{
	return log(1 + exp(aX));
}


double unsigned_sigmoid_derivative(double x)
{
	return x * (1 - x);
}

double tanh_derivative(double x)
{
	return 1 - x * x;
}


FunctionNeuron::FunctionNeuron() {

}

FunctionNeuron::~FunctionNeuron() {
	//	cout << "DELETED CONTROL" << endl; 
}

void FunctionNeuron::init(int id) {
	neuronID = id;
	function_type = randomNum->randInt(14, 0);
	m_a = randomNum->randFloat(-1.0, 1.0);
	m_b = randomNum->randFloat(-1.0, 1.0);
}

void FunctionNeuron::update() {
	//m_b = input;
	m_activation = input;
	output = transformFunction(m_activation, m_a, m_b);
	for (int i = 0; i < connections.size(); i++) {
		connections[i]->input += output;
	}
	input = 0;
}

void FunctionNeuron::reset() {
	input = 0.0;
	output = 0.0;
}

shared_ptr<Neuron> FunctionNeuron::clone() {
	shared_ptr<FunctionNeuron> thisNeuron = make_unique<FunctionNeuron>(*this);
	thisNeuron->input = 0;
	thisNeuron->m_activation = 0;
	thisNeuron->output = 0;
	return thisNeuron;
}

void FunctionNeuron::mutate(float mutationRate) {
	threshold = randomNum->randFloat(-1.0, 1.0);
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		function_type = randomNum->randInt(functionRange, 0);
	}
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		if (randomNum->randFloat(0.0, 1.0) > 0.5) {
			m_a += randomNum->randFloat(-1.0 * sigma, 1.0 * sigma);
		}
		else {
			m_a += randomNum->randFloat(-1.0, 1.0);
		}
	}
	if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
		if (randomNum->randFloat(0.0, 1.0) > 0.5) { // local uniform based on sigma
			m_b += randomNum->randFloat(-1.0 * sigma, 1.0 * sigma);
		}
		else { // random
			m_b += randomNum->randFloat(-1.0, 1.0);
		}
	}
	if (m_a > 1.0) {
		m_a = 1.0;
	}
	else if (m_a < -1.0) {
		m_a = -1.0;
	}
	if (m_b > 1.0) {
		m_b = 1.0;
	}
	else if (m_b < -1.0) {
		m_b = -1.0;
	}
	Neuron::mutate(mutationRate);
}

void FunctionNeuron::setFloatParameters(vector<float> values) {
	// function can be used to manually set specific parameters
	
}

stringstream FunctionNeuron::getNeuronParams() {
	stringstream ss;
	ss << "4," << endl;
	ss << Neuron::getNeuronParams().str();
	ss << "\t" << ",#f_t," << to_string(function_type) << "," << endl;
	ss << "\t" << ",#m_a," << to_string(m_a) << "," << endl;
	ss << "\t" << ",#m_b," << to_string(m_b) << "," << endl;
	return ss;
}

void FunctionNeuron::setNeuronParams(vector<string> values) {
	Neuron::setNeuronParams(values);
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#f_t") {
			it++;
			tmp = values[it];
			function_type = atoi(tmp.c_str());
		}
		else if (tmp == "#m_a") {
			it++;
			tmp = values[it];
			m_a = atof(tmp.c_str());
		}
		else if (tmp == "#m_b") {
			it++;
			tmp = values[it];
			m_b = atof(tmp.c_str());
		}
	}
}

bool FunctionNeuron::checkNeuron(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		
	}
	return Neuron::checkNeuron(values);
}

float FunctionNeuron::transformFunction(float x, float a, float b) {
	float y;
	switch (function_type) {
	case AF_SIGMOID_UNSIGNED:
		y = af_sigmoid_unsigned(x, a, b);
	case AF_SIGMOID_SIGNED:
		y = af_sigmoid_signed(x, a, b);
	case AF_TANH:
		y = af_tanh(x, a, b);
	case AF_TANH_CUBIC:
		y = af_tanh_cubic(x, a, b);
	case AF_STEP_SIGNED:
		y = af_step_signed(x, b);
	case AF_STEP_UNSIGNED:
		y = af_step_unsigned(x, b);
	case AF_GAUSS_SIGNED:
		y = af_gauss_signed(x, a, b);
	case AF_GAUSS_UNSIGNED:
		y = af_gauss_unsigned(x, a, b);
	case AF_ABS:
		y = af_abs(x, b);
	case AF_SINE_SIGNED:
		y = af_sine_signed(x, a, b);
	case AF_SINE_UNSIGNED:
		y = af_sine_unsigned(x, a, b);
	case AF_LINEAR:
		y = af_linear(x, b);
	case AF_RELU:
		y = af_relu(x);
	case AF_SOFTPLUS:
		y = af_softplus(x);
	case UNSIGNED_SIGMOID_DERIVATIVE: // not used in multineat?
		y = unsigned_sigmoid_derivative(x);
	case TANH_DERIVATIVE: // not used in multineat?
		y = tanh_derivative(x);
	default:
		y = af_sigmoid_unsigned(x, a, b);
	}
	return y;
}