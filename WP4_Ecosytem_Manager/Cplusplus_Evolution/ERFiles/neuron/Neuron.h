#pragma once
#include <vector>
#include <memory>
#include <sstream>
#include "../../RandNum.h"
#include "../Settings.h"
using namespace std;

class Neuron // abstract class
{
public:
	shared_ptr<Settings> settings;
	Neuron(){};
	virtual ~Neuron() {};
	shared_ptr<RandNum> randomNum;
	virtual void init(int neuronID) = 0; 
	virtual void update() = 0;
	virtual shared_ptr<Neuron> clone() = 0;
	virtual void reset() = 0;
    /**
    @brief Mutate the neuron
    @param mutationRate mutation rate
    */
	virtual void mutate(float mutationRate) = 0;
	/// store a vector of pointer to which neuron connected to
	vector<shared_ptr<Neuron> > connections;
	/// specify the connection weights between neurons
	vector<float> connectionWeights;
	/// neuron ID of the connection
	vector<int> connectionsID;
	void printNeuron();
	/// input value of this neuron; can be accumulated
	float input = 0;
	/// output value of this neuron; can be accumulated
	float output;
	float threshold;
	float sigma = 0.2;
	virtual stringstream getNeuronParams() = 0; 
	virtual void setNeuronParams(vector<string>) = 0; 
	virtual bool checkNeuron(vector<string>) = 0;
	virtual void setFloatParameters(vector<float> values) = 0;
	/// unique ID of neuron
	int neuronID;
	int sceneNum;
	bool conserved = false;
	/// reset the value of input and output into 0
	virtual void flush() = 0;
protected:
	
};



