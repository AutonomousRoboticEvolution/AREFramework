#pragma once
#include "ER_Module.h"

class Module_Cube :
	public ER_Module
{
public:
	Module_Cube();
	~Module_Cube();
	int init();
	int mutate(float mutationRate);
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
	int createModule(vector<float> configuration, int relativePosHandle, int parentHandle);
	/// Function to check which connection sites are free
	vector<int> getFreeSites(vector<int>);
	vector<int> getObjectHandles();
	/// deep copy
	shared_ptr<ER_Module> clone();
	void setModuleColor();
	void removeModule();
	/// propagation of the update function. Module is updated every frame
	vector<float> updateModule(vector<float> input);
	stringstream getModuleParams();
	void setModuleParams(vector<string>);
	void createControl() {};//define the function
	vector<float> getPosition();
	virtual stringstream getControlParams();
	
private:
	int objectPhysics = 8;
};

