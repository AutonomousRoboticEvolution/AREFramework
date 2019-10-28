#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <vector>
#include <memory>
#include <sstream>
#include "misc/RandNum.h"
#include "ARE/Settings.h"


/// abstract class
class Control
{
public:

    typedef std::shared_ptr<Control> (Factory)
        (int,std::shared_ptr<RandNum>, std::shared_ptr<Settings>);

    Control(){}
    virtual ~Control(){}
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
	virtual std::vector<float> update(std::vector<float> sensorValues) = 0;

	virtual void mutate(float mutationRate) = 0;
	/// deep copy the ANN recursively
	virtual std::shared_ptr<Control> clone() const = 0;
	virtual std::stringstream getControlParams() = 0;
	virtual void setControlParams(std::vector<std::string>) = 0;
    /// clone the parent's neuron parameter
	virtual void cloneControlParameters(std::shared_ptr<Control> parent) = 0;
    /// read the topology from CSV file and check if it is correct
	virtual bool checkControl(const std::vector<std::string> &values) = 0;
	virtual void reset() = 0;
	virtual void flush() = 0;
	//virtual void makeDependenciesUnique() = 0;
	virtual void addInput(std::vector<float> input) = 0;
	float cf = 1.0;
    std::shared_ptr<RandNum> randomNum;
    std::shared_ptr<Settings> settings;
	virtual void setFloatParameters(std::vector<float> values) = 0;
//	vector<ANN> aNN;
//	vector<FixedPosition> fixedControl;
};


#endif //CONTROL_H
