#include "CustomMorphologyVREP.h"

CustomMorphologyVREP::CustomMorphologyVREP()
{
}


CustomMorphologyVREP::~CustomMorphologyVREP()
{
}

vector<int> CustomMorphologyVREP::getObjectHandles(int parentHandle)
{
    return vector<int>();
}

vector<int> CustomMorphologyVREP::getJointHandles(int parentHandle)
{
    simAddObjectToSelection(sim_handle_tree, parentHandle);
    int selectionSize = simGetObjectSelectionSize();
    int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason
    vector<int> jointHandles;
    simGetObjectSelection(tempObjectHandles);
    for (size_t i = 0; i < selectionSize; i++)
    {
        if (simGetObjectType(tempObjectHandles[i]) == sim_object_joint_type) {
            jointHandles.push_back(tempObjectHandles[i]);
        }
        if (simGetObjectType(tempObjectHandles[i]) == sim_object_proximitysensor_type) {
            sensorHandles.push_back(tempObjectHandles[i]);
        }
    }
    if (settings->verbose) {
        cout << "jointHandles size = " << jointHandles.size() << endl;
        cout << "proximitysensorHandles size = " << jointHandles.size() << endl;
    }
    simRemoveObjectFromSelection(sim_handle_all, -1);
    return jointHandles;
}

void CustomMorphologyVREP::update() {
    if (control) {
        vector<float> input; // get sensor parameters, not set in this case
        for (int i = 0; i < sensorHandles.size(); i++) {
            //cout << "sensor size: " << sensorHandles.size() << endl;
            int doh[1]; // detected object handle
            //simHandleProximitySensor(sensorHandles[0], NULL, doh, NULL);
            float pt[4];
            float vecs[3];
            simHandleProximitySensor(sensorHandles[i], pt, doh, vecs);
            int det = simReadProximitySensor(sensorHandles[i], pt, doh, vecs);
            if (det == 1) { //if detecting something
                cout << "read doh : " << doh[0] << endl;
                cout << "pt = " << pt[0] << ", " << pt[1] << ", " << pt[2] << ", " << pt[3] << endl;
            }
            input.push_back(pt[3]);
            cout << "sensor reading: " << input[i] << endl;
        }
        vector<float> output = control->update(input);  //control output
        for (int i = 0; i < output.size(); i++) {
            if (settings->verbose) {
                //cout << "output " << i << " = " << output[i] << endl;
            }
            if (i < outputHandles.size()) {
                simSetJointTargetVelocity(outputHandles[i], output[i]);  //output[i],-1.0
            }
        }
    }
    else {
        std::cout << "No controller present... " << endl;
    }
}

int CustomMorphologyVREP::getMainHandle()
{
    return mainHandle;
}

void CustomMorphologyVREP::create()
{
    simLoadModel("/home/wei/Softwares/V-REP_PRO_EDU_V3_6_1_Ubuntu16_04/models/wheeled-robot.ttm");
    mainHandle = simGetObjectHandle("RobotBase");

    simFloat pos[3] = {-0.05, -0.15056, 0.031662};
    simSetObjectPosition(mainHandle, -1, pos);
//	simLoadModel("/home/wei/Softwares/V-REP_PRO_EDU_V3_6_1_Ubuntu16_04/models/robots/mobile/e-puck.ttm");
//  mainHandle = simGetObjectHandle("ePuck");
    //init();
    if (control) {
        if (settings->verbose) {
            cout << "At create Custom morphology, control is present" << endl;
        }
    }
    else {
        if (settings->verbose) {
            cout << "No control is present" << endl;
        }
    }
    vector<int> jointHandles = getJointHandles(mainHandle);
    outputHandles = jointHandles;

}


shared_ptr<Morphology> CustomMorphologyVREP::clone() const {
    // shared_ptr<Morphology>()
    shared_ptr<CustomMorphologyVREP> cat = make_unique<CustomMorphologyVREP>(*this);
    cat->control = control->clone();
    return cat;
}