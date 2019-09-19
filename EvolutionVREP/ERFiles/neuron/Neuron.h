#pragma once

#include <vector>
#include <memory>
#include <sstream>
#include "../../RandNum.h"
#include "../Settings.h"


class Neuron // abstract class
{
public:
    std::shared_ptr<Settings> settings;
	Neuron(){};
	virtual ~Neuron() {};
    std::shared_ptr<RandNum> randomNum;
    /**
    @brief Assign each neuron an unique ID
    @param neuronID ID of the neuron
    */
	virtual void init(int neuronID) = 0;
	/// update the neuron based on connection weights and input
	virtual void update() = 0;
	/// clone a neuron parameter to another neuron
	virtual std::shared_ptr<Neuron> clone() = 0;
	/// remove the neuron
	virtual void reset() = 0;
    /**
    @brief Mutate connection weight of neuron
    @param mutationRate mutation rate
    */
	virtual void mutate(float mutationRate) = 0;
	/// store a vector of pointer to which neuron connected to
    std::vector<std::shared_ptr<Neuron> > connections;
	/// specify the connection weights between neurons
    std::vector<float> connectionWeights;
	/// neuron ID of the connection
    std::vector<int> connectionsID;
	void printNeuron();
	/// input value of this neuron; can be accumulated
	float input = 0;
	/// output value of this neuron; can be accumulated
	float output;
	/// activation value of the neuron
	float threshold;
	/// the upper bound of a random-generated float
	float sigma = 0.2;
	virtual std::stringstream getNeuronParams() = 0;
	virtual void setNeuronParams(std::vector<std::string>) = 0;
	/// check if the connection is correct
	virtual bool checkNeuron(std::vector<std::string>) = 0;
	virtual void setFloatParameters(std::vector<float> values) = 0;
	/// unique ID of neuron
	int neuronID;
	/// reset the value of input and output into 0
	virtual void flush() = 0;
protected:
	
};



