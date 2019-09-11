#pragma once
#include "ER_Module.h"
#include "Module_Cube.h"
#include "Module_Servo.h"
#include "Module_Misc.h"
#include "Module_Bone.h"
#include <memory>
//#include "../../dynamixel/c++/src/dynamixel_sdk.h"

class ModuleFactory
{
public:
	ModuleFactory();
	~ModuleFactory();
	shared_ptr<ER_Module> createModuleGenome(int moduleType, shared_ptr<RandNum> rn, shared_ptr<Settings> st);
	shared_ptr<ER_Module> copyModuleGenome(shared_ptr<ER_Module> parentModule);
	shared_ptr<RandNum> randomNum;
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

