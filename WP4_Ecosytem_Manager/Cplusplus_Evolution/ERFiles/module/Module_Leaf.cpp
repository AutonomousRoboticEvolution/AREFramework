#include "Module_Leaf.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Module_Leaf::Module_Leaf()
{

}


Module_Leaf::~Module_Leaf()
{
//	cout << "REMOVING MODULE!" << endl;
	removeModule();
}

int Module_Leaf::init() {
	return -1;
}


int Module_Leaf::mutate(float mutationRate) {
	bendAngle = randomNum->randFloat(-0.25 * M_PI,0.75 * M_PI);
	return 1;
}

int Module_Leaf::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	moduleID = 2; 
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
	simSetObjectOrientation(fs, parentHandle, fsR);
	simSetObjectPosition(fs, fs, fsPos);

	// TO DO: Check collision
	// cout << "Creating Bend" << endl; 
		int baseHandle = 0;
		float objectOrigin[3];
		
		objectOrigin[0] = configuration[0];
		objectOrigin[1] = configuration[1];
		objectOrigin[2] = configuration[2] + 0.0275;
	
		float rotationOrigin[3];
		rotationOrigin[0] = configuration[3];
		rotationOrigin[1] = configuration[4]; 
		rotationOrigin[2] = configuration[5]; 

	//	cout << "objectOrigin[2] = " << objectOrigin[2] << endl; 
		
		float size[3] = { 0.055, 0.055, 0.055 };
		float size2[3] = { 0.15,0.15,0.01 };
		float orientation[3] = { 0.1,0.1,0.1 };
		float mass = 0.07;
		float mass2 = 0.03;
		float red[3] = { 1, 0, 0 };
		float blue[3] = { 0, 0, 1 };
		float green[3] = { 0, 1, 0 };
		//objectOrigin[0] = 0;
		//objectOrigin[1] = 0;
		//objectOrigin[2] = 0.05 + (0.501 * size[2]);
		float zeroOrigin[3] = { 0,0,0 };

		// create a box from lines; 
		vector<vector<float>> cubeVertex; // 8 points in 3d space
		vector<vector<float>> points;
		points.resize(8);
	
		baseHandle = simCreatePureShape(0, objectPhysics, size, mass, NULL);
	//	cout << "baseHandle = " << baseHandle << endl; 


		simSetObjectParent(baseHandle, parentHandle, false);
		simSetObjectOrientation(baseHandle, parentHandle, rotationOrigin);
		simSetObjectPosition(baseHandle, parentHandle, zeroOrigin);
		simSetObjectPosition(baseHandle, baseHandle, objectOrigin);

		simSetObjectParent(baseHandle, fs, true);

		float jointSize[3] = { 0.01,0.01,0.01 };
		// add hinge
		int hingeJoint = simCreateForceSensor(0, fsParams, fsFParams, NULL);//simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 1, jointSize, NULL, NULL);
		float jointPos[3] = { 0, -0.0, (0.5f * size[2]) };
		simSetObjectPosition(hingeJoint, baseHandle, jointPos);
		float jointOrientation[3] = { 0.0*M_PI,0.0*M_PI,0.0 * M_PI };
		simSetObjectOrientation(hingeJoint, baseHandle, jointOrientation);
		simSetObjectParent(hingeJoint, baseHandle, true);
	
	//	cout << "hingeJoint = " << hingeJoint << endl; 
	
		int cube2 = simCreatePureShape(0, objectPhysics, size2, mass2, NULL);
		float cube2Origin[3] = { 0, 0.0, (0.5f * size[2]) + (0.2501f * size2[2])};
		float rotation[3] = { 0.0*M_PI,0.0*M_PI,0.0001 * M_PI };
		simSetObjectPosition(cube2, baseHandle, cube2Origin);
		simSetObjectOrientation(cube2, baseHandle, rotation);
	//	simPauseSimulation();
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

	
	freeSites = sites;
	objectHandles.push_back(fs); 
	objectHandles.push_back(baseHandle);
	objectHandles.push_back(hingeJoint);
	objectHandles.push_back(cube2);
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

vector<int> Module_Leaf::getFreeSites(vector<int> targetSites) {
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
vector<int> Module_Leaf::getObjectHandles() {
	return objectHandles;
}

void Module_Leaf::setModuleColor() {

}

shared_ptr<ER_Module> Module_Leaf::clone() {
	return shared_ptr<ER_Module>(new Module_Leaf(*this));
}

void Module_Leaf::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_Leaf::updateModule(vector<float> input) {
	//cout << "updating leaf" << endl;
	ER_Module::updateModule(input);

	vector<float> leafValues;
	int leafHandle = objectHandles[3];
	float pos[3];
	simGetObjectPosition(leafHandle, -1, pos);
	float rot[3];
	simGetObjectOrientation(leafHandle, -1, rot );
	float size[3] ;
	simGetObjectFloatParameter(leafHandle, 18, &size[0]);
	simGetObjectFloatParameter(leafHandle, 19, &size[1]);
	simGetObjectFloatParameter(leafHandle, 20, &size[2]);
	for (int i = 0; i < 3; i++) {
		size[i] = size[i] * 2;
	}
	size[0] = size[0] * 0.9;
	size[1] = size[1] * 0.9;
	size[2] = size[2] * 1.001;
	float objectMatrix[12];
	float rotationOrigin[3] = { 0,0,0 };

//	cout << "size = " << size[0] << ", " << size[1] << ", " << size[2] << endl;

	float red[3] = { 1, 0, 0 };
	float blue[3] = { 0, 0, 1 };
	float green[3] = { 0, 1, 0 };

	// create a box from lines; 
	vector<vector<float>> cubeVertex; // 8 points in 3d space
	vector<vector<float>> points;
	points.resize(5);

	simGetObjectMatrix(leafHandle, -1, objectMatrix);
	/*cout << "objectMatrix = ";
	for (int i = 0; i < 12; i++) {
		cout << objectMatrix[i] << ", ";
	} cout << endl;*/

	points[0].push_back(rotationOrigin[0] + (0.0 * size[0]));
	points[0].push_back(rotationOrigin[1] - (0.0 * size[1]));
	points[0].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[1].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[1].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[1].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[2].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[2].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[2].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[3].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[3].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[3].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[4].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[4].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[4].push_back(rotationOrigin[2] + (0.5 * size[2]));


	/*points[4].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[4].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[4].push_back(rotationOrigin[2] - (0.5 * size[2]));

	points[5].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[5].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[5].push_back(rotationOrigin[2] - (0.5 * size[2]));

	points[6].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[6].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[6].push_back(rotationOrigin[2] - (0.5 * size[2]));

	points[7].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[7].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[7].push_back(rotationOrigin[2] - (0.5 * size[2]));*/

	vector<vector<float>> rotatedPoints;
	rotatedPoints.resize(points.size());
	for (int i = 0; i < points.size(); i++) {
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[0]) + (points[i][1] * objectMatrix[1]) + (points[i][2] * objectMatrix[2]));
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[4]) + (points[i][1] * objectMatrix[5]) + (points[i][2] * objectMatrix[6]));
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[8]) + (points[i][1] * objectMatrix[9]) + (points[i][2] * objectMatrix[10]));
		rotatedPoints[i][0] += pos[0];
		rotatedPoints[i][1] += pos[1];
		rotatedPoints[i][2] += pos[2];
	}
	vector <float> fourPoints;
	for (int i = 0; i < rotatedPoints.size(); i++) {
		for (int j = 0; j < 3; j++) {
			fourPoints.push_back(rotatedPoints[i][j]);
		}
	}
	return fourPoints;
}


stringstream Module_Leaf::getModuleParams() {
	// needs to save variables of the module
	stringstream ss;
	ss << "#ModuleType:," << 3 << endl;  // make sure this is the same number as in the module factory
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

void Module_Leaf::setModuleParams(vector<string> values) {

}

vector<float> Module_Leaf::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_Leaf::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}