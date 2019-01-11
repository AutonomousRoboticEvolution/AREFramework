#pragma once
#include "R_ModuleFactory.h"
#include <iostream>

using namespace std; 

R_ModuleFactory::R_ModuleFactory()
{
}

R_ModuleFactory::~R_ModuleFactory()
{
	cout << "deleted robot module factory" << endl; 
}

shared_ptr<ER_Module> R_ModuleFactory::createRobotModuleGenome(int moduleType, shared_ptr<RandNum> rn, shared_ptr<Settings> st,
	dynamixel::PortHandler *portH, dynamixel::PacketHandler *pack1, dynamixel::PacketHandler *pack2) {
	
	switch (moduleType) {
	case 0:
	{
		cout << "ERROR: Module not specified yet, see R_ModuleFactory.cpp" << endl; 
		return NULL;
	}
	case 1: 
	{
	//	cout << "creating cube genome" << endl;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 2:
	{
	//	cout << "creating bend genome" << endl;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 3: // leaf
	{
	//	cout << "creating leaf genome" << endl;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 4: // rodrigo
	{
		shared_ptr<R_Module_Rodrigo> rmr = shared_ptr<R_Module_Rodrigo>(new R_Module_Rodrigo);
//		rmr->packetHandler1 = pack1;
//		rmr->packetHandler2 = pack2;
//		rmr->portHandler = portH;
		shared_ptr<ER_Module> module = rmr;
		module->randomNum = rn;
		module->settings = st;
		module->type = moduleType;
		module->createControl();
		return module;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 5: // rectangle
	{
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 6: // chloeleaf
	{
	//	cout << "creating leaf chloe genome" << endl; 
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 7: // brain
	{
	//	cout << "creating brain genome" << endl;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 8:
	{
	//	cout << "creating PLantA genome" << endl;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}	
	case 9:
	{
	//	cout << "creating spring genome" << endl;
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 10: 
	{
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	case 11:
	{
		return ModuleFactory::createModuleGenome(moduleType, rn, st);
	}
	default:
	{
		cout << "ERROR: Module type not specified, returning NULL" << endl; 
		return NULL;
	}

	}
	return NULL;
}

shared_ptr<ER_Module> R_ModuleFactory::copyModuleGenome(shared_ptr<ER_Module> parent)
{
//	cout << "trying to copy module" << endl;
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
//	ControlFactory *controlFactory = new ControlFactory(); 
	shared_ptr<ER_Module> cloneModule;
//	cout << "clone" << endl; 
	cloneModule = parent->clone();
//	cout << "return clone" << endl; 
	if (cloneModule->control != NULL) {
		cloneModule->control = controlFactory->copyControlGenome(parent->control);
	}
//	delete controlFactory;
	return cloneModule;
}