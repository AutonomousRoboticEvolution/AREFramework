#pragma once

#include <vector>
#include "Organ.h"
#include "v_repLib.h"
using namespace std;
/**
 * @file Viability.h
 * @brief This class collects all the viability checks.
 */
class Viability {
    public:
        /**
            * @brief This method checks that there is at least one organ of each type.
        */
        int checkOrgansType(vector<Organ> organs);
        /**
            * @brief This method checks that the organ is within the 3D print volume.
        */
        int printVolume(vector<float> coordinates);
        /**
            * @brief This method checks that the component is not colliding with anything.
        */
        int collisionDetector(vector<int> allComponents, int componentHandle);
        /**
            * @brief This method creates a temporal gripper.
        */
        int createTemporalGripper(Organ organ);
        /**
            * @brief This method destroys temporal gripper.
        */
        void destroyTemporalGripper(int gripperHandle);
        /**
            * @brief This method checks that the robot has not obstructed sensor.
        */
        int obstructedSensor(vector<int> allComponents);
};






