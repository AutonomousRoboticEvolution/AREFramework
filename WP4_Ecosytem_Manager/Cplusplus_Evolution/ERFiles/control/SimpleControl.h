#pragma once
#include "Control.h"
#include <memory>
#include <vector>

class SimpleControl :
	public Control
{
public:
	SimpleControl();
	~SimpleControl();
	virtual void init(int input, int inter, int output);
	virtual vector<float> update(vector<float>);
	shared_ptr<Control> clone() const;
	void deleteControl();
	void mutate(float mutationRate);
	stringstream getControlParams(); 
	void setControlParams(vector<string>);
	void cloneControlParameters(shared_ptr<Control> parent) {};
	bool checkControl(vector<string> values);
	vector<int> c_jointHandles;
	void reset() {};
	void makeDependenciesUnique();
	void addInput(vector<float> input) {};
	void setFloatParameters(vector<float> values) {};
};

