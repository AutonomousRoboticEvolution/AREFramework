#include "ControlFactory.h"
#include <iostream>
#include <memory>
using namespace std;

ControlFactory::ControlFactory()
{
}


ControlFactory::~ControlFactory()
{
//	cout << endl << "DELETED CONTROLFACTORY" << endl << endl; 
}

shared_ptr<Control> ControlFactory::createNewControlGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st) {
	if (st->controlType == st->ANN_DEFAULT || st->controlType ==st->ANN_DISTRIBUTED_BOTH
		|| st->controlType == st->ANN_DISTRIBUTED_UP
		|| st->controlType == st->ANN_DISTRIBUTED_DOWN) {
		//create ANN		
		shared_ptr<Control> simpleControl(new ANN);
		simpleControl->randomNum = rn;
		simpleControl->settings = st;
//		Control *simpleControl 
		return simpleControl;

//		Control *aNN = new ANN;
//		aNN->init(jointHandles);
//		return aNN;
	}
	else if (st->controlType == st->ANN_CUSTOM) {
		shared_ptr<Control> simpleControl(new CustomANN);
		simpleControl->randomNum = rn;
		simpleControl->settings = st;
		//Control *simpleControl = new SimpleControl; 
		return simpleControl;
	}
	else if (st->controlType == st->ANN_CPPN) {
		// creates CPPN
		shared_ptr<Control> cppn(new CPPN);
		cppn->randomNum = rn;
		cppn->settings = st;
		return cppn;
	}
	else if (st->controlType == st->ANN_NEAT) {
		cout << "ERROR: Neat is not yet set up" << endl;
	}
}



shared_ptr<Control> ControlFactory::copyControlGenome(shared_ptr<Control> parent) 
{
	shared_ptr<Control> cloneControl;
	cloneControl = parent->clone();
	cloneControl->cloneControlParameters(parent);
	return cloneControl;
}