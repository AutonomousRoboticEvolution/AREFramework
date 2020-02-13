#include "DummyControl.h"

#include <iostream>

using namespace are;

Control::Ptr DummyControl::clone() const
{
    return std::shared_ptr<Control>(new DummyControl(*this));
}





