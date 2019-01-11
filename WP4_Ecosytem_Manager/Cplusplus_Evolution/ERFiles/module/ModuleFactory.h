#pragma once
#include "ER_Module.h"
#include "Module_Cube.h"
//#include "Module_Bend.h"
#include "Module_Leaf.h"
#include "Module_Servo.h"
//#include "Module_Rodrigo.h"
//#include "Module_Rectangle.h"
#include "Module_LeafChloe.h"
//#include "Module_Brain.h"
#include "Module_APlantBase.h"
//#include "Module_Spring.h"
#include "Module_Solar.h"
//#include "Module_Head.h"
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
		BEND = 2,
		LEAF = 3,
		SERVO = 4,
		RECTANGLE = 5,
		LEAFCHLOE = 6,
		BRAIN = 7,
		STICK
	};
};

