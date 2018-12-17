#pragma once
#include "ModuleFactory.h"
#include "../../dynamixel/c++/src/dynamixel_sdk.h"

class R_ModuleFactory : public ModuleFactory
{
public:
	R_ModuleFactory();
	~R_ModuleFactory();
	shared_ptr<ER_Module> createRobotModuleGenome(int moduleType, shared_ptr<RandNum> rn, shared_ptr<Settings> st, 
		dynamixel::PortHandler *portH, dynamixel::PacketHandler *pack1, dynamixel::PacketHandler *pack2);
	shared_ptr<ER_Module> copyModuleGenome(shared_ptr<ER_Module> parentModule);	
};

