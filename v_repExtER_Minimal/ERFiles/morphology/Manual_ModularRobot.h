#pragma once
#include "Modular_Dynamixel.h"
#include "../module/R_ModuleFactory.h"
#include "../module/ER_Module.h"

class Manual_ModularRobot :
	public Modular_Dynamixel
{
public:
	Manual_ModularRobot();
	~Manual_ModularRobot();
	void init();
	void mutate(); 
	void setMainHandlePosition(float position[3]) {};
	typedef shared_ptr<ER_Module> ModulePointer;
	vector<shared_ptr<ER_Module> > createdModules;
	vector<int> uiSliders;
	int uiHandle;
	void update();
	void createThree();
	void createPlant();
	void createThreeBase();
	void openPort();
	void createIt();
};

