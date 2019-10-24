#include "DummyControl.h"

#include <iostream>

std::shared_ptr<Control> DummyControl::clone() const
{
    return std::shared_ptr<Control>(new DummyControl(*this));
}


std::stringstream DummyControl::getControlParams() {
    std::stringstream ss;
    ss << "#SimpleControl," << -1 << std::endl;
    ss << std::endl;
	return ss;
}


