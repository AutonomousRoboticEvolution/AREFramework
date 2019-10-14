#include "ControlFactory.h"
#include <iostream>
#include <memory>

ControlFactory::ControlFactory()
{
}


ControlFactory::~ControlFactory()
{
//	cout << endl << "DELETED CONTROLFACTORY" << endl << endl; 
}

std::shared_ptr<Control> ControlFactory::createNewControlGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st) {
//	if (st->controlType == st->ANN_DEFAULT || st->controlType ==st->ANN_DISTRIBUTED_BOTH
//		|| st->controlType == st->ANN_DISTRIBUTED_UP
//		|| st->controlType == st->ANN_DISTRIBUTED_DOWN) {
//		//create ANN
//		std::shared_ptr<Control> simpleControl(new ANN);
//		simpleControl->randomNum = rn;
//		simpleControl->settings = st;
////		Control *simpleControl
//		return simpleControl;

////		Control *aNN = new ANN;
////		aNN->init(jointHandles);
////		return aNN;
//	}
//	else if (st->controlType == st->ANN_CUSTOM) {
//		std::shared_ptr<Control> simpleControl(new CustomANN);
//		simpleControl->randomNum = rn;
//		simpleControl->settings = st;
//		//Control *simpleControl = new SimpleControl;
//		return simpleControl;
//	}
//	else if (st->controlType == st->ANN_CPPN) {
//		// creates CPPN
//		std::shared_ptr<Control> cppn(new CPPN);
//		cppn->randomNum = rn;
//		cppn->settings = st;
//		return cppn;
//	}
//	else if (st->controlType == st->ANN_NEAT) {
//		cout << "ERROR: Neat is not yet set up" << endl;
//	}
//    else if (st->controlType == st->ANN_FIXED_STRUCTURE) {
//        // creates fixed structure ANN
//        std::shared_ptr<Control> fixedStructureANN(new FixedStructureANN);
//        fixedStructureANN->randomNum = rn;
//        fixedStructureANN->settings = st;
//        return fixedStructureANN;
//    }

    return nullptr;
}



std::shared_ptr<Control> ControlFactory::copyControlGenome(std::shared_ptr<Control> parent)
{
    std::shared_ptr<Control> cloneControl;
	cloneControl = parent->clone();
	cloneControl->cloneControlParameters(parent);
	return cloneControl;
}
