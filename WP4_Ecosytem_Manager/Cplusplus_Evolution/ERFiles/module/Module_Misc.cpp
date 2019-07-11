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
	// All object handles of the module
	vector<int> shapes;
	// all joint handles of the module
	vector<int> joints;
	// all dummy handles of the module
	vector<int> dummies;
	// all sensor handles of the module
	vector<int> sensors;
	// select all objects in tree
	simAddObjectToSelection(sim_handle_tree, miscHandle);
	int selectionSize = simGetObjectSelectionSize();
	// store all these objects (max 50 shapes)
	int shapesStorage[50]; // stores up to 50 shapes
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
		// TODO: Other sensors might need to be added as well
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
	fsParams[2] = settings->consecutiveThresholdViolations;
	fsParams[3] = 0;
	fsParams[4] = 0;
	float fsFParams[5];
	fsFParams[0] = 0.005;
	fsFParams[1] = settings->maxForce_ForceSensor;
	fsFParams[2] = settings->maxTorque_ForceSensor; // change torque
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
    if (parentHandle != -1) {
        fsPos[2] = configuration[2] + 0.0001;
    }
    else{
        fsPos[2] = 1.0;
    }
	float zeroPos[3]; 
	zeroPos[0] = 0.0;
	zeroPos[1] = 0.0;
	zeroPos[2] = 0.0;

    if (parentHandle == -1) {
        // TODO: Change function below in comments
        //simSetObjectPosition(miscHandle, NULL, zeroPos);
        //simSetObjectOrientation(miscHandle, NULL, fsR);
    }
    else{
        simSetObjectPosition(fs, relativePosHandle, zeroPos);
        simSetObjectOrientation(fs, relativePosHandle, fsR);
        simSetObjectPosition(fs, fs, fsPos);
        simSetObjectParent(fs, parentHandle, true);
        simSetObjectPosition(miscHandle, fs, zeroPos);
        simSetObjectOrientation(miscHandle, relativePosHandle, fsR);
    }


	simRemoveObject(miscHandle);
	dummies.erase(dummies.begin());
	miscHandle = shapes[0];
    if (parentHandle != -1) {
        simSetObjectParent(miscHandle, fs, true);
    }
	
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
			site[i]->parentHandle = shapes[shapes.size()-1];
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
    if (parentHandle == -1) {
        simRemoveObject(fs); //dont't need force sensor when parentHandle = -1
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