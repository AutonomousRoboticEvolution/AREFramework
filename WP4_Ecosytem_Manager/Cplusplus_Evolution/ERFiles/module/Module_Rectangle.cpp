#include "Module_Rectangle.h"
#include <iostream>
#include <cmath>

using namespace std; 

Module_Rectangle::Module_Rectangle()
{

}


Module_Rectangle::~Module_Rectangle()
{
	//cout << "REMOVING MODULE!" << endl; 
	removeModule();
}

int Module_Rectangle::init() {
	return -1;
}

int Module_Rectangle::mutate(float mutationRate) {
	
	return -1;
}

int Module_Rectangle::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {

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

	float objectOrigin[3]; 
	float zeroOrigin[3] = { 0,0,0 };


	simSetObjectParent(fs, parentHandle, false);
	simSetObjectOrientation(fs, relativePosHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);

//	cout << "Creating Cube Module" << endl;
	float size[3] = { 0.055, 0.055, (0.055 * 4)};
	int cube = simCreatePureShape(0, objectPhysics, size, (0.1 * 4), false);

	objectOrigin[0] = configuration[0];
	objectOrigin[1] = configuration[1];
	objectOrigin[2] = configuration[2] + (0.5 * size[2]);


	float rotationOrigin[3];
	rotationOrigin[0] = configuration[3];
	rotationOrigin[1] = configuration[4];
	rotationOrigin[2] = configuration[5];

	simSetObjectPosition(cube, relativePosHandle, zeroOrigin);
	simSetObjectOrientation(cube, relativePosHandle, rotationOrigin);
	simSetObjectPosition(cube, cube, objectOrigin);
	simSetObjectParent(cube, fs, true);

//	simSetObjectPosition(cube, parentHandle, objectOrigin);
//	simSetObjectOrientation(cube, parentHandle, orientation);
	
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
	site1.resize(4);
	for (int i = 0; i < 4; i++) {
		site1[i]->x = 0.0;
		site1[i]->y = 0.0;
		site1[i]->z = 0.50001 * size[2];
		site1[i]->rX = 0.0 * M_PI;
		site1[i]->rY = 0.0 * M_PI;
		site1[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site1[i]->parentHandle = cube;
		site1[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(0);
	for (int i = 0; i < 4; i++) {
		if (i == 0) {
			site2[i]->x = (0.0 * size[2]);
			site2[i]->y = (0.5 * size[2]) - (0.5 * size[0]);
		}
		if (i == 1) {
			site2[i]->x = (0.5 * size[2]) - (0.5 * size[0]);
			site2[i]->y = (0.0 * size[2]);
		}
		if (i == 2) {
			site2[i]->x = (0.0 * size[2]);
			site2[i]->y = -((0.5 * size[2]) - (0.5 * size[0]));
		}
		if (i == 3) {
			site2[i]->x = -((0.5 * size[2]) - (0.5 * size[0]));
			site2[i]->y = (0.0 * size[2]);
		}

		site2[i]->z = 0.50001 * size[0];
		site2[i]->rX = 0.5 * M_PI;
		site2[i]->rY = 0.0 * M_PI;
		site2[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site2[i]->parentHandle = cube;
		site2[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site2);
	sites.push_back(1);
	for (int i = 0; i < 4; i++) {
		if (i == 0) {
			site3[i]->x = (0.0 * size[2]);
			site3[i]->y = -((0.5 * size[2]) - (0.5 * size[0]));
		}
		if (i == 1) {
			site3[i]->x = -((0.5 * size[2]) - (0.5 * size[0]));
			site3[i]->y = (0.0 * size[2]);
		}
		if (i == 2) {
			site3[i]->x = (0.0 * size[2]);
			site3[i]->y = ((0.5 * size[2]) - (0.5 * size[0]));
		}
		if (i == 3) {
			site3[i]->x = ((0.5 * size[2]) - (0.5 * size[0]));
			site3[i]->y = (0.0 * size[2]);
		}
		site3[i]->z = 0.50001 * size[0];
		site3[i]->rX = -0.5 * M_PI;
		site3[i]->rY = 0.0 * M_PI;
		site3[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site3[i]->parentHandle = cube;
		site3[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site3);
	sites.push_back(2);
	for (int i = 0; i < 4; i++) {
		if (i == 0) {
			site4[i]->x = (0.0 * size[2]);
			site4[i]->y = -((0.5 * size[2]) - (0.5 * size[0]));
		}
		if (i == 1) {
			site4[i]->x = -((0.5 * size[2]) - (0.5 * size[0]));
			site4[i]->y = (0.0 * size[2]);
		}
		if (i == 2) {
			site4[i]->x = (0.0 * size[2]);
			site4[i]->y = ((0.5 * size[2]) - (0.5 * size[0]));
		}
		if (i == 3) {
			site4[i]->x = ((0.5 * size[2]) - (0.5 * size[0]));
			site4[i]->y = 0.0;
		}
		site4[i]->z = 0.501 * size[0];
		site4[i]->rX = -0.5 * M_PI;
		site4[i]->rY = 0.5 * M_PI;
		site4[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site4[i]->parentHandle = cube;
		site4[i]->relativePosHandle = cube;
	}
	siteConfigurations.push_back(site4);
	sites.push_back(3);
	for (int i = 0; i < 4; i++) {
		if (i == 0) {
			site5[i]->x = (0.0 * size[2]);
			site5[i]->y = (0.5 * size[2]) - (0.5 * size[0]);
		}
		if (i == 1) {
			site5[i]->x = (0.5 * size[2]) - (0.5 * size[0]);
			site5[i]->y = (0.0 * size[2]);
		}
		if (i == 2) {
			site5[i]->x = (0.0 * size[2]);
			site5[i]->y = -((0.5 * size[2]) - (0.5 * size[0]));
		}
		if (i == 3) {
			site5[i]->x = -((0.5 * size[2]) - (0.5 * size[0]));
			site5[i]->y = (0.0 * size[2]);
		}
//		site1[i].y = 0.0;
		site5[i]->z = 0.501 * size[0];
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
	//	cout << "cube collided" << endl; 
	//	return-1;
	}
	if (parentHandle == -1) {
		simRemoveObject(fs);
	}
	for (int j = 0; j < objectHandles.size(); j++) {
		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
	}

	return cube;
}

void Module_Rectangle::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<int> Module_Rectangle::getFreeSites(vector<int> targetSites) {
	vector<int> tempFreeSites; 
	for (int i = 0; i < targetSites.size(); i++) {
		for (int j = i; j < freeSites.size(); j++) {
			if (targetSites[i] == freeSites[j] && targetSites[i] != -1) {
				tempFreeSites.push_back(targetSites[i]);
			}
		}
	}
	return tempFreeSites; 
}

void Module_Rectangle::setModuleColor() {

}

vector<int> Module_Rectangle::getObjectHandles() {
	return objectHandles; 
}
shared_ptr<ER_Module> Module_Rectangle::clone() {
	return shared_ptr<ER_Module>(new Module_Rectangle(*this));
}

vector<float> Module_Rectangle::updateModule(vector<float> input) {
	ER_Module::updateModule(input);
	return input;
}

stringstream Module_Rectangle::getModuleParams() {
	// needs to save variables of the module
	stringstream ss;
	ss << "#ModuleType:," << 5 << endl;  // make sure this is the same number as in the module factory
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

void Module_Rectangle::setModuleParams(vector<string> values) {

}

vector<float> Module_Rectangle::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_Rectangle::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}