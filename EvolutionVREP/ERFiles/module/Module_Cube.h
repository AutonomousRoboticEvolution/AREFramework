#pragma once

#include "ER_Module.h"


class Module_Cube :
	public ER_Module
{
public:
    Module_Cube() : ER_Module (){}
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
	int createModule(std::vector<float> configuration, int relativePosHandle, int parentHandle);
	/// Function to check which connection sites are free
	std::vector<int> getFreeSites(std::vector<int>);
    std::vector<int> getObjectHandles();
	/// deep copy
    std::shared_ptr<ER_Module> clone();
	void setModuleColor();
	void removeModule();
	/// propagation of the update function. Module is updated every frame
    std::vector<float> updateModule(std::vector<float> input);
    std::stringstream getModuleParams();
	void setModuleParams(std::vector<std::string>);
	void createControl() {};//define the function
    std::vector<float> getPosition();
	virtual std::stringstream getControlParams();
	
private:
	int objectPhysics = 8;
};

