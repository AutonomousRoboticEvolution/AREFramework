#pragma once

#include <memory>
#include "ER_Module.h"
#include "Module_Cube.h"
#include "Module_Servo.h"
#include "Module_Misc.h"
#include "Module_Bone.h"


class ModuleFactory
{
public:
	ModuleFactory();
	~ModuleFactory();
    std::shared_ptr<ER_Module> createModuleGenome(int moduleType, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st);
    std::shared_ptr<ER_Module> copyModuleGenome(std::shared_ptr<ER_Module> parentModule);
    std::shared_ptr<RandNum> randomNum;

private:
	enum ModuleType
	{
		DEFAULTMODULE = 0,
		CUBE = 1,
		SERVO = 4,
		WHEEL = 14,
		BRAIN = 13,
		ARE_SERVO = 16,
		SENSOR = 15,
		BONE = 17,
		EXAMPLE = 18
	};
};

