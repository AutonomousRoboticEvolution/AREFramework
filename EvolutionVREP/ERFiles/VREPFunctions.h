#pragma once

#include <vector>
#include <string>
#include "v_repLib.h"
#include "cassert"


class ObjectCreator
{
public:
	ObjectCreator();
	~ObjectCreator();
	int v_createCube(int bitvalue, float size[3], float position[3], float orientation[3],
        float mass, int parent, std::string name, bool handleInName, bool renderable);
	int v_createRevoluteJoint(int bitvalue, float size[3], float position[3], float orientation[3],
        float mass, int parent, std::string name, bool handleInName, bool renderable);
	int v_createVisionSensor(int bitvalue, float size[3], float position[3], float orientation[3],
        float mass, int parent, std::string name, bool handleInName, bool renderable,
		int resolution[2] ); 
	int v_createPrismaticJoint(int bitvalue, float size[3], float position[3], float orientation[3],
        float mass, int parent, std::string name, bool handleInName, bool renderable); // not set
private:
	int objectHandle;
	int jointHandle;
	float objectSize[3];
private:
};

class ObjectRemover {
public:
	ObjectRemover();
	~ObjectRemover();
	void v_removeObject(int);
    void v_removeObjects(int, std::vector<int>);
};

class RetrieveVREPInfo {
public:
	RetrieveVREPInfo();
	~RetrieveVREPInfo();
    std::vector<float> v_getObjectPosition(int, int parent);
    std::vector<float> v_getObjectOrientation(int, int parent);
	int v_getCubeSizeX(int, int parent);
	int v_getCubeSizeY(int, int parent);
	int v_getCubeSizeZ(int, int parent);
	int v_getParent(int);
	float v_getJointPosition(int);
	float v_getJointForce(int);
	float v_getJointTargetVelocity(int);
    bool v_checkObjectCollision(int, std::vector<int> objectsCheck, std::vector<int> excludedObjects);
    void v_saveVisionSensorImage(std::string directory);
	int v_getChildren(int parent);
    std::vector<int> v_getObjectHandlesInTree(int parentHandle);
};
class SetVREPInfo {
public:
	SetVREPInfo();
	~SetVREPInfo();

    void v_nameObject(std::string name, int handle);
	float v_setJointForce(float);
	float v_setJointVelocity(float);
	void v_setObjectParent(int objectHandle, int parentHandle);

};


