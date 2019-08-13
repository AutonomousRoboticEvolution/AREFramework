#pragma once
#include <vector>
#include <memory>
#include <sstream>
#include "../../RandNum.h"
#include "../Settings.h"
using namespace std;

/// abstract class
class Control
{
public:
	Control(){};
	virtual ~Control();
    /**
    @brief Specify the number of neurons in input layer, hidden layer, and output layer
    @param input number of neuron in input layer
    @param inter number of neuron in hidden layer (only one hidden layer)
    @param output number of neuron in output layer
    */
	virtual void init(int input, int inter, int output) = 0;
    /**
    @brief Take sensorInput and returns output
    @param sensorValues sensor values
    */
	virtual vector<float> update(vector<float> sensorValues) = 0;

	virtual void mutate(float mutationRate) = 0;
	/// deep copy the ANN recursively
	virtual shared_ptr<Control> clone() const = 0;
	virtual stringstream getControlParams() = 0; 
	virtual void setControlParams(vector<string>) = 0;
    /// clone the parent's neuron parameter
	virtual void cloneControlParameters(shared_ptr<Control> parent) = 0;
    /// read the topology from CSV file and check if it is correct
	virtual bool checkControl(vector<string> values) = 0;
	virtual void reset() = 0;
	virtual void flush() = 0;
	//virtual void makeDependenciesUnique() = 0;
	virtual void addInput(vector<float> input) = 0;
	float cf = 1.0;
	shared_ptr<RandNum> randomNum;
	shared_ptr<Settings> settings;
	virtual void setFloatParameters(vector<float> values) = 0;
//	virtual void deleteControl() = 0;
//	vector<ANN> aNN;
//	vector<FixedPosition> fixedControl;
};




