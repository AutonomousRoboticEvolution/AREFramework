#include "Module_APlantBase.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>


using namespace std; 

Module_APlantBase::Module_APlantBase()
{

}


Module_APlantBase::~Module_APlantBase()
{
	//cout << "REMOVING MODULE!" << endl; 
	removeModule();
}

int Module_APlantBase::init() {
	return -1;
}

int Module_APlantBase::mutate(float mutationRate) {
	// nothing to mutate
	return -1;
}

int Module_APlantBase::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	type = 8;
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
//	int fs2 = simCreateForceSensor(0, fsParams, fsFParams, NULL);
//	int fs3 = simCreateForceSensor(0, fsParams, fsFParams, NULL);
//	int fs4 = simCreateForceSensor(0, fsParams, fsFParams, NULL);
	int d1 = simCreateDummy(0.005, NULL);
	int d2 = simCreateDummy(0.005, NULL);
	int d3 = simCreateDummy(0.005, NULL);
//	int d1 = simCreateDummy(0.005, NULL);

//	float fsR[3];
//	fsR[0] = configuration[3];
//	fsR[1] = configuration[4];
//	fsR[2] = configuration[5];
//
//	float fsPos[3];
//	fsPos[0] = configuration[0];
//	fsPos[1] = configuration[1];
//	fsPos[2] = configuration[2];

	float objectOrigin[3]; 
	float zeroOrigin[3] = { 0,0,0 };

//	simSetObjectParent(fs, parentHandle, false);
//	simSetObjectOrientation(fs, parentHandle, fsR);
//	simSetObjectPosition(fs, fs, fsPos);

//	cout << "Creating Cube Module" << endl;
	float size[3] = { 0.13, 0.07, 0.0275 };
	int cube = simCreatePureShape(0, objectPhysics, size, 0.1, 0);

	objectOrigin[0] = configuration[0];
	objectOrigin[1] = configuration[1];
	objectOrigin[2] = 0;//configuration[2] + (0.02751 / 2);

	float rotationOrigin[3];
	rotationOrigin[0] = configuration[3];
	rotationOrigin[1] = configuration[4];
	rotationOrigin[2] = configuration[5];

	simSetObjectPosition(cube, parentHandle, zeroOrigin);
	simSetObjectOrientation(cube, parentHandle, rotationOrigin);
	simSetObjectPosition(cube, cube, objectOrigin);
//	simSetObjectParent(cube, fs, true);

	float latchSize[3] = { 0.05,0.05,0.005 };
	int attach1 = simCreatePureShape(0, objectPhysics, latchSize, 0.1, 0);
	float attach1Rot[3] = {0, -M_PI * 0.25,0};
	float attach1Pos[3] = { -0.045,0,0.035 };
	simSetObjectPosition(attach1, cube, attach1Pos);
	simSetObjectOrientation(attach1, cube, attach1Rot);
	simSetObjectPosition(d1, cube, attach1Pos);
	float higherPos[3] = { 0,0,0.00001 };
	simSetObjectOrientation(d1, cube, attach1Rot);
	simSetObjectPosition(d1, d1, higherPos);
	//simSetObjectParent(attach1, cube, true);

	int attach2 = simCreatePureShape(0, objectPhysics, latchSize, 0.1, 0);
	float attach2Rot[3] = { 0, 0,0 };
	float attach2Pos[3] = { 0.0,0,0.055 };
	simSetObjectPosition(attach2, cube, attach2Pos);
	simSetObjectOrientation(attach2, cube, attach2Rot);
	simSetObjectPosition(d2, cube, attach2Pos);
	simSetObjectOrientation(d2, cube, attach2Rot);
	simSetObjectPosition(d2, d2, higherPos);
	simSetObjectParent(attach2, cube, true);

	int attach3 = simCreatePureShape(0, objectPhysics, latchSize, 0.1, 0);
	float attach3Rot[3] = { 0, M_PI * 0.25,0 };
	float attach3Pos[3] = { 0.045,0,0.035 };
	simSetObjectPosition(attach3, cube, attach3Pos);
	simSetObjectOrientation(attach3, cube, attach3Rot);
	simSetObjectPosition(d3, cube, attach3Pos);
	simSetObjectOrientation(d3, cube, attach3Rot);
	simSetObjectPosition(d3, d3, higherPos);
	simSetObjectParent(attach3, cube, true);

	int shapHands[4] = { cube, attach1, attach2, attach3 };
	int groupedShaps = simGroupShapes(shapHands, 4);

	simSetObjectParent(d1, groupedShaps, true);
	simSetObjectParent(d2, groupedShaps, true);
	simSetObjectParent(d3, groupedShaps, true);


//	simSetObjectParent(fs2, cube, true);
//	simSetObjectParent(fs3, cube, true);
//	simSetObjectParent(fs4, cube, true);

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

	site1.resize(4);
	for (int i = 0; i < 4; i++) {
		site1[i]->x = 0.0;
		site1[i]->y = 0.0;
		site1[i]->z = 0.0026;
		site1[i]->rX = 0.0 * M_PI;
		site1[i]->rY = 0.0 * M_PI;
		site1[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site1[i]->parentHandle = groupedShaps;
		site1[i]->relativePosHandle = d1;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(0);
	for (int i = 0; i < 4; i++) {
		site2[i]->x = 0.0;
		site2[i]->y = 0.0;
		site2[i]->z = 0.0026;
		site2[i]->rX = 0.0 * M_PI;
		site2[i]->rY = 0.0 * M_PI;
		site2[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site2[i]->parentHandle = groupedShaps;
		site2[i]->relativePosHandle = d2;
	}
	siteConfigurations.push_back(site2);
	sites.push_back(1);
	for (int i = 0; i < 4; i++) {
		site3[i]->x = 0.0;
		site3[i]->y = 0.0;
		site3[i]->z = 0.0026;
		site3[i]->rX = 0.0 * M_PI;
		site3[i]->rY = 0.0 * M_PI;
		site3[i]->rZ = (0.0 + (0.5 *i)) * M_PI;
		site3[i]->parentHandle = groupedShaps;
		site3[i]->relativePosHandle = d3;
	}
	siteConfigurations.push_back(site3);
	sites.push_back(2);
	
	freeSites = sites; 
//	objectHandles.push_back(fs); 
	objectHandles.push_back(groupedShaps);
//	objectHandles.push_back(attach1);
//	objectHandles.push_back(attach2);
//	objectHandles.push_back(attach3);
	attachHandles.push_back(cube);
	objectHandles.push_back(d1);
	objectHandles.push_back(d2);
	objectHandles.push_back(d3);
	if (simCheckCollision(cube, sim_handle_all) == true) {
		cout << "cube collided" << endl; 
	//	return-1;
	}
//	if (parentHandle == -1) {
//		simRemoveObject(fs);
//	}
	for (int j = 0; j < objectHandles.size(); j++) {
		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
	}

	return cube;
}

void Module_APlantBase::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<int> Module_APlantBase::getFreeSites(vector<int> targetSites) {
//	cout << "Getting Base Free Sites" << endl;
	vector<int> tempFreeSites; 
	for (int i = 0; i < targetSites.size(); i++) {
		for (int j = i; j < freeSites.size(); j++) {
//			cout << "targetSite " << targetSites[i] << " =? " << freeSites[j] << endl;
			if (targetSites[i] == freeSites[j] && targetSites[i] != -1) {
				tempFreeSites.push_back(targetSites[i]);
			}
		}
	}
	return tempFreeSites; 
}

void Module_APlantBase::setModuleColor() {

}

vector<int> Module_APlantBase::getObjectHandles() {
	return objectHandles; 
}
shared_ptr<ER_Module> Module_APlantBase::clone() {
	return shared_ptr<ER_Module>(new Module_APlantBase(*this));
}

vector<float> Module_APlantBase::updateModule(vector<float> input) {
	ER_Module::updateModule(input);
	return input;
}

stringstream Module_APlantBase::getModuleParams() {
	// needs to save variables of the module
	stringstream ss;
	ss << "#ModuleType:," << 8 << endl;  // make sure this is the same number as in the module factory
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

void Module_APlantBase::setModuleParams(vector<string> values) {

}

vector<float> Module_APlantBase::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_APlantBase::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}