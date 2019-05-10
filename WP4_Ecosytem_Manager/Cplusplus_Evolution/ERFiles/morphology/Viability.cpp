#include "Viability.h"
#include <iostream>

// This method checks if there is at least one organ of each type
int Viability::checkOrgansType(vector<Organ> organs) {
    bool result = true;
    int organTypesCounter = 0;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < organs.size(); j++){
            if(organs[j].organType == i){
                organTypesCounter++;
                break;
            }
        }
    }
    // If there are at least 2 different organs.
    if(organTypesCounter != 2){
        result = false;
        std::cout << "WARNING: There is an organ type missing!" << std::endl;
    }
    return result;

}
// This method checks whether if an organ is inside of outside of the 3D print volume
int Viability::printVolume(vector<float> coordinates) {
    bool result = true;
    if(coordinates[0] > 140 || coordinates[0] < -140 ||
        coordinates[1] > 140 || coordinates[1] < -140 ||
        coordinates[2] > 250 || coordinates[2] < 0){
        result = false;
        std::cout << "WARNING: There is an organ outside the print volume!" << std::endl;
    }
    return result;
}
// Detect whether the organ is colliding with anything
int Viability::collisionDetector(vector<int> allComponents, int componentHandle) {
    int result = true;
    for(int i = 0; i < allComponents.size(); i++){
        if(simCheckCollision(allComponents[i], componentHandle)){
            std::cout << simGetObjectName(allComponents[1]) << " and " << simGetObjectName(componentHandle) << " are colliding. " << std::endl;
            std::cout << "WARNING: An organ is colliding!" << std::endl;
            result = false;
            break;
        }
    }
    return result;
}
// Create temporal gripper
int Viability::createTemporalGripper(Organ organ) {
    int gripperHandle;
    float gripperPosition[3] = {0.0, 0.0, 0.0};
    float gripperOrientation[3] = {organ.orientations[0], organ.orientations[1], organ.orientations[2]};

    float offSetX = 0;
    float offSetY = 0;
    float offSetZ = 0.03;
    float tempX = 0;
    float tempY = 0;
    float tempZ = 0;

    gripperHandle = simLoadModel("models/C_Gripper.ttm");

    tempX = offSetX;
    tempY = offSetY * cos(organ.orientations[0]) - offSetZ * sin(organ.orientations[0]);
    tempZ = offSetY * sin(organ.orientations[0]) + offSetZ * cos(organ.orientations[0]);
    offSetX = tempX;
    offSetY = tempY;
    offSetZ = tempZ;
    // 2nd rotation
    // TODO Why minus here!!!
    tempX = offSetX * cos(organ.orientations[1]) - offSetZ * sin(organ.orientations[1]);
    tempY = offSetY;
    tempZ = offSetX * sin(organ.orientations[1]) + offSetZ * cos(organ.orientations[1]);
    offSetX = tempX;
    offSetY = tempY;
    offSetZ = tempZ;
    // 3nd rotation
    tempX = offSetX * cos(organ.orientations[2]) - offSetY * sin(organ.orientations[2]);
    tempY = offSetX * sin(organ.orientations[2]) + offSetY * cos(organ.orientations[2]);
    tempZ = offSetZ;
    offSetX = tempX;
    offSetY = tempY;
    offSetZ = tempZ;
    gripperPosition[0] = (organ.coordinates[0]/100) + offSetX;
    gripperPosition[1] = (organ.coordinates[1]/100) + offSetY;
    gripperPosition[2] = (organ.coordinates[2]/100) + offSetZ + 0.05;

    simSetObjectOrientation(gripperHandle, -1, gripperOrientation);
    simSetObjectPosition(gripperHandle, -1, gripperPosition);
    simSetObjectInt32Parameter(gripperHandle, sim_shapeintparam_static, 1);

    return gripperHandle;
}