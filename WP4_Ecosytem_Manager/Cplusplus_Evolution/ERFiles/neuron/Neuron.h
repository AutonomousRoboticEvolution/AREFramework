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
	virtual void mutate(float mutationRate) = 0; 
	vector<shared_ptr<Neuron> > connections;
	vector<float> connectionWeights;
	vector<int> connectionsID;
	void printNeuron(); 
	float input = 0;
	float output;
	float threshold;
	float sigma = 0.2;
	virtual stringstream getNeuronParams() = 0; 
	virtual void setNeuronParams(vector<string>) = 0; 
	virtual bool checkNeuron(vector<string>) = 0;
	virtual void setFloatParameters(vector<float> values) = 0;
	int neuronID;
	int sceneNum;
	bool conserved = false;
protected:
	
};


