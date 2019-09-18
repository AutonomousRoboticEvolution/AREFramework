#include "Manual_ModularRobot.h"



Manual_ModularRobot::Manual_ModularRobot()
{
	
}


Manual_ModularRobot::~Manual_ModularRobot()
{
}

void Manual_ModularRobot::init() {
	simRemoveUI(uiHandle);
	// createThree();
//	Modular_Dynamixel::closePort();
	//
	// createThreeBase();
	// createPlant();
	createIt();
}

void Manual_ModularRobot::createPlant() {
	createdModules.clear();
	unique_ptr<ModuleFactory> md(new ModuleFactory);
	//	lGenome.lParameters[i].module = lGenome.moduleFactory->createModuleGenome(settings->moduleTypes[0], randomNum, settings);
	cout << "Creating Manual Modular Robot" << endl;
	createdModules.push_back(md->createModuleGenome(8, randomNum, settings));
	int module = 10;
	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));

	createdModules.push_back(md->createModuleGenome(6, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(6, randomNum, settings));

	md.reset();
	createdModules[1]->setModuleID(1);
	createdModules[2]->setModuleID(3);
	createdModules[3]->setModuleID(4);
	createdModules[4]->setModuleID(5);
	int parentHandle = -1;
	vector<float> configuration;
	configuration.resize(6);
	configuration[0] = 0;
	configuration[1] = 0;
	configuration[2] = 0.1;
	configuration[3] = 0;
	configuration[4] = 0;
	configuration[5] = 0;
	cout << "About to create Module" << endl;
	createdModules[0]->createModule(configuration, -1, parentHandle);
	configuration.clear();
	cout << "created first module" << endl;
	// second
	configuration.push_back(createdModules[0]->siteConfigurations[0][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[0][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[0][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[0][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[0][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[0][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[0][0]->parentHandle;
	int relativePosHandle = createdModules[0]->siteConfigurations[0][0]->relativePosHandle;
	createdModules[1]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();
	
	// third
	int site = 2;
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[site][0]->parentHandle;
	relativePosHandle = createdModules[0]->siteConfigurations[site][0]->relativePosHandle;
	createdModules[2]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();


	// fourth
	configuration.push_back(createdModules[1]->siteConfigurations[0][1]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[0][1]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[0][1]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[0][1]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[0][1]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[0][1]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[1]->siteConfigurations[0][0]->parentHandle;
	relativePosHandle = createdModules[1]->siteConfigurations[0][0]->relativePosHandle;
	createdModules[3]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	// fifth
	configuration.push_back(createdModules[2]->siteConfigurations[0][1]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][1]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][1]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][1]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][1]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][1]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[2]->siteConfigurations[0][0]->parentHandle;
	relativePosHandle = createdModules[2]->siteConfigurations[0][0]->relativePosHandle;
	createdModules[4]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();
	
	// flower1
	configuration.push_back(createdModules[3]->siteConfigurations[0][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[3]->siteConfigurations[0][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[3]->siteConfigurations[0][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[3]->siteConfigurations[0][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[3]->siteConfigurations[0][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[3]->siteConfigurations[0][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[3]->siteConfigurations[0][0]->parentHandle;
	relativePosHandle = createdModules[3]->siteConfigurations[0][0]->relativePosHandle;
	createdModules[5]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	// flower2
	configuration.push_back(createdModules[4]->siteConfigurations[0][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[0][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[0][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[0][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[0][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[0][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[4]->siteConfigurations[0][0]->parentHandle;
	relativePosHandle = createdModules[4]->siteConfigurations[0][0]->relativePosHandle;
	createdModules[6]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	cout << "created modules " << endl; /**/

	//	createdModules[0]->createModule()

	// set color
	//	createdModules[createdModules.size() - 1]->colorModule(color);
	int buttonHandles[3] = { 1,2,3 };

	int uiSize[2] = { 7,15 };
	int cellSize[2] = { 24, 24 };
	uiHandle = simCreateUI("controlModule", sim_ui_menu_title, uiSize, cellSize, buttonHandles);
	//button1
	int button1pos[2] = { 0,1 };
	int button1Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button1pos, button1Size, sim_buttonproperty_slider));
	//button2
	int button2pos[2] = { 0,3 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button2pos, button1Size, sim_buttonproperty_slider | sim_buttonproperty_enabled));
	//button3
	int button3pos[2] = { 0,5 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button3pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button4
	int button4pos[2] = { 0,7 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button4pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button5
	int button5pos[2] = { 0,9 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button5pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button6
	int button6pos[2] = { 0,11 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button6pos, button1Size, sim_buttonproperty_slider));
	//button7
	int button7pos[2] = { 0,13 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button7pos, button1Size, sim_buttonproperty_slider));

}

void Manual_ModularRobot::createThreeBase() {
	createdModules.clear();
	unique_ptr<ModuleFactory> md(new ModuleFactory);
	//	lGenome.lParameters[i].module = lGenome.moduleFactory->createModuleGenome(settings->moduleTypes[0], randomNum, settings);
	cout << "Creating Manual Modular Robot" << endl;
	int module = 10;
//	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(8, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(module, randomNum, settings));
	createdModules.push_back(md->createModuleGenome(11, randomNum, settings));

	md.reset();

	createdModules[1]->setModuleID(7);
	createdModules[2]->setModuleID(6);
//	createdModules[2]->setModuleID(9);
	int parentHandle = -1;
	vector<float> configuration;
	configuration.resize(6);
	configuration[0] = 0;
	configuration[1] = 0;
	configuration[2] = 0.1;
	configuration[3] = 0;
	configuration[4] = 0;
	configuration[5] = 0;
	cout << "About to create Module" << endl;
	createdModules[0]->createModule(configuration, -1, parentHandle);
	configuration.clear();
	cout << "created first module" << endl;
	
	int site = 1;
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[site][0]->parentHandle;
	int relativePosHandle = createdModules[0]->siteConfigurations[site][0]->relativePosHandle;
	createdModules[1]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();
	
	site = 0;
	int or = 1;
	configuration.push_back(createdModules[1]->siteConfigurations[site][or]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[site][or ]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[site][or ]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[site][or ]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[site][or ]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[1]->siteConfigurations[site][or ]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[1]->siteConfigurations[site][or ]->parentHandle;
	relativePosHandle = createdModules[1]->siteConfigurations[site][or ]->relativePosHandle;
	createdModules[2]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	// flower1
	configuration.push_back(createdModules[2]->siteConfigurations[0][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[2]->siteConfigurations[0][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[2]->siteConfigurations[0][0]->parentHandle;
	relativePosHandle = createdModules[2]->siteConfigurations[0][0]->relativePosHandle;
	createdModules[3]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	cout << "created modules " << endl;

	//	createdModules[0]->createModule()

	// set color
	//	createdModules[createdModules.size() - 1]->colorModule(color);
	int buttonHandles[4] = { 1,2,3,4};

	int uiSize[2] = { 7,10 };
	int cellSize[2] = { 16, 16 };
	uiHandle = simCreateUI("controlModule", sim_ui_menu_title, uiSize, cellSize, buttonHandles);
	//button1
	int button1pos[2] = { 0,1 };
	int button1Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button1pos, button1Size, sim_buttonproperty_slider));
	//button2
	int button2pos[2] = { 0,3 };
	int button2Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button2pos, button1Size, sim_buttonproperty_slider | sim_buttonproperty_enabled));
	//button3
	int button3pos[2] = { 0,5 };
	int button3Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button3pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button4
	int button4pos[2] = { 0,7 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button4pos, button1Size, sim_buttonproperty_slider));
}

void Manual_ModularRobot::openPort()
{
	Modular_Dynamixel::openPort();
}

void Manual_ModularRobot::createIt()
{
	createdModules.clear();
	unique_ptr<R_ModuleFactory> md(new R_ModuleFactory);
	//	lGenome.lParameters[i].module = lGenome.moduleFactory->createModuleGenome(settings->moduleTypes[0], randomNum, settings);
	cout << "Creating Manual Modular Robot" << endl;
	int module = 1;
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	module = 10;
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	md.reset();
	createdModules[1]->setModuleID(1);
	createdModules[2]->setModuleID(2);
	createdModules[3]->setModuleID(3);
	createdModules[4]->setModuleID(4);
	createdModules[5]->setModuleID(5);
	int parentHandle = -1;
	vector<float> configuration;
	configuration.resize(6);
	configuration[0] = 0;
	configuration[1] = 0;
	configuration[2] = 0.1;
	configuration[3] = 0;
	configuration[4] = 0;
	configuration[5] = 0;
	cout << "About to create Module" << endl;
	createdModules[0]->createModule(configuration, -1, parentHandle);
	configuration.clear();
	cout << "created first module" << endl;
	// 1
	int site = 1;

	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[site][2]->parentHandle;
	int relativePosHandle = createdModules[0]->siteConfigurations[site][2]->relativePosHandle;
	createdModules[1]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	// 2
	site = 2;

	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[site][2]->parentHandle;
	relativePosHandle = createdModules[0]->siteConfigurations[site][2]->relativePosHandle;
	createdModules[2]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();cout << "created modules " << endl;

	// 3

	site = 3;

	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][2]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[site][2]->parentHandle;
	relativePosHandle = createdModules[0]->siteConfigurations[site][2]->relativePosHandle;
	createdModules[3]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	// 4

	site = 4;

	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[0]->siteConfigurations[site][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[0]->siteConfigurations[site][0]->parentHandle;
	relativePosHandle = createdModules[0]->siteConfigurations[site][0]->relativePosHandle;
	createdModules[4]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear();

	// 5
	site = 0;

	configuration.push_back(createdModules[4]->siteConfigurations[site][0]->x);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[site][0]->y);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[site][0]->z);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[site][0]->rX);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[site][0]->rY);// = siteConfiguration[w];
	configuration.push_back(createdModules[4]->siteConfigurations[site][0]->rZ);// = siteConfiguration[w];
	parentHandle = createdModules[4]->siteConfigurations[site][0]->parentHandle;
	relativePosHandle = createdModules[4]->siteConfigurations[site][0]->relativePosHandle;
	createdModules[5]->createModule(configuration, relativePosHandle, parentHandle);
	configuration.clear(); 



	//	createdModules[0]->createModule()

	// set color
	//	createdModules[createdModules.size() - 1]->colorModule(color);
	int buttonHandles[5] = { 1,2,3,4,5 };

	int uiSize[2] = { 7,15 };
	int cellSize[2] = { 16, 16 };
	uiHandle = simCreateUI("controlModule", sim_ui_menu_title, uiSize, cellSize, buttonHandles);
	//button1
	int button1pos[2] = { 0,1 };
	int button1Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button1pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button2
	int button2pos[2] = { 0,3 };
	int button2Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button2pos, button1Size, sim_buttonproperty_slider | sim_buttonproperty_enabled));
	//button3
	int button3pos[2] = { 0,5 };
	int button3Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button3pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	
	int button4pos[2] = { 0,7 };
	int button4Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button4pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	int button5pos[2] = { 0,9 };
	int button5Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button5pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	int button6pos[2] = { 0,11 };
	int button6Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button6pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));

}

void Manual_ModularRobot::createThree() {
	createdModules.clear();
	unique_ptr<R_ModuleFactory> md(new R_ModuleFactory);
	//	lGenome.lParameters[i].module = lGenome.moduleFactory->createModuleGenome(settings->moduleTypes[0], randomNum, settings);
	cout << "Creating Manual Modular Robot" << endl;
	int module = 10;
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	createdModules.push_back(md->createRobotModuleGenome(module, randomNum, settings, portHandler, packetHandler1, packetHandler2));
	md.reset();
	createdModules[0]->setModuleID(1);
	createdModules[1]->setModuleID(2);
	createdModules[2]->setModuleID(3);
	int parentHandle = -1;
	vector<float> configuration;
	configuration.resize(6);
	configuration[0] = 0;
	configuration[1] = 0;
	configuration[2] = 0.1;
	configuration[3] = 0;
	configuration[4] = 0;
	configuration[5] = 0;
	cout << "About to create Module" << endl;
	createdModules[0]->createModule(configuration, -1, parentHandle);
	configuration.clear();
	cout << "created first module" << endl;
	for (int i = 1; i <= createdModules.size() - 1; i++) { // <= !!
		cout << "i : " << i << endl;
		configuration.push_back(createdModules[i - 1]->siteConfigurations[0][0]->x);// = siteConfiguration[w];
		configuration.push_back(createdModules[i - 1]->siteConfigurations[0][0]->y);// = siteConfiguration[w];
		configuration.push_back(createdModules[i - 1]->siteConfigurations[0][0]->z);// = siteConfiguration[w];
		configuration.push_back(createdModules[i - 1]->siteConfigurations[0][0]->rX);// = siteConfiguration[w];
		configuration.push_back(createdModules[i - 1]->siteConfigurations[0][0]->rY);// = siteConfiguration[w];
		configuration.push_back(createdModules[i - 1]->siteConfigurations[0][0]->rZ);// = siteConfiguration[w];
		parentHandle = createdModules[i - 1]->siteConfigurations[0][0]->parentHandle;
		int relativePosHandle = createdModules[i - 1]->siteConfigurations[0][0]->relativePosHandle;
		createdModules[i]->createModule(configuration, relativePosHandle, parentHandle);
		configuration.clear();
	}
	cout << "created modules " << endl;

	//	createdModules[0]->createModule()

	// set color
	//	createdModules[createdModules.size() - 1]->colorModule(color);
	int buttonHandles[3] = { 1,2,3 };

	int uiSize[2] = { 7,10 };
	int cellSize[2] = { 16, 16 };
	uiHandle = simCreateUI("controlModule", sim_ui_menu_title, uiSize, cellSize, buttonHandles);
	//button1
	int button1pos[2] = { 0,1 };
	int button1Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button1pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button2
	int button2pos[2] = { 0,3 };
	int button2Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button2pos, button1Size, sim_buttonproperty_slider | sim_buttonproperty_enabled));
	//button3
	int button3pos[2] = { 0,5 };
	int button3Size[2] = { 10,2 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button3pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));

}


void Manual_ModularRobot::mutate() {
	// Type your function here:

}

void Manual_ModularRobot::update() {
	cout << "Updating ManualRobot" << endl;
	vector<float> input;
	input.push_back(0.1);
	for (int i = 0; i < createdModules.size(); i++) {
		input[0] = simGetUISlider(uiHandle, uiSliders[i]) ;
		input[0] = (input[0] / 500) - 1.0;
		cout << input[0] << "," << i << endl;
		createdModules[i]->updateModule(input);
	}
	cout << simGetUISlider(uiHandle, uiSliders[0]) << endl;
}
