#include "Organ.h"

// Create organ
void Organ::createOrgan(int type, vector<float> coordinate, vector<float> orientation, int* organHandle, int* sensorHandle) {
    int forSenPamsArg1[] = {0, 0, 0, 0, 0};
    float forSenPamsArg2[] = {0, 0, 0, 0, 0};
    float organPosition[3] = {coordinate[0]/100, coordinate[1]/100, coordinate[2]/100 + 0.05}; // 0.0225 Wheels barely touching floor but more room to rotate;
    float organOrientation[3] = {orientation[0],orientation[1],orientation[2]};

    // Create organ and sensor
    switch(type){
        case 0:
            *organHandle = simLoadModel("models/brainOrgan.ttm");
            break;
        case 1:
            *organHandle = simLoadModel("models/motorOrgan3_massFixed.ttm");
            break;
        case 2:
            *organHandle = simLoadModel("models/sensorOrgan.ttm");
            break;
        default:
            std::cout << "WARNING: Organ type does not exist" << std::endl;
            break;
    }
    *sensorHandle = simCreateForceSensor(0, forSenPamsArg1, forSenPamsArg2, NULL);
    // Set positions and orientations
    simSetObjectPosition(*organHandle, -1, organPosition);
    simSetObjectPosition(*sensorHandle, -1, organPosition);
    simSetObjectOrientation(*organHandle, -1, organOrientation);
    simSetObjectOrientation(*sensorHandle, -1, organOrientation);
    // Set parenthood
    simSetObjectParent(*organHandle, *sensorHandle, 1);
}