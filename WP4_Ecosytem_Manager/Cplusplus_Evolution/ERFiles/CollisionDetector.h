#pragma once

// change below path
#include "v_repLib.h"
#include <vector>

using namespace std; 

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();

	struct OriginVector {
		vector<float> originVector;
	};

	struct CollisionPars {
		bool collision;
		float scalar;
		float collisionDistance;
	};

	struct PlanePoints {
		float points[4][3];
	};
	
	bool checkCollisionWithRay(int objectHandle, float rayParameters[6]);
	CollisionPars checkIntersection(float pps[4][3], float color[3], float rayVector[3], float rayOrigin[3], float vectorDirections[3]);
};


