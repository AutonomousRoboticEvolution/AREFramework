#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <vector>
#include <memory>
#include <sstream>
#include "misc/RandNum.h"
#include "ARE/Settings.h"
#include "ARE/Phenotype.h"

namespace are {

/// abstract class
class Control
{
public:

    typedef std::shared_ptr<Control> Ptr;
    typedef std::shared_ptr<const Control> ConstPtr;
    typedef Control::Ptr (Factory)(int,misc::RandNum::Ptr, Settings::Ptr);

    Control(){}
    Control(const Control& ctrl) :
        cf(ctrl.cf),
        randomNum(ctrl.randomNum),
        settings(ctrl.settings)
    {}
    virtual ~Control(){}

    virtual Control::Ptr clone() const = 0;

    /**
    @brief Specify the number of neurons in input layer, hidden layer, and output layer
    @param input number of neuron in input layer
    @param inter number of neuron in hidden layer (only one hidden layer)
    @param output number of neuron in output layer
    */
//	virtual void init(int input, int inter, int output) = 0;
    /**
    @brief Take sensorInput and returns output
    @param sensorValues sensor values
    */
    virtual std::vector<double> update(const std::vector<double> &sensorValues) = 0;


//	virtual std::stringstream getControlParams() = 0;
//	virtual void setControlParams(std::vector<std::string>) = 0;
    /// clone the parent's neuron parameter
//	virtual void cloneControlParameters(std::shared_ptr<Control> parent) = 0;
    /// read the topology from CSV file and check if it is correct
//	virtual bool checkControl(const std::vector<std::string> &values) = 0;
//	virtual void reset() = 0;
//	virtual void flush() = 0;
	//virtual void makeDependenciesUnique() = 0;
//    virtual void addInput(const std::vector<double> &input) = 0;
//    virtual void setFloatParameters(std::vector<float> values) = 0;

    float get_cf(){return cf;}

protected:
	float cf = 1.0;

    misc::RandNum::Ptr randomNum;
    Settings::Ptr settings;
};

}//are

#endif //CONTROL_H
