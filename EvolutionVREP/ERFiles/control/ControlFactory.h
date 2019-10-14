#pragma once

#include "Control.h"


class ControlFactory
{
public:
	ControlFactory();
	~ControlFactory();
    std::shared_ptr<Control> createNewControlGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
    std::shared_ptr<Control> copyControlGenome(std::shared_ptr<Control> parentControl);
    std::shared_ptr<RandNum> randomNum;
};

