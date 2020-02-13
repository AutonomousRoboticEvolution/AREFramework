#pragma once
#ifndef DUMMY_CONTROL_H
#define DUMMY_CONTROL_H

#include "ARE/Control.h"
#include <memory>
#include <vector>

namespace are {

class DummyControl : public Control
{
public:
    DummyControl() : Control(){}
    ~DummyControl(){}
    std::vector<double> update(const std::vector<double>&){return std::vector<double>();}
    std::shared_ptr<Control> clone() const;

};

}//are
#endif //DUMMY_CONTROL_H
