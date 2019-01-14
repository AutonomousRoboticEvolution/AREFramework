#pragma once
#include <vector>
#include <string>
#include "v_repLib.h"
#include "cassert"

using namespace std;
/*
typedef int vrep_handle;
typedef string vrep_name;
typedef bool vrep_addHandleToName;
typedef bool vrep_renderable;
typedef float vrep_size[3];
typedef float vrep_position[3];
typedef float vrep_orientation[3];
typedef float vrep_mass;
typedef int vrep_bitvalue;
*/

class ObjectCreator
{
public:
	ObjectCreator();
	~ObjectCreator();
	int v_createCube(int bitvalue, float size[3], float position[3], float orientation[3],
		float mass, int parent, string name, bool handleInName, bool renderable);
	int v_createRevoluteJoint(int bitvalue, float size[3], float position[3], float orientation[3],
		float mass, int parent, string name, bool handleInName, bool renderable);
	int v_createVisionSensor(int bitvalue, float size[3], float position[3], float orientation[3],
		float mass, int parent, string name, bool handleInName, bool renderable,
		int resolution[2] ); 
	int v_createPrismaticJoint(int bitvalue, float size[3], float position[3], float orientation[3],
		float mass, int parent, string name, bool handleInName, bool renderable); // not set
private:
	int objectHandle;
	int jointHandle;
	float objectSize[3];
//	vector<float>
private:
};

class ObjectRemover {
public:
	ObjectRemover();
	~ObjectRemover();
	void v_removeObject(int);
	void v_removeObjects(int, vector<int>);
};

class RetrieveVREPInfo {
public:
	RetrieveVREPInfo();
	~RetrieveVREPInfo();
	vector<float> v_getObjectPosition(int, int parent);
	vector<float> v_getObjectOrientation(int, int parent);
	int v_getCubeSizeX(int, int parent);
	int v_getCubeSizeY(int, int parent);
	int v_getCubeSizeZ(int, int parent);
	int v_getParent(int);
	float v_getJointPosition(int);
	float v_getJointForce(int);
	float v_getJointTargetVelocity(int);
	bool v_checkObjectCollision(int, vector<int> objectsCheck, vector<int> excludedObjects);
	void v_saveVisionSensorImage(string directory);
	int v_getChildren(int parent);
	vector<int> v_getObjectHandlesInTree(int parentHandle);
};
class SetVREPInfo {
public:
	SetVREPInfo();
	~SetVREPInfo();

	void v_nameObject(string name, int handle);
	float v_setJointForce(float);
	float v_setJointVelocity(float);
	void v_setObjectParent(int objectHandle, int parentHandle);

};


