#include "Organ.h"

// This method creates organ and force sensor
void Organ::createOrgan() {
    // Argument variables for simCreateForceSensor
    int forSenPamsArg1[] = {0, 0, 0, 0, 0};
    float forSenPamsArg2[] = {0, 0, 0, 0, 0};
    // Load organ model
    loadOrganModel();
    // Create force sensor
    this->forceSensorHandle = simCreateForceSensor(0, forSenPamsArg1, forSenPamsArg2, NULL);
    // Set positions and orientations for organ and force sensor
    simSetObjectPosition(this->organHandle, -1, this->coordinates.data());
    simSetObjectPosition(this->forceSensorHandle, -1, this->coordinates.data());
    simSetObjectOrientation(this->organHandle, -1, this->orientations.data());
    simSetObjectOrientation(this->forceSensorHandle, -1, this->orientations.data());
    // Set parenthood
    simSetObjectParent(this->organHandle, this->forceSensorHandle, 1);
}
// This method load organ model from .ttm file.
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