#pragma once
#include "ANN.h"
#include "Control.h"
#include "SimpleControl.h"
#include "ANN.h"
#include "CPPN.h"
#include "CustomANN.h"

class ControlFactory
{
public:
	ControlFactory();
	~ControlFactory();
	shared_ptr<Control> createNewControlGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	shared_ptr<Control> copyControlGenome(shared_ptr<Control> parentControl);
	shared_ptr<RandNum> randomNum;
};

