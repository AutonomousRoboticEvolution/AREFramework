#include "ModuleFactory.h"
#include <iostream>

ModuleFactory::ModuleFactory()
{
}

ModuleFactory::~ModuleFactory()
{
}

std::shared_ptr<ER_Module> ModuleFactory::createModuleGenome(int moduleType, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st) {
    switch (moduleType) {
        case DEFAULTMODULE: {
            std::cout << "ERROR: Module not specified yet, see ModuleFactory.cpp" << std::endl;
            return NULL;
        }
        case CUBE: {
            //	std::cout << "creating cube genome" << std::endl;
            std::shared_ptr<ER_Module> module(new Module_Cube);
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
                std::shared_ptr<ER_Module> module(new Module_Servo);
                module->randomNum = rn;
                module->settings = st;
                module->type = SERVO;
                module->createControl();
                return module;
            }
        }
        case BRAIN: {
            std::shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 13;
            module->createControl();
            module->filename = "C_BrainC.ttm";
            return module;
        }
        case WHEEL: {
            std::shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 14;
            module->createControl();
            module->filename = "C_Wheel.ttm";
            return module;
        }
        case SENSOR: {
            std::shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 15;
            module->createControl();
            module->filename = "C_Sensor.ttm";
            return module;
        }
        case ARE_SERVO: {
            std::shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 16;
            module->createControl();
            module->filename = "C_Servo.ttm";
            return module;
        }
        case EXAMPLE: {
            std::shared_ptr<ER_Module> module(new Module_Misc);
            module->randomNum = rn;
            module->settings = st;
            module->type = 18;
            module->createControl();
            module->filename = "C_Example.ttm";
            return module;
        }
        case BONE: {
            std::shared_ptr<ER_Module> module(new Module_Bone);
            module->randomNum = rn;
            module->settings = st;
            module->type = 17;
            module->createControl();
            module->filename = "C_Bone";
            return module;
        }
        default: {
            std::cout << "ERROR: Module type not specified, returning NULL" << std::endl;
            return NULL;
        }

    }
    return NULL;
}

std::shared_ptr<ER_Module> ModuleFactory::copyModuleGenome(std::shared_ptr<ER_Module> parent)
{
    std::unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//	ControlFactory *controlFactory = new ControlFactory(); 
    std::shared_ptr<ER_Module> cloneModule;
	cloneModule = parent->clone();
	if (cloneModule->control != NULL) {
		cloneModule->control = controlFactory->copyControlGenome(parent->control);
	}
	return cloneModule;
}

