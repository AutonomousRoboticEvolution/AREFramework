#pragma once
#include <vector>
#include <memory>
#include <sstream>
#include "../../RandNum.h"
#include "../Settings.h"
using namespace std;

class Control
{
public:
	Control(){};
	virtual ~Control();
	/**
		@brief This method initilizes the control
		@param input Number of input
		@param input Number of internal (speccific for ANN?)
		@param input Number of output
	*/
	virtual void init(int input, int inter, int output) = 0; 
	/**
		@brief This method takes sensorInput and returns output 
	*/
	virtual vector<float> update(vector<float> sensorValues) = 0;
	/**
		@brief This method mutates the controller
		@param mutationRate mutation rate 
	*/ 
	virtual void mutate(float mutationRate) = 0;
	/**
		@brief This method clone the parameters of the controller 
	*/ 
	virtual shared_ptr<Control> clone() = 0;
	/**
		@brief This method returns the parameters of the controller as a string 
	*/
	virtual stringstream getControlParams() = 0; 
	/**
		@brief This method sets the parameters of the controller
	*/
	virtual void setControlParams(vector<string>) = 0;
	/**
		@brief This method clone the parameters of the parent controller
	*/ 
	virtual void cloneControlParameters(shared_ptr<Control> parent) = 0;
	virtual bool checkControl(vector<string> values) = 0;
	virtual void reset() = 0;
	/**
		@brief This method make the reference unique
	*/ 
	virtual void makeDependenciesUnique() = 0;
	virtual void addInput(vector<float> input) = 0;
	float cf = 1.0;
	shared_ptr<RandNum> randomNum;
	shared_ptr<Settings> settings;
	virtual void setFloatParameters(vector<float> values) = 0;
//	virtual void deleteControl() = 0;
//	vector<ANN> aNN;
//	vector<FixedPosition> fixedControl;
};


