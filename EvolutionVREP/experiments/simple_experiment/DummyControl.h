#pragma once
#ifndef DUMMY_CONTROL_H
#define DUMMY_CONTROL_H

#include "control/Control.h"
#include <memory>
#include <vector>

class DummyControl :
        public Control
{
public:
    DummyControl() : Control(){}
    ~DummyControl(){}
    virtual void init(int input, int inter, int output){}
    virtual std::vector<float> update(std::vector<float>){return std::vector<float>();}
    std::shared_ptr<Control> clone() const;
    void mutate(float mutationRate){}
    std::stringstream getControlParams();
    void setControlParams(std::vector<std::string>){}
    void cloneControlParameters(std::shared_ptr<Control> parent) {}
    bool checkControl(const std::vector<std::string> &values){return true;}
    std::vector<int> c_jointHandles;
    void reset() {}

    void addInput(std::vector<float> input) {}
    void setFloatParameters(std::vector<float> values) {}
    void flush() {}
};

#endif //DUMMY_CONTROL_H
