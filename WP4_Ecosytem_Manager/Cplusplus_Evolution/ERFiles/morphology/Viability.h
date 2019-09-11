#pragma once

#include <vector>
#include "Organ.h"
#include "v_repLib.h"
using namespace std;

class Viability {
    public:
		Viability();
		~Viability();
        int checkOrgansType(vector<Organ> organs);
        int printVolume(vector<float> coordinates);
        int collisionDetector(vector<int> allComponents, int componentHandle);
        int createTemporalGripper(Organ organ);
		int obstructedSensor(int robotHandle);
        void destroyTemporalGripper(int gripperHandle);
};






