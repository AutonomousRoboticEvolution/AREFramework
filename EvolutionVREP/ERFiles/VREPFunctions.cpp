#include "VREPFunctions.h"
#include "v_repLib.h"
#include <iostream>
#include <cassert>

using namespace std;


ObjectCreator::ObjectCreator(){}
ObjectCreator::~ObjectCreator(){}
ObjectRemover::ObjectRemover(){}
ObjectRemover::~ObjectRemover(){}
RetrieveVREPInfo::RetrieveVREPInfo(){}
RetrieveVREPInfo::~RetrieveVREPInfo(){}
SetVREPInfo::SetVREPInfo(){}
SetVREPInfo::~SetVREPInfo(){}

void SetVREPInfo::v_nameObject(string name, int handle) {
	string objectName = name + to_string(handle);
	simSetObjectName(handle, objectName.c_str());
}

int ObjectCreator::v_createCube(int bitvalue, float size[3], float position[3],
	float orientation[3], float mass, int parentHandle, string name, bool handleInName, bool renderable) {
	vector<float> objectParameters;
	int objectHandle = simCreatePureShape(0, bitvalue, size, mass, NULL);
	simSetObjectPosition(objectHandle, parentHandle, position);
	simSetObjectOrientation(objectHandle, parentHandle, orientation);
	string objectName;
	if (handleInName == true) {
		objectName = name + to_string(objectHandle);
	}
	else {
		objectName = name;
	}
	simSetObjectName(objectHandle, objectName.c_str());
	assert(objectHandle > 0);
	return objectHandle;
}

int ObjectCreator::v_createRevoluteJoint(int bitvalue, float size[3], float position[3],
	float orientation[3], float mass, int parentHandle, string name, bool handleInName, bool renderable)
{
	int jointHandle = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, size, NULL, NULL);
	simSetObjectPosition(jointHandle, parentHandle, position);
	simSetObjectOrientation(jointHandle, parentHandle, orientation);

	simSetObjectIntParameter(jointHandle, 2000, 1);
	simSetObjectIntParameter(jointHandle, 2001, 1);

	string objectName;
	if (handleInName == true) {
		objectName = name + to_string(jointHandle);
	}
	else {
		objectName = name;
	}
	simSetObjectName(jointHandle, objectName.c_str());
	return jointHandle;
}

// bitValue
int ObjectCreator::v_createVisionSensor(int bitvalue, float size[3], float position[3],
	float orientation[3], float mass, int parentHandle, string name, bool handleInName, bool renderable, int resolution[2]) {

	int sensorResolution[4] = { resolution[0], resolution[1], 0, 0 };
	//	floatParams(input) : 11 floating point parameters :
	//	floatParams[0] : near clipping plane
	//		floatParams[1] : far clipping plane
	//		floatParams[2] : view angle / ortho view size
	//		floatParams[3] : sensor size x
	//		floatParams[4] : sensor size y
	//		floatParams[5] : sensor size z
	//		floatParams[6] : "null" pixel red - value
	//		floatParams[7] : "null" pixel green - value
	//		floatParams[8] : "null" pixel blue - value
	//		floatParams[9] : reserved.Set to 0.0
	//		floatParams[10] : reserved.Set to 0.0
	float sensorFloatArray[11] = { 0, 0, 0, size[0], size[1], size[2], 0.0, 0.0, 0.0, 0.0, 0.0 };
	float sensorColor[3] = { 1.0, 1.0, 1.0 };
	int visionHandle = 0;
	visionHandle = simCreateVisionSensor(bitvalue, sensorResolution, sensorFloatArray, sensorColor);
	string visionName = name + to_string(visionHandle);
	simSetObjectName(visionHandle, visionName.c_str());
	if (parentHandle < 1) {
		simSetObjectParent(visionHandle, sim_handle_scene, true);
		simSetObjectPosition(visionHandle, sim_handle_scene, position);
		simSetObjectOrientation(visionHandle, sim_handle_scene, orientation);
	}
	else {
		simSetObjectParent(visionHandle, parentHandle, true);
		simSetObjectPosition(visionHandle, parentHandle, position);
		simSetObjectOrientation(visionHandle, parentHandle, orientation);
	}
	//	unsigned char colorScheme[12];
	//	simSaveImage(colorScheme, sensorResolution, 0, "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\Hansen", 0, NULL);
	//	simHandleVisionSensor(visionHandle, NULL, NULL);
	//	visionHandles.push_back(visionHandle);
	return visionHandle;
}

bool RetrieveVREPInfo::v_checkObjectCollision(int, vector<int>, vector<int>) {
	return true;
};

int RetrieveVREPInfo::v_getChildren(int parentHandle) {
	int objIndex = 0;
	int childHandle = simGetObjectChild(parentHandle, objIndex++);
	if (childHandle == -1) {
		return -1;
	}
	else return objIndex;
}

int RetrieveVREPInfo::v_getParent(int childHandle) {
	return simGetObjectParent(childHandle);
}

vector<int> RetrieveVREPInfo::v_getObjectHandlesInTree(int parentHandle) {
	simAddObjectToSelection(sim_handle_tree, parentHandle);
	int selectionSize = simGetObjectSelectionSize();
	int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason
	vector<int> objectHandles;
	//s_objectAmount = selectionSize;
	simGetObjectSelection(tempObjectHandles);
	int objectCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		if (simGetObjectType(tempObjectHandles[i]) == 0) {
			objectHandles.push_back(tempObjectHandles[i]);
		}
	}
	simRemoveObjectFromSelection(sim_handle_all, -1);
	return objectHandles;
}

void SetVREPInfo::v_setObjectParent(int objectHandle, int parentHandle){
	simSetObjectParent(objectHandle, parentHandle, true);
}