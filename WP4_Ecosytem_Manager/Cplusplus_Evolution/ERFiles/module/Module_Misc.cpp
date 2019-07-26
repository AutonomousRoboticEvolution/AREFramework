#include "Module_Misc.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Module_Misc::Module_Misc()
{
	
}

Module_Misc::~Module_Misc()
{
//	siteConfigurations.clear();
//	cout << "REMOVING RODRIGO's MODULE!" << endl;
	removeModule();
}

int Module_Misc::init() {
	return -1;
}

int Module_Misc::mutate(float mutationRate) {
//	cout << "About to mutate" << endl;
	control->mutate(mutationRate);
	return 1;
}

int Module_Misc::createModuleBackup(vector<float> configuration, int relativePosHandle, int parentHandle) {
	int fsParams[5];
	fsParams[0] = 0;
	fsParams[1] = 1;
	fsParams[2] = 10; // settings->consecutiveThresholdViolations;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.01;
	fsFParams[1] = 80000; //settings->maxForceSensor;
	fsFParams[2] = 10000.7; //settings->maxForceSensor; // change torque
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(3, fsParams, fsFParams, NULL);

	float relPos[3];
	simGetObjectPosition(relativePosHandle,-1,relPos);
	phenV = 0;
	phenV += relPos[0];
	phenV += relPos[1];
	phenV += relPos[2];
	if (phenV > 1000) {
		phenV = 1000;
	}
	else if (phenV < -1000) {
		phenV = -1000;
	}
	int d1 = simCreateDummy(0.001, NULL);
	int d2 = simCreateDummy(0.001, NULL);
	int d3 = simCreateDummy(0.001, NULL);
	//	int d4 = simCreateDummy(0.001, NULL);
	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];
	float zeroOrigin[3] = { 0,0,0 };

	float fsPos[3];
	fsPos[0] = 0;
	fsPos[1] = 0;
	fsPos[2] = configuration[2] + 0.00005;
	
	float zeroPos[3];
	zeroPos[0] = 0.0;
	zeroPos[1] = 0.0;
	zeroPos[2] = 0.0;

	//simSetObjectOrientation(fs, relativePosHandle, fsR);
	//	simSetObjectPosition(fs, relativePosHandle, fsPos);
	// simSetObjectParent(fs, parentHandle, true);

	simSetObjectPosition(fs, relativePosHandle, zeroPos);
	simSetObjectOrientation(fs, relativePosHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);
	simSetObjectParent(fs, parentHandle, true);

	float objectOrigin[3];
	float size[3] = { 0.055,0.055,0.003 };

	objectOrigin[0] = configuration[0];
	objectOrigin[1] = configuration[1];
	objectOrigin[2] = configuration[2] + (size[2]*0.5) + 0.0001;

	float rotationOrigin[3];
	rotationOrigin[0] = configuration[3];
	rotationOrigin[1] = configuration[4];
	rotationOrigin[2] = configuration[5];

//	float size[3] = { 0.055,0.055,0.003 };
	float mass = 0.0275;
	int baseHandle = simCreatePureShape(0, objectPhysics, size, mass, 0);
	int part2 = simCreatePureShape(0, objectPhysics, size, mass, 0);
	int part3 = simCreatePureShape(0, objectPhysics, size, mass, 0);
	int part4 = simCreatePureShape(0, objectPhysics, size, mass, 0);

	float appearanceSize[3] = { 0.032, 0.036, 0.058 };
	int appearancePeace = simCreatePureShape(0, objectPhysics, appearanceSize, 0.055, 0);
	//	cout << "baseHandle and appearancePeace: " << baseHandle << ", " << appearancePeace << endl; 
	float jointSize[3] = { 0.048,0.018, 0 };
	float appearancePos[3] = { 0.0, 0.00, 0.0305 };
	int joint = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	float part2Pos[3] = { 0,0,0.074001 };
	float part3Pos[3] = { 0,0.029001,0.045 };
	float part4Pos[3] = { 0,-0.029001,0.045 };

	simSetShapeMaterial(baseHandle, 2290011); // default material
	simSetShapeMaterial(part2, 2290011);
	simSetShapeMaterial(part3, 2290011);
	simSetShapeMaterial(part4, 2290011);
	simSetShapeMaterial(appearancePeace, 2290011);

	simSetObjectPosition(baseHandle, relativePosHandle, zeroOrigin);
	simSetObjectOrientation(baseHandle, relativePosHandle, rotationOrigin);
	simSetObjectPosition(baseHandle, baseHandle, objectOrigin);
	simSetObjectPosition(appearancePeace, baseHandle, appearancePos);
	//	simSetObjectPosition(innerfs, baseHandle, appearancePos);
	simSetObjectPosition(part2, baseHandle, part2Pos);
	simSetObjectPosition(part3, baseHandle, part3Pos);
	simSetObjectPosition(part4, baseHandle, part4Pos);
	simSetObjectPosition(d1, baseHandle, part2Pos);
	simSetObjectPosition(d2, baseHandle, part3Pos);
	simSetObjectPosition(d3, baseHandle, part4Pos);

	float part2Or[3] = { 0,0,0 };
	float part3Or[3] = { -0.5 * M_PI,0.0,0 };
	float part4Or[3] = { 0.5 * M_PI,-0.0,0 };

	simSetObjectOrientation(appearancePeace, baseHandle, zeroOrigin);
	//	simSetObjectOrientation(innerfs, baseHandle, zeroOrigin);
	simSetObjectOrientation(part2, baseHandle, part2Or);
	simSetObjectOrientation(part3, baseHandle, part3Or);
	simSetObjectOrientation(part4, baseHandle, part4Or);
	simSetObjectOrientation(d1, baseHandle, part2Or);
	simSetObjectOrientation(d2, baseHandle, part3Or);
	simSetObjectOrientation(d3, baseHandle, part4Or);
	float pos[3] = { 0.0,0.0,size[2]*0.5f };
	simSetObjectPosition(d1, d1, pos);
	simSetObjectPosition(d2, d2, pos);
	simSetObjectPosition(d3, d3, pos);

	float jointPos[3] = { 0,0,0.045 };
	float jointOr[3] = { 0.5*M_PI,0,0 };

	simSetObjectPosition(joint, baseHandle, jointPos);
	simSetObjectOrientation(joint, baseHandle, jointOr);
	//	float tpos[3];
	//	simGetObjectPosition(part2, baseHandle, tpos);
	//
	int shapHands[3] = { part4, part3, part2 };
	int groupedShaps = simGroupShapes(shapHands, 3);

	int baseshapHands[2] = { baseHandle, appearancePeace };
	int groupedBase = simGroupShapes(baseshapHands, 2);

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
	for (int i = 0; i < 4; i++) {
		site1[i]->x = 0.0;
		site1[i]->y = 0.0;
		site1[i]->z = 0.0;
		site1[i]->rX = 0.0 * M_PI;
		site1[i]->rY = 0.0 * M_PI;
		site1[i]->rZ = (0.0 + (0.5*i)) * M_PI;
		site1[i]->parentHandle = groupedShaps;
		site1[i]->relativePosHandle = d1;
	}
	siteConfigurations.push_back(site1);
	sites.push_back(0);
	for (int i = 0; i < 4; i++) {
		site2[i]->x = 0.0;
		site2[i]->y = 0.0;
		site2[i]->z = 0.00;
		site2[i]->rX = 0.0 * M_PI;
		site2[i]->rY = 0.0 * M_PI;
		site2[i]->rZ = (0.0 + (0.5*i)) * M_PI;
		site2[i]->parentHandle = groupedShaps;
		site2[i]->relativePosHandle = d2;
	}
	siteConfigurations.push_back(site2);
	sites.push_back(1);
	for (int i = 0; i < 4; i++) {
		site3[i]->x = 0.0;
		site3[i]->y = 0.0;
		site3[i]->z = 0.0;
		site3[i]->rX = 0.0 * M_PI;
		site3[i]->rY = 0.0 * M_PI;
		site3[i]->rZ = (0.0 + (0.5*i)) * M_PI;
		site3[i]->parentHandle = groupedShaps;
		site3[i]->relativePosHandle = d3;
	}
	siteConfigurations.push_back(site3);
	sites.push_back(2);

	freeSites = sites;
	site1.clear();
	objectHandles.push_back(fs);
	objectHandles.push_back(groupedBase);
	objectHandles.push_back(groupedShaps);
	objectHandles.push_back(d1);
	objectHandles.push_back(d2);
	objectHandles.push_back(d3);
	objectHandles.push_back(joint);

	controlHandles.push_back(joint);

	simSetObjectParent(groupedBase, fs, true);
	simSetObjectParent(d1, groupedShaps, true);
	simSetObjectParent(d2, groupedShaps, true);
	simSetObjectParent(d3, groupedShaps, true);
	simSetObjectParent(joint, groupedBase, true);
	simSetObjectParent(groupedShaps, joint, true);

	simSetObjectInt32Parameter(joint, 2000, 1);
	simSetObjectInt32Parameter(joint, 2001, 1);
	simSetObjectFloatParameter(joint, 2002, 0.1);
	simSetObjectFloatParameter(joint, 2003, 0.01);
	simSetObjectFloatParameter(joint, 2004, 0.0);
	simSetJointTargetPosition(joint, 0.0);
	//	cout << "maxForce = " << settings->maxForce << endl;
	simSetJointForce(controlHandles[0], settings->maxForce);
	simSetJointTargetVelocity(controlHandles[0], 0.05);

	for (int j = 0; j < objectHandles.size(); j++) {
		simSetObjectSpecialProperty(objectHandles[j], sim_objectspecialproperty_detectable_all);
	}

	// ------------------------
	//	simSetJointTargetPosition(joint, 0);
	return baseHandle;

}


int Module_Misc::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	int miscHandle = simLoadModel(("models/" + filename).c_str());
	if (miscHandle < 0) {
		std::cerr << "Error models/"+filename << std::endl;
	}
	int shapeAmount[1];

	if (settings->verbose) {
		std::cout << "creating " + filename << std::endl;
	}

	// this possibly causes a leak, advice to use simAddObjectsToSelection with sim_handle_tree
	// then get simGetObjectSelection to get the object types. 
	/**/
	vector<int> shapes;
	vector<int> joints;
	vector<int> dummies;
	vector<int> sensors;
	// select all objects in tree
	simAddObjectToSelection(sim_handle_tree, miscHandle);
	int selectionSize = simGetObjectSelectionSize();
	// store all these objects (max 20 shapes)
	int shapesStorage[50]; // stores up to 20 shapes
	simGetObjectSelection(shapesStorage);
	for (int i = 0; i < selectionSize; i++) {
		int objectType = simGetObjectType(shapesStorage[i]);
		if (objectType == sim_object_shape_type) {
			shapes.push_back(shapesStorage[i]);
		}
		else if (objectType == sim_object_joint_type) {
			joints.push_back(shapesStorage[i]);
		}
		else if (objectType == sim_object_dummy_type) {
			// attachment location
			dummies.push_back(shapesStorage[i]);
		}
		else if (objectType == sim_object_proximitysensor_type) {
			// sensor
			sensors.push_back(shapesStorage[i]);
		}
	}
	// deselect
	simRemoveObjectFromSelection(sim_handle_all, miscHandle);

	// get the difference between the parent attachment configurations and the main handle of the object?


	if (settings->verbose) {
		cout << "creating force sensor" << endl;
	}
	int fsParams[5];
	fsParams[0] = 0;
	fsParams[1] = 1;
	fsParams[2] = 20; // settings->consecutiveThresholdViolations;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.005;
	fsFParams[1] = 800; //settings->maxForceSensor;
	fsFParams[2] = 1000;// 1.7; //settings->maxForceSensor; // change torque
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(3, fsParams, fsFParams, NULL);

	// force sensor rotation. (should be orientation)
	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];
	float zeroOrigin[3] = { 0,0,0 };
	// force sensor position (+0.0001 away from z direction of previous)
	float fsPos[3];
	fsPos[0] = 0.0;
	fsPos[1] = 0.0;
	fsPos[2] = configuration[2] + 0.0001;
	float zeroPos[3]; 
	zeroPos[0] = 0.0;
	zeroPos[1] = 0.0;
	zeroPos[2] = 0.0;

	simSetObjectPosition(fs, relativePosHandle, zeroPos);
	simSetObjectOrientation(fs, relativePosHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);
	simSetObjectParent(fs, parentHandle, true);

	simSetObjectPosition(miscHandle, fs, zeroPos);
	simSetObjectOrientation(miscHandle, relativePosHandle, fsR);
	
	simRemoveObject(miscHandle);
	dummies.erase(dummies.begin());
	miscHandle = shapes[0];
	simSetObjectParent(miscHandle, fs, true);
	
	// get dummy position and orientation
//	float dummyPos[3];
//	simGetObjectPosition(dummies[0], miscHandle, dummyPos); // should become 0,0,0
//	float dummyOr[3];
//	simGetObjectOrientation(dummies[0], miscHandle, dummyOr); // should become 0,0,0
//	
//	float miscPos[3];
//	simGetObjectPosition(miscHandle, fs, miscPos);
//	float miscOr[3];
//	simGetObjectOrientation(miscHandle, fs, miscOr);
//	
//	float difPos[3];
//	float difOr[3];
//	for (int i = 0; i < 3; i++) {
//		difPos[i] = miscPos[i] + dummyPos[i];
////		difOr[i] = dummyOr[i] - miscOr[i];
//	}
////	simSetObjectOrientation(miscHandle, relativePosHandle, fsR);
//	simSetObjectPosition(miscHandle, fs, difPos);
//	simSetObjectParent(miscHandle, fs, true);
//	// transform position and orientation of components

//	simSetObjectParent(miscHandle, dummies[0], false);

//	simSetObjectPosition(dummies[0], relativePosHandle, zeroPos);
//	simSetObjectOrientation(dummies[0], relativePosHandle, fsR);
//	simSetObjectPosition(dummies[0], dummies[0], fsPos);
//	simSetObjectParent(dummies[0], fs, false);

//	simSetObjectParent(dummies[0], miscHandle, false);
//	simSetObjectParent(miscHandle, fs, false);


	float objectOrigin[3];
	simGetObjectPosition(miscHandle, relativePosHandle, objectOrigin);

//	float rotationOrigin[3];
//	simGetObjectOrientation(miscHandle, relativePosHandle, rotationOrigin);

	// set the force handle position relative to dummy 1
//	simSetObjectOrientation(miscHandle, fs, rotationOrigin);
//	simSetObjectPosition(miscHandle, fs, objectOrigin);
//	simSetObjectParent(miscHandle, fs, true);

	if (settings->verbose) {
		cout << "Done creating force sensor" << endl;
	}


	for (int n = 0; n < dummies.size(); n++) {
		// add a connection site for every dummy object in the model
		vector<shared_ptr<SITE>> site;
		for (int i = 0; i < 4; i++) {
			site.push_back(shared_ptr<SITE>(new SITE));
			//float pos[3];
			//float orien[3];
			//simGetObjectOrientation(dummies[n], shapes[1], orien);
			//simGetObjectPosition(dummies[n], shapes[1], pos);
			site[i]->x = 0.0;
			site[i]->y = 0.0;
			site[i]->z = 0.0;
			site[i]->rX = 0;
			site[i]->rY = 0;
			site[i]->rZ = (0.0 + (0.5*i)) * M_PI;
			site[i]->parentHandle = shapes[1];
			site[i]->relativePosHandle = dummies[n];
		}
		siteConfigurations.push_back(site);
		sites.push_back(n);
	}
	
	freeSites = sites;
	int objectAmount[1]; // not used??
	for (int i = 0; i < shapes.size(); i++) {
		objectHandles.push_back(shapes[i]);
	}
	for (int i = 0; i < dummies.size(); i++) {
	//	simRemoveObject(dummies[i]);
	//	dummies.clear();
		objectHandles.push_back(dummies[i]);
	}
	
	objectHandles.push_back(fs);
	for (int i = 0; i < joints.size(); i++) {
		controlHandles.push_back(joints[i]);
		objectHandles.push_back(joints[i]);
	}
	for (int i = 0; i < sensors.size(); i++) {
		inputHandles.push_back(sensors[i]);
		objectHandles.push_back(sensors[i]);
	}
//	controlHandles.push_back(joint);
	if (settings->verbose) {
		cout << "Done creating misc module" << endl;
	}
	return miscHandle;
}

vector<int> Module_Misc::getFreeSites(vector<int> targetSites) {
	vector<int> tempFreeSites;
	for (int i = 0; i < targetSites.size(); i++) {
		for (int j = 0; j < freeSites.size(); j++) {
			if (targetSites[i] == freeSites[j]) {
				tempFreeSites.push_back(targetSites[i]);
			}
		}
	}
	return tempFreeSites;
}
vector<int> Module_Misc::getObjectHandles() {
	return objectHandles;
}

void Module_Misc::setModuleColor() {

}

shared_ptr<ER_Module> Module_Misc::clone() {
	shared_ptr<Module_Misc> clonedModule = make_unique<Module_Misc>(*this);
//	clonedModule->control = control->clone();
	return clonedModule;
}

void Module_Misc::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_Misc::updateModule(vector<float> input) {
//	controlModule();
	vector<float> output = ER_Module::updateModule(input);
//	energy = energy - energyDissipationRate;
	float pos[3];
	if (controlHandles.size() > 0) {
		if (previousPosition == -1) {
			simGetJointPosition(controlHandles[0], pos);
			previousPosition = pos[0];
		}
		else {
			simGetJointPosition(controlHandles[0], pos);
			float currentPos = pos[0];
			float distanceDifference = (sqrt((currentPos*currentPos) - (previousPosition*previousPosition)));
			//	cout << "distanceDifference " << distanceDifference << endl;
			float energyLost = 0;
			if (distanceDifference > 0.01) {
				energyLost = (settings->energyDissipationRate * distanceDifference);
			}

			//		cout << "pos: " << currentPos << ", " << previousPosition << endl;
			if (energyLost < 0) {
				energyLost = 0;
			}
			if (energyLost > 1000) {
				energyLost = 1000;
			}
			//		cout << "energy dissipation rate = " << settings->energyDissipationRate << endl;
			//		cout << "energy lost = " << energyLost << endl;
			energy = energy - energyLost;
			//		cout << "energy = " << energy << endl;
			simGetJointPosition(controlHandles[0], pos);
			previousPosition = pos[0];
		}

		if (parentModulePointer != NULL) {
			if (parentModulePointer->broken == true) {
				this->broken = true;
			}
			else if (broken == false) {
				//			vector<float> sensorValues;
					//		sensorValues.push_back(0);
					//		output = controlModule(sensorValues); // sensor values set to zero, no intrinsic sensors in the servo module
				controlModule(output[0]);
			}
		}
	}
	
	return output; // 
}

stringstream Module_Misc::getModuleParams() {
	// needs to save variables of the module
	stringstream ss;
	ss << "#ModuleType:," << 4 << endl;  // make sure this is the same number as in the module factory
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
	}
	ss << endl;*/
	return ss;
}

void Module_Misc::setModuleParams(vector<string> values) {
	ER_Module::setControlParams(values);
	/*for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		vector<string> controlParams;
		if (tmp == "#ControlParams:") {
			it++;
			controlParams.push_back(tmp);
			bendAngle = atof(tmp.c_str());
		}
		if (tmp == "#EndControl") {
			break;
		}
	}*/
}


void Module_Misc::createControl() {
	//	cout << "creating Rodrigo genome" << endl; 
	// Do not use!
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	//control = controlFactory->createNewControlGenome(settings->controlType, randomNum, settings); // 0 is ANN
	//control = controlFactory->createNewControlGenome(1, randomNum, settings); // 0 is ANN
	control = controlFactory->createNewControlGenome(settings->controlType, randomNum, settings);
	control->init(1, 1, 1); // bias, 6 input, 2 inter, 2 output
	controlFactory.reset();
}

void Module_Misc::controlModule(float input) {
	//	control->update(sensorValues);
//	vector<float> output = control->update(sensorValues);
//	if (output.size() != 1) {
	//	cout << "ERROR:: output size is not 1, but should be" << endl;
//	}
//	cout << "output[0] = " << input << endl;
	simSetJointTargetPosition(controlHandles[0], 0.5 * input * M_PI);
//	return output;
}

vector<float> Module_Misc::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_Misc::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << settings->controlType << endl; // CUSTOM_ANN
	ss << control->getControlParams().str();
	ss << "#EndControl," << endl;
	ss << endl;
	return ss;
}