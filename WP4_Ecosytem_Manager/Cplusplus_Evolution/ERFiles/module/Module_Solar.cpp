#include "Module_Solar.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Module_Solar::Module_Solar()
{

}


Module_Solar::~Module_Solar()
{
//	cout << "REMOVING MODULE!" << endl;

	removeModule();
}


// --------------------------------
// calculate the cofactor of element (row,col)
int GetMinor(float **src, float **dest, int row, int col, int order)
{
	// indicate which col and row is being copied to dest
	int colCount = 0, rowCount = 0;

	for (int i = 0; i < order; i++)
	{
		if (i != row)
		{
			colCount = 0;
			for (int j = 0; j < order; j++)
			{
				// when j is not the element
				if (j != col)
				{
					dest[rowCount][colCount] = src[i][j];
					colCount++;
				}
			}
			rowCount++;
		}
	}

	return 1;
}


// Calculate the determinant recursively.
double CalcDeterminant(float **mat, int order)
{
	// order must be >= 0
	// stop the recursion when matrix is a single element
	if (order == 1)
		return mat[0][0];

	// the determinant value
	float det = 0;

	// allocate the cofactor matrix
	float **minor;
	minor = new float*[order - 1];
	for (int i = 0;i<order - 1;i++)
		minor[i] = new float[order - 1];

	for (int i = 0; i < order; i++)
	{
		// get minor of element (0,i)
		GetMinor(mat, minor, 0, i, order);
		// the recusion is here!

		det += (i % 2 == 1 ? -1.0 : 1.0) * mat[0][i] * CalcDeterminant(minor, order - 1);
		//det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
	}

	// release memory
	for (int i = 0;i<order - 1;i++)
		delete[] minor[i];
	delete[] minor;

	return det;
}

// matrix inversioon
// the result is put in Y
void MatrixInversion(float **A, int order, float **Y)
{
	// get the determinant of a
	double det = 1.0 / CalcDeterminant(A, order);

	// memory allocation
	float *temp = new float[(order - 1)*(order - 1)];
	float **minor = new float*[order - 1];
	for (int i = 0;i<order - 1;i++)
		minor[i] = temp + (i*(order - 1));

	for (int j = 0;j<order;j++)
	{
		for (int i = 0;i<order;i++)
		{
			// get the co-factor (matrix) of A(j,i)
			GetMinor(A, minor, j, i, order);
			Y[i][j] = det*CalcDeterminant(minor, order - 1);
			if ((i + j) % 2 == 1)
				Y[i][j] = -Y[i][j];
		}
	}

	// release memory
	//delete [] minor[0];
	delete[] temp;
	delete[] minor;
}

// --------------------------------


int Module_Solar::init() {
	return -1;
}


int Module_Solar::mutate(float mutationRate) {
	bendAngle = randomNum->randFloat(-0.25 * M_PI,0.75 * M_PI);
	return 1;
}

int Module_Solar::createModule(vector<float> configuration, int relativePosHandle, int parentHandle) {
	energy = 0;
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
	objectOrigin[2] = configuration[2] + 0.00251;

	float rotationOrigin[3];
	rotationOrigin[0] = configuration[3];
	rotationOrigin[1] = configuration[4];
	rotationOrigin[2] = configuration[5];

	//	cout << "objectOrigin[2] = " << objectOrigin[2] << endl; 
	float size[3] = { 0.055,0.055,0.005 };
	float mass = 0.033;
	//		int baseHandle = simCreatePureShape(0, 8, size, mass, false);
	int baseHandle = simCreatePureShape(0, objectPhysics, size, mass, NULL);

	//	cout << "baseHandle = " << baseHandle << endl; 
	
	simSetObjectPosition(baseHandle, relativePosHandle, zeroOrigin);
	simSetObjectOrientation(baseHandle, relativePosHandle, rotationOrigin);
	simSetObjectPosition(baseHandle, baseHandle, objectOrigin);
	simSetObjectParent(baseHandle, fs, true);

	float leafSize[3] = { 0.141,0.124,0.006 };
	int leaf1 = simCreatePureShape(0, objectPhysics, leafSize, mass, 0);

	//	float appearanceSize[3] = { 0.02, 0.02, 0.04 };
	//	int appearancePeace = simCreatePureShape(0, 8, appearanceSize, 0, false);
	//	float appearancePos[3] = { 0,0,0.0225 };
	//	int joint = simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
//		float baseHandlePos[3] = { 0,0,size[2] / 2 };

		//	simSetObjectPosition(appearancePeace, baseHandle, appearancePos);
		float baseHandleOr[3] = { 0,0,0 };
		float leaf1Or[3] = { 0,0,0.0 };

	//	simSetObjectOrientation(baseHandle, -1, baseHandleOr);
		simSetObjectOrientation(leaf1, baseHandle, leaf1Or);

		float leaf1Pos[3] = { 0,0,0.0031 };

//		simSetObjectPosition(baseHandle, -1, objectOrigin);
		simSetObjectPosition(leaf1, baseHandle, leaf1Pos);

		simSetObjectParent(leaf1, baseHandle, true);

		int intpars[8] = { 1,1,1,1,1,1,0,0 };
		float sensorSize = 0.004;
		float floatpars[15] = { 0,10,0.0010,0.001,0.001,0.0001,0.001,0.0001,0.0001,0.0001,0.0001,0.0001,sensorSize,0,0 };
		vector<vector<float>> proxPositions;
		vector<float> pos1;
		vector<float> pos2;
		vector<float> pos3;
		vector<float> pos4;
		vector<float> pos5;
		pos1.push_back(leafSize[0] * 0.0);		pos1.push_back(0.0);						pos1.push_back(0.0035);
		pos2.push_back(leafSize[0] * -0.45);	pos2.push_back(leafSize[1] * 0.45);			pos2.push_back(0.0035);
		pos3.push_back(leafSize[0] * -0.45);	pos3.push_back(leafSize[1] * -0.45);		pos3.push_back(0.0035);
		pos4.push_back(leafSize[0] * 0.45);		pos4.push_back(leafSize[1] * 0.45);			pos4.push_back(0.0035);
		pos5.push_back(leafSize[0] * 0.45);		pos5.push_back(leafSize[1] * -0.45);		pos5.push_back(0.0035);
		proxPositions.push_back(pos1);
		proxPositions.push_back(pos2);
		proxPositions.push_back(pos3);
		proxPositions.push_back(pos4);
		proxPositions.push_back(pos5);

		vector<int> leafHandles;
		leafHandles.push_back(leaf1);
		// leaf one

		for (int l = 0; l < 1; l++) {
			for (int n = 0; n < amountProxSensors; n++) {
				int proxSens = simCreateProximitySensor(sim_proximitysensor_cone_subtype, sim_objectspecialproperty_detectable_ultrasonic, 0, intpars, floatpars, NULL);
				float pos[3] = { proxPositions[n][0], proxPositions[n][1], proxPositions[n][2] };
				simSetObjectPosition(proxSens, leafHandles[l], pos);
				simSetObjectParent(proxSens, leafHandles[l], true);
				proxSensors.push_back(proxSens);
			//	simSetObjectInt32Parameter(proxSens,10, 0);
			//	objectHandles.push_back(proxSens);
			}
		}


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

	int shapHands[2] = { baseHandle, leaf1 };
	int groupedShaps = simGroupShapes(shapHands, 2);

	freeSites = sites;
	objectHandles.push_back(fs);
	objectHandles.push_back(groupedShaps);
	for (int i = 0; i < proxSensors.size(); i++) {
		objectHandles.push_back(proxSensors[i]);
	}
//	objectHandles.push_back(baseHandle);
//	objectHandles.push_back(leaf1);

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
	/**/
	return baseHandle;
}

vector<int> Module_Solar::getFreeSites(vector<int> targetSites) {
	vector<int> tempFreeSites;
	
	return tempFreeSites;
}
vector<int> Module_Solar::getObjectHandles() {
	return objectHandles;
}

void Module_Solar::setModuleColor() {

}

shared_ptr<ER_Module> Module_Solar::clone() {
	return shared_ptr<ER_Module>(new Module_Solar(*this));
}

void Module_Solar::removeModule() {
	for (int i = 0; i < objectHandles.size(); i++) {
		simRemoveObject(objectHandles[i]);
	}
}

vector<float> Module_Solar::updateModule(vector<float> input) {
	//cout << "updating leaf" << endl;

	vector <float> zOfNotCollided;
	vector<int> notCollidedProxSensors;
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
		float objectMatrix[12];
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
		}
	}
	// ----------------------------------------------------------
	// measures the z angle of the panels that did not collide. 
	float objectMatrix[12];
	float addedEnergy = 0;

	if (simGetSimulationTime() > 0.2) {
		for (int n = 0; n < notCollidedProxSensors.size(); n++) {
			simGetObjectMatrix(objectHandles[1], notCollidedProxSensors[n], objectMatrix);
			if (objectMatrix[8] == 1) {
				cout << "huh, objectMatrix[8] is one???::" << endl;
				cout << objectMatrix[0] << ", " << objectMatrix[4] << ", " << objectMatrix[8] << endl;
				cout << objectMatrix[1] << ", " << objectMatrix[5] << ", " << objectMatrix[9] << endl;
				cout << objectMatrix[2] << ", " << objectMatrix[6] << ", " << objectMatrix[10] << endl;
				cout << objectMatrix[3] << ", " << objectMatrix[7] << ", " << objectMatrix[11] << endl;
			}
			zOfNotCollided.push_back(objectMatrix[8]);
			addedEnergy += objectMatrix[8];
		}
	}
	currentStep = 0;
	//	return zOfNotCollided;
	notCollidedProxSensors.clear();
	currentStep++;
//	cout << "addedEnergy = " << addedEnergy << endl;
	if (addedEnergy > 0.0) {
		energy += addedEnergy * 0.00002;
	}
	float difAngle[12];
	simGetObjectMatrix(objectHandles[0], -1, difAngle);
	for (int i = 0; i < 12; i++) {
		float t = sqrt((difAngle[i] * difAngle[i]) - (previousAngle[i] * previousAngle[i]));
		if (t > 0.01) {
			movedAngle[i] += t;
		}
	}
	simGetObjectMatrix(objectHandles[0], -1, previousAngle);
	if (simGetSimulationTime() > 4.85) {
	//	cout << "at " << simGetSimulationTime() << " the movement angle is " << endl;
	//  cout << movedAngle[0] << ", " << movedAngle[4] << ", " << movedAngle[8] << ", and sum angle = ";
		sumAngle = sqrt((movedAngle[0] * movedAngle[0]) + (movedAngle[4] * movedAngle[4]) + (movedAngle[8] * movedAngle[8]));
	//	cout << "sumAngle = " << sumAngle << endl;
	}

	input.clear();
	float newInput = addedEnergy * energyNormalizer;
	input.push_back(newInput);
	ER_Module::updateModule(input);
	return input;
//	return zOfNotCollided;
}


stringstream Module_Solar::getModuleParams() {
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

void Module_Solar::setModuleParams(vector<string> values) {

}

vector<float> Module_Solar::getPosition() {
	vector<float> positionVector;
	float pos[3];
	simGetObjectPosition(objectHandles[1], -1, pos);
	positionVector.push_back(pos[0]);
	positionVector.push_back(pos[1]);
	positionVector.push_back(pos[2]);
	return positionVector;
};

stringstream Module_Solar::getControlParams() {
	stringstream ss;
	ss << "#ControlType," << -1 << endl;
	ss << endl;
	return ss;
}