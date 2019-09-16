#include "NeuronFactory.h"
#include <iostream>
#include <memory>
using namespace std;

NeuronFactory::NeuronFactory()
{
}


NeuronFactory::~NeuronFactory()
{
	
}

shared_ptr<Neuron> NeuronFactory::createNewNeuronGenome(int neuronType, shared_ptr<Settings> st) {
	switch (neuronType) {
	case 0:
	{
		shared_ptr<Neuron> simpleNeuron(new SimpleNeuron);
		simpleNeuron->settings = st;
		return simpleNeuron;
	}
	case 1:
	{
		shared_ptr<Neuron> inputNeuron(new InputNeuron);
		inputNeuron->settings = st;
		return inputNeuron;
	}
	case 2:
	{
		shared_ptr<Neuron> outputNeuron(new OutputNeuron);
		outputNeuron->settings = st;
		return outputNeuron;
	}
	case 3:
	{
		shared_ptr<Neuron> cpgNeuron(new CPGNeuron);
		cpgNeuron->settings = st;
		return cpgNeuron;
	}
	case 4:
	{
		shared_ptr<Neuron> functionNeuron(new FunctionNeuron);
		functionNeuron->settings = st;
		return functionNeuron;
	}
	case 5:
	{
		shared_ptr<Neuron> neur(new PatternNeuron);
		neur->settings = st;
		return neur;
	}
	default:
	{
		shared_ptr<Neuron> simpleNeuron(new SimpleNeuron);
		simpleNeuron->settings = st;
		return simpleNeuron;
	}
	}
}


shared_ptr<Neuron> NeuronFactory::copyNeuronGenome(shared_ptr<Neuron> parent) 
{
	shared_ptr<Neuron> cloneModule;
	cloneModule = parent->clone();
	return cloneModule;
}