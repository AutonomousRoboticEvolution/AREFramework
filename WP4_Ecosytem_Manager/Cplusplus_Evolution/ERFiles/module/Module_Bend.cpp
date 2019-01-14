#include "Module_Bend.h"
#include <iostream>
#include <cmath>

using namespace std;

Module_Bend::Module_Bend()
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


Module_Bend::~Module_Bend()
{
//	cout << "REMOVING MODULE!" << endl;
	removeModule();
}

int Module_Bend::init() {
	return -1;
}

int Module_Bend::mutate(float mutationRate) {
	bendAngle = randomNum->randFloat(-0.25 * M_PI, 0.75 * M_PI);
	return 1;
}

int Module_Bend::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	
	int fsParams[5];
	fsParams[0] = 1;
	fsParams[1] = 5;
	fsParams[2] = 5;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.01;
	fsFParams[1] = 0.001;
	fsFParams[2] = 0.001;
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(0, fsParams, fsFParams, NULL);

	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];

	float fsPos[3];
	fsPos[0] = configuration[0];
	fsPos[1] = configuration[1];
	fsPos[2] = configuration[2];

	simSetObjectParent(fs, parentHandle, false);
	simSetObjectOrientation(fs, parentHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);

	// TO DO: Check collision
	// cout << "Creating Bend" << endl; 
		
		int baseHandle = 0;
		float objectOrigin[3];
		
		objectOrigin[0] = configuration[0];
		objectOrigin[1] = configuration[1];
		objectOrigin[2] = configuration[2] + 0.02751;
	
		float rotationOrigin[3];
		rotationOrigin[0] = configuration[3];
		rotationOrigin[1] = configuration[4]; 
		rotationOrigin[2] = configuration[5]; 

	//	cout << "objectOrigin[2] = " << objectOrigin[2] << endl; 
		
	//	objectOrigin[0] = 0;
	//	objectOrigin[1] = 0;
	//	objectOrigin[2] = 0.1;
		float size[3] = { 0.055, 0.055, 0.055 };
		float orientation[3] = { 0.1,0.1,0.1 };
		float mass = 0.1;
		float red[3] = { 1, 0, 0 };
		float blue[3] = { 0, 0, 1 };
		float green[3] = { 0, 1, 0 };
	
		// create a box from lines; 
		vector<vector<float>> cubeVertex; // 8 points in 3d space
		vector<vector<float>> points;
		points.resize(8);
	
		baseHandle = simCreatePureShape(0, objectPhysics, size, mass, NULL);
	//	cout << "baseHandle = " << baseHandle << endl; 
		float zeroOrigin[3] = { 0,0,0 };
		simSetObjectPosition(baseHandle, parentHandle, zeroOrigin);
		simSetObjectOrientation(baseHandle, parentHandle, rotationOrigin);
		simSetObjectPosition(baseHandle, baseHandle, objectOrigin);
		simSetObjectParent(baseHandle, fs, true);


		//simSetObjectParent(baseHandle, parentHandle, false);
		//simSetObjectOrientation(baseHandle, parentHandle, rotationOrigin);
		//
		//simSetObjectPosition(baseHandle, parentHandle, zeroOrigin);
		//simSetObjectPosition(baseHandle, baseHandle, objectOrigin);

		//simSetObjectParent(baseHandle, fs, true);

		// add hinge
		float jointSize[3] = {0.001,0.001,0.001};
		int hingeJoint = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 1, jointSize, NULL, NULL);
		float jointPos[3] = { 0, -0.05 * 0.2751 * 2, 0.05 * 0.2751 * 2 };
		simSetObjectPosition(hingeJoint, baseHandle, jointPos);
		float jointOrientation[3] = { 0.0*M_PI,0.5*M_PI,0.0 * M_PI };
		simSetObjectOrientation(hingeJoint, baseHandle, jointOrientation);
		simSetObjectParent(hingeJoint, baseHandle, true);
	
	//	cout << "hingeJoint = " << hingeJoint << endl; 
	
		int cube2 = simCreatePureShape(0, objectPhysics, size, mass, NULL);
		float cube2Origin[3] = { 0, - 0.02751 ,0.1205 * 0.2751 * 2 };
		float rotation[3] = { 0.25*M_PI,0.0*M_PI,0.0 * M_PI };
		simSetObjectPosition(cube2, baseHandle, cube2Origin);
		simSetObjectOrientation(cube2, baseHandle, rotation);
//		simPauseSimulation();
		simSetObjectParent(cube2, hingeJoint, true);
		simSetJointPosition(hingeJoint, bendAngle);
	
	//	cout << "cube 2 = " << cube2 << endl; 
		
	//	cout << "the RGB data of 0 = " << lGenome.lParameters[newState].rgb[0] << ", " << lGenome.lParameters[newState].rgb[1] << ", " << lGenome.lParameters[newState].rgb[2] << endl;
	
	simSetObjectName(baseHandle, "part" + parentHandle);
	simSetObjectName(cube2, "part" + parentHandle);
	simSetObjectName(hingeJoint, "joint" + parentHandle);
	
	//cout << "done creating bend" << endl; 

	//float objectAddedPosition[3] = { 0,0,0.1};
	//simSetObjectPosition(baseHandle, parentHandle, objectAddedPosition);

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
		site1[i]->z = 0.02751;
		site1[i]->rX = 0.0 * M_PI;
		site1[i]->rY = 0.0 * M_PI;
		site1[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site1[i]->parentHandle = cube2;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(0);
	for (int i = 0; i < 4; i++) {
		site2[i]->x = 0.0;
		site2[i]->y = 0;//-0.1;
		site2[i]->z = 0.02751;
		site2[i]->rX = 0.5 * M_PI;
		site2[i]->rY = 0.0 * M_PI;
		site2[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site2[i]->parentHandle = cube2;
	}
	siteConfigurations.push_back(site2);
	sites.push_back(1);
	for (int i = 0; i < 4; i++) {
		site3[i]->x = 0.0;
		site3[i]->y = 0.0;
		site3[i]->z = 0.02751;
		site3[i]->rX = -0.5 * M_PI;
		site3[i]->rY = 0.0 * M_PI;
		site3[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site3[i]->parentHandle = cube2;
	}
	siteConfigurations.push_back(site3);
	sites.push_back(2);
	for (int i = 0; i < 4; i++) {
		site3[i]->x = 0.0;
		site3[i]->y = 0.0;
		site3[i]->z = 0.02751;
		site3[i]->rX = -0.5 * M_PI;
		site3[i]->rY = 0.5 * M_PI;
		site3[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site3[i]->parentHandle = cube2;
	}
	siteConfigurations.push_back(site4);
	sites.push_back(3);
	for (int i = 0; i < 4; i++) {
		site3[i]->x = 0.0;
		site3[i]->y = 0.0;
		site3[i]->z = 0.02751;
		site3[i]->rX = 0.5 * M_PI;
		site3[i]->rY = -0.5 * M_PI;
		site3[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site3[i]->parentHandle = cube2;
	}
	siteConfigurations.push_back(site5);
	sites.push_back(4);
	freeSites = sites;
	objectHandles.push_back(fs); 
	objectHandles.push_back(baseHandle);
	objectHandles.push_back(hingeJoint);
	objectHandles.push_back(cube2);
	attachHandles.push_back(cube2); 
//	for (int i = 0; i < 3; i++) {
//		if (simCheckCollision(objectHandles[i], parentHandle) == true) {
//			cout << "Bend Collided" << endl;
//			return-1;
//		}
//	}

	simSetObjectInt32Parameter(hingeJoint, 2000, 1);
	simSetObjectInt32Parameter(hingeJoint, 2001, 1);
	simSetObjectFloatParameter(hingeJoint, 2002, 1);
	simSetObjectFloatParameter(hingeJoint, 2003, 0.01);
	simSetObjectFloatParameter(hingeJoint, 2004, 0.0);
	//	sim_jointintparam_ctrl_enabled(2001) : int32 parameter : dynamic motor control loop enable state(0 or != 0)
	//	simSetJointTargetVelocity(joint, 0.2);
	//	simSetJointForce(joint, 100);
	simSetJointTargetPosition(hingeJoint, bendAngle);
	controlHandles.push_back(hingeJoint);
	simSetJointForce(controlHandles[0], 1.0);
	simSetJointTargetVelocity(controlHandles[0], 0.05);
	for (int j = 0; j < objectHandles.size(); j++) {
		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
	}

	return baseHandle;
}

vector<int> Module_Bend::getFreeSites(vector<int> targetSites) {
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
vector<int> Module_Bend::getObjectHandles() {
	return objectHandles;
}

void Module_Bend::setModuleColor() {

}

shared_ptr<ER_Module> Module_Bend::clone() {
	return shared_ptr<ER_Module>(new Module_Bend(*this));
}

void Module_Bend::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_Bend::updateModule(vector<float> input) {
	simSetJointTargetPosition(controlHandles[0], bendAngle);
	ER_Module::updateModule(input);
	return input;
}

stringstream Module_Bend::getModuleParams() {
	// needs to save variables of the module
	stringstream ss; 
	ss << "#ModuleType:," << 2 << endl;  // make sure this is the same number as the module factory
	ss << "#bendAngle:," << bendAngle << "," << endl; 
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

void Module_Bend::setModuleParams(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#bendAngle:") {
			it++;
			tmp = values[it];
			bendAngle = atof(tmp.c_str());
		}
	}
}

vector<float> Module_Bend::getPosition() {
	vector<float> positionVector; 
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector; 
};

stringstream Module_Bend::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}

