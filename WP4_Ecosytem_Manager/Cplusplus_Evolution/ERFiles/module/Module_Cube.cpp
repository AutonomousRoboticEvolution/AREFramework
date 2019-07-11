#include "Module_Cube.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std; 

Module_Cube::Module_Cube()
{

}


Module_Cube::~Module_Cube()
{
	//cout << "REMOVING MODULE!" << endl; 
	removeModule();
}

int Module_Cube::init() {
	return -1;
}

int Module_Cube::mutate(float mutationRate) {
	
	return -1;
}

int Module_Cube::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	/*
		create order:
		parent(if parent != -1)---ForceSensor---CUBE
	*/

	// Force sensor parameters start >>>>>>>>>>>>>>>>>>>>>>>
	int fsParams[5];
	fsParams[0] = 1;
	fsParams[1] = 5;
	fsParams[2] = 5;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.01;
	fsFParams[1] = settings->maxForce_ForceSensor;
	fsFParams[2] = settings->maxTorque_ForceSensor;
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(0, fsParams, fsFParams, NULL);

	// force sensor orientation
	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];

	float fsPos[3];
	fsPos[0] = configuration[0];
	fsPos[1] = configuration[1];
	fsPos[2] = configuration[2];

	// force sensor parameters end <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	float objectOrigin[3]; 
	float zeroOrigin[3] = { 0,0,0 };

	float relPos[3];
	simGetObjectPosition(relativePosHandle, NULL, relPos);

	// connect the previously created force sensor to the parent object
	simSetObjectParent(fs, parentHandle, false);
	simSetObjectOrientation(fs, relativePosHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);

    if (settings->verbose) {
        cout << "Creating Cube Module" << endl;
    }
	float size[3] = { 0.055, 0.055, 0.055 };
	int cube = simCreatePureShape(0, objectPhysics, size, 0.3, 0);
    //configuration: position and orientation of the relative position handle (e.g. dummy)
	objectOrigin[0] = configuration[0];
	objectOrigin[1] = configuration[1];
	objectOrigin[2] = configuration[2] + (size[2]/2) + 0.00001; // equals 0.055/5

	float rotationOrigin[3];
	rotationOrigin[0] = configuration[3];
	rotationOrigin[1] = configuration[4];
	rotationOrigin[2] = configuration[5];

	// Cube will be set exactly to the position of the dummy
	simSetObjectPosition(cube, relativePosHandle, zeroOrigin);
	// Change the orientation of the cube so the z directional
	// vector of the cube and the dummy are the same
	simSetObjectOrientation(cube, relativePosHandle, rotationOrigin);
	// Just move up the cube based on the objectOrigin which moves the
	// object 0.055/2 meter up
	simSetObjectPosition(cube, cube, objectOrigin);
	simSetObjectParent(cube, fs, true);

	//	define CUBE's children's position, orientation and parent
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
		site1[i]->z = (size[2] / 2) + 0.00001;
		site1[i]->rX = 0.0 * M_PI;
		site1[i]->rY = 0.0 * M_PI;
		site1[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site1[i]->parentHandle = cube;
		site1[i]->relativePosHandle = cube;
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
		site2[i]->parentHandle = cube;
		site2[i]->relativePosHandle = cube;
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
		site3[i]->parentHandle = cube;
		site3[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site3);
	sites.push_back(2);
	for (int i = 0; i < 4; i++) {
		site4[i]->x = 0.0;
		site4[i]->y = 0.0;
		site4[i]->z = 0.02751;
		site4[i]->rX = -0.5 * M_PI;
		site4[i]->rY = 0.5 * M_PI;
		site4[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site4[i]->parentHandle = cube;
		site4[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site4);
	sites.push_back(3);
	for (int i = 0; i < 4; i++) {
		site5[i]->x = 0.0;
		site5[i]->y = 0.0;
		site5[i]->z = 0.02751;
		site5[i]->rX = 0.5 * M_PI;
		site5[i]->rY = -0.5 * M_PI;
		site5[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site5[i]->parentHandle = cube;
		site5[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site5);
	sites.push_back(4);
	freeSites = sites; 
	objectHandles.push_back(fs); 
	objectHandles.push_back(cube);
	attachHandles.push_back(cube);
	if (simCheckCollision(cube, sim_handle_all) == true) {
	    if (settings->verbose){
	        cout << "cube collided" << endl;
	    }
	}
	if (parentHandle == -1) {
		simRemoveObject(fs); //dont't need force sensor when parentHandle = -1
	}
	for (int j = 0; j < objectHandles.size(); j++) {
		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
	}
	return cube;
}

void Module_Cube::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<int> Module_Cube::getFreeSites(vector<int> targetSites) {
	vector<int> tempFreeSites; 
	for (int i = 0; i < targetSites.size(); i++) {
		for (int j = 0; j < freeSites.size(); j++) {
			if (targetSites[i] == freeSites[j] && targetSites[i] != -1) {
				tempFreeSites.push_back(targetSites[i]);
			}
		}
	}
	return tempFreeSites; 
}

void Module_Cube::setModuleColor() {

}

vector<int> Module_Cube::getObjectHandles() {
	return objectHandles; 
}
shared_ptr<ER_Module> Module_Cube::clone() {
	return shared_ptr<ER_Module>(new Module_Cube(*this));
}

vector<float> Module_Cube::updateModule(vector<float> input) {
    // input.append(sensor->getInput());
    vector<float> output = ER_Module::updateModule(input);
	return output;
}


stringstream Module_Cube::getModuleParams() {
	// needs to save variables of the module
	stringstream ss;
	ss << "#ModuleType:," << 1 << endl;  // make sure this is the same number as in the module factory
	//ss << "#siteParameters:,";
	//for (int i = 0; i < siteConfigurations.size(); i++) {
	//	ss << "/t,#site:," << 1 << "," << endl;
	//	for (int j = 0; j < siteConfigurations[i].size(); j++) {
	//		ss << siteConfigurations[i][j].x << ",";
	//		ss << siteConfigurations[i][j].y << ",";
	//		ss << siteConfigurations[i][j].z << ",";
	//		ss << siteConfigurations[i][j].rX << ",";
	//		ss << siteConfigurations[i][j].rY << ",";
	//		ss << siteConfigurations[i][j].rZ << ",";
	//	}
	//}
	ss << endl;
	return ss;

}

void Module_Cube::setModuleParams(vector<string> values) {

}

vector<float> Module_Cube::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_Cube::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}