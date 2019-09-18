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
    std::shared_ptr<Neuron> createNewNeuronGenome(int neuronType, std::shared_ptr<Settings> st);
    std::shared_ptr<Neuron> copyNeuronGenome(std::shared_ptr<Neuron> parentNeuron);

};

