#include "Module_Bone.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
using namespace std;

Module_Bone::Module_Bone()
{
	
}

Module_Bone::~Module_Bone()
{
	removeModule();
}

int Module_Bone::init() {
	return -1;
}

int Module_Bone::mutate(float mutationRate) {
//	cout << "About to mutate" << endl;
	control->mutate(mutationRate);
	return 1;
}

vector<int> Module_Bone::createBone(vector<float> configuration, int relativePosHandle, int parentHandle) {
	if (settings->verbose) {
		cout << "Creating Bone" << endl;
	}
	vector<int> partHandles;
    partHandles = createCube(configuration, relativePosHandle, parentHandle);
	return partHandles;
}

int Module_Bone::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	vector<int> shapes;
	vector<int> joints;
	vector<int> dummies;
	vector<int> sensors;

	// These three vectors need to be populated with the appropriate parts when constructing the skeleton
	vector<int> partHandles = createBone(configuration, relativePosHandle, parentHandle);
    cout << "DEBUG - partHandles.size(): " << partHandles.size() << endl;
	if (partHandles.size() < 1) {
		cout << "ERROR: No parts have been created for the bones." << endl;
		return -1;
	}
	int miscHandle = partHandles[0];
	simAddObjectToSelection(sim_handle_tree, partHandles[0]);
	// store all these objects (max 20 shapes)
	int shapesStorage[100]; // stores up to 20 shapes
	simGetObjectSelection(shapesStorage);
	int selectionSize = simGetObjectSelectionSize();
    cout << "DEBUG - selectionSize: " << selectionSize << endl;
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

	if (shapes.size() < 1) {
		cout << "ERROR: No shapes have been created for the bones." << endl;
		return -1;
	}

	// get the difference between the parent attachment configurations and the main handle of the object?

	if (settings->verbose) {
		cout << "creating force sensor" << endl;
        cout << "shape and dummy handles size = " << shapes.size() << ", " << dummies.size() << endl;
	}
	int fsParams[5];
	fsParams[0] = 0;
	fsParams[1] = 1;
	fsParams[2] = 1000000; // settings->consecutiveThresholdViolations;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.005;
	fsFParams[1] = 10000000;//800; //settings->maxForceSensor;
	fsFParams[2] = 10000000;//1000;// 1.7; //settings->maxForceSensor; // change torque
	fsFParams[3] = 0;
	fsFParams[4] = 0;
	int fs = simCreateForceSensor(3, fsParams, fsFParams, NULL);

	// force sensor rotation. (should be orientation)
	float fsR[3];
	fsR[0] = configuration[3];
	fsR[1] = configuration[4];
	fsR[2] = configuration[5];
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
	
	float objectOrigin[3];
	simGetObjectPosition(miscHandle, relativePosHandle, objectOrigin);

	if (settings->verbose) {
		cout << "Done creating force sensor" << endl;
	}


	for (int n = 0; n < dummies.size(); n++) {
		// add a connection site for every dummy object in the model
		vector<shared_ptr<SITE>> site;
		for (int i = 0; i < 4; i++) {
			site.push_back(shared_ptr<SITE>(new SITE));
			site[i]->x = 0.0;
			site[i]->y = 0.0;
			site[i]->z = 0.0;
			site[i]->rX = 0;
			site[i]->rY = 0;
			site[i]->rZ = (0.0 + (0.5*i)) * M_PI;
			site[i]->parentHandle = shapes[0];
			site[i]->relativePosHandle = dummies[n];
		}
		siteConfigurations.push_back(site);
		sites.push_back(n);
	}
	
	freeSites = sites;
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
	moduleHandle = miscHandle;
	return miscHandle;
}

vector<int> Module_Bone::getFreeSites(vector<int> targetSites) {
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
vector<int> Module_Bone::getObjectHandles() {
	return objectHandles;
}

void Module_Bone::setModuleColor() {

}

shared_ptr<ER_Module> Module_Bone::clone() {
	shared_ptr<Module_Bone> clonedModule = make_unique<Module_Bone>(*this);
//	clonedModule->control = control->clone();
	return clonedModule;
}

void Module_Bone::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_Bone::updateModule(vector<float> input) {
//	controlModule();
	vector<float> output = ER_Module::updateModule(input);
//	energy = energy - energyDissipationRate;
	float pos[3];
	if (controlHandles.size() > 0) {
		if (previousPosition == -1) {
			simGetJointPosition(controlHandles[0], pos);
			previousPosition = pos[0];
		}

		if (parentModulePointer != NULL) {
			if (parentModulePointer->broken) {
				this->broken = true;
			}
			else if (!broken) {
				//			vector<float> sensorValues;
					//		sensorValues.push_back(0);
					//		output = controlModule(sensorValues); // sensor values set to zero, no intrinsic sensors in the servo module
				controlModule(output[0]);
			}
		}
	}
	
	return output; // 
}

stringstream Module_Bone::getModuleParams() {
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

void Module_Bone::setModuleParams(vector<string> values) {
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


void Module_Bone::createControl() {
	//	cout << "creating Rodrigo genome" << endl; 
	// Do not use!
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	//control = controlFactory->createNewControlGenome(settings->controlType, randomNum, settings); // 0 is ANN
	//control = controlFactory->createNewControlGenome(1, randomNum, settings); // 0 is ANN
	control = controlFactory->createNewControlGenome(settings->controlType, randomNum, settings);
	control->init(1, 1, 1); // bias, 6 input, 2 inter, 2 output
	controlFactory.reset();
}

void Module_Bone::controlModule(float input) {
	//	control->update(sensorValues);
//	vector<float> output = control->update(sensorValues);
//	if (output.size() != 1) {
	//	cout << "ERROR:: output size is not 1, but should be" << endl;
//	}
//	cout << "output[0] = " << input << endl;
	simSetJointTargetPosition(controlHandles[0], 0.5 * input * M_PI);
//	return output;
}

vector<float> Module_Bone::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
}

stringstream Module_Bone::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << settings->controlType << endl; // CUSTOM_ANN
	ss << control->getControlParams().str();
	ss << "#EndControl," << endl;
	ss << endl;
	return ss;
}

vector<int> Module_Bone::createCube(vector<float> configuration, int relativePosHandle, int parentHandle) {
    // All functionality of the bone should be added here.
    vector<int> partHandles;
    // Create parent dummy
    int dummy = simCreateDummy(0.1, NULL);
    float dR[3];
    dR[0] = configuration[3];
    dR[1] = configuration[4];
    dR[2] = configuration[5];
    float zeroOrigin[3] = { 0,0,0 };
    float dPos[3];
    dPos[0] = 0.0;
    dPos[1] = 0.0;
    dPos[2] = configuration[2] + 0.0001;
    float zeroPos[3];
    zeroPos[0] = 0.0;
    zeroPos[1] = 0.0;
    zeroPos[2] = 0.0;
    simSetObjectPosition(dummy, relativePosHandle, zeroPos);
    simSetObjectOrientation(dummy, relativePosHandle, dR);
    simSetObjectPosition(dummy, dummy, dPos);
    simSetObjectParent(dummy, parentHandle, true);
    // Create cube
    const float cubeSize = 0.018;
    const float size[3] = { cubeSize, cubeSize, cubeSize };
    int cube = simCreatePureShape(0, objectPhysics, size, 0.3, nullptr);
    float objectOrigin[3];
    objectOrigin[0] = configuration[0];
    objectOrigin[1] = configuration[1];
    objectOrigin[2] = configuration[2] + cubeSize * 0.5;
    float rotationOrigin[3];
    rotationOrigin[0] = configuration[3];
    rotationOrigin[1] = configuration[4];
    rotationOrigin[2] = configuration[5];
    simSetObjectPosition(cube, relativePosHandle, zeroOrigin);
    simSetObjectOrientation(cube, relativePosHandle, rotationOrigin);
    simSetObjectPosition(cube, cube, objectOrigin);
    simSetObjectParent(cube, dummy, true);

    simGetObjectPosition(cube, -1, absPos);
    simGetObjectOrientation(cube, -1, absOri);

    partHandles.push_back(dummy);
    partHandles.push_back(cube);

    // Create children dummies
    vector<int> dummies;
    for (int i = 0; i < 5; i++) {
        dummies.push_back(simCreateDummy(0.01, nullptr));
    }
    float d1[3] = { 0.0,0.0, cubeSize * 0.5f };
    float d1R[3] = { 0.0,0.0, 0.0 };
    float d2[3] = { 0.0,0.0, 0.0 };
    float d2R[3] = { 0.5 * M_PI,0.0, 0.0 };
    float d3[3] = { 0.0,0.0, 0.0 };
    float d3R[3] = { -0.5 * M_PI,0.0, 0.0 };
    float d4[3] = { 0.0,0.0, 0.0 };
    float d4R[3] = { -0.5 * M_PI,0.5 * M_PI, 0.0 };
    float d5[3] = { 0.0,0.0, 0.0 };
    float d5R[3] = { 0.5 * M_PI,-0.5 * M_PI, 0.0 };

    simSetObjectPosition(dummies[0], cube, d1);
    simSetObjectOrientation(dummies[0], cube, d1R);

    simSetObjectPosition(dummies[1], cube, d2);
    simSetObjectOrientation(dummies[1], cube, d2R);
    simSetObjectPosition(dummies[1], dummies[1], d1);

    simSetObjectPosition(dummies[2], cube, d3);
    simSetObjectOrientation(dummies[2], cube, d3R);
    simSetObjectPosition(dummies[2], dummies[2], d1);

    simSetObjectPosition(dummies[3], cube, d4);
    simSetObjectOrientation(dummies[3], cube, d4R);
    simSetObjectPosition(dummies[3], dummies[3], d1);

    simSetObjectPosition(dummies[4], cube, d5);
    simSetObjectOrientation(dummies[4], cube, d5R);
    simSetObjectPosition(dummies[4], dummies[4], d1);

    for (int dummie : dummies) {
        simSetObjectParent(dummie, cube, true);
        partHandles.push_back(dummie);
    }
    // Return all handles
    return partHandles;
}