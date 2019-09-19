#include "ModuleFactory.h"
#include <iostream>

using namespace std; 

ModuleFactory::ModuleFactory()
{
}

ModuleFactory::~ModuleFactory()
{
}

shared_ptr<ER_Module> ModuleFactory::createModuleGenome(int moduleType, shared_ptr<RandNum> rn, shared_ptr<Settings> st) {
    switch (moduleType) {
        case DEFAULTMODULE: {
            cout << "ERROR: Module not specified yet, see ModuleFactory.cpp" << endl;
            return NULL;
        }
        case CUBE: {
            //	cout << "creating cube genome" << endl;
            shared_ptr<ER_Module> module(new Module_Cube);
            module->randomNum = rn;
            module->settings = st;
            module->type = CUBE;
            module->createControl();
            return module;
        }
        case SERVO: {
            if (st->evolutionType == st->EMBODIED_EVOLUTION) {
                // used to be to evolve real robot directly
            } else {
                shared_ptr<ER_Module> module(new Module_Servo);
                module->randomNum = rn;
                module->settings = st;
                module->type = SERVO;
                module->createControl();
                return module;
            }
        }
        case BRAIN: {
            shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 13;
            module->createControl();
            module->filename = "C_BrainC.ttm";
            return module;
        }
        case WHEEL: {
            shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 14;
            module->createControl();
            module->filename = "C_Wheel.ttm";
            return module;
        }
        case SENSOR: {
            shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 15;
            module->createControl();
            module->filename = "C_Sensor.ttm";
            return module;
        }
        case ARE_SERVO: {
            shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 16;
            module->createControl();
            module->filename = "C_Servo.ttm";
            return module;
        }
        case EXAMPLE: {
            shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 18;
            module->createControl();
            module->filename = "C_Example.ttm";
            return module;
        }
        case BONE: {
            shared_ptr<ER_Module> module(new Module_Bone);
            module->randomNum = rn;
            module->settings = st;
            module->type = 17;
            module->createControl();
            module->filename = "C_Bone";
            return module;
        }
        default: {
            cout << "ERROR: Module type not specified, returning NULL" << endl;
            return NULL;
        }

    }
    return NULL;
}

shared_ptr<ER_Module> ModuleFactory::copyModuleGenome(shared_ptr<ER_Module> parent)
{
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//	ControlFactory *controlFactory = new ControlFactory(); 
	shared_ptr<ER_Module> cloneModule;
	cloneModule = parent->clone();
	if (cloneModule->control != NULL) {
		cloneModule->control = controlFactory->copyControlGenome(parent->control);
	}
	return cloneModule;
}

