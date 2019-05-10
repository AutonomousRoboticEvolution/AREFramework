#include "Organ.h"

// Create organ
void Organ::createOrgan() {
    // Argument variables
    int forSenPamsArg1[] = {0, 0, 0, 0, 0};
    float forSenPamsArg2[] = {0, 0, 0, 0, 0};

    float organPosition[3] = {this->coordinates[0]/100, this->coordinates[1]/100, this->coordinates[2]/100}; // 0.0225 Wheels barely touching floor but more room to rotate;
    float organOrientation[3] = {this->orientations[0], this->orientations[1], this->orientations[2]};
    // Create organ and sensor
    loadOrganModel();
    this->forceSensorHandle = simCreateForceSensor(0, forSenPamsArg1, forSenPamsArg2, NULL);
    // Set positions and orientations
    simSetObjectPosition(this->organHandle, -1, organPosition);
    simSetObjectPosition(this->forceSensorHandle, -1, organPosition);
    simSetObjectOrientation(this->organHandle, -1, organOrientation);
    simSetObjectOrientation(this->forceSensorHandle, -1, organOrientation);
    // Set parenthood
    simSetObjectParent(this->organHandle, this->forceSensorHandle, 1);
}
void Organ::loadOrganModel() {
    switch(this->organType){
        case BRAINORGAN:
            this->organHandle = simLoadModel("models/C_Brain.ttm");
            break;
        case MOTORORGAN:
            this->organHandle = simLoadModel("models/C_Wheel.ttm");
            break;
        case SENSORORGAN:
            this->organHandle = simLoadModel("models/C_Sensor.ttm");
            break;
        case JOINTORGAN:
            std::cout << "WARNING: Organ JOINT does not exist" << std::endl;
            break;
        default:
            std::cout << "WARNING: Organ type does not exist" << std::endl;
            break;
    }
}