#include "Module_LeafChloe.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Module_LeafChloe::Module_LeafChloe()
{

}


Module_LeafChloe::~Module_LeafChloe()
{
//	cout << "REMOVING MODULE!" << endl;

	removeModule();
}

int Module_LeafChloe::init() {
	return -1;
}


int Module_LeafChloe::mutate(float mutationRate) {
	bendAngle = randomNum->randFloat(-0.25 * M_PI,0.75 * M_PI);
	return 1;
}

int Module_LeafChloe::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	currentStep = 10;
	checkStep = 10;
	moduleID = 70; 
	int fsParams[5];
	fsParams[0] = 0;
	fsParams[1] = 1;
	fsParams[2] = 1;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.01;
	fsFParams[1] = 1000.1;
	fsFParams[2] = 1000.1;
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(1, fsParams, fsFParams, NULL);

	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];

	float fsPos[3];
	fsPos[0] = configuration[0];
	fsPos[1] = configuration[1];
	fsPos[2] = configuration[2];

	simSetObjectParent(fs, parentHandle, false);
	simSetObjectOrientation(fs, relativePosHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);


	// TO DO: Check collision
	// cout << "Creating Bend" << endl; 
//		int baseHandle = 0;
		float objectOrigin[3];
		float zeroOrigin[3] = { 0,0,0 };
		
		objectOrigin[0] = configuration[0];
		objectOrigin[1] = configuration[1];
		objectOrigin[2] = configuration[2] + 0.02751;
	
		float rotationOrigin[3];
		rotationOrigin[0] = configuration[3];
		rotationOrigin[1] = configuration[4]; 
		rotationOrigin[2] = configuration[5]; 

	//	cout << "objectOrigin[2] = " << objectOrigin[2] << endl; 
		float size[3] = { 0.055,0.055,0.055 };
		float mass = 0.033;
//		int baseHandle = simCreatePureShape(0, 8, size, mass, false);
		int baseHandle = simCreatePureShape(0, objectPhysics, size, mass, NULL);
		//	cout << "baseHandle = " << baseHandle << endl; 
		
		simSetObjectPosition(baseHandle, relativePosHandle, zeroOrigin);
		simSetObjectOrientation(baseHandle, relativePosHandle, rotationOrigin);
		simSetObjectPosition(baseHandle, baseHandle, objectOrigin);
		simSetObjectParent(baseHandle, fs, true);

		float leafSize[3] = { 0.01,0.05,0.08 };
		int leaf1 = simCreatePureShape(0, objectPhysics, leafSize, mass, 0);
		int leaf2 = simCreatePureShape(0, objectPhysics, leafSize, mass, 0);
		int leaf3 = simCreatePureShape(0, objectPhysics, leafSize, mass, 0);
		int leaf4 = simCreatePureShape(0, objectPhysics, leafSize, mass, 0);
		int leaf5 = simCreatePureShape(0, objectPhysics, leafSize, mass, 0);

		//	float appearanceSize[3] = { 0.02, 0.02, 0.04 };
		//	int appearancePeace = simCreatePureShape(0, 8, appearanceSize, 0, false);
		//	float appearancePos[3] = { 0,0,0.0225 };
		float jointSize[3] = { 0.045,0.005, 0 };
		//	int joint = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
//		float baseHandlePos[3] = { 0,0,size[2] / 2 };
		float radius = 0.041;
		float degree = 0.4 * M_PI;

		int joint1 = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
		int joint2 = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
		int joint3 = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
		int joint4 = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
		int joint5 = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);


		//	simSetObjectPosition(appearancePeace, baseHandle, appearancePos);
		float baseHandleOr[3] = { 0,0,0 };
		float leaf1Or[3] = { 0,0,0 };
		float leaf2Or[3] = { 0,0,(2.0 * M_PI * 1 * 0.2) };
		float leaf3Or[3] = { 0,0,(2.0 * M_PI * 2 * 0.2) };
		float leaf4Or[3] = { 0,0,(2.0 * M_PI * 3 * 0.2) };
		float leaf5Or[3] = { 0,0,(2.0 * M_PI * 4 * 0.2) };
	//	simSetObjectOrientation(baseHandle, -1, baseHandleOr);
		simSetObjectOrientation(leaf1, baseHandle, leaf1Or);
		simSetObjectOrientation(leaf2, baseHandle, leaf2Or);
		simSetObjectOrientation(leaf3, baseHandle, leaf3Or);
		simSetObjectOrientation(leaf4, baseHandle, leaf4Or);
		simSetObjectOrientation(leaf5, baseHandle, leaf5Or);

		float leaf1Pos[3] = { radius * cosf(0 * degree), radius * sinf(0 * degree),0.065 };
		float leaf2Pos[3] = { radius * cosf(1 * degree), radius * sinf(1 * degree),0.065 };
		float leaf3Pos[3] = { radius * cosf(2 * degree), radius * sinf(2 * degree),0.065 };
		float leaf4Pos[3] = { radius * cosf(3 * degree), radius * sinf(3 * degree),0.065 };
		float leaf5Pos[3] = { radius * cosf(4 * degree), radius * sinf(4 * degree),0.065 };

//		simSetObjectPosition(baseHandle, -1, objectOrigin);
		simSetObjectPosition(leaf1, baseHandle, leaf1Pos);
		simSetObjectPosition(leaf2, baseHandle, leaf2Pos);
		simSetObjectPosition(leaf3, baseHandle, leaf3Pos);
		simSetObjectPosition(leaf4, baseHandle, leaf4Pos);
		simSetObjectPosition(leaf5, baseHandle, leaf5Pos);

		float joint1Pos[3] = { radius * cosf(0 * degree), radius * sinf(0 * degree),0.025 };
		float joint2Pos[3] = { radius * cosf(1 * degree), radius * sinf(1 * degree),0.025 };
		float joint3Pos[3] = { radius * cosf(2 * degree), radius * sinf(2 * degree),0.025 };
		float joint4Pos[3] = { radius * cosf(3 * degree), radius * sinf(3 * degree),0.025 };
		float joint5Pos[3] = { radius * cosf(4 * degree), radius * sinf(4 * degree),0.025 };

		float joint1Or[3] = { 0.5*M_PI,(1.0 * M_PI),0 };
		float joint2Or[3] = { 0.5*M_PI,((1.0 * M_PI) + (2.0 * M_PI * 1 * 0.2)),0 };
		float joint3Or[3] = { 0.5*M_PI,((1.0 * M_PI) + (2.0 * M_PI * 2 * 0.2)),0 };
		float joint4Or[3] = { 0.5*M_PI,((1.0 * M_PI) + (2.0 * M_PI * 3 * 0.2)),0 };
		float joint5Or[3] = { 0.5*M_PI,((1.0 * M_PI) + (2.0 * M_PI * 4 * 0.2)),0 };

		simSetObjectOrientation(joint1, baseHandle, joint1Or);
		simSetObjectOrientation(joint2, baseHandle, joint2Or);
		simSetObjectOrientation(joint3, baseHandle, joint3Or);
		simSetObjectOrientation(joint4, baseHandle, joint4Or);
		simSetObjectOrientation(joint5, baseHandle, joint5Or);

		simSetObjectPosition(joint1, baseHandle, joint1Pos);
		simSetObjectPosition(joint2, baseHandle, joint2Pos);
		simSetObjectPosition(joint3, baseHandle, joint3Pos);
		simSetObjectPosition(joint4, baseHandle, joint4Pos);
		simSetObjectPosition(joint5, baseHandle, joint5Pos);

		simSetObjectParent(joint1, baseHandle, true);
		simSetObjectParent(joint2, baseHandle, true);
		simSetObjectParent(joint3, baseHandle, true);
		simSetObjectParent(joint4, baseHandle, true);
		simSetObjectParent(joint5, baseHandle, true);

		simSetObjectParent(leaf1, joint1, true);
		simSetObjectParent(leaf2, joint2, true);
		simSetObjectParent(leaf3, joint3, true);
		simSetObjectParent(leaf4, joint4, true);
		simSetObjectParent(leaf5, joint5, true);

		vector <int> jointHandles;
		jointHandles.push_back(joint1);
		jointHandles.push_back(joint2);
		jointHandles.push_back(joint3);
		jointHandles.push_back(joint4);
		jointHandles.push_back(joint5);

		for (int i = 0; i < 5; i++) {
			simSetJointPosition(jointHandles[i], 0.5 * M_PI);
			simSetJointPosition(jointHandles[i], 0.5 * M_PI);
			simSetJointPosition(jointHandles[i], 0.5 * M_PI);
			simSetJointPosition(jointHandles[i], 0.5 * M_PI);
			simSetJointPosition(jointHandles[i], 0.5 * M_PI);

			simSetObjectInt32Parameter(jointHandles[i], 2000, 1);
			simSetObjectInt32Parameter(jointHandles[i], 2001, 1);
			simSetObjectFloatParameter(jointHandles[i], 2002, 1);
			simSetObjectFloatParameter(jointHandles[i], 2003, 0.01);
			simSetObjectFloatParameter(jointHandles[i], 2004, 0.0);
			//	sim_jointintparam_ctrl_enabled(2001) : int32 parameter : dynamic motor control loop enable state(0 or != 0)
			//	simSetJointTargetVelocity(joint, 0.2);
			//	simSetJointForce(joint, 100);
			simSetJointTargetPosition(jointHandles[i], 1.0);
			simSetJointForce(jointHandles[i], 1.0);
			simSetJointTargetVelocity(jointHandles[i], 0.05);
		}

		int intpars[8] = { 1,1,1,1,1,1,0,0 };
		float sensorSize = 0.004;
		float floatpars[15] = { 0,10,0.0010,0.001,0.001,0.0001,0.001,0.0001,0.0001,0.0001,0.0001,0.0001,sensorSize,0,0 };
		vector<vector<float>> proxPositions; 
		vector<float> pos1;
		vector<float> pos2;
		vector<float> pos3;
		vector<float> pos4;
		vector<float> pos5;
		pos1.push_back(-0.01); 		pos1.push_back(leafSize[1] * 0.0);		pos1.push_back(0.0);
		pos2.push_back(-0.01); 		pos2.push_back(leafSize[1] * -0.4);		pos2.push_back(leafSize[2] * 0.45);
		pos3.push_back(-0.01); 		pos3.push_back(leafSize[1] * -0.4);		pos3.push_back(leafSize[2] * -0.45);
		pos4.push_back(-0.01); 		pos4.push_back(leafSize[1] * 0.4);		pos4.push_back(leafSize[2] * 0.45);
		pos5.push_back(-0.01); 		pos5.push_back(leafSize[1] * 0.4);		pos5.push_back(leafSize[2] * -0.45);
		proxPositions.push_back(pos1);
		proxPositions.push_back(pos2);
		proxPositions.push_back(pos3);
		proxPositions.push_back(pos4);
		proxPositions.push_back(pos5);

		vector<int> leafHandles;
		leafHandles.push_back(leaf1);
		leafHandles.push_back(leaf2);
		leafHandles.push_back(leaf3);
		leafHandles.push_back(leaf4);
		leafHandles.push_back(leaf5);

		// leaf one
		
		for (int l = 0; l < 5; l++) {
			for (int n = 0; n < amountProxSensors; n++) {
				int proxSens = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
				float pos[3] = { proxPositions[n][0], proxPositions[n][1], proxPositions[n][2] };
				simSetObjectPosition(proxSens, leafHandles[l], pos);
				simSetObjectParent(proxSens, leafHandles[l], true);
				proxSensors.push_back(proxSens);
			//	simSetObjectInt32Parameter(proxSens,10, 0);
				objectHandles.push_back(proxSens);
			}
		}
	/*	int proxSens = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
		float pos[3] = { -0.04,0.0,0.0 };
		simSetObjectPosition(proxSens, leaf1, pos);
		simSetObjectParent(proxSens, leaf1, true);
		proxSensors.push_back(proxSens);
		int prox2 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
		simSetObjectPosition(prox2, leaf2, pos);
		simSetObjectParent(prox2, leaf2, true);
		proxSensors.push_back(prox2);
		int prox3 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
		simSetObjectPosition(prox3, leaf3, pos);
		simSetObjectParent(prox3, leaf3, true);
		proxSensors.push_back(prox3);
		int prox4 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
		simSetObjectPosition(prox4, leaf4, pos);
		simSetObjectParent(prox4, leaf4, true);
		proxSensors.push_back(prox4);
		int prox5 = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
		simSetObjectPosition(prox5, leaf5, pos);
		simSetObjectParent(prox5, leaf5, true);
		proxSensors.push_back(prox5);*/

		//float size[3] = { 0.055, 0.055, 0.055 };
		//float size2[3] = { 0.15,0.15,0.01 };
		//float orientation[3] = { 0.1,0.1,0.1 };
		//float mass = 0.07;
		//float mass2 = 0.03;
		//float red[3] = { 1, 0, 0 };
		//float blue[3] = { 0, 0, 1 };
		//float green[3] = { 0, 1, 0 };
		//objectOrigin[0] = 0;
		//objectOrigin[1] = 0;
		//objectOrigin[2] = 0.05 + (0.501 * size[2]);
	

		// create a box from lines; 
		vector<vector<float>> cubeVertex; // 8 points in 3d space
		vector<vector<float>> points;
		points.resize(8);
	


	//	float jointSize[3] = { 0.01,0.01,0.01 };
		// add hinge
	//	int hingeJoint = simCreateForceSensor(0, fsParams, fsFParams, NULL);//simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 1, jointSize, NULL, NULL);
	//	float jointPos[3] = { 0, -0.0, (0.5 * size[2]) };
	//	simSetObjectPosition(hingeJoint, baseHandle, jointPos);
	//	float jointOrientation[3] = { 0.0*M_PI,0.0*M_PI,0.0 * M_PI };
	//	simSetObjectOrientation(hingeJoint, baseHandle, jointOrientation);
	//	simSetObjectParent(hingeJoint, baseHandle, true);
	
	//	cout << "hingeJoint = " << hingeJoint << endl; 
	
	//	int cube2 = simCreatePureShape(0, objectPhysics, size2, mass2, NULL);
	//	float cube2Origin[3] = { 0, 0.0, (0.5 * size[2]) + (0.2501 * size2[2])};
	//	float rotation[3] = { 0.0*M_PI,0.0*M_PI,0.0001 * M_PI };
	//	simSetObjectPosition(cube2, baseHandle, cube2Origin);
	//	simSetObjectOrientation(cube2, baseHandle, rotation);
	//	simPauseSimulation();
	//	simSetObjectParent(cube2, hingeJoint, true);
	//	simSetJointPosition(hingeJoint, bendAngle);
	
	//	cout << "cube 2 = " << cube2 << endl; 
		
	//	cout << "the RGB data of 0 = " << lGenome.lParameters[newState].rgb[0] << ", " << lGenome.lParameters[newState].rgb[1] << ", " << lGenome.lParameters[newState].rgb[2] << endl;
	
//	simSetObjectName(baseHandle, "part" + parentHandle);
//	simSetObjectName(cube2, "part" + parentHandle);
//	simSetObjectName(hingeJoint, "joint" + parentHandle);
	
	//cout << "done creating bend" << endl; 

	//float objectAddedPosition[3] = { 0,0,0.1};
	//simSetObjectPosition(baseHandle, parentHandle, objectAddedPosition);

	
	freeSites = sites;
	objectHandles.push_back(fs); 
	objectHandles.push_back(baseHandle);
	objectHandles.push_back(joint1);
	objectHandles.push_back(joint2);
	objectHandles.push_back(joint3);
	objectHandles.push_back(joint4);
	objectHandles.push_back(joint5);
	objectHandles.push_back(leaf1);
	objectHandles.push_back(leaf2);
	objectHandles.push_back(leaf3);
	objectHandles.push_back(leaf4);
	objectHandles.push_back(leaf5);
	
//	objectHandles.push_back(hingeJoint);
//	objectHandles.push_back(cube2);
//	for (int i = 0; i < 3; i++) {
//		if (simCheckCollision(objectHandles[i], parentHandle) == true) {
//			cout << "Bend Collided" << endl;
//			return-1;
//		}
//	}
	for (int j = 0; j < objectHandles.size(); j++) {
		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
	}

	return baseHandle;
}

vector<int> Module_LeafChloe::getFreeSites(vector<int> targetSites) {
	vector<int> tempFreeSites;
	for (int i = 0; i < targetSites.size(); i++) {
		for (int j = i; j < freeSites.size(); j++) {
			if (targetSites[i] == freeSites[j]) {
				tempFreeSites.push_back(targetSites[i]);
			}
		}
	}
	return tempFreeSites;
}
vector<int> Module_LeafChloe::getObjectHandles() {
	return objectHandles;
}

void Module_LeafChloe::setModuleColor() {

}

shared_ptr<ER_Module> Module_LeafChloe::clone() {
	return shared_ptr<ER_Module>(new Module_LeafChloe(*this));
}

void Module_LeafChloe::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_LeafChloe::updateModule(vector<float> input) {
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
		//int rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0, -1, 2, blue, NULL, NULL, blue); // startEndLine );
		//float ab[6] = { proxSensPos[0], proxSensPos[1], proxSensPos[2], proxSensPos[0] +(normalizedRayVector[0] * 10), 
		//	proxSensPos[1] + (normalizedRayVector[1] * 10), proxSensPos[2] + (normalizedRayVector[2] * 10) };
		//simAddDrawingObjectItem(rayHandle, ab);

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


stringstream Module_LeafChloe::getModuleParams() {
	// needs to save variables of the module
	stringstream ss;
	ss << "#ModuleType:," << 6 << endl;  // make sure this is the same number as in the module factory
	/*ss << "#siteParameters:,";
	for (int i = 0; i < siteConfigurations.size(); i++) {
		ss << "/t,#site:," << 1 << "," << endl;
		for (int j = 0; j < siteConfigurations[i].size(); j++) {
			ss << siteConfigurations[i][j].x << ",";
			ss << siteConfigurations[i][j].y << ",";
			ss << siteConfigurations[i][j].z << ",";
			ss << siteConfigurations[i][j].rX << ",";
			ss << siteConfigurations[i][j].rY << ",";
			ss << siteConfigurations[i][j].rZ << ",";
		}
	}*/
	ss << endl;
	return ss; 
}

void Module_LeafChloe::setModuleParams(vector<string> values) {

}

vector<float> Module_LeafChloe::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_LeafChloe::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}