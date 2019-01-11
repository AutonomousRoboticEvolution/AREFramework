#pragma once
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
	case DEFAULTMODULE:
	{
		cout << "ERROR: Module not specified yet, see ModuleFactory.cpp" << endl; 
		return NULL;
	}
	case CUBE: 
	{
	//	cout << "creating cube genome" << endl;
		shared_ptr<ER_Module> module(new Module_Cube);
		module->randomNum = rn;
		module->settings = st;
		module->type = CUBE;
		module->createControl(); 
		return module;
	}
	//case BEND:
	//{
	////	cout << "creating bend genome" << endl;
	//	shared_ptr<ER_Module> module(new Module_Bend);
	//	module->randomNum = rn;
	//	module->settings = st;
	//	module->type = BEND;
	//	module->createControl();
	//	return module;
	//}
	case LEAF:
	{
	//	cout << "creating leaf genome" << endl;
		shared_ptr<ER_Module> module(new Module_Leaf);
		module->randomNum = rn;
		module->settings = st;
		module->type = LEAF;
		module->createControl();
		return module;
	}
	case SERVO:
	{
		if (st->evolutionType == st->EMBODIED_EVOLUTION) {
			// used to be to evolve real robot directly
		}
		else {
			shared_ptr<ER_Module> module(new Module_Servo);
			module->randomNum = rn;
			module->settings = st;
			module->type = SERVO;
			module->createControl();
			return module;
		}
	}
	/*case RECTANGLE:
	{
		shared_ptr<ER_Module> module(new Module_Rectangle);
		module->randomNum = rn;
		module->settings = st;
		module->type = RECTANGLE;
		module->createControl();
		return module;
	}*/
	case LEAFCHLOE:
	{
	//	cout << "creating leaf chloe genome" << endl; 
		shared_ptr<ER_Module> module(new Module_LeafChloe);
		module->randomNum = rn;
		module->settings = st;
		module->type = LEAFCHLOE;
		module->createControl();
		return module;
	}
	//case BRAIN:
	//{
	////	cout << "creating brain genome" << endl;
	//	shared_ptr<ER_Module> module(new Module_Brain);
	//	module->randomNum = rn;
	//	module->settings = st;
	//	module->type = BRAIN;
	//	module->createControl();
	//	return module;
	//}
	case 8:
	{
	//	cout << "creating PLantA genome" << endl;
		shared_ptr<ER_Module> module(new Module_APlantBase);
		module->randomNum = rn;
		module->settings = st;
		module->type = 8;
		module->createControl();
		return module;
	}	
	//case 9:
	//{
	////	cout << "creating spring genome" << endl;
	//	shared_ptr<ER_Module> module(new Module_Spring);
	//	module->randomNum = rn;
	//	module->settings = st;
	//	module->type = 9;
	//	module->createControl();
	//	return module;
	//}
	case 11:
	{
		shared_ptr<ER_Module> module(new Module_Solar);
		module->randomNum = rn;
		module->settings = st;
		module->type = 11;
		module->createControl();
		return module;
	}
	/*case 12:
	{
		shared_ptr<ER_Module> module(new Module_Head);
		module->randomNum = rn;
		module->settings = st;
		module->type = 12;
		module->createControl();
		return module;
	}
*/
	default:
	{
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

