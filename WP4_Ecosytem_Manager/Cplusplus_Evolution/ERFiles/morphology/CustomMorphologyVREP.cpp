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

	/*simRemoveObjectFromSelection(sim_handle_all, -1);
	//cout << "jointHandles size = " << jointHandles.size() << endl;
	if (name != "Pioneer_p3dx") {  //??
		for (int i = 0; i < jointHandles.size(); i++) {
			outputHandles.push_back(jointHandles[i]);
			simSetObjectInt32Parameter(jointHandles[i], 2000, 1);
			simSetObjectInt32Parameter(jointHandles[i], 2001, 1);  //dynamic motor control loop enable state
			simSetObjectFloatParameter(jointHandles[i], 2002, 0.5);
			simSetObjectFloatParameter(jointHandles[i], 2003, 0.1);
			simSetObjectFloatParameter(jointHandles[i], 2004, 0.0);
			//	sim_jointintparam_ctrl_enabled(2001) : int32 parameter : dynamic motor control loop enable state(0 or != 0)
			//	simSetJointTargetVelocity(joint, 0.2);
			//	simSetJointForce(joint, 100);
			simSetJointTargetPosition(jointHandles[i], 1.0);
			simSetJointForce(jointHandles[i], 1.0);
			simSetJointTargetVelocity(jointHandles[i], 0.05);
		}
	}*/
	return jointHandles;
}

void CustomMorphologyVREP::update() {
	if (control) {
		vector<float> input; // get sensor parameters, not set in this case
        int doh[1]; // detected object handle
        //simHandleProximitySensor(sensorHandles[0], NULL, doh, NULL);
        float pt[4];
        float vecs[3];
        simHandleProximitySensor(sensorHandles[0], pt, doh, vecs);
        int det = simReadProximitySensor(sensorHandles[0], pt, doh, vecs);
        if (det == 1) { //if detecting something
              cout << "read doh : " << doh[0] << endl;
              cout << "pt = " << pt[0] << ", " << pt[1] << ", " << pt[2] << ", " << pt[3] << endl;
        }
        input.push_back(pt[3]);
        //input.push_back(1.0);
//        if (name == "Pioneer_p3dx") {
//			int lightHandle = simGetObjectHandle("Light");
//			if (settings->verbose == true) {
//				cout << "controlling pioneer" << endl;
//				cout << "Sensor handles size = " << sensorHandles.size() << endl;
//				cout << "LightHandle = " << lightHandle << endl;
//				cout << "Output handles size = " << outputHandles.size() << endl;
//			}
//			float sunPos[3];
//			for (int i = 0; i < sensorHandles.size(); i++) {
//				simGetObjectPosition(lightHandle, -1, sunPos);
//				float proxSensPos[3];
//				simGetObjectPosition(sensorHandles[i], -1, proxSensPos);
//				float ax = sunPos[0] - proxSensPos[0];
//				float ay = sunPos[1] - proxSensPos[1];
//				float az = sunPos[2] - proxSensPos[2];
//
//				float a = sqrt(((ax * ax) + (ay * ay) + (az * az)));  //calculate the relative position
//				ax = ax / a;
//				ay = ay / a;
//				az = az / a;
//				if (settings->verbose == true) {
//					cout << "magnitude = " << sqrt(((ax * ax) + (ay * ay) + (az * az))) << endl;
//				}
//				vector<float> normalizedRayVector;
//
//				normalizedRayVector.push_back(ax);
//				normalizedRayVector.push_back(ay);
//				normalizedRayVector.push_back(az);
//
//				float blue[3] = { 0,0,1 };
//				//int rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0, -1, 2, blue, NULL, NULL, blue); // startEndLine );
//				//float ab[6] = { proxSensPos[0], proxSensPos[1], proxSensPos[2], proxSensPos[0] +(normalizedRayVector[0] * 10),
//				//	proxSensPos[1] + (normalizedRayVector[1] * 10), proxSensPos[2] + (normalizedRayVector[2] * 10) };
//				//simAddDrawingObjectItem(rayHandle, ab);
//
//				float xAx[3] = { 0.0,0.0,0.0 };
//				float yAx[3] = { 0.0,0.0,0.0 };
//				float zAx[3] = { normalizedRayVector[0],normalizedRayVector[1],normalizedRayVector[2] };
//				float matrix[12];
//				simGetObjectMatrix(sensorHandles[i], -1, matrix);
//				matrix[0] = xAx[0];
//				matrix[4] = xAx[1];
//				matrix[8] = xAx[2];
//				matrix[1] = yAx[0];
//				matrix[5] = yAx[1];
//				matrix[9] = yAx[2];
//				matrix[2] = zAx[0];
//				matrix[6] = zAx[1];
//				matrix[10] = zAx[2];
//				simSetObjectMatrix(sensorHandles[i], -1, matrix);
//
//				int doh[1]; // detected object handle
//				simHandleProximitySensor(sensorHandles[i], NULL, doh, NULL);
//				float pt[4];
//				float vecs[3];
//				simHandleProximitySensor(sensorHandles[i], pt, doh, vecs);
//				int det = simReadProximitySensor(sensorHandles[i], pt, doh, vecs);
//				if (det == 0) { // for measuring if the proximity sensor collided
//					// notCollidedProxSensors.push_back(sensorHandles[i]);
//					//	cout << "read doh? : " << doh[0] << " but " << det << endl;
//					//	cout << "pt = " << pt[0] << ", " << pt[1] << ", " << pt[2] << ", " << pt[3] << endl;
//				}
//				int sParent = simGetObjectParent(sensorHandles[i]);
//				float sOr[12];
//				simGetObjectMatrix(sensorHandles[i],sParent, sOr);
//				// simGetObjectOrientation(sensorHandles[i], sParent, sOr);
//				input.push_back(sOr[10]);  //why??
//			}
//		}
		vector<float> output = control->update(input);  //control output
		for (int i = 0; i < output.size(); i++) {
			if (settings->verbose) {
				cout << "output " << i << " = " << output[i] << endl;
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
	simLoadModel("/home/wei/Softwares/V-REP_PRO_EDU_V3_6_1_Ubuntu16_04/models/wheeled-robot2.ttm");
    mainHandle = simGetObjectHandle("RobotBase");
//	simLoadModel("/home/wei/Softwares/V-REP_PRO_EDU_V3_6_1_Ubuntu16_04/models/robots/mobile/e-puck.ttm");
//  mainHandle = simGetObjectHandle("ePuck");
    if (control) {
    if (settings->verbose) {
			cout << "At create Custom morphology, control is present" << endl;
		}
	}
	vector<int> jointHandles = getJointHandles(mainHandle);
	outputHandles = jointHandles;

	//	name = "Pioneer_p3dx";
//	if (name == "Pioneer_p3dx") {
//		mainHandle = simGetObjectHandle(name.c_str());
//		int s1 = simGetObjectHandle("Pioneer_p3dx_ultrasonicSensor4");
//		int s2 = simGetObjectHandle("Pioneer_p3dx_ultrasonicSensor6");
//
//		int intpars[8] = { 1,1,1,1,1,1,0,0 };
//		float sensorSize = 0.004;
//		float floatpars[15] = { 0,10,0.0010,0.001,0.001,0.0001,0.001,0.0001,0.0001,0.0001,0.0001,0.0001,sensorSize,0,0 };
//
//		float sensor1pos[3];
//		float sensor2pos[3];
//		simGetObjectPosition(s1, -1, sensor1pos);
//		simGetObjectPosition(s2, -1, sensor2pos);
//
//		int proxSens1 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
//		int proxSens2 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
//
//		sensorHandles.push_back(proxSens1);
//		sensorHandles.push_back(proxSens2);
//
//		float pos[3] = { 0.0, 0.0, 0.1 };
//		simSetObjectPosition(proxSens1, s1, pos);
//		simSetObjectPosition(proxSens2, s2, pos);
//		simSetObjectParent(proxSens1, s1, true);
//		simSetObjectParent(proxSens2, s2, true);
//	}
//	simSetObjectInt32Parameter(proxSens,10, 0);

//  mainHandle = createWei();

}


shared_ptr<Morphology> CustomMorphologyVREP::clone() const {
	// shared_ptr<Morphology>()
	shared_ptr<CustomMorphologyVREP> cat = make_unique<CustomMorphologyVREP>(*this);
	cat->control = control->clone();
	return cat;
}
