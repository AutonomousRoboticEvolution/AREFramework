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
	virtual void init(int input, int inter, int output) = 0; 
	virtual vector<float> update(vector<float> sensorValues) = 0; // takes sensorInput and returns output 
	virtual void mutate(float mutationRate) = 0; 
	virtual shared_ptr<Control> clone() const = 0 ;
	virtual stringstream getControlParams() = 0; 
	virtual void setControlParams(vector<string>) = 0; 
	virtual void cloneControlParameters(shared_ptr<Control> parent) = 0;
	virtual bool checkControl(vector<string> values) = 0;
	virtual void reset() = 0;
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


