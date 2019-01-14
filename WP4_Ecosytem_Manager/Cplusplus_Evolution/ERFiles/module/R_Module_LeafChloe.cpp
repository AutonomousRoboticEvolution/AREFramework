#include "R_Module_LeafChloe.h"
#include <iostream>
#include <cmath>

using namespace std;

R_Module_LeafChloe::R_Module_LeafChloe()
{

}


R_Module_LeafChloe::~R_Module_LeafChloe()
{
//	cout << "REMOVING MODULE!" << endl;

	removeModule();
}

int R_Module_LeafChloe::init() {
	return -1;
}


vector<float> R_Module_LeafChloe::updateModule(vector<float> input) {
	//cout << "updating leaf" << endl;
	vector <float> fourPoints;
	ER_Module::updateModule(input);
	vector<int> notCollidedProxSensors;

	vector<float> leafValues;
	vector<int> leafHandles;
	leafHandles.push_back(objectHandles[7]);
	leafHandles.push_back(objectHandles[8]);
	leafHandles.push_back(objectHandles[9]);
	leafHandles.push_back(objectHandles[10]);
	leafHandles.push_back(objectHandles[11]);

	for (int i = 0; i < 5; i++) {
		simSetJointPosition(leafHandles[i], 0);
		simSetJointTargetPosition(leafHandles[0], 0 * M_PI);
	}


	int sunHandle = simGetObjectHandle("Light");
	float sunPos[3];
	for (int i = 0; i < proxSensors.size(); i++) {
		simGetObjectPosition(sunHandle, -1, sunPos);
		float proxSensPos[3];
		simGetObjectPosition(proxSensors[i], -1, proxSensPos);
		float ax = sunPos[0] - proxSensPos[0];
		float ay = sunPos[1] - proxSensPos[1];
		float az = sunPos[2] - proxSensPos[2];

		float a = sqrt(((ax * ax) + (ay * ay) + (az * az)));
		ax = ax / a;
		ay = ay / a;
		az = az / a;
		//		cout << "magnitude = " << sqrt(((ax * ax) + (ay * ay) + (az * az))) << endl; 
		vector<float> normalizedRayVector;

		normalizedRayVector.push_back(ax);
		normalizedRayVector.push_back(ay);
		normalizedRayVector.push_back(az);

		float blue[3] = { 0,0,1 };

		float xAx[3] = { 0.0,0.0,0.0 };
		float yAx[3] = { 0.0,0.0,0.0 };
		float zAx[3] = { normalizedRayVector[0],normalizedRayVector[1],normalizedRayVector[2] };
		float matrix[12];
		simGetObjectMatrix(proxSensors[i], -1, matrix);
		matrix[0] = xAx[0];
		matrix[4] = xAx[1];
		matrix[8] = xAx[2];
		matrix[1] = yAx[0];
		matrix[5] = yAx[1];
		matrix[9] = yAx[2];
		matrix[2] = zAx[0];
		matrix[6] = zAx[1];
		matrix[10] = zAx[2];
		simSetObjectMatrix(proxSensors[i], -1, matrix);

		int doh[1]; // detected object handle
		simHandleProximitySensor(proxSensors[i], NULL, doh, NULL);
		//	cout << "doh : " << doh[0] << endl;
		float pt[4];
		float vecs[3];
		simHandleProximitySensor(proxSensors[i], pt, doh, vecs);
		//	cout << "doh : " << doh[0] << endl;
		int det = simReadProximitySensor(proxSensors[i], pt, doh, vecs);
		if (det == 0) {
			notCollidedProxSensors.push_back(proxSensors[i]);
			//	cout << "read doh? : " << doh[0] << " but " << det << endl;
			//	cout << "pt = " << pt[0] << ", " << pt[1] << ", " << pt[2] << ", " << pt[3] << endl;
		}
	}
	//	int det = simReadProximitySensor(proxSensors[0], NULL, doh, NULL);
	//	cout << "doh? : " << doh[0] << " but " << det << endl;

	//	simSetObjectOrientation(proxSensors[0], -1, vec);

	//	simSetObjectQuaternion(proxSensors[0], -1,vec);


		// ----------------------------------------------------------

	for (int n = 0; n < notCollidedProxSensors.size(); n++) {
		float pos[3];
		simGetObjectPosition(notCollidedProxSensors[n], -1, pos);

		fourPoints.push_back(pos[0]);
		fourPoints.push_back(pos[1]);
		fourPoints.push_back(pos[2]);
	}
	currentStep = 0;
	return fourPoints;
	fourPoints.clear();
	notCollidedProxSensors.clear();

	currentStep++;
	return fourPoints;
}

int R_Module_LeafChloe::init(dynamixel::PacketHandler * packetH1, dynamixel::PacketHandler * packetH2, dynamixel::PortHandler * portH)
{
	return 0;
}

