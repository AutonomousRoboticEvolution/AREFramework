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
    /**
 * @brief The function creates a simple cube module that takes configuration values and
 * some identifier values as inputs specifying where the module should be created
 * @param configuration : six values determining the position and orientation of the
 * module relative to the relativePosHandle
 * @param relativePosHandle : to which parent object the module
 * configuration is set relative to (usually a dummy object)
 * @param parentHandle : handle of the parent object used to create a physical connection
 * (not same as mainHandle of module; mainHandle is the root of the module)
 * @return
 */
	virtual int createModule(vector<float> configuration, int relativePosHandle, int parentHandle) = 0;
	virtual void createControl() = 0;
	shared_ptr<Control> control;

	virtual stringstream getModuleParams() = 0; // appends module parameters to genome
	virtual stringstream getControlParams() = 0; // appends module control parameters to genome
	virtual void setModuleParams(vector<string>) = 0;
	virtual void setControlParams(vector<string>);

	virtual void removeModule() = 0;
	virtual vector<float> getPosition() =0;

	vector<int> getMirrorSite(int site, int configuration, int mirrorAxis);
	void updateParentModuleMorphology(int num);

	virtual shared_ptr<ER_Module> clone() = 0;
	bool checkControlParams(vector<string>);
	/// specifies if the modules has been instantiated (and not deleted)
	bool handled = false;
	/// Can be used to specify .ttm filename
	string filename = "default";
	void setFileName(string filename);

	// sets the module in and outputs by looping through parts
	void colorModule(float color[3], float alpha);
	/// color of the created module
	float moduleColor[3];

	// L-System info
	/// Defines which attachment sites have child modules connected to them.
	vector<int> sites;
	/// Defines which attachment sites are free
	vector<int> freeSites; 
	/// state used by L-System
	int state;
	/// module type
	int type;
	/// Specifying one connection site
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
	/// vector specifying all the child sites of the module
	vector<vector<shared_ptr<SITE>> > siteConfigurations;
	virtual vector<int> getFreeSites(vector<int>) = 0;
    /// can be used to add an input vector to the module (sensor)
	vector<float> moduleInput;
	/// Used to define the output of the module (e.g. to handle servo control)
	vector<float> moduleOutput;

	/// Updates the morphology (This can be called right after creating the module
	/// allowing the user to adjust the morphology)
	virtual void updateMorph(int num);

	/// Used to update the module, can receive additional input values from environment or adjacent modules
	virtual vector<float> updateModule(vector<float> input) = 0; // should return output
	int moduleID = 0;
	void setModuleID(int id);


	shared_ptr<ER_Module> parentModulePointer;
	vector<shared_ptr<ER_Module>> childModulePointers;

	float phenV = 0; // deprecated
	int maxChilds;  // TODO: maxChilds is currently defined as a fixed value for each module
	                // This could be change to become a variable.
	// object info
	/// All the handles of the module
	vector<int> objectHandles;
	/// The handles of the objects that can have other modules attached ro them (cannot be a dummy)
	vector<int> attachHandles;
	/// The relative position handles of where other modules can be attached (relativePosHandle)
	vector<int> attachRefHandles;
	/// Handles of objects that can be controlled using a controller output (e.g. joints).
	vector<int> controlHandles;
	/// Handles of objects that can pass an input to the controller (e.g. sensors)
	vector<int> inputHandles;
    /// Appends an input vector to the existing input of the module
	void addInput(vector<float>);

//	vector<vector<float>> dragFacet; // four points for each face with drag. 

	virtual vector<int> getObjectHandles() = 0; 

	// color
	bool broken = false;
	bool sleepMode = false; // should limit energy update
	bool battery = false; // define module as a battery module. Currently, the first module can only be the parent. 
	virtual void setModuleColor() = 0; 
	
};

