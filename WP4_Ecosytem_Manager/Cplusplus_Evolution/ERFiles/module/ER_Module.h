#pragma once
#include <vector>
#include <memory>
#include "v_repLib.h"
#include "../control/Control.h"
#include "../control/ControlFactory.h"
#include <sstream>
#include <iostream>
#include "../../RandNum.h"

using namespace std;
class ER_Module // Abstract Class
{
public:
	ER_Module();
	~ER_Module();

	shared_ptr<RandNum> randomNum;
	shared_ptr<Settings> settings;
	int parent; // number of the parent as stored in createdModules
	int parentSite;
	int orientation;	// orientation relative ro parent connection site 
						// so that the morphology can be determined for the robot
	virtual int init() = 0;
	virtual int mutate(float mutationRate) = 0;
	virtual int createModule(vector<float> configuration, int relativePosHandle, int parentHandle) = 0;
	virtual void createControl() = 0;
	shared_ptr<Control> control; 
	virtual stringstream getModuleParams() = 0; // appends module parameters to genome
	virtual stringstream getControlParams() = 0; // appends module parameters to genome
	virtual void setModuleParams(vector<string>) = 0;
	virtual void setControlParams(vector<string>);
	virtual void removeModule() = 0;
	virtual vector<float> getPosition() =0;
	void addEnergy(float energy);
	float energy = 0.0;
	float energyDissipationRate = 0.0;
	float deathThreshold = 0.5; 
	vector<int> getMirrorSite(int site, int configuration, int mirrorAxis); 
	virtual shared_ptr<ER_Module> clone() = 0; 
	void diffuseEnergy();
	void directDiffuseEnergy();
	int maxAmountModules = 100; 
	bool checkControlParams(vector<string>);
	bool handled = false;


	void colorModule(float color[3], float alpha);
	float moduleColor[3];
	// L-System info
	vector<int> sites;
	vector<int> freeSites; 
	int state;
	int type;
	struct SITE {
		float x;
		float y;
		float z;
		float rX;
		float rY;
		float rZ;
		int parentHandle; 
		int relativePosHandle;
	};
	vector<vector<shared_ptr<SITE>> > siteConfigurations;

	virtual vector<int> getFreeSites(vector<int>) = 0;
	vector<float> moduleInput; 
	vector<float> moduleOutput; 
	virtual vector<float> updateModule(vector<float> input) = 0; // should return output
	int moduleID = 0; 
	void setModuleID(int id);

	shared_ptr<ER_Module> parentModulePointer; // not used 
	vector<shared_ptr<ER_Module>> childModulePointers; // not used  

	float phenV = 0;
	// object info
	vector<int> objectHandles;
	vector<int> attachHandles; 
	vector<int> attachRefHandles;
	vector<int> controlHandles; \
	void addInput(vector<float>);

//	vector<vector<float>> dragFacet; // four points for each face with drag. 

	virtual vector<int> getObjectHandles() = 0; 

	// color
	bool broken = false;
	bool sleepMode = false; // should limit energy update
	bool battery = false; // define module as a battery module. Currently, the first module can only be the parent. 
	virtual void setModuleColor() = 0; 
	
};

