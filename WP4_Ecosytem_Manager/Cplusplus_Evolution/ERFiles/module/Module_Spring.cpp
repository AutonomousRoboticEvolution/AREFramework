#include "Module_Spring.h"
#include <iostream>
#include <cmath>

using namespace std;

Module_Spring::Module_Spring()
{

	/*vector<SITE> site1;
	site1.resize(4);
	for (int i = 0; i < 4; i++) {
		site1[i].x = 0.0;
		site1[i].y = 0.0;
		site1[i].z = 0.0;
		site1[i].rX = 0.0 * M_PI;
		site1[i].rY = 0.0 * M_PI;
		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(0);
	for (int i = 0; i < 4; i++) {
		site1[i].x = 0.0;
		site1[i].y = -0.1;
		site1[i].z = -0.00;
		site1[i].rX = 0.5 * M_PI;
		site1[i].rY = 0.0 * M_PI;
		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(1);
	for (int i = 0; i < 4; i++) {
		site1[i].x = 0.0;
		site1[i].y = 0.1;
		site1[i].z = -0.0;
		site1[i].rX = -0.5 * M_PI;
		site1[i].rY = 0.0 * M_PI;
		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(2);
	for (int i = 0; i < 4; i++) {
		site1[i].x = 0.1;
		site1[i].y = 0.0;
		site1[i].z = -0.0;
		site1[i].rX = -0.5 * M_PI;
		site1[i].rY = 0.5 * M_PI;
		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(3);
	for (int i = 0; i < 4; i++) {
		site1[i].x = -0.1;
		site1[i].y = 0.0;
		site1[i].z = -0.0;
		site1[i].rX = 0.5 * M_PI;
		site1[i].rY = -0.5 * M_PI;
		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(4);*/
}


Module_Spring::~Module_Spring()
{
//	cout << "REMOVING MODULE!" << endl;
	simRemoveModel(baseHandle);
	removeModule();
}

int Module_Spring::init() {
	return -1;
}

int Module_Spring::mutate(float mutationRate) {
	bendAngle = randomNum->randFloat(-0.25 * M_PI, 0.75 * M_PI);
	return 1;
}

int Module_Spring::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {

	int sprJ = simLoadModel("models/SpringJointModule.ttm");
	baseHandle = sprJ;
	int shapeAmount[1];
	int *shapesPointer = simGetObjectsInTree(sprJ, sim_object_shape_type, 1, shapeAmount);
	int shapes[2];
	shapes[0] = shapesPointer[0];
	shapes[1] = shapesPointer[1];
	simReleaseBuffer((simChar*)shapesPointer);
	int joint;
	int jointAmount[1];
	int *jointPointer = simGetObjectsInTree(sprJ, sim_object_joint_type, 1, jointAmount);
	joint = jointPointer[0];
	simReleaseBuffer((simChar*)jointPointer);

	//float rdrOr[3];
	//rdrOr[0] = 0;
	//rdrOr[1] = 0.5 * M_PI;
	//rdrOr[2] = 0;

	int fsParams[5];
	fsParams[0] = 0;
	fsParams[1] = 1;
	fsParams[2] = 10; // settings->consecutiveThresholdViolations;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.005;
	fsFParams[1] = 80; //settings->maxForceSensor;
	fsFParams[2] = 1.7; //settings->maxForceSensor; // change torque
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(3, fsParams, fsFParams, NULL);

	int d1 = simGetObjectHandle("attachDum");

	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];
	float zeroOrigin[3] = { 0,0,0 };

	float fsPos[3];
	fsPos[0] = 0.0;
	fsPos[1] = 0.0;
	fsPos[2] = configuration[2];
	float zeroPos[3];
	zeroPos[0] = 0.0;
	zeroPos[1] = 0.0;
	zeroPos[2] = 0.0;

	simSetObjectPosition(fs, relativePosHandle, zeroPos);
	simSetObjectOrientation(fs, relativePosHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);
	simSetObjectParent(fs, parentHandle, true);

	float objectOrigin[3];
	objectOrigin[0] = 0.0;
	objectOrigin[1] = 0.0;
	objectOrigin[2] = 0.0385001;

	float rotationOrigin[3];
	rotationOrigin[0] = 0.0;
	rotationOrigin[1] = 0.0 - 0.5 * M_PI;
	rotationOrigin[2] = 0.0;


	simSetObjectOrientation(sprJ, fs, rotationOrigin);
	simSetObjectPosition(sprJ, fs, objectOrigin);

	simSetObjectParent(sprJ, fs, true);
	vector<shared_ptr<SITE>> site1;
	for (int i = 0; i < 4; i++) {
		site1.push_back(shared_ptr<SITE>(new SITE));
	}
	vector<shared_ptr<SITE>> site2;
	for (int i = 0; i < 4; i++) {
		site2.push_back(shared_ptr<SITE>(new SITE));
	}
	vector<shared_ptr<SITE>> site3;
	for (int i = 0; i < 4; i++) {
		site3.push_back(shared_ptr<SITE>(new SITE));
	}
	vector<shared_ptr<SITE>> site4;
	for (int i = 0; i < 4; i++) {
		site4.push_back(shared_ptr<SITE>(new SITE));
	}
	vector<shared_ptr<SITE>> site5;
	for (int i = 0; i < 4; i++) {
		site5.push_back(shared_ptr<SITE>(new SITE));
	}
	for (int i = 0; i < 4; i++) {
		site1[i]->x = 0.0;
		site1[i]->y = 0.0;
		site1[i]->z = 0.0;
		site1[i]->rX = 0.0 * M_PI;
		site1[i]->rY = 0.0 * M_PI;
		site1[i]->rZ = (0.0 + (0.5*i)) * M_PI;
		site1[i]->parentHandle = shapes[1];
		site1[i]->relativePosHandle = d1;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(0);

	freeSites = sites;
	int objectAmount[1];
	int *allHandles = simGetObjectsInTree(sprJ, sim_handle_all, 1, objectAmount);
	for (int i = 0; i < objectAmount[0]; i++) {
		objectHandles.push_back(allHandles[i]);
	}
	simReleaseBuffer((simChar*)allHandles);
	simReleaseBuffer(NULL);
	objectHandles.push_back(fs);
	//	simSetObjectParent(rdr, -1, true);
	//	attachHandles.push_back(rdr);
	simRemoveObject(sprJ);
	//	objectHandles.push_back(fs);
	//	objectHandles.push_back(shapes[0]);
	//	objectHandles.push_back(shapes[1]);
	//	objectHandles.push_back(joint);
	controlHandles.push_back(joint);
	return shapes[0];
}
//
//int Module_Spring::createModuleBackup(vector<float> configuration, int relativePosHandle, int parentHandle) {
//	
//	int fsParams[5];
//	fsParams[0] = 1;
//	fsParams[1] = 5;
//	fsParams[2] = 5;
//	fsParams[3] = 0;
//	fsParams[4] = 0;
//	float fsFParams[5];
//	fsFParams[0] = 0.01;
//	fsFParams[1] = 0.001;
//	fsFParams[2] = 0.001;
//	fsFParams[3] = 0;
//	fsFParams[4] = 0;
//	int fs = simCreateForceSensor(0, fsParams, fsFParams, NULL);
//
//	float fsR[3];
//	fsR[0] = configuration[3];
//	fsR[1] = configuration[4];
//	fsR[2] = configuration[5];
//
//	float fsPos[3];
//	fsPos[0] = configuration[0];
//	fsPos[1] = configuration[1];
//	fsPos[2] = configuration[2];
//
//	simSetObjectParent(fs, parentHandle, false);
//	simSetObjectOrientation(fs, parentHandle, fsR);
//	simSetObjectPosition(fs, fs, fsPos);
//
//	// TO DO: Check collision
//	// cout << "Creating Bend" << endl; 
//		
//		int baseHandle = 0;
//		float objectOrigin[3];
//		
//		objectOrigin[0] = configuration[0];
//		objectOrigin[1] = configuration[1];
//		objectOrigin[2] = configuration[2] + 0.02751;
//	
//		float rotationOrigin[3];
//		rotationOrigin[0] = configuration[3];
//		rotationOrigin[1] = configuration[4]; 
//		rotationOrigin[2] = configuration[5]; 
//
//	//	cout << "objectOrigin[2] = " << objectOrigin[2] << endl; 
//		
//	//	objectOrigin[0] = 0;
//	//	objectOrigin[1] = 0;
//	//	objectOrigin[2] = 0.1;
//		float size[3] = { 0.055, 0.055, 0.055 };
//		float orientation[3] = { 0.1,0.1,0.1 };
//		float mass = 0.1;
//		float red[3] = { 1, 0, 0 };
//		float blue[3] = { 0, 0, 1 };
//		float green[3] = { 0, 1, 0 };
//	
//		// create a box from lines; 
//		vector<vector<float>> cubeVertex; // 8 points in 3d space
//		vector<vector<float>> points;
//		points.resize(8);
//	
//		baseHandle = simCreatePureShape(0, objectPhysics, size, mass, NULL);
//	//	cout << "baseHandle = " << baseHandle << endl; 
//		float zeroOrigin[3] = { 0,0,0 };
//		simSetObjectPosition(baseHandle, parentHandle, zeroOrigin);
//		simSetObjectOrientation(baseHandle, parentHandle, rotationOrigin);
//		simSetObjectPosition(baseHandle, baseHandle, objectOrigin);
//		simSetObjectParent(baseHandle, fs, true);
//
//
//		//simSetObjectParent(baseHandle, parentHandle, false);
//		//simSetObjectOrientation(baseHandle, parentHandle, rotationOrigin);
//		//
//		//simSetObjectPosition(baseHandle, parentHandle, zeroOrigin);
//		//simSetObjectPosition(baseHandle, baseHandle, objectOrigin);
//
//		//simSetObjectParent(baseHandle, fs, true);
//
//		// add hinge
//		float jointSize[3] = {0.01,0.01,0.01};
//		int hingeJoint = simCreateJoint(sim_joint_prismatic_subtype, sim_jointmode_force, 1, jointSize, NULL, NULL);
//		float jointPos[3] = { 0, 0, 0.02751 };// { 0, -0.05 * 0.2751 * 2, 0.05 * 0.2751 * 2 };
//		simSetObjectPosition(hingeJoint, baseHandle, jointPos);
//		float jointOrientation[3] = { 0.0*M_PI,0.0*M_PI,0.0 * M_PI };
//		simSetObjectOrientation(hingeJoint, baseHandle, jointOrientation);
//		simSetObjectParent(hingeJoint, baseHandle, true);
//	
//	//	cout << "hingeJoint = " << hingeJoint << endl; 
//	
//		int cube2 = simCreatePureShape(0, objectPhysics, size, mass, NULL);
//		float cube2Origin[3] = { 0,0 ,0.02751 * 4 };
//		float rotation[3] = { 0*M_PI,0.0*M_PI,0.0 * M_PI };
//		simSetObjectPosition(cube2, baseHandle, cube2Origin);
//		simSetObjectOrientation(cube2, baseHandle, rotation);
////		simPauseSimulation();
//		simSetObjectParent(cube2, hingeJoint, true);
//	
//	//	cout << "cube 2 = " << cube2 << endl; 
//		
//	//	cout << "the RGB data of 0 = " << lGenome.lParameters[newState].rgb[0] << ", " << lGenome.lParameters[newState].rgb[1] << ", " << lGenome.lParameters[newState].rgb[2] << endl;
//	
//	simSetObjectName(baseHandle, "part" + parentHandle);
//	simSetObjectName(cube2, "part" + parentHandle);
//	simSetObjectName(hingeJoint, "joint" + parentHandle);
//	
//	//cout << "done creating bend" << endl; 
//
//	//float objectAddedPosition[3] = { 0,0,0.1};
//	//simSetObjectPosition(baseHandle, parentHandle, objectAddedPosition);
//
//	vector<SITE> site1;
//	site1.resize(4);
//	for (int i = 0; i < 4; i++) {
//		site1[i].x = 0.0;
//		site1[i].y = 0.0;
//		site1[i].z = 0.02751;
//		site1[i].rX = 0.0 * M_PI;
//		site1[i].rY = 0.0 * M_PI;
//		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
//		site1[i].parentHandle = cube2;
//	}
//	siteConfigurations.push_back(site1);
//	sites.push_back(0);
//	for (int i = 0; i < 4; i++) {
//		site1[i].x = 0.0;
//		site1[i].y = 0;//-0.1;
//		site1[i].z = 0.02751;
//		site1[i].rX = 0.5 * M_PI;
//		site1[i].rY = 0.0 * M_PI;
//		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
//		site1[i].parentHandle = cube2;
//	}
//	siteConfigurations.push_back(site1);
//	sites.push_back(1);
//	for (int i = 0; i < 4; i++) {
//		site1[i].x = 0.0;
//		site1[i].y = 0.0;
//		site1[i].z = 0.02751;
//		site1[i].rX = -0.5 * M_PI;
//		site1[i].rY = 0.0 * M_PI;
//		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
//		site1[i].parentHandle = cube2;
//	}
//	siteConfigurations.push_back(site1);
//	sites.push_back(2);
//	for (int i = 0; i < 4; i++) {
//		site1[i].x = 0.0;
//		site1[i].y = 0.0;
//		site1[i].z = 0.02751;
//		site1[i].rX = -0.5 * M_PI;
//		site1[i].rY = 0.5 * M_PI;
//		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
//		site1[i].parentHandle = cube2;
//	}
//	siteConfigurations.push_back(site1);
//	sites.push_back(3);
//	for (int i = 0; i < 4; i++) {
//		site1[i].x = 0.0;
//		site1[i].y = 0.0;
//		site1[i].z = 0.02751;
//		site1[i].rX = 0.5 * M_PI;
//		site1[i].rY = -0.5 * M_PI;
//		site1[i].rZ = (0.0 + (0.5 *i)) * M_PI;
//		site1[i].parentHandle = cube2;
//	}
//	siteConfigurations.push_back(site1);
//	sites.push_back(4);
//	freeSites = sites;
//	objectHandles.push_back(fs); 
//	objectHandles.push_back(baseHandle);
//	objectHandles.push_back(hingeJoint);
//	objectHandles.push_back(cube2);
//	attachHandles.push_back(cube2); 
////	for (int i = 0; i < 3; i++) {
////		if (simCheckCollision(objectHandles[i], parentHandle) == true) {
////			cout << "Bend Collided" << endl;
////			return-1;
////		}
////	}
//
//	simSetObjectInt32Parameter(hingeJoint, 2000, 1);
//	simSetObjectInt32Parameter(hingeJoint, 2001, 1);
//	simSetObjectFloatParameter(hingeJoint, 2002, 0.02); // p
////	simSetObjectFloatParameter(hingeJoint, 2003, 0.01); // i
////	simSetObjectFloatParameter(hingeJoint, 2004, 0.00010); // d
//	//	sim_jointintparam_ctrl_enabled(2001) : int32 parameter : dynamic motor control loop enable state(0 or != 0)
//	//	simSetJointTargetVelocity(joint, 0.2);
//	//	simSetJointForce(joint, 100);
//
//	simSetJointTargetPosition(hingeJoint, 0);
//	controlHandles.push_back(hingeJoint);
//	simSetJointForce(controlHandles[0], 2.0);
////	simSetJointTargetVelocity(controlHandles[0], 0.5);
//	bendAngle = -0.025;
//	for (int j = 0; j < objectHandles.size(); j++) {
//		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
//	}
//
//	return baseHandle;
//}

vector<int> Module_Spring::getFreeSites(vector<int> targetSites) {
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
vector<int> Module_Spring::getObjectHandles() {
	return objectHandles;
}

void Module_Spring::setModuleColor() {

}

shared_ptr<ER_Module> Module_Spring::clone() {
	return shared_ptr<ER_Module>(new Module_Spring(*this));
}

void Module_Spring::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_Spring::updateModule(vector<float> input) {
	simSetJointTargetPosition(controlHandles[0], 0.0);
	/*float pos[3];
	simGetJointPosition(controlHandles[0], pos);
	if (pos[0] > maxSpringPosition) {
		simSetJointPosition(controlHandles[0], maxSpringPosition);
	}
	if (pos[0] < minSpringPosition) {
		simSetJointPosition(controlHandles[0], minSpringPosition);
	}*/
	ER_Module::updateModule(input);
	return input;
}

stringstream Module_Spring::getModuleParams() {
	// needs to save variables of the module
	stringstream ss; 
	ss << "#ModuleType:," << 9 << endl;  // make sure this is the same number as the module factory
	/*ss << "#siteParameters:," << endl;
	for (int i = 0; i < siteConfigurations.size(); i++) {
		ss << "\t,#site:," << i << ","; 
		for (int j = 0; j < siteConfigurations[i].size(); j++) {
			ss << "conf," << j << ",";
			ss << siteConfigurations[i][j].x << ",";
			ss << siteConfigurations[i][j].y << ",";
			ss << siteConfigurations[i][j].z << ",";
			ss << siteConfigurations[i][j].rX << ",";
			ss << siteConfigurations[i][j].rY << ",";
			ss << siteConfigurations[i][j].rZ << ",";
		}
		ss << endl; 
	}*/
	ss << endl; 

	// save module control
	return ss; 
}

void Module_Spring::setModuleParams(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#bendAngle:") {
			it++;
			tmp = values[it];
			bendAngle = atof(tmp.c_str());
		}
	}
}

vector<float> Module_Spring::getPosition() {
	vector<float> positionVector; 
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector; 
};

stringstream Module_Spring::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << "#EndControl," << endl;
	ss << endl;
	return ss;
}

