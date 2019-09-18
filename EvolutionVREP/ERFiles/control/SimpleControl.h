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
	virtual std::vector<float> update(std::vector<float>);
    std::shared_ptr<Control> clone() const;
	void mutate(float mutationRate);
    std::stringstream getControlParams();
	void setControlParams(std::vector<std::string>);
	void cloneControlParameters(std::shared_ptr<Control> parent) {};
	bool checkControl(const std::vector<std::string> &values) override;
    std::vector<int> c_jointHandles;
	void reset() {};
	void makeDependenciesUnique();
	void addInput(std::vector<float> input) {};
	void setFloatParameters(std::vector<float> values) {};
	void flush() {};
};

