#pragma once
#include "Neuron.h"
#include "SimpleNeuron.h"
#include "InputNeuron.h"
#include "OutputNeuron.h"
#include "CPGNeuron.h"
#include "FunctionNeuron.h"
#include "PatternNeuron.h"
#include "../Settings.h"

class NeuronFactory
{
public:
	NeuronFactory();
	~NeuronFactory();
	shared_ptr<Neuron> createNewNeuronGenome(int neuronType, shared_ptr<Settings> st);
	shared_ptr<Neuron> copyNeuronGenome(shared_ptr<Neuron> parentNeuron);

};

