#pragma once
#include "Environment.h"

class BasicSunEnv :
	public Environment
{
public:
	BasicSunEnv();
	~BasicSunEnv();
	void init(); 
	struct CollisionPars {
		bool collision;
		float scalar;
		float collisionDistance;
	};
	float fitnessFunction(MorphologyPointer morph);
	float updateEnv(MorphologyPointer morph);
	float checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules);
	float checkAngle(vector<float> fourPoints, shared_ptr<ER_Module> createdModule);
	bool checkCollisionHandleRay(int objectHandle, vector<float> rayOrigin, vector<float> rayVector);
	struct PlanePoints {
		float points[4][3];
	};
	
	CollisionPars checkIntersection(float pps[4][3], float color[3], float rayVector[3], float rayOrigin[3], float vectorDirections[3]);
	float checkArea(float interSection[3], float pps[4][3]);
};

