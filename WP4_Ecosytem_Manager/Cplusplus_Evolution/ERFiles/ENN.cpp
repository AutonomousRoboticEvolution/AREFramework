// bubbleRob plugin artefact:
// "
// Copyright 2006-2014 Dr. Marc Andreas Freese. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------	
//
// This file was automatically created for V-REP release V3.1.0 on January 20th 2014

//**************************************************
// This class represents a BubbleRob inside this
// plugin. Each BubbleRob model in V-REP
// should have its corresponding CBubbleRob object
// in this plugin. If a BubbleRob model
// in V-REP is removed (i.e. erased), then its
// corresponding CBubbleRob object should also be
// removed. Same when a new BubbleRob model is added
//**************************************************
// "

#include "ENN.h"
#include "v_repLib.h"
// #include "Serial.h"
#include <iostream>
#include <cmath>
#include <thread>
//csv
#include <fstream> // for storing arrays and using ofstream
#include <cstdlib> // for converting values
//#include <vector>  //
#include <cstring>
#include <sstream>
#include <list>		// for reading files and handling the acquired information
#include <iterator> // for reading files and handling the acquired information
#include <string>
#include <windows.h>
#include <vector>
#include <cassert>
#include <memory>

//#include "Settings.h"




//#include <afxwin.h> ;

using namespace std; // to use cout giving feedback from the DLL to the command prompt in V-Rep


//***************************************************************************************//
// for reading text files
void split_line(string& line, string delim, list<string>& values)
{
	size_t pos = 0;
	while ((pos = line.find(delim, (pos + 1))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// create class bodies?

CUserInterface::CUserInterface(){}
CUserInterface::~CUserInterface(){}
CENN::CENN(){}
CENN::~CENN(){}
CCat::CCat(){}
CCat::~CCat(){}

class CPreparationApp
{
public :
	BOOL InitInstance();
};

BOOL CPreparationApp::InitInstance()
{
	return TRUE;
}

CPreparationApp theAPP;

void CCat::startOfSimulation() //
{
}

bool CCat::endOfSimulation() // 
{
	//	catCounter = 0; 
	//	generationCounter = 0;
	//	generation = 0;

	return 0;
	//	_initialized = false;
	return(false); // We don't want this object automatically destroyed at the end of simulation
}

void CCat::handleSimulation() // 
{

}

void CENN::displayPars() {
	cout << "Displaying Parameters: " << endl;
	cout << "0:" << s_amountJoints << ", ";
	cout << "1:" << s_jointControl << ", ";					// #1
	cout << "2:" << s_initialPopulationSize << ", ";			// #2
	cout << "3:" << s_islandPopulations << ", ";				// #3
	cout << "4:" << s_useSensors << ", ";						// #4
	cout << "5:" << s_usePatternGenerators << ", ";							// #5
	cout << "6:" << s_useProprioception << ", ";				// #6
	cout << "7:" << s_useVestibularSystem << ", ";				// #7
	cout << "8:" << s_useTactileSensing << ", ";				// #8
	cout << "9:" << s_tactileSensingType << ", ";			// #9
	cout << "10:" << s_amountSenseParts << ", ";					// #10
	cout << "11:" << s_specifiedPartNames << ", ";			// #11
	cout << "12:" << s_reproductionType << ", ";				// #12
	cout << "13:" << s_genomeType << ", ";					// #13
	cout << "14:" << s_evolvableMutationRate << ", ";			// #14
	cout << "15:" << s_initialMutationRate << ", ";			// #15
	cout << "16:" << s_amountInterNeurons << ", ";				// #16
	cout << "17:" << s_amountPatternGenerators << ", ";				// #17
	cout << "18:" << s_amountInputNeurons << ", ";				// #18
	cout << "19:" << s_amountOutputNeurons << ", ";				// #19
	cout << "20:" << s_useCrossover << ", ";					// #10
	cout << "21:" << s_maxAmountCrossover << ", ";					// #21
	cout << "22:" << s_evolvableCrossover << ", ";				// #22
	cout << "23:" << s_variableGenomeSize << ", ";				// #23
	cout << "24:" << s_geneDuplication << ", ";					// #24
	cout << "25:" << s_evolvingMorphology << ", ";				// #25
	cout << "26:" << s_evolvingDimensions << ", ";				// #26
	cout << "27:" << s_deleteRandomParts << ", ";				// #27
	cout << "28:" << s_addRandomParts << ", ";					// #28
	cout << "29:" << s_lEvolution << endl;					// #29
}

void CUserInterface::createUserInterface()
{
	cout << "userInterface should be created" << endl;
	int uiSize[2] = { 20, 12 };
	int cellSize[2] = { 20, 20 };
	int somePar[2] = { 5, 5 };
	char* uiName = "Initial UI";
	simCreateUI("Initial UI", sim_ui_menu_title || sim_ui_menu_systemblock, uiSize, cellSize, somePar);
	int uiHandle = simGetUIHandle("Initial UI");
	simSetUIButtonLabel(uiHandle, 0, uiName, uiName);
	int pos[2] = { 0, 1 };
	int size[2] = { 20, 2 };
	simCreateUIButton(uiHandle, pos, size, 8280);
	//simCreateUIButton(uiHandle, pos, size, sim_buttonproperty_staydown || sim_buttonproperty_enabled);
	simSetUIButtonLabel(uiHandle, 1, "\n\nClick here to open ENN interface", "NN interface opened, V-REP paused");
	cout << "uiHandle = " << uiHandle << endl;
	cout << "ui created" << endl;

	// temp ***************************************
	//CreateProcess()
//	string filePath = "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2.exe";
//	system("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2.exe");
	//	system("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2");
//	GetFileAttributes("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2.exe"); // from winbase.h
//	if (0xffffffff == GetFileAttributes("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2.exe"))
//	{
//		cout << "//File not found" << endl;
//	}
//	GetFileAttributes("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2.exe"); // from winbase.h
//	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\WindowsFormsApplication2.exe") && GetLastError() == ERROR_FILE_NOT_FOUND)
//	{
//		cout << "//File really not found" << endl;
//	}
	// ********************************************
}

void CENN::storeJointPars(int partNum, float jointPosition[3], float jointOrientation[3], float jointSize[3], float jointMass, int jointParent, int jointType
	, float jointMaxMinAngles[2], float jointMaxForce, float jointMaxVelocity){
	i_jointMass.resize(partNum + 1);
	i_jointType.resize(partNum + 1);
	i_jointParent.resize(partNum + 1);
	i_jointSizes.resize(partNum + 1);
	i_jointPosition.resize(partNum + 1);
	i_jointOrientation.resize(partNum + 1);
	i_jointMaxMinAngles.resize(partNum + 1);
	i_jointMaxForce.resize(partNum + 1);
	i_jointMaxVelocity.resize(partNum + 1);
	for (int i = 0; i < 3; i++)
	{
		i_jointSizes[partNum].resize(3);
		i_jointPosition[partNum].resize(3);
		i_jointOrientation[partNum].resize(3);
		i_jointMaxMinAngles[partNum].resize(2);
	}

	// simple save
	i_jointType[partNum] = jointType;
	i_jointParent[partNum] = jointParent;
	i_jointMaxForce[partNum] = jointMaxForce;
	i_jointMaxVelocity[partNum] = jointMaxVelocity;
	for (int i = 0; i < 3; i++) {
		i_jointSizes[partNum][i] = jointSize[i];
		i_jointPosition[partNum][i] = jointPosition[i];
		i_jointOrientation[partNum][i] = jointOrientation[i];
	}
	for (int i = 0; i < 2; i++) {
		i_jointMaxMinAngles[partNum][i] = jointMaxMinAngles[i];
	}

}

void CENN::storeObjectPars(int partNum, float objectPosition[3], float objectOrientation[3], float objectSize[3], float objectMass, int objectParent, int objectType) {
	//resize vectors based on partNum
	i_objectMass.resize(partNum+1);
	i_objectType.resize(partNum + 1);
	i_objectParent.resize(partNum + 1);
	i_objectSizes.resize(partNum + 1);
	i_objectPosition.resize(partNum + 1);
	i_objectOrientation.resize(partNum + 1);
	for (int i = 0; i < 3; i++)
	{
		i_objectSizes[partNum].resize(3);
		i_objectPosition[partNum].resize(3);
		i_objectOrientation[partNum].resize(3);
	}

	// save obj props #0
	i_objectMass[partNum] = objectMass;
	i_objectType[partNum] = objectType;
	i_objectParent[partNum] = objectParent;
	for (int i = 0; i < 3; i++)
	{
		i_objectSizes[partNum][i] = objectSize[i];
		i_objectPosition[partNum][i] = objectPosition[i];
		i_objectOrientation[partNum][i] = objectOrientation[i];
	}
}

void CENN::createCube(float length, float width, float height, float x, float y, float z, float rotX, float rotY, float rotZ, float mass, int parent, int partNum) {
	float objPar[3] = { length, width, height };
	cout << "object Parent = " << parent << endl;

	simCreatePureShape(0, 16, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	float objPos[3] = { x, y, z };
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], objPos);
	float rot[3] = { rotX, rotY, rotZ };
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);


	float worldPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
		simRemoveObject(currentGenome.g_objectHandles[partNum]);
	}

	currentGenome.g_objectPosition.resize(partNum + 1);
	currentGenome.g_objectPosition[partNum].resize(3);
	currentGenome.g_objectPosition[partNum][0] = x;
	currentGenome.g_objectPosition[partNum][1] = y;
	currentGenome.g_objectPosition[partNum][2] = z;

	currentGenome.g_objectOrientation.resize(partNum + 1);
	currentGenome.g_objectOrientation[partNum].resize(3);
	currentGenome.g_objectOrientation[partNum][0] = rotX;
	currentGenome.g_objectOrientation[partNum][1] = rotY;
	currentGenome.g_objectOrientation[partNum][2] = rotZ;

	currentGenome.g_objectSizes.resize(partNum + 1);
	currentGenome.g_objectSizes[partNum].resize(3);
	currentGenome.g_objectSizes[partNum][0] = length;
	currentGenome.g_objectSizes[partNum][1] = width;
	currentGenome.g_objectSizes[partNum][2] = height;

	currentGenome.g_objectMass.resize(partNum + 1);
	currentGenome.g_objectMass[partNum] = mass;
	currentGenome.g_objectParent.resize(partNum + 1);
	currentGenome.g_objectParent[partNum] = parent;
	currentGenome.g_objectType.resize(partNum + 1);
	currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];
//	cout << "Cube created " << endl;
}

void CENN::testCreateRay(int object1, int object2) {

	//	float startEndLine[6] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	
	int testHandle = mainHandle[0];
	int lightHandle = simGetObjectHandle("Omnidirectional_light");


	cout << "LightHandle = " << lightHandle << endl;
	float m[12];
	simGetObjectMatrix(lightHandle, NULL, m);
	int rayHandle = 0;
	float blue[3] = { 0, 0, 1 };
	rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0, testHandle, 2, blue, NULL, NULL, blue); // startEndLine );
	float a[3];
	float b[3];
	simGetObjectPosition(testHandle, -1, a);
	simGetObjectPosition(lightHandle, -1, b);
	cout << "mainHandlePosition = ";
	for (int i = 0; i < 3; i++) {
		cout << "a[" << i << "] = " << a[i] << ", ";
	} cout << endl;
	float ab[6] = { a[0], a[1], a[2], b[0], b[1], b[2] };
	simAddDrawingObjectItem(rayHandle, ab);
	//	for (int i = 0; i < currentGenome.g_objectAmount){
	//		if (simCheckCollision(rayHandle, currentGenome.g_objectHandles[i]) == true) {
	//			cout << "ray collided thus make red" << endl;
	//
	//		}
	//	}

}

bool CENN::createRay(int object1, int object2) {
	//	float startEndLine[6] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	cout << "LightHandle = " << lightHandle << endl;
	float m[12];
	int rayHandle = 0;
	float blue[3] = { 0, 0, 1 };
	float red[3] = { 1, 0, 0 };
	rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0, object1, 2, blue, NULL, NULL, blue); // startEndLine );
	float a[3];
	float b[3];
	simGetObjectPosition(object1, -1, a);
	simGetObjectPosition(object2, -1, b);
	cout << "mainHandlePosition = ";
	for (int i = 0; i < 3; i++) {
		cout << "a[" << i << "] = " << a[i] << ", ";
	} cout << endl;
	float ab[6] = { a[0], a[1], a[2], b[0], b[1], b[2] };
	simAddDrawingObjectItem(rayHandle, ab);
//	simRemoveDrawingObject(rayHandle);
	cout << "rayHandle == " << rayHandle << endl;
	for (int i = 0; i < currentGenome.g_objectAmount; i++) {
		//	assert(currentGenome.g_objectHandles[i] != rayHandle);
	}
//	vector<double> vertices;
//	vertices.resize(18);
//	vertices = { 0, 0.2, 0, 0.1, 0.1, 0.1, 0.5, 0.5, 0.5, 0.2, 0.2, 0.2, 0.3, 0.3, 0.1, 0.4, 0.4, 0.4 };
//	float verticesArray[18] = { 0, 0.2, 0, 0.1, 0.1, 0.1, 0.5, 0.5, 0.5, 0.2, 0.2, 0.2, 0.3, 0.3, 0.1, 0.4, 0.4, 0.4 };;
//	vector<int> indices;
//	indices.resize(6); 
//	indices = { 0, 1, 2, 3, 4, 5 };
//	int indicesArray[6] = { 0, 1, 2, 3, 4, 5 };
	float verticesArray[9] = { a[0], a[1], a[2], b[0], b[1], b[2], (a[0] + 0.001), (a[1] + 0.001), (a[2] + 0.001) };
	int indicesArray[3] = { 0, 1, 2 };
	int meshHandle = simCreateMeshShape(2, NULL, verticesArray, 9, indicesArray, 3, NULL);
	int sunCounter = 0;
	bool noCollision = false;

	//simSetShapeColor(rayHandle, NULL, sim_colorcomponent_ambient_diffuse, red);
	for (int i = 0; i < currentGenome.g_objectAmount; i++) {
		if (simCheckCollision(meshHandle, currentGenome.g_objectHandles[i]) == true && currentGenome.g_objectHandles[i] != object1
			&& currentGenome.g_objectHandles[i] != object2) {
			float lineProperties[6];
			simRemoveDrawingObject(rayHandle);
			rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0, object1, 2, red, NULL, NULL, red);
			simAddDrawingObjectItem(rayHandle, ab);
			cout << "Collision at: " << currentGenome.g_objectHandles[i] << " and not " << object1 << " or " << object2 << endl; 
			//simSetObjectColo
			//simRemoveObject(rayHandle);
			noCollision = false;
		}
		noCollision = true;
	} 
	rayHandles.push_back(rayHandle);
	simRemoveObject(meshHandle);
	return noCollision;
	
	//	for (int i = 0; i < currentGenome.g_objectAmount){
	//		if (simCheckCollision(rayHandle, currentGenome.g_objectHandles[i]) == true) {
	//			cout << "ray collided thus make red" << endl;
	//
	//		}
	//	}

}




void CENN::createArtificialLeafWithJoint(float length, float width, float height, float x, float y, float z,
	float rotX, float rotY, float rotZ, float mass, int parent, int partNum)
{
	// initialize vectors to make 1 new joint and cube
	cout << "pushing back leafs" << endl;
	currentGenome.g_leafHandles.push_back(0);
	currentGenome.g_leafSunContact.push_back(false);
	currentGenome.g_leafJointHandles.push_back(0);
//	cout << "sizes vs joint sizes = " << currentGenome.g_leafHandles.size() << ", " << currentGenome.g_leafJointHandles.size() << endl;
//	currentGenome.g_leafJointHandles.resize(currentGenome.g_leafHandles.size());
	cout << "sizes vs joint sizes = " << currentGenome.g_leafHandles.size() << ", " << currentGenome.g_leafJointHandles.size() << endl;
	assert(currentGenome.g_leafJointHandles.size() == currentGenome.g_leafHandles.size());
	cout << "leafs pushed back" << endl; 
	cout << "partNum == " << partNum << endl;
	
	// create a new leaf joint
	
	float objPar[3] = { length, width, height };
	//	cout << "object Parent = " << parent << endl;t
	float jointSize[3] = { 0.1, 0.02, 0.0 };
//	assert(simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL) != -1);
	currentGenome.g_leafJointHandles[partNum]= simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);

	// rename joint
	cout << "leafJoint " << simGetObjectName(currentGenome.g_leafJointHandles[partNum]) << endl;
	string jointName = "leaf joint" + to_string(partNum);
//	string leafName = "Leaf" + to_string(currentGenome.g_leafHandles.size());
	jointName = "Leaf joint" + to_string(currentGenome.g_leafHandles.size());

//	assert(simSetObjectName(currentGenome.g_leafJointHandles[partNum], jointName.c_str())!=-1);
	simSetObjectName(currentGenome.g_leafJointHandles[partNum], jointName.c_str());
	cout << "joint created with name: " << jointName << endl;
	assert(currentGenome.g_leafJointHandles[partNum] >= 0);

	/* setJointPars /*
	simSetJointForce(currentGenome.g_leafJointHandles[partNum], 1.0);
	simSetJointTargetVelocity(currentGenome.g_leafJointHandles[partNum], 0.0);
	simSetObjectIntParameter(currentGenome.g_leafJointHandles[partNum], 2000, 1);
	simSetObjectIntParameter(currentGenome.g_leafJointHandles[partNum], 2001, 1);
	/**/

	// set jointPosition based on parent Object
	float objSize[1];
	simGetObjectFloatParameter(currentGenome.g_objectHandles[parent], 20, objSize);
	float jointPos[3] = { x, y, objSize[0] /*+ (0.1*objSize[0])*/ };
	simSetObjectPosition(currentGenome.g_leafJointHandles[partNum], currentGenome.g_objectHandles[parent], jointPos);
	cout << "joint position and parent set" << endl; 

	// joint rotation

	// create cube
	cout << "vector size = " << currentGenome.g_leafHandles.size() << " while partnum = " << partNum << endl;
	float objParams[3] = { length, width, height };
	cout << "object Parent = " << parent << endl;
	mass = length * width * height;
	// cube creation
	currentGenome.g_leafHandles[partNum] = simCreatePureShape(0, 16, objPar, mass, NULL);
	// renaming cube
	cout << "handle = " << currentGenome.g_leafHandles[partNum] << endl;
	cout << "object name = " << simGetObjectName(currentGenome.g_leafHandles[partNum]) << endl;
	string leafName = "Leaf" + to_string(currentGenome.g_leafHandles.size());
	leafName = "Leaf" + to_string(currentGenome.g_leafHandles.size());
	cout << "leafName = " << leafName << endl;
	simSetObjectName(currentGenome.g_leafHandles[partNum], leafName.c_str());
	cout << "handleName after setting name: " << simGetObjectName(currentGenome.g_leafHandles[partNum]) << endl;

	// set joint and cube orientation
	float objPos[3] = { 0, 0, objSize[0] };
	float rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = rotX / 2;
	rot[1] = rotY / 2;
	rot[2] = rotZ / 2;

	//	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	float jointRot[3] = { 0.0, 0.0, 0.0 };
	jointRot[0] = rotX / 4;
	jointRot[1] = rotY / 4;
	jointRot[2] = rotZ / 4;



	simSetObjectOrientation(currentGenome.g_leafJointHandles[partNum], currentGenome.g_objectHandles[parent], jointRot);
	simSetObjectOrientation(currentGenome.g_leafHandles[partNum], currentGenome.g_leafJointHandles[partNum], rot);
	simSetObjectPosition(currentGenome.g_leafHandles[partNum], currentGenome.g_leafJointHandles[partNum], objPos);
	simSetObjectParent(currentGenome.g_leafJointHandles[partNum], currentGenome.g_objectHandles[parent], TRUE);
	simSetObjectParent(currentGenome.g_leafHandles[partNum], currentGenome.g_leafJointHandles[partNum], TRUE);
//	simSetObjectName(currentGenome.g_leafJointHandles[partNum], jointName.c_str());




	// ------------------------------------------

	//char* jointNam = jointName;
	/*
	simCreatePureShape(0, 11, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	nameObject("part", currentGenome.g_objectAmount - 1);
	float objPos[3] = { 0, 0, objSize[0] };
	float rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = rotX / 2;
	rot[1] = rotY / 2;
	rot[2] = rotZ / 2;

	//	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	float jointRot[3] = { 0.0, 0.0, 0.0 };
	jointRot[0] = rotX / 4;
	jointRot[1] = rotY / 4;
	jointRot[2] = rotZ / 4;
	//	jointRot[0] = 0.5 * 3.1415;
	//	jointRot[1] = 0;
	//	jointRot[2] = 0;
	//	simGetObjectOrientation(currentGenome.g_objectHandles[parent], NULL, jointRot);
	simSetObjectOrientation(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointRot);
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], objPos);

	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_jointHandles[jointNum], TRUE);

	/**/
	// ------------------------------------------

	//	jointRot[0] = 0.5 * 3.1415;
	//	jointRot[1] = 0;
	//	jointRot[2] = 0;
	//	simGetObjectOrientation(currentGenome.g_objectHandles[parent], NULL, jointRot);
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	
	//	if (simSetObjectName(currentGenome.g_leafHandles[partNum], leafName.c_str()) == -1) {
	//		cout << "neuron name not sent" << endl;
	//	}
	//	if (simSetObjectName(currentGenome.g_leafHandles[partNum], "why not")) {
	//		cout << "neuron name not sent" << endl;
	//	}
	//	simSetObjectName(currentGenome.g_leafHandles[partNum],  "zx99");
	//	cout << "handleName after setting name: " << simGetObjectName(currentGenome.g_leafHandles[partNum]) << endl;


	//simSetObjectParent(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], true);
	//simAddObjectToSelection(sim_handle_self, currentGenome.g_leafHandles[partNum]);
	//	cout << "pure" << endl;
	//	currentGenome.g_leafHandles[partNum] = simGetObjectHandle("Cuboid");
//	float objPos[3] = { x, y, z };
//	float objParPos[3];
//	simGetObjectPosition(currentGenome.g_objectHandles[parent], sim_handle_parent, objParPos);
//	cout << "objPar values: " << objParPos[0] << ", " << objParPos[1] << ", " << objParPos[2] << endl;

	//simGetObjectSizeValues(currentGenome.g_objectHandles[parent], objParPos);
//	objPos[0] = x;
//	objPos[1] = y;
//	objPos[2] = (objParPos[2]);
//	cout << "objPar values: " << objParPos[0] << ", " << objParPos[1] << ", " << objParPos[2] << endl;
//	simSetObjectPosition(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], objPos);
//	float rot[3] = { rotX, rotY, rotZ };
//	simSetObjectOrientation(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], rot);
///	//	simSetObjectIntParameter(currentGenome.g_leafHandles[partNum], 3019, 0);
//	cout << "leaf 2" << endl;
	//	float worldPos[3];
	//	simGetObjectPosition(currentGenome.g_leafHandles[partNum], NULL, worldPos);
	//	if (worldPos[2] < 0) {
	//		simRemoveObject(currentGenome.g_leafHandles[partNum]);
	//	}
	//	currentGenome.g_objectType[partNum] = 6;
	//	simSetObjectIntParameter(currentGenome.g_leafHandles[partNum], 3019, 65535);
	//	string jointName = "joint" + to_string(jointNum);
	//	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());

	// delete the object if it collides
	cout << "leaf 3" << endl;
	bool deletedPart = false;
	/*for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
	if (i != parent) {
	if (simCheckCollision(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[i]) == true) {
	deletedPart = true;
	break;
	}
	}
	}/**/
	if (deletedPart == false){
		for (int i = 0; i < currentGenome.g_leafHandles.size() - 1; i++) {
			if (simCheckCollision(currentGenome.g_leafHandles[partNum], currentGenome.g_leafHandles[i]) == true) {
				deletedPart = true;
				break;
			}
		}
	}/**/
	if (deletedPart == true) {
		cout << "leaf " << partNum << " collided!" << " leaf " << partNum << " will therefore be deleted" << endl;
		simRemoveObject(currentGenome.g_leafHandles[partNum]);
		simRemoveObject(currentGenome.g_leafJointHandles[partNum]);
		cout << "leaf removed" << endl;
		if (currentGenome.g_leafHandles.size() - 1 <= 0) { // || currentGenome.g_leafHandles.size() - 1 < 0){
			currentGenome.g_leafHandles.clear();
			currentGenome.g_leafSunContact.clear();
			currentGenome.g_leafJointHandles.clear();
		}
		else {
			currentGenome.g_leafHandles.resize(currentGenome.g_leafHandles.size() - 1);
			currentGenome.g_leafSunContact.resize(currentGenome.g_leafHandles.size());
			currentGenome.g_leafJointHandles.resize(currentGenome.g_leafHandles.size());
			//		currentGenome.g_leafSunContact.resize(currentGenome.g_leafHandles.size());
			cout << "leafhandle vs contact size: " << currentGenome.g_leafHandles.size() << ", " << currentGenome.g_leafSunContact.size() << endl;
			assert(currentGenome.g_leafHandles.size() == currentGenome.g_leafSunContact.size());
		}
		cout << "leafHandles resized" << endl;

	}
	else {
		float color[3] = { 0.0, 0.0, 0.0 };
		simSetShapeColor(currentGenome.g_leafHandles[partNum], NULL, sim_colorcomponent_ambient_diffuse, color);
		//	simPauseSimulation();
	}

	cout << "myvector contains:";
	for (unsigned i = 0; i < currentGenome.g_leafHandles.size(); i++) {
		cout << ' ' << currentGenome.g_leafHandles[i];
		cout << ", " << currentGenome.g_leafSunContact[i];
	}
	cout << '\n';

	/*
	currentGenome.g_objectPosition.resize(partNum + 1);
	currentGenome.g_objectPosition[partNum].resize(3);
	currentGenome.g_objectPosition[partNum][0] = x;
	currentGenome.g_objectPosition[partNum][1] = y;
	currentGenome.g_objectPosition[partNum][2] = z;

	currentGenome.g_objectOrientation.resize(partNum + 1);
	currentGenome.g_objectOrientation[partNum].resize(3);
	currentGenome.g_objectOrientation[partNum][0] = rotX;
	currentGenome.g_objectOrientation[partNum][1] = rotY;
	currentGenome.g_objectOrientation[partNum][2] = rotZ;

	currentGenome.g_objectSizes.resize(partNum + 1);
	currentGenome.g_objectSizes[partNum].resize(3);
	currentGenome.g_objectSizes[partNum][0] = length;
	currentGenome.g_objectSizes[partNum][1] = width;
	currentGenome.g_objectSizes[partNum][2] = height;
	cout << "leaf 4" << endl;
	currentGenome.g_objectMass.resize(partNum + 1);
	currentGenome.g_objectMass[partNum] = mass;
	currentGenome.g_objectParent.resize(partNum + 1);
	currentGenome.g_objectParent[partNum] = parent;
	currentGenome.g_objectType.resize(partNum + 1);
	currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];
	simSetObjectParent(currentGenome.g_leafHandles[currentGenome.g_objectAmount - 1], currentGenome.g_objectHandles[parent], TRUE);
	nameObject("leaf", currentGenome.g_objectAmount - 1);
	cout << "leaf 5" << endl;


	// visionSensor
	int resX = 4;
	int resY = 4;
	int sensorResolution[4] = { resX, resY, 0, 0 };
	float sensorFloatArray[11] = { 0, 0, 0, 0.1, 0.1, 0.01, 0.1, 0.1, 0.1, 0.0, 0.0 };
	float sensorColor[3] = { 0.5, 0.5, 1.0 };
	simCreateVisionSensor(2, sensorResolution, sensorFloatArray, sensorColor);
	int visionHandle = 0;
	simGetObjectHandle("Vision_sensor");
	visionHandle = currentGenome.g_leafHandles[partNum] + 1;


	string visionName = "Vision" + to_string(partNum);

	simSetObjectName(visionHandle, visionName.c_str());

	assert(visionHandle != 0);
	cout << "VisionHandle = " << visionHandle << endl;
	simSetObjectParent(visionHandle, currentGenome.g_objectHandles[parent], false);
	float visionPos[3] = { 0, 0, 0.01 };
	simSetObjectPosition(visionHandle, currentGenome.g_objectHandles[parent], visionPos);
	vector<float> colors;
	colors.resize((resX * resY));
	for (int i = 0; i < colors.size(); i++) {
	colors[i] = 0.7;
	//	colors[i - 4] = 0.8;
	//	colors[i - 2] = 0.8;
	}
	float colorSS[12] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
	simSetVisionSensorImage(visionHandle, colorSS);
	float *colors2;
	colors2 = simGetVisionSensorImage(visionHandle);
	simSetVisionSensorImage(visionHandle, colors2);
	cout << "Colors: " << colors2[0] << endl;
	for (int i = 0; i < colors.size(); i++) {
	cout << colors[i] << ", ";
	cout << colors2[i] << endl;
	}
	unsigned char colorScheme[12];
	int resolutionX = 2;

	simSaveImage(colorScheme, sensorResolution, 0, "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\Hansen", 0, NULL);
	simHandleVisionSensor(visionHandle, NULL, NULL);
	amountVisionSensors++;
	visionHandles.resize(amountVisionSensors);
	visionHandles[amountVisionSensors - 1] = visionHandle;

	*/
	//	int leafHandle = currentGenome.g_leafHandles[partNum];
	//	return leafHandle;
}

int CENN::createArtificialLeaf(float length, float width, float height, float x, float y, float z,
	float rotX, float rotY, float rotZ, float mass, int parent, int partNum)
{
	int leafHandle;
	float objPar[3] = { length, width, height };
	mass = length * width * height;
	leafHandle = simCreatePureShape(0, 16, objPar, mass, NULL);

	string leafName = "Leaf" + to_string(leafHandle);
	cout << "leafName = " << leafName << endl;
	simSetObjectName(leafHandle, leafName.c_str());
	cout << "handleName after setting name: " << simGetObjectName(leafHandle) << endl;
	
	simSetObjectParent(leafHandle, currentGenome.g_objectHandles[parent], true);

	float objPos[3] = { x, y, z };
	float objGotPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[parent], sim_handle_parent, objGotPos);
	cout << "objPar values: " << objGotPos[0] << ", " << objGotPos[1] << ", " << objGotPos[2] << endl;
//	objPos[0] = x;
//	objPos[1] = y;
//	objPos[2] = (objParPos[2]);
	float parentSizeZ[1]; 
	simGetObjectFloatParameter(currentGenome.g_objectHandles[parent], 20, parentSizeZ);
	objPos[2] = parentSizeZ[0];
	simSetObjectPosition(leafHandle, currentGenome.g_objectHandles[parent], objPos);
	float rot[3] = { rotX, rotY, rotZ };
	float zeroRot[3] = { 0, 0, 0 };
	simSetObjectOrientation(leafHandle, currentGenome.g_objectHandles[parent], zeroRot);
//	simSetObjectOrientation(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	float worldPos[3];
	simGetObjectPosition(leafHandle, NULL, worldPos);
	if (worldPos[2] < 0) {
		simRemoveObject(leafHandle);
	}

	// delete the object if it collides
	cout << "leaf 3" << endl;
	bool deletedPart = false;
	for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
		if (i != parent) {
			if (simCheckCollision(leafHandle, currentGenome.g_objectHandles[i]) == true) {
				deletedPart = true;
				break;
			}
		}
	}
	
	if (deletedPart == true) {
		cout << "leaf " << partNum << " collided!" << " leaf " << partNum << " will therefore be deleted" << endl;
		simRemoveObject(leafHandle);
		cout << "leaf removed" << endl;
		return -1;
	}
	else {
		float color[3] = { 1.0, 1.0, 1.0 };
		simSetShapeColor(leafHandle, NULL, sim_colorcomponent_ambient_diffuse, color);
		//	simPauseSimulation();
	}

//	cout << "myvector contains:";
//	for (unsigned i = 0; i < currentGenome.g_leafHandles.size(); i++) {
//		cout << ' ' << currentGenome.g_leafHandles[i];
//		cout << ", " << currentGenome.g_leafSunContact[i];
// }
//	cout << '\n';

	
	//	int leafHandle = currentGenome.g_leafHandles[partNum];
	return leafHandle;

}

// BACKUP
/*void CENN::createArtificialLeaf(float length, float width, float height, ,
	float rotX, float rotY, float rotZ, float mass, int parent, int partNum)
{
	cout << "pushing back leafs" << endl;
	currentGenome.g_leafHandles.push_back(0);
	currentGenome.g_leafSunContact.push_back(false);
	currentGenome.g_leafJointHandles.push_back(0);

	// leaf joint


	cout << "vector size = " << currentGenome.g_leafHandles.size() << " while partnum = " << partNum << endl;
	float objPar[3] = { length, width, height };
	cout << "object Parent = " << parent << endl;
	mass = length * width * height;
	currentGenome.g_leafHandles[partNum] = simCreatePureShape(0, 16, objPar, mass, NULL);
	cout << "handle = " << currentGenome.g_leafHandles[partNum] << endl;
	cout << "object name = " << simGetObjectName(currentGenome.g_leafHandles[partNum]) << endl;

	string leafName = "Leaf" + to_string(currentGenome.g_leafHandles.size());
	leafName = "Leaf" + to_string(currentGenome.g_leafHandles.size());


	//	string jointName = "joint" + to_string(jointNum);
	//	currentGenome.g_jointAmount = jointNum + 1;
	//	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());


	cout << "leafName = " << leafName << endl;
	//	if (simSetObjectName(currentGenome.g_leafHandles[partNum], leafName.c_str()) == -1) {
	//		cout << "neuron name not sent" << endl;
	//	}
	//	if (simSetObjectName(currentGenome.g_leafHandles[partNum], "why not")) {
	//		cout << "neuron name not sent" << endl;
	//	}
	//	simSetObjectName(currentGenome.g_leafHandles[partNum],  "zx99");
	//	cout << "handleName after setting name: " << simGetObjectName(currentGenome.g_leafHandles[partNum]) << endl;
	simSetObjectName(currentGenome.g_leafHandles[partNum], leafName.c_str());
	cout << "handleName after setting name: " << simGetObjectName(currentGenome.g_leafHandles[partNum]) << endl;


	simSetObjectParent(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], true);
	simAddObjectToSelection(sim_handle_self, currentGenome.g_leafHandles[partNum]);
	//	cout << "pure" << endl;
	//	currentGenome.g_leafHandles[partNum] = simGetObjectHandle("Cuboid");
	float objPos[3] = { x, y, z };
	float objParPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[parent], sim_handle_parent, objParPos);
	cout << "objPar values: " << objParPos[0] << ", " << objParPos[1] << ", " << objParPos[2] << endl;

	//simGetObjectSizeValues(currentGenome.g_objectHandles[parent], objParPos);
	objPos[0] = x;
	objPos[1] = y;
	objPos[2] = (objParPos[2]);
	cout << "objPar values: " << objParPos[0] << ", " << objParPos[1] << ", " << objParPos[2] << endl;
	simSetObjectPosition(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], objPos);
	float rot[3] = { rotX, rotY, rotZ };
	simSetObjectOrientation(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	//	simSetObjectIntParameter(currentGenome.g_leafHandles[partNum], 3019, 0);
	cout << "leaf 2" << endl;
	//	float worldPos[3];
	//	simGetObjectPosition(currentGenome.g_leafHandles[partNum], NULL, worldPos);
	//	if (worldPos[2] < 0) {
	//		simRemoveObject(currentGenome.g_leafHandles[partNum]);
	//	}
	//	currentGenome.g_objectType[partNum] = 6;
	//	simSetObjectIntParameter(currentGenome.g_leafHandles[partNum], 3019, 65535);
	//	string jointName = "joint" + to_string(jointNum);
	//	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());

	// delete the object if it collides
	cout << "leaf 3" << endl;
	bool deletedPart = false;
	for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
	if (i != parent) {
	if (simCheckCollision(currentGenome.g_leafHandles[partNum], currentGenome.g_objectHandles[i]) == true) {
	deletedPart = true;
	break;
	}
	}
	}
	if (deletedPart == false){
		for (int i = 0; i < currentGenome.g_leafHandles.size() - 1; i++) {
			if (simCheckCollision(currentGenome.g_leafHandles[partNum], currentGenome.g_leafHandles[i]) == true) {
				deletedPart = true;
				break;
			}
		}
	}
	if (deletedPart == true) {
		cout << "leaf " << partNum << " collided!" << " leaf " << partNum << " will therefore be deleted" << endl;
		simRemoveObject(currentGenome.g_leafHandles[partNum]);
		cout << "leaf removed" << endl;
		if (currentGenome.g_leafHandles.size() - 1 <= 0) { // || currentGenome.g_leafHandles.size() - 1 < 0){
			currentGenome.g_leafHandles.clear();
			currentGenome.g_leafSunContact.clear();
		}
		else {
			currentGenome.g_leafHandles.resize(currentGenome.g_leafHandles.size() - 1);
			currentGenome.g_leafSunContact.resize(currentGenome.g_leafHandles.size());
			//		currentGenome.g_leafSunContact.resize(currentGenome.g_leafHandles.size());
			cout << "leafhandle vs contact size: " << currentGenome.g_leafHandles.size() << ", " << currentGenome.g_leafSunContact.size() << endl;
			assert(currentGenome.g_leafHandles.size() == currentGenome.g_leafSunContact.size());
		}
		cout << "leafHandles resized" << endl;

	}
	else {
		float color[3] = { 0.0, 0.0, 0.0 };
		simSetShapeColor(currentGenome.g_leafHandles[partNum], NULL, sim_colorcomponent_ambient_diffuse, color);
		//	simPauseSimulation();
	}

	cout << "myvector contains:";
	for (unsigned i = 0; i < currentGenome.g_leafHandles.size(); i++) {
		cout << ' ' << currentGenome.g_leafHandles[i];
		cout << ", " << currentGenome.g_leafSunContact[i];
	}
	cout << '\n';

	/*
	currentGenome.g_objectPosition.resize(partNum + 1);
	currentGenome.g_objectPosition[partNum].resize(3);
	currentGenome.g_objectPosition[partNum][0] = x;
	currentGenome.g_objectPosition[partNum][1] = y;
	currentGenome.g_objectPosition[partNum][2] = z;

	currentGenome.g_objectOrientation.resize(partNum + 1);
	currentGenome.g_objectOrientation[partNum].resize(3);
	currentGenome.g_objectOrientation[partNum][0] = rotX;
	currentGenome.g_objectOrientation[partNum][1] = rotY;
	currentGenome.g_objectOrientation[partNum][2] = rotZ;

	currentGenome.g_objectSizes.resize(partNum + 1);
	currentGenome.g_objectSizes[partNum].resize(3);
	currentGenome.g_objectSizes[partNum][0] = length;
	currentGenome.g_objectSizes[partNum][1] = width;
	currentGenome.g_objectSizes[partNum][2] = height;
	cout << "leaf 4" << endl;
	currentGenome.g_objectMass.resize(partNum + 1);
	currentGenome.g_objectMass[partNum] = mass;
	currentGenome.g_objectParent.resize(partNum + 1);
	currentGenome.g_objectParent[partNum] = parent;
	currentGenome.g_objectType.resize(partNum + 1);
	currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];
	simSetObjectParent(currentGenome.g_leafHandles[currentGenome.g_objectAmount - 1], currentGenome.g_objectHandles[parent], TRUE);
	nameObject("leaf", currentGenome.g_objectAmount - 1);
	cout << "leaf 5" << endl;


	// visionSensor
	int resX = 4;
	int resY = 4;
	int sensorResolution[4] = { resX, resY, 0, 0 };
	float sensorFloatArray[11] = { 0, 0, 0, 0.1, 0.1, 0.01, 0.1, 0.1, 0.1, 0.0, 0.0 };
	float sensorColor[3] = { 0.5, 0.5, 1.0 };
	simCreateVisionSensor(2, sensorResolution, sensorFloatArray, sensorColor);
	int visionHandle = 0;
	simGetObjectHandle("Vision_sensor");
	visionHandle = currentGenome.g_leafHandles[partNum] + 1;


	string visionName = "Vision" + to_string(partNum);

	simSetObjectName(visionHandle, visionName.c_str());

	assert(visionHandle != 0);
	cout << "VisionHandle = " << visionHandle << endl;
	simSetObjectParent(visionHandle, currentGenome.g_objectHandles[parent], false);
	float visionPos[3] = { 0, 0, 0.01 };
	simSetObjectPosition(visionHandle, currentGenome.g_objectHandles[parent], visionPos);
	vector<float> colors;
	colors.resize((resX * resY));
	for (int i = 0; i < colors.size(); i++) {
	colors[i] = 0.7;
	//	colors[i - 4] = 0.8;
	//	colors[i - 2] = 0.8;
	}
	float colorSS[12] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
	simSetVisionSensorImage(visionHandle, colorSS);
	float *colors2;
	colors2 = simGetVisionSensorImage(visionHandle);
	simSetVisionSensorImage(visionHandle, colors2);
	cout << "Colors: " << colors2[0] << endl;
	for (int i = 0; i < colors.size(); i++) {
	cout << colors[i] << ", ";
	cout << colors2[i] << endl;
	}
	unsigned char colorScheme[12];
	int resolutionX = 2;

	simSaveImage(colorScheme, sensorResolution, 0, "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\Hansen", 0, NULL);
	simHandleVisionSensor(visionHandle, NULL, NULL);
	amountVisionSensors++;
	visionHandles.resize(amountVisionSensors);
	visionHandles[amountVisionSensors - 1] = visionHandle;

	
	//	int leafHandle = currentGenome.g_leafHandles[partNum];
	//	return leafHandle;
}
*/

int CENN::createVisionSensor(int parentHandle) {
	// visionSensor
	float parentPos[3];	
	float parentRot[3];
	float parentSizeX[1];
	float parentSizeY[1];
	float parentSizeZ[1];

	simGetObjectPosition(parentHandle, sim_handle_parent, parentPos);
	simGetObjectOrientation(parentHandle, sim_handle_parent, parentPos);
	simGetObjectFloatParameter(parentHandle, 20, parentSizeZ);
	simGetObjectFloatParameter(parentHandle, 18, parentSizeX); // can be used to adjust sensor size
	simGetObjectFloatParameter(parentHandle, 19, parentSizeY);

	int resX = 2;
	int resY = 2;
	int sensorResolution[4] = { resX, resY, 0, 0 };
	float sensorFloatArray[11] = { 0, 0, 0, 0.1, 0.1, 0.01, 0.1, 0.1, 0.1, 0.0, 0.0 };
	float sensorColor[3] = { 0.5, 0.5, 1.0 };
	int visionHandle = 0; 
	visionHandle = simCreateVisionSensor(2, sensorResolution, sensorFloatArray, sensorColor);
	
	string visionName = "Vision" + to_string(visionHandle);

	simSetObjectName(visionHandle, visionName.c_str());

	assert(visionHandle != 0);
	cout << "VisionHandle = " << visionHandle << endl;
	simSetObjectParent(visionHandle, parentHandle, false);
	float visionPos[3] = { 0, 0, parentSizeZ[0] };
	simSetObjectPosition(visionHandle, parentHandle, visionPos);
	float visionOr[3] = { 0, 0, 0 };
	simSetObjectOrientation(visionHandle, parentHandle, visionOr);
	vector<float> colors;
	colors.resize((resX * resY));
	for (int i = 0; i < colors.size(); i++) {
		colors[i] = 0.7;
		//	colors[i - 4] = 0.8;
		//	colors[i - 2] = 0.8;
	}
	float colorSS[12] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
	simSetVisionSensorImage(visionHandle, colorSS);
	float *colors2;
	colors2 = simGetVisionSensorImage(visionHandle);
	simSetVisionSensorImage(visionHandle, colors2);
	cout << "Colors: " << colors2[0] << endl;
	for (int i = 0; i < colors.size(); i++) {
		cout << colors[i] << ", ";
		cout << colors2[i] << endl;
	}
	unsigned char colorScheme[12];
	int resolutionX = 2;

	simSaveImage(colorScheme, sensorResolution, 0, "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\Hansen", 0, NULL);
	simHandleVisionSensor(visionHandle, NULL, NULL);
//	visionHandles.push_back(visionHandle);
	return visionHandle;
}

void CENN::createJointAndCube(float length, float width, float height, float x, float y, float z, float rotX, float rotY, float rotZ, float mass, int parent, int partNum, int jointNum) {
	float objPar[3] = { length, width, height };
	float jointSize[3] = { 0.1, 0.02, 0.0 };
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	currentGenome.g_jointAmount = jointNum;
	currentGenome.g_jointHandles.resize(jointNum + 1);
	currentGenome.g_jointHandles[jointNum] = simGetObjectHandle("Revolute_joint");
	simSetJointForce(currentGenome.g_jointHandles[jointNum], 200.0);
	simSetJointTargetVelocity(currentGenome.g_jointHandles[jointNum], 0.0);
//	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	//	simSetJointForce(currentGenome.g_jointHandles[partNum], 2);
	//	simSetJointTargetVelocity(currentGenome.g_jointHandles[partNum], 2);
	//	simSetJointTargetPosition(currentGenome.g_jointHandles[partNum], 0);
	string jointName = "joint" + to_string(jointNum);
	currentGenome.g_jointAmount = jointNum + 1;
	i_jointAmount = jointNum + 1;
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2001, 1);

	float objSize[1];
	simGetObjectFloatParameter(currentGenome.g_objectHandles[parent], 20, objSize);
	float jointPos[3] = { x, y, objSize[0] + (0.1*objSize[0])};

	//char* jointNam = jointName;
	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());
	simSetObjectPosition(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointPos);
	simSetObjectParent(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], TRUE);
	simCreatePureShape(0, 11, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	float objPos[3] = { x, height, y};
	float rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = -0.5 *3.1415;
	rot[1] = 0.0;
	rot[2] = 0.0;

	//	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	float jointRot[3] = {0.0, 0.0, 0.0};
	jointRot[0] = (rotX / 2) + (0.5 * 3.1415);
	jointRot[1] = (rotY / 2);
	jointRot[2] = rotZ / 2; 
//	jointRot[0] = 0.5 * 3.1415;
//	jointRot[1] = 0;
//	jointRot[2] = 0;
//	simGetObjectOrientation(currentGenome.g_objectHandles[parent], NULL, jointRot);
	simSetObjectOrientation(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointRot);
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], objPos);
	
	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_jointHandles[jointNum], TRUE);
	nameObject("part", currentGenome.g_objectAmount - 1);
	/**/
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);


	float worldPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
//		simRemoveObject(currentGenome.g_objectHandles[partNum]); 
//		simRemoveObject(currentGenome.g_jointHandles[jointNum]);
	}

	// obj
//	currentGenome.g_objectAmount = partNum;
//	currentGenome.g_objectHandles.resize(partNum + 1);
	
//	currentGenome.g_objectFriction[partNum]
	// check object collisions:
//	for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
//		for (int j = 0; j < currentGenome.g_objectHandles.size(); j++) {
//			if (i != j && i > j) {
//				if (simCheckCollision(currentGenome.g_objectHandles[i], currentGenome.g_objectHandles[j]) == true) {
//					cout << "object collided!" << endl;
//				}
//			}
//		}
//	}
	bool deletedPart = false;
	for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
		if (i != parent) {
		/*	if (simCheckCollision(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[i]) == true) {
				cout << "object " << i << " collided!" << " object " << partNum << " will therefore be deleted" << endl;
				simRemoveObject(currentGenome.g_jointHandles[jointNum]); 
				simRemoveObject(currentGenome.g_objectHandles[partNum]);
				currentGenome.g_jointHandles.resize(currentGenome.g_jointAmount - 1);
			    currentGenome.g_jointAmount--;
				i_jointAmount = currentGenome.g_jointAmount; 
		//		currentGenome.g_jointAmount--;
				currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount - 1);
				currentGenome.g_objectAmount--;

				currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
		//		currentGenome.g_objectAmount--;
				cout << "jointAmount and objectAmount = " << currentGenome.g_objectAmount << ", " << currentGenome.g_jointAmount << endl;
		//		i_objectAmount--;
		//		i_jointAmount--;
				deletedPart = true; 
				break;
			} */
		} 
	}
	if (deletedPart == true){
	//	cout << "deletedPart = true" << endl;
	}
	if (deletedPart == false) {

	//	s_amountJoints = currentGenome.g_jointAmount;
	//	s_amountObjects = currentGenome.g_objectAmount;
		
//		cout << "previous part did not collide" << endl;
		currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
		currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
		currentGenome.g_jointPosition.resize(jointNum + 1);
		currentGenome.g_jointPosition[jointNum].resize(3);
		currentGenome.g_jointOrientation.resize(jointNum + 1);
		currentGenome.g_jointOrientation[jointNum].resize(3);
		currentGenome.g_jointSizes.resize(jointNum + 1);
		currentGenome.g_jointSizes[jointNum].resize(3);

		currentGenome.g_jointPosition[jointNum][0] = jointPos[0];
		currentGenome.g_jointPosition[jointNum][1] = jointPos[1];
		currentGenome.g_jointPosition[jointNum][2] = jointPos[2];

		currentGenome.g_jointOrientation[jointNum][0] = rotX / 2;
		currentGenome.g_jointOrientation[jointNum][1] = rotY / 2;
		currentGenome.g_jointOrientation[jointNum][2] = rotY / 2;

		currentGenome.g_jointSizes[jointNum][0] = jointSize[0];
		currentGenome.g_jointSizes[jointNum][1] = jointSize[1];
		currentGenome.g_jointSizes[jointNum][2] = jointSize[2];

		currentGenome.g_jointMass.resize(jointNum + 1);
		currentGenome.g_jointMass[jointNum] = mass;
		currentGenome.g_jointParent.resize(jointNum + 1);
		currentGenome.g_jointParent[jointNum] = parent;
		currentGenome.g_jointType.resize(jointNum + 1);
		currentGenome.g_jointType[jointNum] = 1;
		currentGenome.g_jointMaxMinAngles.resize(jointNum + 1);
		currentGenome.g_jointMaxMinAngles[jointNum].resize(2);
		currentGenome.g_jointMaxMinAngles[jointNum][0] = -0.5;
		currentGenome.g_jointMaxMinAngles[jointNum][0] = 0.5;
		currentGenome.g_jointMaxForce.resize(jointNum + 1);
		currentGenome.g_jointMaxForce[jointNum] = 0.1;
		currentGenome.g_jointMaxVelocity.resize(jointNum + 1);
		currentGenome.g_jointMaxVelocity[jointNum] = 0.1;

		currentGenome.g_objectPosition.resize(partNum + 1);
		currentGenome.g_objectPosition[partNum].resize(3);
		currentGenome.g_objectPosition[partNum][0] = x;
		currentGenome.g_objectPosition[partNum][1] = y;
		currentGenome.g_objectPosition[partNum][2] = z;

		currentGenome.g_objectOrientation.resize(partNum + 1);
		currentGenome.g_objectOrientation[partNum].resize(3);
		currentGenome.g_objectOrientation[partNum][0] = rotX;
		currentGenome.g_objectOrientation[partNum][1] = rotY;
		currentGenome.g_objectOrientation[partNum][2] = rotZ;

		currentGenome.g_objectSizes.resize(partNum + 1);
		currentGenome.g_objectSizes[partNum].resize(3);
		currentGenome.g_objectSizes[partNum][0] = length;
		currentGenome.g_objectSizes[partNum][1] = width;
		currentGenome.g_objectSizes[partNum][2] = height;

		currentGenome.g_objectMass.resize(partNum + 1);
		currentGenome.g_objectMass[partNum] = mass;
		currentGenome.g_objectParent.resize(partNum + 1);
		currentGenome.g_objectParent[partNum] = jointNum;
		currentGenome.g_objectType.resize(partNum + 1);
		currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];

		currentGenome.g_senseInput.resize(jointNum + 1);
		currentGenome.g_senseInput[jointNum] = 0;
		currentGenome.g_jointActivity.resize(jointNum + 1);
		currentGenome.g_jointActivity[jointNum] = 0;
	}
}


void CENN::createJointAndCubeST(float length, float width, float height, float x, float y, float z, float rotX, float rotY, float rotZ, float mass, int parent, int partNum, int jointNum) {
	float objPar[3] = { length, width, height };
	//	cout << "object Parent = " << parent << endl;t
	float jointSize[3] = { 0.1, 0.02, 0.0 };
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	currentGenome.g_jointAmount = jointNum;
	currentGenome.g_jointHandles.resize(jointNum + 1);
	currentGenome.g_jointHandles[jointNum] = simGetObjectHandle("Revolute_joint");
	simSetJointForce(currentGenome.g_jointHandles[jointNum], 0);
	simSetJointTargetVelocity(currentGenome.g_jointHandles[jointNum], 0.0);
	string jointName = "joint" + to_string(jointNum);
	currentGenome.g_jointAmount = jointNum + 1;
	i_jointAmount = jointNum + 1;
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2001, 1);

	float objSize[1];
	simGetObjectFloatParameter(currentGenome.g_objectHandles[parent], 20, objSize);
	float jointPos[3] = { x, y, 0.0/*objSize[0] /*+ (0.1*objSize[0])*/ };
	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());
	simSetObjectPosition(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointPos);
	simSetObjectParent(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], TRUE);

	float blue[3] = { 0, 0, 1 };
	float red[3] = { 1, 0, 0 };
	simSetShapeColor(currentGenome.g_objectHandles[partNum], NULL, sim_colorcomponent_ambient_diffuse, red);
	simSetShapeColor(currentGenome.g_objectHandles[parent], NULL, sim_colorcomponent_ambient_diffuse, blue);

	assert(currentGenome.g_objectAmount == currentGenome.g_objectHandles.size());
	assert(partNum == currentGenome.g_objectAmount - 1);
	simCreatePureShape(0, 16, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	nameObject("part", currentGenome.g_objectAmount - 1);
	float objPos[3] = { 0, 0, objSize[0] };
	float rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = rotX / 2;
	rot[1] = rotY / 2;
	rot[2] = rotZ / 2;
	float jointRot[3] = { 0.0, 0.0, 0.0 };
	jointRot[0] = rotX / 4;
	jointRot[1] = rotY / 4;
	jointRot[2] = rotZ / 4;
	simSetObjectOrientation(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointRot);
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], objPos);
	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_jointHandles[jointNum], TRUE);
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);


	float worldPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
		//		simRemoveObject(currentGenome.g_objectHandles[partNum]); 
		//		simRemoveObject(currentGenome.g_jointHandles[jointNum]);
	}

	bool deletedPart = false;
	int tempObjAmount = currentGenome.g_objectAmount;
	for (int i = 0; i < tempObjAmount; i++) {
		if (i != parent) {
			if (simCheckCollision(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[i]) == true) {
				simRemoveObject(currentGenome.g_jointHandles[jointNum]);
				simRemoveObject(currentGenome.g_objectHandles[partNum]);
				currentGenome.g_jointHandles.resize(currentGenome.g_jointAmount - 1);
				currentGenome.g_jointAmount--;
				i_jointAmount = currentGenome.g_jointAmount;
				currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount - 1);
				currentGenome.g_objectAmount--;
				currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
				deletedPart = true;
				break;
			} /**/
		}
	}
	if (deletedPart == true){
	}
	if (deletedPart == false) {
		currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
		currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
		currentGenome.g_jointPosition.resize(jointNum + 1);
		currentGenome.g_jointPosition[jointNum].resize(3);
		currentGenome.g_jointOrientation.resize(jointNum + 1);
		currentGenome.g_jointOrientation[jointNum].resize(3);
		currentGenome.g_jointSizes.resize(jointNum + 1);
		currentGenome.g_jointSizes[jointNum].resize(3);

		currentGenome.g_jointPosition[jointNum][0] = jointPos[0];
		currentGenome.g_jointPosition[jointNum][1] = jointPos[1];
		currentGenome.g_jointPosition[jointNum][2] = jointPos[2];

		currentGenome.g_jointOrientation[jointNum][0] = rotX / 2;
		currentGenome.g_jointOrientation[jointNum][1] = rotY / 2;
		currentGenome.g_jointOrientation[jointNum][2] = rotY / 2;

		currentGenome.g_jointSizes[jointNum][0] = jointSize[0];
		currentGenome.g_jointSizes[jointNum][1] = jointSize[1];
		currentGenome.g_jointSizes[jointNum][2] = jointSize[2];

		currentGenome.g_jointMass.resize(jointNum + 1);
		currentGenome.g_jointMass[jointNum] = mass;
		currentGenome.g_jointParent.resize(jointNum + 1);
		currentGenome.g_jointParent[jointNum] = parent;
		currentGenome.g_jointType.resize(jointNum + 1);
		currentGenome.g_jointType[jointNum] = 1;
		currentGenome.g_jointMaxMinAngles.resize(jointNum + 1);
		currentGenome.g_jointMaxMinAngles[jointNum].resize(2);
		currentGenome.g_jointMaxMinAngles[jointNum][0] = -0.5;
		currentGenome.g_jointMaxMinAngles[jointNum][0] = 0.5;
		currentGenome.g_jointMaxForce.resize(jointNum + 1);
		currentGenome.g_jointMaxForce[jointNum] = 0.1;
		currentGenome.g_jointMaxVelocity.resize(jointNum + 1);
		currentGenome.g_jointMaxVelocity[jointNum] = 0.1;

		currentGenome.g_objectPosition.resize(partNum + 1);
		currentGenome.g_objectPosition[partNum].resize(3);
		currentGenome.g_objectPosition[partNum][0] = x;
		currentGenome.g_objectPosition[partNum][1] = y;
		currentGenome.g_objectPosition[partNum][2] = z;

		currentGenome.g_objectOrientation.resize(partNum + 1);
		currentGenome.g_objectOrientation[partNum].resize(3);
		currentGenome.g_objectOrientation[partNum][0] = rotX;
		currentGenome.g_objectOrientation[partNum][1] = rotY;
		currentGenome.g_objectOrientation[partNum][2] = rotZ;

		currentGenome.g_objectSizes.resize(partNum + 1);
		currentGenome.g_objectSizes[partNum].resize(3);
		currentGenome.g_objectSizes[partNum][0] = length;
		currentGenome.g_objectSizes[partNum][1] = width;
		currentGenome.g_objectSizes[partNum][2] = height;

		currentGenome.g_objectMass.resize(partNum + 1);
		currentGenome.g_objectMass[partNum] = mass;
		currentGenome.g_objectParent.resize(partNum + 1);
		currentGenome.g_objectParent[partNum] = jointNum;
		currentGenome.g_objectType.resize(partNum + 1);
		currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];

		currentGenome.g_senseInput.resize(jointNum + 1);
		currentGenome.g_senseInput[jointNum] = 0;
		currentGenome.g_jointActivity.resize(jointNum + 1);
		currentGenome.g_jointActivity[jointNum] = 0;
	}
}


void CENN::createJointAndCubeNew(float length, float width, float height, float x, float y, float z, float rotX, float rotY, float rotZ, float mass, int parent, int partNum, int jointNum) {
	float objPar[3] = { length, width, height };
	//	cout << "object Parent = " << parent << endl;t
	float jointSize[3] = { 0.1, 0.02, 0.0 };
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	currentGenome.g_jointAmount = jointNum;
	currentGenome.g_jointHandles.resize(jointNum + 1);
	currentGenome.g_jointHandles[jointNum] = simGetObjectHandle("Revolute_joint");
	simSetJointForce(currentGenome.g_jointHandles[jointNum], 0);
	simSetJointTargetVelocity(currentGenome.g_jointHandles[jointNum], 0.0);
	string jointName = "joint" + to_string(jointNum);
	currentGenome.g_jointAmount = jointNum + 1;
	i_jointAmount = jointNum + 1;
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2001, 1);

	float objSize[1];
	simGetObjectFloatParameter(currentGenome.g_objectHandles[parent], 20, objSize);
	float jointPos[3] = { x, y, objSize[0] /*+ (0.1*objSize[0])*/ };
	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());
	simSetObjectPosition(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointPos);
	simSetObjectParent(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], TRUE);

	float blue[3] = { 0, 0, 1 };
	float red[3] = { 1, 0, 0 };
	simSetShapeColor(currentGenome.g_objectHandles[partNum], NULL, sim_colorcomponent_ambient_diffuse, red);
	simSetShapeColor(currentGenome.g_objectHandles[parent], NULL, sim_colorcomponent_ambient_diffuse, blue);

	assert(currentGenome.g_objectAmount == currentGenome.g_objectHandles.size());
	assert(partNum == currentGenome.g_objectAmount - 1);
	simCreatePureShape(0, 11, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	nameObject("part", currentGenome.g_objectAmount - 1);
	float objPos[3] = { 0, 0, objSize[0] };
	float rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = rotX / 2;
	rot[1] = rotY / 2;
	rot[2] = rotZ / 2;
	float jointRot[3] = { 0.0, 0.0, 0.0 };
	jointRot[0] = rotX / 4;
	jointRot[1] = rotY / 4;
	jointRot[2] = rotZ / 4;
	simSetObjectOrientation(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointRot);
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], objPos);
	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_jointHandles[jointNum], TRUE);
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);


	float worldPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
		//		simRemoveObject(currentGenome.g_objectHandles[partNum]); 
		//		simRemoveObject(currentGenome.g_jointHandles[jointNum]);
	}

	bool deletedPart = false;
	int tempObjAmount = currentGenome.g_objectAmount;
	for (int i = 0; i < tempObjAmount; i++) {
		if (i != parent) {
			if (simCheckCollision(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[i]) == true) {
				simRemoveObject(currentGenome.g_jointHandles[jointNum]);
				simRemoveObject(currentGenome.g_objectHandles[partNum]);
				currentGenome.g_jointHandles.resize(currentGenome.g_jointAmount - 1);
				currentGenome.g_jointAmount--;
				i_jointAmount = currentGenome.g_jointAmount;
				currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount - 1);
				currentGenome.g_objectAmount--;
				currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
				deletedPart = true;
				break;
			} /**/
		}
	}
	if (deletedPart == true){
	}
	if (deletedPart == false) {
		currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
		currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
		currentGenome.g_jointPosition.resize(jointNum + 1);
		currentGenome.g_jointPosition[jointNum].resize(3);
		currentGenome.g_jointOrientation.resize(jointNum + 1);
		currentGenome.g_jointOrientation[jointNum].resize(3);
		currentGenome.g_jointSizes.resize(jointNum + 1);
		currentGenome.g_jointSizes[jointNum].resize(3);

		currentGenome.g_jointPosition[jointNum][0] = jointPos[0];
		currentGenome.g_jointPosition[jointNum][1] = jointPos[1];
		currentGenome.g_jointPosition[jointNum][2] = jointPos[2];

		currentGenome.g_jointOrientation[jointNum][0] = rotX / 2;
		currentGenome.g_jointOrientation[jointNum][1] = rotY / 2;
		currentGenome.g_jointOrientation[jointNum][2] = rotY / 2;

		currentGenome.g_jointSizes[jointNum][0] = jointSize[0];
		currentGenome.g_jointSizes[jointNum][1] = jointSize[1];
		currentGenome.g_jointSizes[jointNum][2] = jointSize[2];

		currentGenome.g_jointMass.resize(jointNum + 1);
		currentGenome.g_jointMass[jointNum] = mass;
		currentGenome.g_jointParent.resize(jointNum + 1);
		currentGenome.g_jointParent[jointNum] = parent;
		currentGenome.g_jointType.resize(jointNum + 1);
		currentGenome.g_jointType[jointNum] = 1;
		currentGenome.g_jointMaxMinAngles.resize(jointNum + 1);
		currentGenome.g_jointMaxMinAngles[jointNum].resize(2);
		currentGenome.g_jointMaxMinAngles[jointNum][0] = -0.5;
		currentGenome.g_jointMaxMinAngles[jointNum][0] = 0.5;
		currentGenome.g_jointMaxForce.resize(jointNum + 1);
		currentGenome.g_jointMaxForce[jointNum] = 0.1;
		currentGenome.g_jointMaxVelocity.resize(jointNum + 1);
		currentGenome.g_jointMaxVelocity[jointNum] = 0.1;

		currentGenome.g_objectPosition.resize(partNum + 1);
		currentGenome.g_objectPosition[partNum].resize(3);
		currentGenome.g_objectPosition[partNum][0] = x;
		currentGenome.g_objectPosition[partNum][1] = y;
		currentGenome.g_objectPosition[partNum][2] = z;

		currentGenome.g_objectOrientation.resize(partNum + 1);
		currentGenome.g_objectOrientation[partNum].resize(3);
		currentGenome.g_objectOrientation[partNum][0] = rotX;
		currentGenome.g_objectOrientation[partNum][1] = rotY;
		currentGenome.g_objectOrientation[partNum][2] = rotZ;

		currentGenome.g_objectSizes.resize(partNum + 1);
		currentGenome.g_objectSizes[partNum].resize(3);
		currentGenome.g_objectSizes[partNum][0] = length;
		currentGenome.g_objectSizes[partNum][1] = width;
		currentGenome.g_objectSizes[partNum][2] = height;

		currentGenome.g_objectMass.resize(partNum + 1);
		currentGenome.g_objectMass[partNum] = mass;
		currentGenome.g_objectParent.resize(partNum + 1);
		currentGenome.g_objectParent[partNum] = jointNum;
		currentGenome.g_objectType.resize(partNum + 1);
		currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];

		currentGenome.g_senseInput.resize(jointNum + 1);
		currentGenome.g_senseInput[jointNum] = 0;
		currentGenome.g_jointActivity.resize(jointNum + 1);
		currentGenome.g_jointActivity[jointNum] = 0;
	}
}


// function not working
void CENN::createJointAndCubeInLeafHandles(float length, float width, float height, float x, float y, float z, float rotX, float rotY, float rotZ, float mass, int parent, int partNum, int jointNum) {
	float objPar[3] = { length, width, height };
	//	cout << "object Parent = " << parent << endl;t
	float jointSize[3] = { 0.1, 0.02, 0.0 };
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	currentGenome.g_jointAmount = jointNum;
	currentGenome.g_jointHandles.resize(jointNum + 1);
	currentGenome.g_jointHandles[jointNum] = simGetObjectHandle("Revolute_joint");
	simSetJointForce(currentGenome.g_jointHandles[jointNum], 0);
	//	simSetObjectFloatParameter(currentGenome.g_jointHandles[jointNum], 2018, 0.01);
	//	simSetObjectFloatParameter(currentGenome.g_jointHandles[jointNum], 2019, 2000);
	simSetJointTargetVelocity(currentGenome.g_jointHandles[jointNum], 0.0);
	//	simSetJointInterval()
	//	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	//	simSetJointForce(currentGenome.g_jointHandles[partNum], 2);
	//	simSetJointTargetVelocity(currentGenome.g_jointHandles[partNum], 2);
	//	simSetJointTargetPosition(currentGenome.g_jointHandles[partNum], 0);
	string jointName = "leaf joint" + to_string(jointNum);
	currentGenome.g_jointAmount = jointNum + 1;
	i_jointAmount = jointNum + 1;
	//	cout << "passed joint creation" << endl;

	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2001, 1);

	float objSize[1];
	simGetObjectFloatParameter(currentGenome.g_objectHandles[parent], 20, objSize);
	//	cout << "objSize = " << objSize[0] << endl;

	float jointPos[3] = { x, y, objSize[0] /*+ (0.1*objSize[0])*/ };

	//char* jointNam = jointName;
	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());
	simSetObjectPosition(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointPos);
	simSetObjectParent(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], TRUE);

	float blue[3] = { 0, 0, 1 };
	float red[3] = { 1, 0, 0 };
	simSetShapeColor(currentGenome.g_objectHandles[partNum], NULL, sim_colorcomponent_ambient_diffuse, red);
	simSetShapeColor(currentGenome.g_objectHandles[parent], NULL, sim_colorcomponent_ambient_diffuse, blue);

	objPar[0] = 0.1;
	objPar[0] = 0.1;
	objPar[0] = 0.01;

	assert(currentGenome.g_objectAmount == currentGenome.g_objectHandles.size());
	assert(partNum == currentGenome.g_objectAmount - 1);
	simCreatePureShape(0, 11, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	currentGenome.g_leafHandles.push_back(currentGenome.g_objectHandles[partNum]);
	nameObject("part", currentGenome.g_objectAmount - 1);
	float objPos[3] = { 0, 0, objSize[0] };
	float rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = rotX / 2;
	rot[1] = rotY / 2;
	rot[2] = rotZ / 2;

	//	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	float jointRot[3] = { 0.0, 0.0, 0.0 };
	jointRot[0] = rotX / 4;
	jointRot[1] = rotY / 4;
	jointRot[2] = rotZ / 4;
	//	jointRot[0] = 0.5 * 3.1415;
	//	jointRot[1] = 0;
	//	jointRot[2] = 0;
	//	simGetObjectOrientation(currentGenome.g_objectHandles[parent], NULL, jointRot);
	simSetObjectOrientation(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[parent], jointRot);
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], objPos);

	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_jointHandles[jointNum], TRUE);

	/**/
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);


	float worldPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
		//		simRemoveObject(currentGenome.g_objectHandles[partNum]); 
		//		simRemoveObject(currentGenome.g_jointHandles[jointNum]);
	}

	// obj
	//	currentGenome.g_objectAmount = partNum;
	//	currentGenome.g_objectHandles.resize(partNum + 1);

	//	currentGenome.g_objectFriction[partNum]
	// check object collisions:
	//	for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
	//		for (int j = 0; j < currentGenome.g_objectHandles.size(); j++) {
	//			if (i != j && i > j) {
	//				if (simCheckCollision(currentGenome.g_objectHandles[i], currentGenome.g_objectHandles[j]) == true) {
	//					cout << "object collided!" << endl;
	//				}
	//			}
	//		}
	//	}
	bool deletedPart = false;
	int tempObjAmount = currentGenome.g_objectAmount;
	for (int i = 0; i < tempObjAmount; i++) {
		if (i != parent) {
			if (simCheckCollision(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[i]) == true) {
				// cout << "object " << i << " collided!" << " object " << partNum << " will therefore be deleted" << endl;
				simRemoveObject(currentGenome.g_jointHandles[jointNum]);
				simRemoveObject(currentGenome.g_objectHandles[partNum]);
				currentGenome.g_jointHandles.resize(currentGenome.g_jointAmount - 1);
				currentGenome.g_jointAmount--;
				i_jointAmount = currentGenome.g_jointAmount;
				//		currentGenome.g_jointAmount--;
				currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount - 1);
				currentGenome.g_objectAmount--;

				currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
				//		currentGenome.g_objectAmount--;
				//	cout << "jointAmount and objectAmount = " << currentGenome.g_objectAmount << ", " << currentGenome.g_jointAmount << endl;
				//		i_objectAmount--;
				//		i_jointAmount--;
				deletedPart = true;
				break;
			} /**/
		}
	}
	if (deletedPart == true){
		// cout << "deletedPart = true" << endl;
	}
	if (deletedPart == false) {

		//	s_amountJoints = currentGenome.g_jointAmount;
		//	s_amountObjects = currentGenome.g_objectAmount;

		//		cout << "previous part did not collide" << endl;
		currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
		currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
		currentGenome.g_jointPosition.resize(jointNum + 1);
		currentGenome.g_jointPosition[jointNum].resize(3);
		currentGenome.g_jointOrientation.resize(jointNum + 1);
		currentGenome.g_jointOrientation[jointNum].resize(3);
		currentGenome.g_jointSizes.resize(jointNum + 1);
		currentGenome.g_jointSizes[jointNum].resize(3);

		currentGenome.g_jointPosition[jointNum][0] = jointPos[0];
		currentGenome.g_jointPosition[jointNum][1] = jointPos[1];
		currentGenome.g_jointPosition[jointNum][2] = jointPos[2];

		currentGenome.g_jointOrientation[jointNum][0] = rotX / 2;
		currentGenome.g_jointOrientation[jointNum][1] = rotY / 2;
		currentGenome.g_jointOrientation[jointNum][2] = rotY / 2;

		currentGenome.g_jointSizes[jointNum][0] = jointSize[0];
		currentGenome.g_jointSizes[jointNum][1] = jointSize[1];
		currentGenome.g_jointSizes[jointNum][2] = jointSize[2];

		currentGenome.g_jointMass.resize(jointNum + 1);
		currentGenome.g_jointMass[jointNum] = mass;
		currentGenome.g_jointParent.resize(jointNum + 1);
		currentGenome.g_jointParent[jointNum] = parent;
		currentGenome.g_jointType.resize(jointNum + 1);
		currentGenome.g_jointType[jointNum] = 1;
		currentGenome.g_jointMaxMinAngles.resize(jointNum + 1);
		currentGenome.g_jointMaxMinAngles[jointNum].resize(2);
		currentGenome.g_jointMaxMinAngles[jointNum][0] = -0.5;
		currentGenome.g_jointMaxMinAngles[jointNum][0] = 0.5;
		currentGenome.g_jointMaxForce.resize(jointNum + 1);
		currentGenome.g_jointMaxForce[jointNum] = 0.1;
		currentGenome.g_jointMaxVelocity.resize(jointNum + 1);
		currentGenome.g_jointMaxVelocity[jointNum] = 0.1;

		currentGenome.g_objectPosition.resize(partNum + 1);
		currentGenome.g_objectPosition[partNum].resize(3);
		currentGenome.g_objectPosition[partNum][0] = x;
		currentGenome.g_objectPosition[partNum][1] = y;
		currentGenome.g_objectPosition[partNum][2] = z;

		currentGenome.g_objectOrientation.resize(partNum + 1);
		currentGenome.g_objectOrientation[partNum].resize(3);
		currentGenome.g_objectOrientation[partNum][0] = rotX;
		currentGenome.g_objectOrientation[partNum][1] = rotY;
		currentGenome.g_objectOrientation[partNum][2] = rotZ;

		currentGenome.g_objectSizes.resize(partNum + 1);
		currentGenome.g_objectSizes[partNum].resize(3);
		currentGenome.g_objectSizes[partNum][0] = length;
		currentGenome.g_objectSizes[partNum][1] = width;
		currentGenome.g_objectSizes[partNum][2] = height;

		currentGenome.g_objectMass.resize(partNum + 1);
		currentGenome.g_objectMass[partNum] = mass;
		currentGenome.g_objectParent.resize(partNum + 1);
		currentGenome.g_objectParent[partNum] = jointNum;
		currentGenome.g_objectType.resize(partNum + 1);
		currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];

		currentGenome.g_senseInput.resize(jointNum + 1);
		currentGenome.g_senseInput[jointNum] = 0;
		currentGenome.g_jointActivity.resize(jointNum + 1);
		currentGenome.g_jointActivity[jointNum] = 0;
	}
}


void CENN::createCubeModule(float length, float width, float height, float x, float y, float z, float rotX, float rotY, float rotZ, float mass, int parent, int partNum, int jointNum) {
	float objPar[3] = { 0.1, 0.1, 0.1 };
	currentGenome.g_objectAmount++;
	currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
	simCreatePureShape(0, 16, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	float rot[3] = { rotX, rotY, rotZ };
	rot[0] = rotX / 2;
	rot[1] = rotY / 2;
	rot[2] = rotZ / 2;
	float objPos[3] = { 0, 0, 0.1 };
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], objPos);

	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_objectHandles[parent], TRUE);
	nameObject("part", currentGenome.g_objectAmount - 1);
	/**/
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);


	float worldPos[3];
	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
		//		simRemoveObject(currentGenome.g_objectHandles[partNum]); 
		//		simRemoveObject(currentGenome.g_jointHandles[jointNum]);
	}
	
	partNum++;

//	float objPar[3] = { length, width, height };
	//	cout << "object Parent = " << parent << endl;
	float jointSize[3] = { 0.1, 0.02, 0.0 };
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	currentGenome.g_jointAmount = jointNum;
	currentGenome.g_jointHandles.resize(jointNum + 1);
	currentGenome.g_jointHandles[jointNum] = simGetObjectHandle("Revolute_joint");
	simSetJointForce(currentGenome.g_jointHandles[jointNum], 0);
	//	simSetObjectFloatParameter(currentGenome.g_jointHandles[jointNum], 2018, 0.01);
	//	simSetObjectFloatParameter(currentGenome.g_jointHandles[jointNum], 2019, 2000);
	simSetJointTargetVelocity(currentGenome.g_jointHandles[jointNum], 0.0);
	//	simSetJointInterval()
	//	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	//	simSetJointForce(currentGenome.g_jointHandles[partNum], 2);
	//	simSetJointTargetVelocity(currentGenome.g_jointHandles[partNum], 2);
	//	simSetJointTargetPosition(currentGenome.g_jointHandles[partNum], 0);
	string jointName = "joint" + to_string(jointNum);
	currentGenome.g_jointAmount = jointNum + 1;
	i_jointAmount = jointNum + 1;
		cout << "passed joint creation" << endl;

	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2000, 1);
	simSetObjectIntParameter(currentGenome.g_jointHandles[jointNum], 2001, 1);

	float objSize[1];
	simGetObjectFloatParameter(currentGenome.g_objectHandles[partNum-1], 20, objSize);
		cout << "objSize = " << objSize[0] << endl;

	float jointPos[3] = { x, y, objSize[0] + 0.02 /*+ (0.1*objSize[0])*/ };

	//char* jointNam = jointName;
	simSetObjectName(currentGenome.g_jointHandles[jointNum], jointName.c_str());
	simSetObjectPosition(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[partNum-1], jointPos);
	simSetObjectParent(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[partNum-1], TRUE);
	currentGenome.g_objectAmount++;
	currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);




	simCreatePureShape(0, 16, objPar, mass, NULL);
	currentGenome.g_objectHandles[partNum] = simGetObjectHandle("Cuboid");
	objPos[0] = 0;
	objPos[1] = 0;
	objPos[2] = objSize[0] + 0.02;
//	rot[3] = { rotX, rotY - (0.5 *3.1415), rotZ };
	rot[0] = 0; 
	rot[1] = 0; 
	rot[2] = 0; 
	//	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], rot);
	float jointRot[3] = { 0.0, 0.0, 0.0 };
//	jointRot[0] = rotX / 4;
//	jointRot[1] = rotY / 4;
//	jointRot[2] = rotZ / 4;
	//	jointRot[0] = 0.5 * 3.1415;
	//	jointRot[1] = 0;
	//	jointRot[2] = 0;
	//	simGetObjectOrientation(currentGenome.g_objectHandles[parent], NULL, jointRot);
	simSetObjectOrientation(currentGenome.g_jointHandles[jointNum], currentGenome.g_objectHandles[partNum-1], jointRot);
	simSetObjectOrientation(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], rot);
	simSetObjectPosition(currentGenome.g_objectHandles[partNum], currentGenome.g_jointHandles[jointNum], objPos);

	simSetObjectParent(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], currentGenome.g_jointHandles[jointNum], TRUE);
	nameObject("part", currentGenome.g_objectAmount - 1);
	/**/
	simSetObjectIntParameter(currentGenome.g_objectHandles[partNum], 3019, 65280);

	simGetObjectPosition(currentGenome.g_objectHandles[partNum], NULL, worldPos);
	if (worldPos[2] < 0) {
		//		simRemoveObject(currentGenome.g_objectHandles[partNum]); 
		//		simRemoveObject(currentGenome.g_jointHandles[jointNum]);
	}

	// obj
	//	currentGenome.g_objectAmount = partNum;
	//	currentGenome.g_objectHandles.resize(partNum + 1);

	//	currentGenome.g_objectFriction[partNum]
	// check object collisions:
	//	for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {
	//		for (int j = 0; j < currentGenome.g_objectHandles.size(); j++) {
	//			if (i != j && i > j) {
	//				if (simCheckCollision(currentGenome.g_objectHandles[i], currentGenome.g_objectHandles[j]) == true) {
	//					cout << "object collided!" << endl;
	//				}
	//			}
	//		}
	//	}
	
	bool deletedPart = false;
	cout << "Checking Collision" << endl; 
	assert(currentGenome.g_objectHandles.size() == partNum + 1);
	for (int i = 0; i < parent;  i++) {
		if (i != parent) {
			assert(i < currentGenome.g_objectHandles.size());
		//	cout << "This is a handle: " << currentGenome.g_objectHandles[i] << endl;
			if ((simCheckCollision(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[i]) == true) || 
				(simCheckCollision(currentGenome.g_objectHandles[partNum-1], currentGenome.g_objectHandles[i]) == true)) {
				cout << "object " << i << " collided!" << " object " << partNum << " will therefore be deleted" << endl;
				simRemoveObject(currentGenome.g_jointHandles[jointNum]);
				simRemoveObject(currentGenome.g_objectHandles[partNum]);
				simRemoveObject(currentGenome.g_objectHandles[partNum - 1]);
				currentGenome.g_jointHandles.resize(currentGenome.g_jointAmount - 1);
				currentGenome.g_jointAmount--;
				i_jointAmount = currentGenome.g_jointAmount;
				//		currentGenome.g_jointAmount--;
				currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount - 2);
				currentGenome.g_objectAmount--;
				currentGenome.g_objectAmount--;

				currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
				//		currentGenome.g_objectAmount--;
				//	cout << "jointAmount and objectAmount = " << currentGenome.g_objectAmount << ", " << currentGenome.g_jointAmount << endl;
				//		i_objectAmount--;
				//		i_jointAmount--;
				deletedPart = true;
				break;
			} /**/
		}
	}

	if (deletedPart == true){
//		cout << "This: " << currentGenome.g_objectHandles[partNum] << " should not exist" << endl;

		// cout << "deletedPart = true" << endl;
	}
	if (deletedPart == false) {

		//	s_amountJoints = currentGenome.g_jointAmount;
		//	s_amountObjects = currentGenome.g_objectAmount;
		cout << "save joint and cubes" << endl;

		//		cout << "previous part did not collide" << endl;
		currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
		currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
		currentGenome.g_jointPosition.resize(jointNum + 1);
		currentGenome.g_jointPosition[jointNum].resize(3);
		currentGenome.g_jointOrientation.resize(jointNum + 1);
		currentGenome.g_jointOrientation[jointNum].resize(3);
		currentGenome.g_jointSizes.resize(jointNum + 1);
		currentGenome.g_jointSizes[jointNum].resize(3);

		currentGenome.g_jointPosition[jointNum][0] = jointPos[0];
		currentGenome.g_jointPosition[jointNum][1] = jointPos[1];
		currentGenome.g_jointPosition[jointNum][2] = jointPos[2];

		currentGenome.g_jointOrientation[jointNum][0] = rotX / 2;
		currentGenome.g_jointOrientation[jointNum][1] = rotY / 2;
		currentGenome.g_jointOrientation[jointNum][2] = rotY / 2;

		currentGenome.g_jointSizes[jointNum][0] = jointSize[0];
		currentGenome.g_jointSizes[jointNum][1] = jointSize[1];
		currentGenome.g_jointSizes[jointNum][2] = jointSize[2];

		currentGenome.g_jointMass.resize(jointNum + 1);
		currentGenome.g_jointMass[jointNum] = mass;
		currentGenome.g_jointParent.resize(jointNum + 1);
		currentGenome.g_jointParent[jointNum] = parent;
		currentGenome.g_jointType.resize(jointNum + 1);
		currentGenome.g_jointType[jointNum] = 1;
		currentGenome.g_jointMaxMinAngles.resize(jointNum + 1);
		currentGenome.g_jointMaxMinAngles[jointNum].resize(2);
		currentGenome.g_jointMaxMinAngles[jointNum][0] = -0.5;
		currentGenome.g_jointMaxMinAngles[jointNum][0] = 0.5;
		currentGenome.g_jointMaxForce.resize(jointNum + 1);
		currentGenome.g_jointMaxForce[jointNum] = 0.1;
		currentGenome.g_jointMaxVelocity.resize(jointNum + 1);
		currentGenome.g_jointMaxVelocity[jointNum] = 0.1;

		cout << "specifying object vector lengths: " << endl; 
		currentGenome.g_objectPosition.resize(partNum + 2);
		currentGenome.g_objectOrientation.resize(partNum + 2);
		currentGenome.g_objectSizes.resize(partNum + 2);

		currentGenome.g_objectParent.resize(partNum + 2);
		currentGenome.g_objectType.resize(partNum + 2);
		currentGenome.g_objectMass.resize(partNum + 2);
		// part 1
		cout << "saving part 1" << endl; 
		currentGenome.g_objectPosition[partNum - 1].resize(3);
		currentGenome.g_objectPosition[partNum - 1][0] = x;
		currentGenome.g_objectPosition[partNum - 1][1] = y;
		currentGenome.g_objectPosition[partNum - 1][2] = z;

		currentGenome.g_objectOrientation[partNum - 1].resize(3);
		currentGenome.g_objectOrientation[partNum - 1][0] = rotX;
		currentGenome.g_objectOrientation[partNum - 1][1] = rotY;
		currentGenome.g_objectOrientation[partNum - 1][2] = rotZ;

		
		currentGenome.g_objectSizes[partNum - 1].resize(3);
		currentGenome.g_objectSizes[partNum - 1][0] = length;
		currentGenome.g_objectSizes[partNum - 1][1] = width;
		currentGenome.g_objectSizes[partNum - 1][2] = height;

		currentGenome.g_objectMass[partNum - 1] = mass;
		currentGenome.g_objectParent[partNum - 1] = parent;
		currentGenome.g_objectType[partNum - 1] = currentGenome.g_objectType[partNum];





		cout << "saving part 2" << endl; 
		// part 2
		currentGenome.g_objectPosition[partNum].resize(3);
		currentGenome.g_objectPosition[partNum][0] = x;
		currentGenome.g_objectPosition[partNum][1] = y;
		currentGenome.g_objectPosition[partNum][2] = z;

		currentGenome.g_objectOrientation[partNum].resize(3);
		currentGenome.g_objectOrientation[partNum][0] = rotX;
		currentGenome.g_objectOrientation[partNum][1] = rotY;
		currentGenome.g_objectOrientation[partNum][2] = rotZ;

		currentGenome.g_objectSizes[partNum].resize(3);
		currentGenome.g_objectSizes[partNum][0] = length;
		currentGenome.g_objectSizes[partNum][1] = width;
		currentGenome.g_objectSizes[partNum][2] = height;

		currentGenome.g_objectMass[partNum] = mass;
		currentGenome.g_objectParent[partNum] = jointNum;
		currentGenome.g_objectType[partNum] = currentGenome.g_objectType[partNum];

		currentGenome.g_senseInput.resize(jointNum + 1);
		currentGenome.g_senseInput[jointNum] = 0;
		currentGenome.g_jointActivity.resize(jointNum + 1);
		currentGenome.g_jointActivity[jointNum] = 0;
		cout << "saved both parts" << endl; 
	}
}


void CENN::loadCreature(int creature)
{
	if (creature == 0) {
		cout << "loadCreature() is called" << endl;
		float shapeSize[3] = { 1, 1, 0.1 };
		float objectMass = shapeSize[0] * shapeSize[1] * shapeSize[2];
		simCreatePureShape(0, 8, shapeSize, 1.0, NULL);
		mainHandle.resize(1);
		mainHandle[0] = simGetObjectHandle("Cuboid");
		simSetObjectName(mainHandle[0], "Head");
		float newObjPos[3] = {0, 0.0, 0.4};
		simSetObjectPosition(mainHandle[0], -1, newObjPos);
	}
	else if (creature == 1) {
		simLoadModel("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\models\\robots\\mobile\\NAO.ttm");
		mainHandle.resize(1);
		mainHandle[0] = simGetObjectHandle("NAO");
		cout << "creatureHandle = " << mainHandle[0] << endl;
	}
	else if (creature == 2) {
		cout << "Evolving cat is loading" << endl;
		simLoadModel("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\models\\ENN\\catModel.ttm");
		mainHandle.resize(1);
		mainHandle[0] = simGetObjectHandle("Cat_Head");
		cout << "creatureHandle = " << mainHandle[0] << endl;
	}
	else if (creature == 3) { // to be used for debugging purposes
		// initialize parameters to create the creature
		cout << "Loading Custom Creature" << endl;
		initializeCreaturePars();
		createCreature();
	}
	else if (creature == 4) {
		cout << "Loading plant" << endl;
		initializePlantPars();
		createCreature();
	}
	else {
		cout << "creature type is unknown";
	}
}

void CENN::initializeCreaturePars() {
	i_objectAmount = 5;
	i_objectSizes.resize(i_objectAmount);
	i_objectMass.resize(i_objectAmount);
	i_objectPosition.resize(i_objectAmount);
	i_objectOrientation.resize(i_objectAmount);
	i_objectType.resize(i_objectAmount);
	i_objectParent.resize(i_objectAmount);

	for (int i = 0; i < i_objectAmount; i++)
	{
		i_objectSizes[i].resize(3);
		i_objectPosition[i].resize(3);
		i_objectOrientation[i].resize(3);
	}

	i_jointAmount = 4;
	i_jointSizes.resize(i_jointAmount);
	i_jointMass.resize(i_jointAmount);
	i_jointPosition.resize(i_jointAmount);
	i_jointOrientation.resize(i_jointAmount);
	i_jointType.resize(i_jointAmount);
	i_jointParent.resize(i_jointAmount);
	i_jointMaxMinAngles.resize(i_jointAmount);
	i_jointMaxVelocity.resize(i_jointAmount);
	i_jointMaxForce.resize(i_jointAmount);
	
	for (int i = 0; i < i_jointAmount; i++) {
		i_jointPosition[i].resize(3);
		i_jointOrientation[i].resize(3);
		i_jointSizes[i].resize(3);
		i_jointMaxMinAngles[i].resize(2);
	}

	//obj0
	int objNum = -1;
	int jointNum = -1;
	objNum++;
	i_objectPosition[objNum][0] = 0.0;
	i_objectPosition[objNum][1] = 0.0;
	i_objectPosition[objNum][2] = 0.5;
	i_objectOrientation[objNum][0] = 0;
	i_objectOrientation[objNum][1] = 0;
	i_objectOrientation[objNum][2] = 0;
	i_objectSizes[objNum][0] = 0.1;
	i_objectSizes[objNum][1] = 0.1;
	i_objectSizes[objNum][2] = 0.1;
	i_objectMass[objNum] = i_objectSizes[objNum][0] * i_objectSizes[objNum][1] * i_objectSizes[objNum][2];
	i_objectType[objNum] = 0;
	i_objectParent[objNum] = -1;
	//joint0
	jointNum++;
	i_jointPosition[jointNum][0] = 0.05;
	i_jointPosition[jointNum][1] = 0.0;
	i_jointPosition[jointNum][2] = 0.425;
	i_jointOrientation[jointNum][0] = 0.5*M_PI;
	i_jointOrientation[jointNum][1] = 0;
	i_jointOrientation[jointNum][2] = 0;
	i_jointSizes[jointNum][0] = 0.05;
	i_jointSizes[jointNum][1] = 0.025;
	i_jointSizes[jointNum][2] = 0.1;
	i_jointMass[jointNum] = i_jointSizes[jointNum][0] * i_jointSizes[jointNum][1] * i_jointSizes[jointNum][2];
	i_jointType[jointNum] = 1;
	i_jointParent[jointNum] = 0;
	//joint1
	jointNum++;
	i_jointPosition[jointNum][0] = -0.05;
	i_jointPosition[jointNum][1] = 0.0;
	i_jointPosition[jointNum][2] = 0.425;
	i_jointOrientation[jointNum][0] = -0.5*M_PI;
	i_jointOrientation[jointNum][1] = 0;
	i_jointOrientation[jointNum][2] = 0;
	i_jointSizes[jointNum][0] = 0.05;
	i_jointSizes[jointNum][1] = 0.025;
	i_jointSizes[jointNum][2] = 0.1;
	i_jointMass[jointNum] = i_jointSizes[jointNum][0] * i_jointSizes[jointNum][1] * i_jointSizes[jointNum][2];
	i_jointType[jointNum] = 1;
	i_jointParent[jointNum] = 0;
	//obj1
	objNum++;
	i_objectPosition[objNum][0] = 0.1;
	i_objectPosition[objNum][1] = 0.0;
	i_objectPosition[objNum][2] = 0.35;
	i_objectOrientation[objNum][0] = 0;
	i_objectOrientation[objNum][1] = 0;
	i_objectOrientation[objNum][2] = 0;
	i_objectSizes[objNum][0] = 0.1;
	i_objectSizes[objNum][1] = 0.1;
	i_objectSizes[objNum][2] = 0.1;
	i_objectMass[objNum] = i_objectSizes[objNum][0] * i_objectSizes[objNum][1] * i_objectSizes[objNum][2];
	i_objectType[objNum] = 0;
	i_objectParent[objNum] = 5;
	//obj2
	objNum++;
	i_objectPosition[objNum][0] = -0.1;
	i_objectPosition[objNum][1] = 0.0;
	i_objectPosition[objNum][2] = 0.35;
	i_objectOrientation[objNum][0] = 0;
	i_objectOrientation[objNum][1] = 0;
	i_objectOrientation[objNum][2] = 0;
	i_objectSizes[objNum][0] = 0.1;
	i_objectSizes[objNum][1] = 0.1;
	i_objectSizes[objNum][2] = 0.1;
	i_objectMass[objNum] = i_objectSizes[objNum][0] * i_objectSizes[objNum][1] * i_objectSizes[objNum][2];
	i_objectType[objNum] = 0;
	i_objectParent[objNum] = 6;
	//joint2
	jointNum++;
	i_jointPosition[jointNum][0] = 0.1;
	i_jointPosition[jointNum][1] = 0.0;
	i_jointPosition[jointNum][2] = 0.275;
	i_jointOrientation[jointNum][0] = 0;
	i_jointOrientation[jointNum][1] = 0.5*M_PI;
	i_jointOrientation[jointNum][2] = 0;
	i_jointSizes[jointNum][0] = 0.05;
	i_jointSizes[jointNum][1] = 0.025;
	i_jointSizes[jointNum][2] = 0.1;
	i_jointMass[jointNum] = i_jointSizes[jointNum][0] * i_jointSizes[jointNum][1] * i_jointSizes[jointNum][2];
	i_jointType[jointNum] = 1;
	i_jointParent[jointNum] = 1;
	//joint3
	jointNum++;
	i_jointPosition[jointNum][0] = -0.1;
	i_jointPosition[jointNum][1] = 0.0;
	i_jointPosition[jointNum][2] = 0.275;
	i_jointOrientation[jointNum][0] = 0;
	i_jointOrientation[jointNum][1] = -0.5*M_PI;
	i_jointOrientation[jointNum][2] = 0;
	i_jointSizes[jointNum][0] = 0.05;
	i_jointSizes[jointNum][1] = 0.025;
	i_jointSizes[jointNum][2] = 0.1;
	i_jointMass[jointNum] = i_jointSizes[jointNum][0] * i_jointSizes[jointNum][1] * i_jointSizes[jointNum][2];
	i_jointType[jointNum] = 1;
	i_jointParent[jointNum] = 2;
	//obj3
	objNum++;
	i_objectPosition[objNum][0] = 0.1;
	i_objectPosition[objNum][1] = 0.0;
	i_objectPosition[objNum][2] = 0.15;
	i_objectOrientation[objNum][0] = 0;
	i_objectOrientation[objNum][1] = 0;
	i_objectOrientation[objNum][2] = 0;
	i_objectSizes[objNum][0] = 0.05;
	i_objectSizes[objNum][1] = 0.05;
	i_objectSizes[objNum][2] = 0.2;
	i_objectMass[objNum] = i_objectSizes[objNum][0] * i_objectSizes[objNum][1] * i_objectSizes[objNum][2];
	i_objectType[objNum] = 0;
	i_objectParent[objNum] = 7;
	//obj4
	objNum++;
	i_objectPosition[objNum][0] = -0.1;
	i_objectPosition[objNum][1] = 0.0;
	i_objectPosition[objNum][2] = 0.15;
	i_objectOrientation[objNum][0] = 0;
	i_objectOrientation[objNum][1] = 0;
	i_objectOrientation[objNum][2] = 0;
	i_objectSizes[objNum][0] = 0.05;
	i_objectSizes[objNum][1] = 0.05;
	i_objectSizes[objNum][2] = 0.2;
	i_objectMass[objNum] = i_objectSizes[objNum][0] * i_objectSizes[objNum][1] * i_objectSizes[objNum][2];
	i_objectType[objNum] = 0;
	i_objectParent[objNum] = 8;
}

void CENN::initializePlantPars() {
	i_objectAmount = 1;
	i_objectSizes.resize(i_objectAmount);
	i_objectMass.resize(i_objectAmount);
	i_objectPosition.resize(i_objectAmount);
	i_objectOrientation.resize(i_objectAmount);
	i_objectType.resize(i_objectAmount);
	i_objectParent.resize(i_objectAmount);

	for (int i = 0; i < i_objectAmount; i++)
	{
		i_objectSizes[i].resize(3);
		i_objectPosition[i].resize(3);
		i_objectOrientation[i].resize(3);
	}

	i_jointAmount = 0;
	i_jointSizes.resize(i_jointAmount);
	i_jointMass.resize(i_jointAmount);
	i_jointPosition.resize(i_jointAmount);
	i_jointOrientation.resize(i_jointAmount);
	i_jointType.resize(i_jointAmount);
	i_jointParent.resize(i_jointAmount);
	i_jointMaxMinAngles.resize(i_jointAmount);
	i_jointMaxVelocity.resize(i_jointAmount);
	i_jointMaxForce.resize(i_jointAmount);

	for (int i = 0; i < i_jointAmount; i++) {
		i_jointPosition[i].resize(3);
		i_jointOrientation[i].resize(3);
		i_jointSizes[i].resize(3);
		i_jointMaxMinAngles[i].resize(2);
	}

	//obj0
	int objNum = -1;
	int jointNum = -1;
	objNum++;
	i_objectPosition[objNum][0] = 0.0;
	i_objectPosition[objNum][1] = 0.0;
	i_objectPosition[objNum][2] = 0.1;
	i_objectOrientation[objNum][0] = 0;
	i_objectOrientation[objNum][1] = 0;
	i_objectOrientation[objNum][2] = 0;
	i_objectSizes[objNum][0] = 0.1;
	i_objectSizes[objNum][1] = 0.1;
	i_objectSizes[objNum][2] = 0.1;
	i_objectMass[objNum] = i_objectSizes[objNum][0] * i_objectSizes[objNum][1] * i_objectSizes[objNum][2];
	i_objectType[objNum] = 0;
	i_objectParent[objNum] = -1;
}


void CENN::createCreature() {
	cout << "new default test " << endl;
	objectCreator(i_objectAmount + i_jointAmount);
	saveCreatureMorphology();
}


void CENN::objectCreator(int totalAmountObjects) {
	// first create head
	cout << "object creator is called" << endl;
	float size[3];
	float position[3];
	float orientation[3];
	size[0] = i_objectSizes[0][0];
	size[1] = i_objectSizes[0][1];
	size[2] = i_objectSizes[0][2];
	position[0] = i_objectPosition[0][0];
	position[1] = i_objectPosition[0][1];
	position[2] = i_objectPosition[0][2];
	orientation[0] = i_objectOrientation[0][0];
	orientation[1] = i_objectOrientation[0][1];
	orientation[2] = i_objectOrientation[0][2];
	simCreatePureShape(0, 16, size, i_objectMass[0] * 10, NULL);
	vector<int> handles;
	handles.resize(totalAmountObjects);
	handles[0] = simGetObjectHandle("Cuboid");
	if (mainHandle.size() < 1) {
		mainHandle.resize(1);
	}
	mainHandle[0] = handles[0];
	simSetObjectPosition(handles[0], -1, position);
	simSetObjectOrientation(handles[0], -1, orientation);
	simSetObjectName(handles[0], "mainObject");
	mainHandle[0] = handles[0];
	int objectNum = 0;
	int jointNum = -1;
//	storeObjectPars(//objectNum, position, orientation, size, i_objectMass[objectNum, ])
	for (int i = 1; i < i_objectMass.size(); i++) {
		if (i_objectType[i] == 0) {
			objectNum++;
			size[0] = i_objectSizes[i][0];
			size[1] = i_objectSizes[i][1];
			size[2] = i_objectSizes[i][2];
			position[0] = i_objectPosition[i][0];
			position[1] = i_objectPosition[i][1];
			position[2] = i_objectPosition[i][2];
			orientation[0] = i_objectOrientation[i][0];
			orientation[1] = i_objectOrientation[i][1];
			orientation[2] = i_objectOrientation[i][2];

			simCreatePureShape(0, 8, size, i_objectMass[0], NULL);
			handles[i] = simGetObjectHandle("Cuboid");
			string oName = "object" + to_string(i);
			const char *objectName = oName.c_str();
			simSetObjectPosition(handles[i], -1, position);
			simSetObjectOrientation(handles[i], -1, orientation);
			simSetObjectName(handles[i], objectName);
		}
	}
	for (int i = 0; i < i_jointMass.size(); i++) {
		if (i_jointType[i] == 1){
			//
			jointNum++;
			size[0] = i_jointSizes[i][0];
			size[1] = i_jointSizes[i][1];
			position[0] = i_jointPosition[i][0];
			position[1] = i_jointPosition[i][1];
			position[2] = i_jointPosition[i][2];
			orientation[0] = i_jointOrientation[i][0];
			orientation[1] = i_jointOrientation[i][1];
			orientation[2] = i_jointOrientation[i][2];

/*			float jointSize[2] = { 0.1, 0.025 };
			float jointPosition[3] = { 0.05, 0, 0.425 };
			float jointOrientation[3] = { 0.5 *M_PI, 0, 0 };
			simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
			joint1Handle = simGetjointHandle("Revolute_joint");
			simSetjointName(joint1Handle, "joint1");
			simSetjointPosition(joint1Handle, -1, jointPosition);
			simSetjointOrientation(joint1Handle, -1, jointOrientation);
			*/
			simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, size, NULL, NULL);
			handles[i + objectNum + 1] = simGetObjectHandle("Revolute_joint");
			//enable joint motor
			simSetObjectIntParameter(handles[i + objectNum + 1], 2000, 1);
			string joint = "joint" + to_string(i);
			const char *jointName = joint.c_str();
			simSetObjectName(handles[i + objectNum + 1], jointName);
			simSetObjectPosition(handles[i + objectNum + 1], -1, position);
			simSetObjectOrientation(handles[i + objectNum + 1], -1, orientation);
			cout << "Joint created " << endl;
		}
	}
	
	cout << "objects created, creating parents";
	
	for (int i = 1; i < i_objectAmount; i++)
	{
		cout << i_objectParent[i] << endl;
		simSetObjectParent(handles[i], handles[i_objectParent[i]], true);
	}
	for (int i = 0; i < i_jointAmount; i++) {
		simSetObjectParent(handles[i + i_objectAmount], handles[i_jointParent[i]], true);
	}
	objectHandles = handles;

}
/*
void CENN::createCreature2() { // not working anymore
	cout << "Default test is loading" << endl;
	int defaultHandles;
	// create object
	i_objectSizes.resize(9);
	i_objectMass.resize(9);
	i_objectPosition.resize(9);
	i_objectOrientation.resize(9);
	i_objectType.resize(9);
	for (int i = 0; i < 9; i++)
	{
		i_objectSizes[i].resize(3);
		i_objectPosition[i].resize(3);
		i_objectOrientation[i].resize(3);
	}

	float shapeSize[3] = { 0.1, 0.1, 0.1 };
	i_objectSizes[0][0] = 0.1;
	i_objectSizes[0][1] = 0.1;
	i_objectSizes[0][2] = 0.1;

	cout << "1";
	float shapeMass = shapeSize[0] * shapeSize[1] * shapeSize[2];
	float objectPosition[3] = { 0, 0, 0.5 };

	simCreatePureShape(0, 16, shapeSize, shapeMass, NULL);
	mainHandle = simGetObjectHandle("Cuboid");
	simSetObjectName(mainHandle, "defaultMain");
	simSetObjectPosition(mainHandle, -1, objectPosition);
	float objectOrientation[3] = { 0, 0, 0 };
	cout << "1";
	// save obj props #0
	storeObjectPars(0, objectPosition, objectOrientation, shapeSize, shapeMass, -1, 0);

	// create two joints
	int joint1Handle;
	int joint2Handle;
	float jointSize[2] = { 0.1, 0.025 };
	float jointPosition[3] = { 0.05, 0, 0.425 };
	float jointOrientation[3] = { 0.5 *M_PI, 0, 0 };
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	joint1Handle = simGetObjectHandle("Revolute_joint");
	simSetObjectName(joint1Handle, "joint1");
	simSetObjectPosition(joint1Handle, -1, jointPosition);
	simSetObjectOrientation(joint1Handle, -1, jointOrientation);

	// save joint props�#1
	storeObjectPars(1, jointPosition, jointOrientation, jointSize, 0, 0, 1);

	jointPosition[0] = -0.05;
	jointOrientation[0] = -0.5*M_PI;
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	joint2Handle = simGetObjectHandle("Revolute_joint");
	simSetObjectName(joint2Handle, "joint2");
	simSetObjectPosition(joint2Handle, -1, jointPosition);
	simSetObjectOrientation(joint2Handle, -1, jointOrientation);

	// save joint props 2
	storeObjectPars(2, jointPosition, jointOrientation, jointSize, 0, 0, 1);

	// create two objects
	int object1Handle;
	int object2Handle;
	cout << "1";
	objectPosition[0] = 0.1;
	objectPosition[2] = 0.35;
	simCreatePureShape(0, 8, shapeSize, shapeMass, NULL);
	object1Handle = simGetObjectHandle("Cuboid");
	simSetObjectName(object1Handle, "ojbect1");
	simSetObjectPosition(object1Handle, -1, objectPosition);
	// save obj props
	storeObjectPars(3, objectPosition, objectOrientation, shapeSize, 0, 1, 0);

	objectPosition[0] = -0.1;
	simCreatePureShape(0, 8, shapeSize, shapeMass, NULL);
	object2Handle = simGetObjectHandle("Cuboid");
	simSetObjectName(object2Handle, "ojbect2");
	simSetObjectPosition(object2Handle, -1, objectPosition);
	// save obj props
	storeObjectPars(4, objectPosition, objectOrientation, shapeSize, 0, 2, 0);

	// create two joints
	int joint3Handle;
	int joint4Handle;

	jointPosition[0] = 0.1;
	jointPosition[2] = 0.275;
	jointOrientation[0] = 0;
	jointOrientation[1] = 0.5*M_PI;
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	joint3Handle = simGetObjectHandle("Revolute_joint");
	simSetObjectName(joint3Handle, "joint3");
	simSetObjectPosition(joint3Handle, -1, jointPosition);
	simSetObjectOrientation(joint3Handle, -1, jointOrientation);
	// save joint props
	storeObjectPars(5, jointPosition, jointOrientation, jointSize, 0, 3, 1);

	jointPosition[0] = -0.1;
	jointOrientation[0] = 0;
	jointOrientation[1] = -0.5*M_PI;
	simCreateJoint(sim_joint_revolute_subtype, sim_jointmode_force, 0, jointSize, NULL, NULL);
	joint4Handle = simGetObjectHandle("Revolute_joint");
	simSetObjectName(joint4Handle, "joint4");
	simSetObjectPosition(joint4Handle, -1, jointPosition);
	simSetObjectOrientation(joint4Handle, -1, jointOrientation);
	// save joint props
	storeObjectPars(6, jointPosition, jointOrientation, jointSize, 0, 4, 1);
	cout << "1";
	// create two objects
	int object3Handle;
	int object4Handle;

	shapeSize[2] = 0.2;
	shapeSize[0] = 0.05;
	shapeSize[1] = 0.05;
	objectPosition[0] = 0.1;
	objectPosition[2] = 0.15;
	simCreatePureShape(0, 8, shapeSize, shapeMass, NULL);
	object3Handle = simGetObjectHandle("Cuboid");
	simSetObjectName(object3Handle, "ojbect3");
	simSetObjectPosition(object3Handle, -1, objectPosition);
	storeObjectPars(7, objectPosition, objectOrientation, shapeSize, 0, 5, 0);

	objectPosition[0] = -0.1;
	simCreatePureShape(0, 8, shapeSize, shapeMass, NULL);
	object4Handle = simGetObjectHandle("Cuboid");
	simSetObjectName(object4Handle, "ojbect4");
	simSetObjectPosition(object4Handle, -1, objectPosition);
	storeObjectPars(8, objectPosition, objectOrientation, shapeSize, 0, 6, 0);
	cout << "1";
	// set object tree
	simSetObjectParent(joint1Handle, mainHandle, true);
	simSetObjectParent(joint2Handle, mainHandle, true);
	simSetObjectParent(object1Handle, joint1Handle, true);
	simSetObjectParent(object2Handle, joint2Handle, true);
	simSetObjectParent(joint3Handle, object1Handle, true);
	simSetObjectParent(joint4Handle, object2Handle, true);
	simSetObjectParent(object3Handle, joint3Handle, true);
	simSetObjectParent(object4Handle, joint4Handle, true);

	// save morphology
	saveCreatureMorphology();
	
} */

void CENN::saveCreatureMorphology() {
	// 
//	all pars to a .csv file
	cout << "Creature morphology is being saved" << endl;
	ofstream savedBlueprint;
	ostringstream creatureFileName;
	creatureFileName << "interfaceFiles\\creature.csv";
	savedBlueprint.open(creatureFileName.str()); //ios::out | ios::ate | ios::app);
	savedBlueprint << "This file saves a creature blueprint\n";
	savedBlueprint << "\nThese are the Object Properties:\n";
	savedBlueprint << "\n,amountObj," << i_objectAmount << "," << endl;
	savedBlueprint << "\n,#objPos,";
	for (int i = 0; i < i_objectAmount; i++) {
		for (int j = 0; j < 3; j++) {
			savedBlueprint << i_objectPosition[i][j] << ",";
		}
	}
	cout << "object position saved" << endl;
	savedBlueprint << "\n,#objOr,";
	for (int i = 0; i < i_objectAmount; i++) {
		for (int j = 0; j < 3; j++) {
			savedBlueprint << i_objectOrientation[i][j] << ",";
		}
	}
	cout << "object orientation saved" << endl;
	savedBlueprint << "\n,#objSize,";
	for (int i = 0; i < i_objectAmount; i++) {
		for (int j = 0; j < 3; j++) {
			savedBlueprint << i_objectSizes[i][j] << ",";
		}
	}
	cout << "object size saved" << endl;
	savedBlueprint << "\n,#objMass,";
	for (int i = 0; i < i_objectAmount; i++) {
		savedBlueprint << i_objectMass[i] << ",";
	}
	cout << "object mass saved" << endl;
	savedBlueprint << "\n,#objPar,";
	for (int i = 0; i < i_objectAmount; i++) {
		savedBlueprint << i_objectParent[i] << ",";
	}
	cout << "object parent saved" << endl;
	savedBlueprint << "\n,#objType,";
	for (int i = 0; i < i_objectAmount; i++) {
		savedBlueprint << i_objectType[i] << ",";
	}
	savedBlueprint << "\n,#objFriction,reserved,";
	savedBlueprint << "\n\nJointProperties:\n";
	savedBlueprint << "\n,#amountJoints," << i_jointAmount;
	savedBlueprint << "\n,#jointPos,";
	for (int i = 0; i < i_jointAmount; i++) {
		for (int j = 0; j < 3; j++) {
			savedBlueprint << i_jointPosition[i][j] << ",";
		}
	}
	cout << "joint position saved" << endl;
	savedBlueprint << "\n,#jointOr,";
	for (int i = 0; i < i_jointAmount; i++) {
		for (int j = 0; j < 3; j++) {
			savedBlueprint << i_jointOrientation[i][j] << ",";
		}
	}
	cout << "joint orientation saved" << endl;
	savedBlueprint << "\n,#jointSize,";
	for (int i = 0; i < i_jointAmount; i++) {
		for (int j = 0; j < 3; j++) {
			savedBlueprint << i_jointSizes[i][j] << ",";
		}
	}
	cout << "joint size saved" << endl;
	savedBlueprint << "\n,#jointMass,";
	for (int i = 0; i < i_jointAmount; i++) {
		savedBlueprint << i_jointMass[i] << ",";
	}
	cout << "joint mass saved" << endl;
	savedBlueprint << "\n,#jointPar,";
	for (int i = 0; i < i_jointAmount; i++) {
		savedBlueprint << i_jointParent[i] << ",";
	}
	cout << "joint parent saved" << endl;
	savedBlueprint << "\n,#jointType,";
	for (int i = 0; i < i_jointAmount; i++) {
		savedBlueprint << i_jointType[i] << ",";
	}
	cout << "joint type saved" << endl;
	savedBlueprint << "\n,#jointMaxMinAngles,";
	for (int i = 0; i < i_jointAmount; i++) {
		for (int j = 0; j < 2; j++) {
			savedBlueprint << i_jointMaxMinAngles[i][j] << ",";
		}
	}
	cout << "jointMaxMinAngles Saved" << endl;
	savedBlueprint << "\n,#jointMaxVelocities,";
	for (int i = 0; i < i_jointAmount; i++) {
		savedBlueprint << i_jointMaxVelocity[i] << ",";
	}
	cout << "jointMaxVelocities saved";
	savedBlueprint << "\n,#jointMaxForce," << endl;
	for (int i = 0; i < i_jointAmount; i++) {
		savedBlueprint << i_jointMaxForce[i] << ",";
	}
	cout << "jointMaxForces saved" << endl;
	savedBlueprint << "\n\nEnd of construction info" << endl;
	savedBlueprint.close();
	cout << "blueprint saved" << endl;
}

void CENN::useDefaultPlantPars() {
	cout << "Using default plant parts" << endl;
	// new saved .csv
	//Robot Properties--------------------------
	s_amountJoints = 4;
	s_jointControl = 1;
	s_useSensors = true;
	s_useProprioception = true;
	s_useVestibularSystem = false;
	s_useTactileSensing = false;
	s_tactileSensingType = -1;
	s_amountSenseParts = 0;
	s_specifiedPartNames = "";
	s_identifyControl = true;
	s_identifyMaxForceAndVelocity = true;
	s_usePID = true;
	s_upperVelLimit = true;
	s_pValue = 0;
	s_iValue = 0;
	s_dValue = 0;
	s_useSpringDamper = 0;
	s_kValue = 0;
	s_cValue = 0;
	s_useMaxVelocity = true;
	s_useMaxForce = true;
	s_useDesiredAngle = true;

	//	Evolution Parameters--------------------------
	s_initialPopulationSize = 10;
	s_variablePopulationSize = true;
	s_islandPopulations = true;
	s_maxAmountIslandPopulations = 4;
	s_defaultIslandInvasionRate = 0.3;
	s_evolvableInvasionRate = true;
	s_initialMutationRate = 0.05; // in percentage of genome
	s_evolvableMutationRate = true;
	s_incrementalEvolution = true;
	s_incrementInterval = 20;
	s_realTimeEvolution = true;
	s_realTimeEvolutionUpdateTime = 1.0; // evaluation happens after each given timeStep
	s_realTimeEvolutionTimeLimit = 20;
	s_evolvingMorphology = true;
	s_evolvingDimensions = true; // dimensions of objects are evolved by percentage
	s_deleteRandomParts = true;
	s_addRandomParts = true;
	s_lEvolution = true;

	s_genomeType = -1;
	s_ploidity = 1; // meaning that the genome has 1 copy
	s_reproductionType = 0; // 0 is asexual, 1 = sexual
	s_randSeed = 1;
	s_neuroEvolution = true;
	s_variableGenomeSize = true;
	s_geneDuplication = true;
	s_useCrossover = true;
	s_maxAmountCrossover = 10;
	s_evolvableCrossover = true;
	s_historicalMarkers = true;
	s_geneScopeFitness = true;
	s_simulationTime = 1.0;

	//	ANN Parameters--------------------------
	s_useCustomNN = false;
	s_customNNname = "default";
	s_calculateAmountInputNeurons = true;
	s_jointCalculator = true;
	s_calculateAmountOutputneurons = true;
	s_amountInterNeurons = 20;
	s_usePatternGenerators = true;
	s_patternGeneratorTypes = 1;
	s_amountPatternGenerators = 2;
	s_variableAmountNeurons = true;
	s_variableAmountInputNeurons = true;
	s_variableAmountInterNeurons = true;
	s_variableAmountOutputNeurons = true;
	s_variableAmountPatternGenerators = true;
	s_maxAmountInputNeurons = 200;
	s_maxAmountInterNeurons = 200;
	s_maxAmountOutputNeurons = 200;
	s_maxAmountPatternGenerators = 100;
	s_maxAmountNeurons = s_maxAmountInputNeurons + s_maxAmountInterNeurons + s_maxAmountOutputNeurons + s_maxAmountPatternGenerators;

	//	Evaluation Properties--------------------------
	s_evaluationType = 1;
	//	Summary--------------------------
	s_amountInputNeurons = 2;
	s_amountOutputNeurons = 0;
	s_totalInput = s_amountInputNeurons;



	// not defined yet
	s_totalInput = 0;
	if (s_useSensors) {
		if (s_useProprioception == true) {
			s_totalInput += s_amountJoints;
		}
		if (s_useTactileSensing == true) {
			s_totalInput += s_amountSenseParts;
		}
		if (s_useVestibularSystem == true) {
			s_totalInput += 18;
		}
	}
}

void CENN::useDefaultPars() {

	// new saved .csv
	//Robot Properties--------------------------
	s_amountJoints = 0;
	s_jointControl = 1;
	s_useSensors = true;
	s_useProprioception = true;
	s_useVestibularSystem = true;
	s_useTactileSensing = true;
	s_tactileSensingType = -1;
	s_amountSenseParts = 2;
	s_specifiedPartNames = "object3;object4";
	s_identifyControl = true;
	s_identifyMaxForceAndVelocity = true;
	s_usePID = true;
	s_upperVelLimit = true;
	s_pValue = 0;
	s_iValue = 0;
	s_dValue = 0;
	s_useSpringDamper = 0;
	s_kValue = 0;
	s_cValue = 0;
	s_useMaxVelocity = true;
	s_useMaxForce = true;
	s_useDesiredAngle = true;

	//	Evolution Parameters--------------------------
	s_initialPopulationSize = 5;
	s_variablePopulationSize = true;
	s_islandPopulations = true;
	s_maxAmountIslandPopulations = 4;
	s_defaultIslandInvasionRate = 0.3;
	s_evolvableInvasionRate = true;
	s_initialMutationRate = 0.05; // in percentage of genome
	s_evolvableMutationRate = true;
	s_incrementalEvolution = true;
	s_incrementInterval = 20;
	s_realTimeEvolution = true;
	s_realTimeEvolutionUpdateTime = 1.0; // evaluation happens after each given timeStep
	s_realTimeEvolutionTimeLimit = 20;
	s_evolvingMorphology = true;
	s_evolvingDimensions = true; // dimensions of objects are evolved by percentage
	s_deleteRandomParts = true;
	s_addRandomParts = true;
	s_lEvolution = true;

	s_genomeType = -1;
	s_ploidity = 1; // meaning that the genome has 1 copy
	s_reproductionType = 0; // 0 is asexual, 1 = sexual
	s_randSeed = 1;
	s_neuroEvolution = true;
	s_variableGenomeSize = true;
	s_geneDuplication = true;
	s_useCrossover = true;
	s_maxAmountCrossover = 10;
	s_evolvableCrossover = true;
	s_historicalMarkers = true;
	s_geneScopeFitness = true;
	s_simulationTime = 1.0;

	//	ANN Parameters--------------------------
	s_useCustomNN = true;
	s_customNNname = "default";
	s_calculateAmountInputNeurons = true;
	s_jointCalculator = true;
	s_calculateAmountOutputneurons = true;
	s_amountInterNeurons = 10;
	s_usePatternGenerators = true;
	s_patternGeneratorTypes = 1;
	s_amountPatternGenerators = 2;
	s_variableAmountNeurons = true;
	s_variableAmountInputNeurons = true;
	s_variableAmountInterNeurons = true;
	s_variableAmountOutputNeurons = true;
	s_variableAmountPatternGenerators = true;
	s_maxAmountInputNeurons = 200;
	s_maxAmountInterNeurons = 200;
	s_maxAmountOutputNeurons = 200;
	s_maxAmountPatternGenerators = 100;
	s_maxAmountNeurons = s_maxAmountInputNeurons + s_maxAmountInterNeurons + s_maxAmountOutputNeurons + s_maxAmountPatternGenerators;

	//	Evaluation Properties--------------------------
	s_evaluationType = 1;
	//	Summary--------------------------
	s_amountInputNeurons = 2;
	s_amountOutputNeurons = 0;
	s_totalInput = 2;



	// not defined yet
	s_totalInput = 0;
	if (s_useSensors) {
		if (s_useProprioception == true) {
			s_totalInput += s_amountJoints;
		}
		if (s_useTactileSensing == true) {
			s_totalInput += s_amountSenseParts;
		}
		if (s_useVestibularSystem == true) {
			s_totalInput += 18;
		}
	}
}

void CENN::createMorphologyWithLeafs() {
	for (int i = 0; i < currentGenome.amountIncrement / 2; i++){
		if (i == 0) {
			//	cout << "li = " << i << endl;
			//
			currentGenome.g_objectPosition.resize(1);
			currentGenome.g_objectPosition[0].resize(3);
			currentGenome.g_objectPosition[0][0] = 0;
			currentGenome.g_objectPosition[0][1] = 0;
			currentGenome.g_objectPosition[0][2] = 0.5;

			currentGenome.g_objectOrientation.resize(0 + 1);
			currentGenome.g_objectOrientation[0].resize(3);
			currentGenome.g_objectOrientation[0][0] = 0;
			currentGenome.g_objectOrientation[0][1] = 0;
			currentGenome.g_objectOrientation[0][2] = 0;

			currentGenome.g_objectSizes.resize(1);
			currentGenome.g_objectSizes[0].resize(3);
			currentGenome.g_objectSizes[0][0] = 0;
			currentGenome.g_objectSizes[0][1] = 0;
			currentGenome.g_objectSizes[0][2] = 0;

			currentGenome.g_objectMass.resize(1);
			currentGenome.g_objectMass[0] = 0;
			currentGenome.g_objectParent.resize(1);
			currentGenome.g_objectParent[0] = 0;
			currentGenome.g_objectType.resize(1);
			currentGenome.g_objectType[0] = 0;
			// save initial
			currentGenome.g_objectHandles.resize(1);
			currentGenome.g_objectHandles[0] = mainHandle[0];
			currentGenome.g_objectType.resize(1);
			currentGenome.g_objectType[0] = 0;
			growStem(1, 0);
		}
		else {
			if (currentGenome.g_objectAmount < 100) {// && i < 10) {
				if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 0) {
					lGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, currentGenome.g_objectAmount - 1);
					vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
					int parent = currentGenome.g_objectAmount - 1;
					for (int j = 0; j < currentGenome.g_lStateProperties[4].size(); j++) {
					//	createArtificialLeaf(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
					//	a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
						//	createArtificialLeafWithJoint(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
						//		a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
					//	createArtificialLeafWithJoint(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
					//		a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
					}
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 1) {
					lGrowth(currentGenome.g_lStateProperties[1].size(), 1, 2, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 2) {
					lGrowth(currentGenome.g_lStateProperties[2].size(), 2, 3, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 3) {
					lGrowth(currentGenome.g_lStateProperties[3].size(), 3, 4, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 4) {
					lGrowth(currentGenome.g_lStateProperties[4].size(), 4, 1, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 5) {
					vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
					int parent = currentGenome.g_objectAmount - 1;
					for (int j = 0; j < currentGenome.g_lStateProperties[5].size(); j++) {
					//	currentGenome.g_objectAmount++;
					//	currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
					//	currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
					//	createArtificialLeaf(0.1, 0.1, 0.01, a[5][j][3], a[5][j][4], a[5][j][5],
					//		a[5][j][7], a[5][j][8], a[5][j][9], 0, parent, currentGenome.g_objectAmount - 1);
					//	float color[3] = { 0.9, 0.9, 0.9 };
					//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], NULL, sim_colorcomponent_ambient_diffuse, color);
					}
				}

				bool complexL = false;
				if (complexL == true) {
					int maxIt = currentGenome.g_objectAmount - 1;
					for (int j = 0; j < maxIt; j++ && currentGenome.g_objectAmount < 100) {
						int objIndex = 0;
						vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
						int childHandle = simGetObjectChild(currentGenome.g_objectHandles[j], objIndex++);
			//			if (childHandle == -1) {
							if (currentGenome.g_objectType[j] == 0) {
								lGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, j);
								//vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
								int parent = currentGenome.g_objectAmount - 1;
								for (int j = 0; j < currentGenome.g_lStateProperties[4].size(); j++) {
								//	cout << "pushing back leafs" << endl;
							//		currentGenome.g_leafHandles.push_back(0);
							//		createArtificialLeaf(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
							//			a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
								}
							}
							else if (currentGenome.g_objectType[j] == 1) {
								lGrowth(currentGenome.g_lStateProperties[1].size(), 1, 2, j);
							}
							else if (currentGenome.g_objectType[j] == 2) {
								lGrowth(currentGenome.g_lStateProperties[2].size(), 2, 3, j);
							}
							else if (currentGenome.g_objectType[j] == 3) {
								lGrowth(currentGenome.g_lStateProperties[3].size(), 3, 4, j);
							}
							else if (currentGenome.g_objectType[j] == 4) {
								lGrowth(currentGenome.g_lStateProperties[4].size(), 4, 1, j);
							}
							else if (currentGenome.g_objectType[j] == 5) {
							}
						}
					}
		//		}
			}
			else {
				cout << "STRUCTURE TOO LARGE!" << endl;
			}
			//	growStem(i + 1, i);
			//cout << "Stem has grown" << endl;
		}
	}
}

void CENN::lSystem() {
//	cout << "initializing LSystem" << endl;
	for (int i = 0; i < currentGenome.amountIncrement; i++){
		if (i == 0) {
		//	cout << "li = " << i << endl;
			//
			currentGenome.g_objectPosition.resize(1);
			currentGenome.g_objectPosition[0].resize(3);
			currentGenome.g_objectPosition[0][0] = 0;
			currentGenome.g_objectPosition[0][1] = 0;
			currentGenome.g_objectPosition[0][2] = 0.5;

			currentGenome.g_objectOrientation.resize(0 + 1);
			currentGenome.g_objectOrientation[0].resize(3);
			currentGenome.g_objectOrientation[0][0] = 0;
			currentGenome.g_objectOrientation[0][1] = 0;
			currentGenome.g_objectOrientation[0][2] = 0;

			currentGenome.g_objectSizes.resize(1);
			currentGenome.g_objectSizes[0].resize(3);
			currentGenome.g_objectSizes[0][0] = 0;
			currentGenome.g_objectSizes[0][1] = 0;
			currentGenome.g_objectSizes[0][2] = 0;

			currentGenome.g_objectMass.resize(1);
			currentGenome.g_objectMass[0] = 0;
			currentGenome.g_objectParent.resize(1);
			currentGenome.g_objectParent[0] = 0;
			currentGenome.g_objectType.resize(1);
			currentGenome.g_objectType[0] = 0;


			// save initial
			currentGenome.g_objectHandles.resize(1);
			currentGenome.g_objectHandles[0] = mainHandle[0];
			currentGenome.g_objectType.resize(1);
			currentGenome.g_objectType[0] = 0;
			// create initial cube
			growStem(1, 0);
		//	currentGenome.g_objectAmount = 1; 
		}
		else {
			cout << "else is called " << endl;
			cout << "object amount: " << currentGenome.g_objectAmount << endl;
			cout << "currentGenome object type = " << currentGenome.g_objectType[currentGenome.g_objectAmount - 1] << endl;
			//cout << "grow depending on state" << endl;
			if (currentGenome.g_objectAmount < 100) {// && i < 10) {
				cout << "currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = " << currentGenome.g_objectType[currentGenome.g_objectAmount - 1] << endl;
				if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 0) {
					//		cout << "g_objectType[currentGenome.g_objectAmount-1] = 0" << endl;
					lGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, currentGenome.g_objectAmount-1);
					if (indCounter > s_initialPopulationSize) {
					//	currentGenome.g_leafHandles.push_back(currentGenome.g_objectHandles[currentGenome.g_objectHandles.size() - 1]);
					}

				//	float color[3] = { 0.1, 0.1, 0.1 };
				//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount-1] == 1) {
					lGrowth(currentGenome.g_lStateProperties[1].size(), 1, 2, currentGenome.g_objectAmount-1);
				//	createArtificialLeaf();
				//	float color[3] = { 0.1, 0.1, 0.8 };
				//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount-1] == 2) {
					lGrowth(currentGenome.g_lStateProperties[2].size(), 2, 3, currentGenome.g_objectAmount-1);
				//	float color[3] = { 0.1, 0.8, 0.1 };
				//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);

				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount-1] == 3) {
					lGrowth(currentGenome.g_lStateProperties[3].size(), 3, 4, currentGenome.g_objectAmount-1);
				//	float color[3] = { 0.8, 0.1, 0.1 };
				//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);

				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount-1] == 4) {
					lGrowth(currentGenome.g_lStateProperties[4].size(), 4, 5, currentGenome.g_objectAmount-1);
					// add new object to  leafHandles, 
				
				//	float color[3] = { 0.8, 0.1, 0.8 };
				//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
				//	cout << "about to create artificial leaf" << endl;
				//	vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
				//	int parent = currentGenome.g_objectAmount - 1;
				//	cout << "parent is correct" << endl;
				//	cout << "currentGenome.g_lStateProperties[4].size() -1 = " << currentGenome.g_lStateProperties[4].size() - 1 << endl;
				//	for (int j = 0; j < currentGenome.g_lStateProperties[4].size() - 1; j++) {
				//		currentGenome.g_leafHandles.push_back(0);
				//		createArtificialLeaf(0.01, 0.05, 0.05, a[4][j][3], a[4][j][4], a[4][j][5],
				//			a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
				//		createArtificialLeafWithJoint(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
				//			a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
				//		createJointAndCubeInLeafHandles(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
				//			a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size(), currentGenome.g_leafHandles.size());
				//	}
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount-1] == 5) {
					
				//	float color[3] = { 0.1, 0.8, 0.8 };
				//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
				//	simPauseSimulation();
				}
				
				bool complexL = true;
				if (complexL == true) {
					// continue object creation
					int maxIt = currentGenome.g_objectAmount - 1;
					for (int j = 0; j < maxIt; j++ && currentGenome.g_objectAmount < 100) {
						int objIndex = 0;
						int childHandle = simGetObjectChild(currentGenome.g_objectHandles[j], objIndex++);
						//	while (childHandle != -1) {
						//		cout << "childHandleThing!!!!!!" << endl; 
						//		childHandle = simGetObjectChild(currentGenome.g_objectHandles[j], objIndex++);
						//	}
						if (childHandle == -1) {
							// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							if (currentGenome.g_objectType[j] == 0) {
								//		cout << "g_objectType[currentGenome.g_objectAmount-1] = 0" << endl;
								lGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, j);
								if (indCounter > s_initialPopulationSize) {
							//		currentGenome.g_leafHandles.push_back(currentGenome.g_objectHandles[currentGenome.g_objectHandles.size() - 1]);
								}
								//	float color[3] = { 0.1, 0.1, 0.1 };
								//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (currentGenome.g_objectType[j] == 1) {
								lGrowth(currentGenome.g_lStateProperties[1].size(), 1, 2, j);
								//	float color[3] = { 0.1, 0.1, 0.8 };
								//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (currentGenome.g_objectType[j] == 2) {
								lGrowth(currentGenome.g_lStateProperties[2].size(), 2, 3, j);
								//	float color[3] = { 0.1, 0.8, 0.1 };
								//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);

							}
							else if (currentGenome.g_objectType[j] == 3) {
								lGrowth(currentGenome.g_lStateProperties[3].size(), 3, 4, j);
								//	float color[3] = { 0.8, 0.1, 0.1 };
								//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);

							}
							else if (currentGenome.g_objectType[j] == 4) {
								lGrowth(currentGenome.g_lStateProperties[4].size(), 4, 5, currentGenome.g_objectAmount - 1);
								// add new object to  leafHandles, 
								
								
								//	float color[3] = { 0.8, 0.1, 0.8 };
								//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
							//	cout << "about to create artificial leaf" << endl;
							//	vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
							//	int parent = j;
							//	cout << "parent is correct" << endl;
							//	cout << "currentGenome.g_lStateProperties[4].size() -1 = " << currentGenome.g_lStateProperties[4].size() - 1 << endl;
							//	for (int j = 0; j < currentGenome.g_lStateProperties[4].size() - 1; j++) {
									//		currentGenome.g_leafHandles.push_back(0);
									//		createArtificialLeaf(0.01, 0.05, 0.05, a[4][j][3], a[4][j][4], a[4][j][5],
									//			a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
							//		createArtificialLeafWithJoint(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
							//			a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size());
								//	createJointAndCubeInLeafHandles(0.1, 0.1, 0.01, a[4][j][3], a[4][j][4], a[4][j][5],
								//		a[4][j][7], a[4][j][8], a[4][j][9], 0, parent, currentGenome.g_leafHandles.size(), currentGenome.g_leafHandles.size());

							//	}
							}
							else if (currentGenome.g_objectType[j] == 5) {
							//	vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
						//		int parent = currentGenome.g_objectAmount - 1;
						//		for (int j = 0; j < currentGenome.g_lStateProperties[5].size(); j++) {
						//		//	currentGenome.g_leafHandles.push_back(0);
						//			createArtificialLeaf(0.05, 0.05, 0.01, a[5][j][3], a[5][j][4], a[5][j][5],
						//				a[5][j][7], a[5][j][8], a[5][j][9], 0, parent, currentGenome.g_leafHandles.size());
						//			float color[3] = { 0.9, 0.9, 0.9 };
						//			simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], NULL, sim_colorcomponent_ambient_diffuse, color);
						//		//	simPauseSimulation();
						//		}
								/*	cout << "artificial leaf is being created" << endl;
									cout << "Object Type == 5 " << endl;
									cout << "statePropSize " << currentGenome.g_lStateProperties[5].size() << endl;
									vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
									int parent = j;
									for (int j = 0; j < currentGenome.g_lStateProperties[5].size(); j++) {
									currentGenome.g_objectAmount++;
									currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
									currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
									createArtificialLeaf(0.1, 0.1, 0.01, a[5][j][3], a[5][j][4], a[5][j][5],
									a[5][j][7], a[5][j][8], a[5][j][9], 0, parent, j);
									float color[3] = { 0.9, 0.9, 0.9 };
									simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);

									}
									//	float color[3] = { 0.1, 0.8, 0.8 };
									//	simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount-1], NULL, sim_colorcomponent_ambient_diffuse, color);
									simPauseSimulation(); */
							}
							//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

							//		cout << "childHandle NONONONO" << endl;
							//		cout << simGetObjectName(currentGenome.g_objectHandles[j]) << endl;
							//		childHandle = simGetObjectChild(currentGenome.g_objectHandles[j], objIndex++);
						}
						//	cout << "objIndex = " << objIndex << endl << endl;
						//	if (objIndex == 0) {
						//		cout << "Lgrowth could happen" << endl;
						//	}
					}
				}

				for (int j = 0; j < currentGenome.g_objectAmount; j++) {
					for (int k = 0; k < currentGenome.g_lStateProperties.size(); k++){
						if (currentGenome.g_objectType[j] == k) {
							float color[3];
							if (k == 0)	{
								color[0] = 0.1;
								color[1] = 0.1;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 1) {
								color[0] = 0.8;
								color[1] = 0.1;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 2){
								color[0] = 0.1;
								color[1] = 0.8;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 3) {
								color[0] = 0.1;
								color[1] = 0.1;
								color[2] = 0.8;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 4){
								color[0] = 0.8;
								color[1] = 0.8;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							} 
							else if (k == 5) {
								color[0] = 0.8;
								color[1] = 0.1;
								color[2] = 0.8;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							/**/
							/*
							switch (k) {
							case 0:
								color[0] = 0.1;
								color[1] = 0.1;
								color[2] = 0.1; 
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							case 1:
								color[0] = 0.8;
								color[1] = 0.1;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							case 2:
								color[0] = 0.1;
								color[1] = 0.8;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							case 3:
								color[0] = 0.1;
								color[1] = 0.1;
								color[2] = 0.8;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							case 4:
								color[0] = 0.8;
								color[1] = 0.8;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							/**/
						}
					}
				}
			}
			else {
				cout << "STRUCTURE TOO LARGE!" << endl;
			}
			//	growStem(i + 1, i);
			//cout << "Stem has grown" << endl;
		}
	}
//	simPauseSimulation();
}


void CENN::modularLSystem() {
	//	cout << "initializing LSystem" << endl;
	for (int i = 0; i < currentGenome.amountIncrement; i++){
		if (i == 0) {
			//	cout << "li = " << i << endl;
			//
			currentGenome.g_objectPosition.resize(1);
			currentGenome.g_objectPosition[0].resize(3);
			currentGenome.g_objectPosition[0][0] = 0;
			currentGenome.g_objectPosition[0][1] = 0;
			currentGenome.g_objectPosition[0][2] = 0.5;

			currentGenome.g_objectOrientation.resize(0 + 1);
			currentGenome.g_objectOrientation[0].resize(3);
			currentGenome.g_objectOrientation[0][0] = 0;
			currentGenome.g_objectOrientation[0][1] = 0;
			currentGenome.g_objectOrientation[0][2] = 0;

			currentGenome.g_objectSizes.resize(1);
			currentGenome.g_objectSizes[0].resize(3);
			currentGenome.g_objectSizes[0][0] = 0;
			currentGenome.g_objectSizes[0][1] = 0;
			currentGenome.g_objectSizes[0][2] = 0;

			currentGenome.g_objectMass.resize(1);
			currentGenome.g_objectMass[0] = 0;
			currentGenome.g_objectParent.resize(1);
			currentGenome.g_objectParent[0] = 0;
			currentGenome.g_objectType.resize(1);
			currentGenome.g_objectType[0] = 0;

			// save initial
			currentGenome.g_objectHandles.resize(1);
			currentGenome.g_objectHandles[0] = mainHandle[0];
			currentGenome.g_objectType.resize(1);
			currentGenome.g_objectType[0] = 0;
			// create initial cube
			growStem(1, 0);
			//	currentGenome.g_objectAmount = 1; 
		}
		else {
			if (currentGenome.g_objectAmount < 100) {// && i < 10) {
				if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 0) {
					modularLGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 1) {
					modularLGrowth(currentGenome.g_lStateProperties[1].size(), 1, 2, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 2) {
					modularLGrowth(currentGenome.g_lStateProperties[2].size(), 2, 3, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 3) {
					modularLGrowth(currentGenome.g_lStateProperties[3].size(), 3, 4, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 4) {
					modularLGrowth(currentGenome.g_lStateProperties[4].size(), 4, 1, currentGenome.g_objectAmount - 1);
				}
				else if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] == 5) {
					vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
					int parent = currentGenome.g_objectAmount - 1;
					for (int j = 0; j < currentGenome.g_lStateProperties[5].size(); j++) {
						currentGenome.g_objectAmount++;
						currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
						currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
						createArtificialLeaf(0.1, 0.1, 0.01, a[5][j][3], a[5][j][4], a[5][j][5],
							a[5][j][7], a[5][j][8], a[5][j][9], 0, parent, currentGenome.g_objectAmount - 1);
						float color[3] = { 0.9, 0.9, 0.9 };
						simSetShapeColor(currentGenome.g_objectHandles[currentGenome.g_objectAmount - 1], NULL, sim_colorcomponent_ambient_diffuse, color);
					}
				}

				bool complexL = true;
				if (complexL == true) {
					// continue object creation
					int maxIt = currentGenome.g_objectAmount - 1;
					for (int j = 0; j < maxIt; j++ && currentGenome.g_objectAmount < 100) {
						int objIndex = 0;
						int childHandle = simGetObjectChild(currentGenome.g_objectHandles[j], objIndex++);
						if (childHandle == -1) {
							if (currentGenome.g_objectType[j] == 0) {
								modularLGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, j);
							//	lGrowth(currentGenome.g_lStateProperties[0].size(), 0, 1, j);
							}
							else if (currentGenome.g_objectType[j] == 1) {
								modularLGrowth(currentGenome.g_lStateProperties[0].size(), 1, 2, j);
								//	lGrowth(currentGenome.g_lStateProperties[1].size(), 1, 2, j);
							}
							else if (currentGenome.g_objectType[j] == 2) {
								modularLGrowth(currentGenome.g_lStateProperties[0].size(), 2, 3, j);
								//	lGrowth(currentGenome.g_lStateProperties[2].size(), 2, 3, j);
							}
							else if (currentGenome.g_objectType[j] == 3) {
								modularLGrowth(currentGenome.g_lStateProperties[0].size(), 3, 4, j);
								//	lGrowth(currentGenome.g_lStateProperties[3].size(), 3, 4, j);
							}
							else if (currentGenome.g_objectType[j] == 4) {
								modularLGrowth(currentGenome.g_lStateProperties[0].size(), 4, 5, j);
								//	lGrowth(currentGenome.g_lStateProperties[4].size(), 4, 1, j);
							}
							else if (currentGenome.g_objectType[j] == 5) {
							}
						}
					}
				}

				for (int j = 0; j < currentGenome.g_objectAmount; j++) {
					for (int k = 0; k < currentGenome.g_lStateProperties.size(); k++){
						if (currentGenome.g_objectType[j] == k) {
							float color[3];
							if (k == 0)	{
								color[0] = 0.1;
								color[1] = 0.1;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 1) {
								color[0] = 0.8;
								color[1] = 0.1;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 2){
								color[0] = 0.1;
								color[1] = 0.8;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 3) {
								color[0] = 0.1;
								color[1] = 0.1;
								color[2] = 0.8;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 4){
								color[0] = 0.8;
								color[1] = 0.8;
								color[2] = 0.1;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
							else if (k == 5) {
								color[0] = 0.8;
								color[1] = 0.1;
								color[2] = 0.8;
								simSetShapeColor(currentGenome.g_objectHandles[j], NULL, sim_colorcomponent_ambient_diffuse, color);
							}
						}
					}
				}
			}
			else {
				cout << "STRUCTURE TOO LARGE!" << endl;
			}
		}
	}
}


void CENN::defaultPlantNN() {
	DefaultGenome *individualGenome;
	// CIndividualGenome 
	for (int i = 0; i < s_initialPopulationSize; i++) {
		populationFitness.resize(s_initialPopulationSize);

		unique_ptr<DefaultGenome> individualGenome(new DefaultGenome);
//		individualGenome = new DefaultGenome;
		individualGenome->individualNumber = i;
		// give every genome a creature blueprint, should have a if createCreature is used function
		//		individualGenome->storeAgentBlueprint(mainHandleName, s_objectPosition, s_objectOrientation, s_objectSizes, s_objectMass, s_objectParent, s_objectType, 
		//			s_jointPosition, s_jointOrientation, s_jointSizes, s_jointMass, s_jointParent, s_jointType, s_jointMaxMinAngles, s_jointMaxForce, s_jointMaxVelocity);
		//		individualGenome->amountInterNeurons = 2;
		//		individualGenome->amountOutputNeurons = 8;
		// create a default neat based NN? not now, simpler even;

		//++++++++++++++++++++++++++++++++++++++++++++++++

		// plant pars are based on amount joints available
		//	int countJoints = 0;
		//	simAddObjectToSelection(sim_handle_tree, mainHandle);
		//	for (int j = 0; j < simGetObjectSelectionSize(); j++) {
		//		if (simGetObject)
		//	}
		//	s_amountInputNeurons = currentGenome
		currentGenome.g_objectHandles.resize(1);
		currentGenome.g_objectHandles[0] = mainHandle[0];

		currentGenome.g_objectAmount = 1;

		s_amountSenseParts = 0;
		s_totalInput = currentGenome.g_jointHandles.size();
		s_amountJoints = currentGenome.g_jointHandles.size();
		individualGenome->g_amountSenseParts = s_amountSenseParts;
		s_amountInputNeurons = currentGenome.g_jointHandles.size();
		individualGenome->g_amountInputNeurons = s_amountInputNeurons;

		individualGenome->g_amountInterNeurons = s_amountInterNeurons;
		s_amountOutputNeurons = currentGenome.g_jointHandles.size();
		individualGenome->g_amountOutputNeurons = s_amountOutputNeurons;
		individualGenome->g_amountPatternGenerators = s_amountPatternGenerators;

		int defaultAmountNeurons = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators;
		individualGenome->g_amountNeurons = defaultAmountNeurons;

		vector<int> defaultInputNeuronConnectionAmount;
		vector<int> defaultInterNeuronConnectionAmount;
		vector<int> defaultOutputNeuronConnectionAmount;
		vector<int> defaultCPGConnectionAmount;
		vector<int> defaultInputNeuronToSense;
		vector<float> defaultSenseInput;

		defaultInputNeuronConnectionAmount.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			defaultInputNeuronConnectionAmount[i] = 1;
		}
		defaultInterNeuronConnectionAmount.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++) {
			defaultInterNeuronConnectionAmount[i] = 5;
		}
		defaultOutputNeuronConnectionAmount.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++) {
			defaultOutputNeuronConnectionAmount[i] = 1;
		}
		defaultCPGConnectionAmount.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++) {
			defaultCPGConnectionAmount[i] = 1;
		}
		defaultInputNeuronToSense.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			if (i < s_totalInput) {
				defaultInputNeuronToSense[i] = i;
			}
			else {
				cout << " - Note: there are more input neurons than inputs meaning that some input neurons are not connected to any sensors" << endl;
			}
		}


		individualGenome->g_inputNeuronConnectionAmount = defaultInputNeuronConnectionAmount;
		individualGenome->g_interNeuronConnectionAmount = defaultInterNeuronConnectionAmount;
		individualGenome->g_outputNeuronConnectionAmount = defaultOutputNeuronConnectionAmount;
		individualGenome->g_patternGeneratorConnectionAmount = defaultCPGConnectionAmount;
		individualGenome->g_inputNeuronToInputSense = defaultInputNeuronToSense;


		// real initialization begins

		// input
		vector<vector<int>> defaultInputNeuronConnections;
		defaultInputNeuronConnections.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronConnections[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "it happened" << endl;
				}
				else {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + i;
				}
		}

		individualGenome->g_inputNeuronConnections = defaultInputNeuronConnections;
		//	cout << "inputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInputNeuronWeights;
		defaultInputNeuronWeights.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronWeights[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
			{
				defaultInputNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialInputNeuronWeights = defaultInputNeuronWeights;
		individualGenome->g_inputNeuronWeights = defaultInputNeuronWeights;

		vector<float> defaultInputThreshold;
		defaultInputThreshold.resize(s_amountInputNeurons);
		for (size_t i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputThreshold[i] = -0.2;
		}
		individualGenome->g_inputActivationThreshold = defaultInputThreshold;

		// inter
		vector<vector<int>> defaultInterNeuronConnections;
		defaultInterNeuronConnections.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronConnections[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons +
						s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "It happened again" << endl;
				}
				else {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons + i + j;
				}
			}
		}

		individualGenome->g_interNeuronConnections = defaultInterNeuronConnections;
		//	cout << "interNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInterNeuronWeights;
		defaultInterNeuronWeights.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronWeights[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				defaultInterNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialInterNeuronWeights = defaultInterNeuronWeights;
		individualGenome->g_interNeuronWeights = defaultInterNeuronWeights;

		vector<float> defaultInterThreshold;
		defaultInterThreshold.resize(s_amountInterNeurons);
		for (size_t i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterThreshold[i] = -0.2;
		}
		individualGenome->g_interNeuronActivationThresholds = defaultInterThreshold;
		//output
		vector<vector<int>> defaultOutputNeuronConnections;
		defaultOutputNeuronConnections.resize(s_amountOutputNeurons);

		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronConnections[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				if (i >= (s_amountJoints)+s_amountPatternGenerators) { // *3 depending on control type 
					defaultOutputNeuronConnections[i][j] = s_amountJoints - 1;
					cout << "It happened to the joints" << endl;
				}
				else {
					defaultOutputNeuronConnections[i][j] = i;
				}
			}
		}

		individualGenome->g_outputNeuronConnections = defaultOutputNeuronConnections;
		//	cout << "outputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultOutputNeuronWeights;
		defaultOutputNeuronWeights.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronWeights[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				defaultOutputNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialOutputNeuronWeights = defaultOutputNeuronWeights;
		individualGenome->g_outputNeuronWeights = defaultOutputNeuronWeights;

		vector<float> defaultOutputThreshold;
		defaultOutputThreshold.resize(s_amountOutputNeurons);
		for (size_t i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputThreshold[i] = 0.2;
		}
		individualGenome->g_outputActivationThreshold = defaultOutputThreshold;

		// CPGs
		vector<vector<int>> defaultCPGConnections;
		defaultCPGConnections.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGConnections[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons) {
					defaultCPGConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons - 1;
					cout << "It happened to the CPGs" << endl;
				}
				else {
					defaultCPGConnections[i][j] = s_amountInputNeurons + i;
				}
			}
		}



		individualGenome->g_patternGeneratorConnections = defaultCPGConnections;
		//	cout << "CPGs just saved in an individual genome" << endl;

		vector<vector<float>> defaultCPGWeights;
		defaultCPGWeights.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGWeights[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				defaultCPGWeights[i][j] = 0.1;
			}
		}
		individualGenome->g_initialPatternGenerationWeights = defaultCPGWeights;
		individualGenome->g_patternGenerationWeights = defaultCPGWeights;

		vector<vector<float>> defaultPatternGenerationTime;
		defaultPatternGenerationTime.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationTime[i].resize(2);
			defaultPatternGenerationTime[i][0] = 0.5;
			defaultPatternGenerationTime[i][1] = 0.5;
		}
		individualGenome->g_initialPatternGenerationTime = defaultPatternGenerationTime;
		individualGenome->g_patternGenerationTime = defaultPatternGenerationTime;

		vector<int> defaultPatternGenerationType;
		defaultPatternGenerationType.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationType[i] = 1;
		}
		individualGenome->g_patternGenerationType = defaultPatternGenerationType;

		// initialize other parameters
		individualGenome->g_individualAge = 0;

		if (s_useSensors == true) {
			individualGenome->g_useSensors = true;
			if (s_useTactileSensing == true) {
				individualGenome->g_useTactileSensors = true;
			}
			else {
				individualGenome->g_useTactileSensors = false;
			}
			if (s_useVestibularSystem == true){
				individualGenome->g_useVestibularSystem = true;
			}
			else {
				individualGenome->g_useVestibularSystem = false;
			}
			if (s_useProprioception == true) {
				individualGenome->g_useProprioception = true;
			}
			else {
				individualGenome->g_useProprioception = false;
			}
		}
		else {
			individualGenome->g_useSensors = false;
			individualGenome->g_useProprioception = false;
			individualGenome->g_useTactileSensors = false;
			individualGenome->g_useVestibularSystem = false;
		}

		// specify where they start at
		individualGenome->g_proprioceptionStartsAt = s_amountSenseParts;
		individualGenome->g_tactileSensingStartsAt = 0;
		individualGenome->g_vestibularSystemStartsAt = s_amountSenseParts + s_amountJoints;

		// resize how acquired activationLevels 
		individualGenome->g_acquiredActivationLevels.resize(s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators);
		individualGenome->g_jointActivity.resize(i_jointAmount);

		//individualGenome->
		individualGenome->g_jointHandles = i_jointHandles;
		// morphology saver
		individualGenome->g_objectAmount = i_objectAmount;
		individualGenome->g_objectMass = i_objectMass;
		individualGenome->g_objectOrientation = i_objectOrientation;
		individualGenome->g_objectParent = i_objectParent;
		individualGenome->g_objectPosition = i_objectPosition;
		individualGenome->g_tactileObjects = i_tactileObjects;
		individualGenome->g_objectSizes = i_objectSizes;
		individualGenome->g_objectType = i_objectType;

		individualGenome->g_jointSizes = i_jointSizes;
		individualGenome->g_jointAmount = i_jointAmount;
		if (individualGenome->g_jointAmount < 0) {
			individualGenome->g_jointAmount = 0;
		}
		individualGenome->g_jointMass = i_jointMass;
		individualGenome->g_jointOrientation = i_jointOrientation;
		individualGenome->g_jointParent = i_jointParent;
		individualGenome->g_jointPosition = i_jointPosition;
		individualGenome->g_jointType = i_objectType;
		individualGenome->g_jointMaxForce = i_jointMaxForce;
		individualGenome->g_jointMaxVelocity = i_jointMaxVelocity;
		individualGenome->g_jointMaxMinAngles = i_jointMaxMinAngles;

		defaultSenseInput.resize(s_totalInput);

		for (int i = 0; i < s_totalInput; i++) {
			defaultSenseInput[i] = 0.0;
		}
		// mutationRate
		individualGenome->g_mutationRate = s_initialMutationRate;

		individualGenome->g_senseInput = defaultSenseInput;

		vector<vector<vector<float>>> defaultLStateProperties;
		// five different states
		defaultLStateProperties.resize(5);
		defaultLStateProperties[0].resize(1);
		defaultLStateProperties[1].resize(2);
		defaultLStateProperties[2].resize(1);
		defaultLStateProperties[3].resize(1);
		defaultLStateProperties[4].resize(1);
		for (int i = 0; i < defaultLStateProperties.size(); i++) {
			for (size_t j = 0; j < defaultLStateProperties[i].size(); j++)
			{
				//				cout << "1." << i << "." << j << endl;
				defaultLStateProperties[i][j].resize(14);
				for (size_t k = 0; k < 14; k++)
				{
					defaultLStateProperties[i][j][k] = 0.0f;
				}
				// set properties manually
				defaultLStateProperties[i][j][0] = 0.05; // length
				defaultLStateProperties[i][j][1] = 0.05; // width
				defaultLStateProperties[i][j][2] = 0.05; // height
				defaultLStateProperties[i][j][3] = 0; // x
				defaultLStateProperties[i][j][4] = 0; // y
				defaultLStateProperties[i][j][5] = 0.1; // z
				defaultLStateProperties[i][j][6] = 0.1; // rotX
				defaultLStateProperties[i][j][7] = 0.1; // rotY
				defaultLStateProperties[i][j][8] = 0; // rotZ
				defaultLStateProperties[i][j][9] = 0.0; // mass
				defaultLStateProperties[i][j][10] = 0.0;
				defaultLStateProperties[i][j][11] = 0.0; // color 1
				defaultLStateProperties[i][j][12] = 0.5; // color 2
				defaultLStateProperties[i][j][13] = 1.0; // color 3

			}
		}

		vector<bool> defaultLeafState;
		defaultLeafState.resize(5);
		for (int i = 0; i < 5; i++) {
			defaultLeafState[i] = false;
		}
		defaultLeafState[4] = true;
		//		cout << "test access of defaultStateProperties [0][1][0]: " << defaultLStateProperties[1][1][0] << endl;

		individualGenome->g_lStateProperties = defaultLStateProperties;

		// saving the tactile objects if present
		if (s_useTactileSensing == true) {
			// separate ;
			string partNames = s_specifiedPartNames;
			stringstream partString(partNames);
			int partCount = 0;
			int tactileParts = 0;
			//			cout << "Lines here please: " << endl;
			while (partString.good()) {
				string singlePart;
				getline(partString, singlePart, ';');
				partCount++;
				//i_tactileObjects.resize(partCount);
				int tempHandle;
				const char* charPart = singlePart.c_str();
				tempHandle = simGetObjectHandle(charPart);
				//				cout << "tempHandle " << tempHandle << ", charPartHandle " << simGetObjectHandle(charPart)
				//					<< ", Charpart: " << charPart << ", objectAmount " << currentGenome.g_objectAmount << endl;
				for (int i = 0; i < currentGenome.g_objectAmount; i++) {
					if (currentGenome.g_objectHandles[i] == tempHandle) {
						//						cout << "The tactile handle is found and saved in the individual" << endl;
						tactileParts++;
						currentGenome.g_tactileObjects.resize(tactileParts);
						if (currentGenome.g_tactileObjects.size() < tactileParts) {
							//							cout << "Something went wrong with resizing the tactileObjects" << endl;
						}
						//						cout << "CONFIRMING TACTILE SIZE: " << currentGenome.g_tactileObjects.size() << endl;
						currentGenome.g_tactileHandles.resize(tactileParts);
						currentGenome.g_tactileObjects[tactileParts - 1] = i;
						currentGenome.g_tactileHandles[tactileParts - 1] = tempHandle;
					}
				}
				if (tactileParts < partCount){
					cout << "tactileParts = " << tactileParts << ", partCount = " << partCount << endl;
					cout << "Note: more tactile object were given than were identified" << endl;
					cout << " -> check if the given object strings correspond with the tactile sensor object names of your agent" << endl;
				}
			}

			// save handles and objects in genome
			individualGenome->g_tactileObjects = currentGenome.g_tactileObjects;
		}

		// additional functionality is to be updated
		storedIndividualGenomes.push_back(move(individualGenome));
	}
	//	printLStateProperties(storedIndividualGenomes[0]);
	vector<unique_ptr<DefaultGenome>>::iterator it;
	//	vector<CIndividualGenome>::iterator it;

	for (it = storedIndividualGenomes.begin(); it != storedIndividualGenomes.end(); ++it) {
		//	it->printIndividualInfo();
	}
}

void CENN::defaultModuleNN() {
	DefaultGenome *individualGenome;
	// CIndividualGenome 
	for (int i = 0; i < s_initialPopulationSize; i++) {
		populationFitness.resize(s_initialPopulationSize);

		unique_ptr<DefaultGenome> individualGenome(new DefaultGenome);
		individualGenome->individualNumber = i;
		currentGenome.g_objectHandles.resize(1);
		currentGenome.g_objectHandles[0] = mainHandle[0];

		currentGenome.g_objectAmount = 1;

		s_amountSenseParts = 0;
		s_totalInput = currentGenome.g_jointHandles.size();
		s_amountJoints = currentGenome.g_jointHandles.size();
		individualGenome->g_amountSenseParts = s_amountSenseParts;
		s_amountInputNeurons = currentGenome.g_jointHandles.size();
		individualGenome->g_amountInputNeurons = s_amountInputNeurons;

		individualGenome->g_amountInterNeurons = s_amountInterNeurons;
		s_amountOutputNeurons = currentGenome.g_jointHandles.size();
		individualGenome->g_amountOutputNeurons = s_amountOutputNeurons;
		individualGenome->g_amountPatternGenerators = s_amountPatternGenerators;

		int defaultAmountNeurons = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators;
		individualGenome->g_amountNeurons = defaultAmountNeurons;

		vector<int> defaultInputNeuronConnectionAmount;
		vector<int> defaultInterNeuronConnectionAmount;
		vector<int> defaultOutputNeuronConnectionAmount;
		vector<int> defaultCPGConnectionAmount;
		vector<int> defaultInputNeuronToSense;
		vector<float> defaultSenseInput;

		defaultInputNeuronConnectionAmount.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			defaultInputNeuronConnectionAmount[i] = 1;
		}
		defaultInterNeuronConnectionAmount.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++) {
			defaultInterNeuronConnectionAmount[i] = 5;
		}
		defaultOutputNeuronConnectionAmount.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++) {
			defaultOutputNeuronConnectionAmount[i] = 1;
		}
		defaultCPGConnectionAmount.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++) {
			defaultCPGConnectionAmount[i] = 1;
		}
		defaultInputNeuronToSense.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			if (i < s_totalInput) {
				defaultInputNeuronToSense[i] = i;
			}
			else {
				cout << " - Note: there are more input neurons than inputs meaning that some input neurons are not connected to any sensors" << endl;
			}
		}


		individualGenome->g_inputNeuronConnectionAmount = defaultInputNeuronConnectionAmount;
		individualGenome->g_interNeuronConnectionAmount = defaultInterNeuronConnectionAmount;
		individualGenome->g_outputNeuronConnectionAmount = defaultOutputNeuronConnectionAmount;
		individualGenome->g_patternGeneratorConnectionAmount = defaultCPGConnectionAmount;
		individualGenome->g_inputNeuronToInputSense = defaultInputNeuronToSense;


		// real initialization begins

		// input
		vector<vector<int>> defaultInputNeuronConnections;
		defaultInputNeuronConnections.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronConnections[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "it happened" << endl;
				}
				else {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + i;
				}
		}

		individualGenome->g_inputNeuronConnections = defaultInputNeuronConnections;
		//	cout << "inputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInputNeuronWeights;
		defaultInputNeuronWeights.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronWeights[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
			{
				defaultInputNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialInputNeuronWeights = defaultInputNeuronWeights;
		individualGenome->g_inputNeuronWeights = defaultInputNeuronWeights;

		vector<float> defaultInputThreshold;
		defaultInputThreshold.resize(s_amountInputNeurons);
		for (size_t i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputThreshold[i] = -0.2;
		}
		individualGenome->g_inputActivationThreshold = defaultInputThreshold;

		// inter
		vector<vector<int>> defaultInterNeuronConnections;
		defaultInterNeuronConnections.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronConnections[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons +
						s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "It happened again" << endl;
				}
				else {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons + i + j;
				}
			}
		}

		individualGenome->g_interNeuronConnections = defaultInterNeuronConnections;
		//	cout << "interNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInterNeuronWeights;
		defaultInterNeuronWeights.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronWeights[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				defaultInterNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialInterNeuronWeights = defaultInterNeuronWeights;
		individualGenome->g_interNeuronWeights = defaultInterNeuronWeights;

		vector<float> defaultInterThreshold;
		defaultInterThreshold.resize(s_amountInterNeurons);
		for (size_t i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterThreshold[i] = -0.2;
		}
		individualGenome->g_interNeuronActivationThresholds = defaultInterThreshold;
		//output
		vector<vector<int>> defaultOutputNeuronConnections;
		defaultOutputNeuronConnections.resize(s_amountOutputNeurons);

		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronConnections[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				if (i >= (s_amountJoints)+s_amountPatternGenerators) { // *3 depending on control type 
					defaultOutputNeuronConnections[i][j] = s_amountJoints - 1;
					cout << "It happened to the joints" << endl;
				}
				else {
					defaultOutputNeuronConnections[i][j] = i;
				}
			}
		}

		individualGenome->g_outputNeuronConnections = defaultOutputNeuronConnections;
		//	cout << "outputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultOutputNeuronWeights;
		defaultOutputNeuronWeights.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronWeights[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				defaultOutputNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialOutputNeuronWeights = defaultOutputNeuronWeights;
		individualGenome->g_outputNeuronWeights = defaultOutputNeuronWeights;

		vector<float> defaultOutputThreshold;
		defaultOutputThreshold.resize(s_amountOutputNeurons);
		for (size_t i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputThreshold[i] = 0.2;
		}
		individualGenome->g_outputActivationThreshold = defaultOutputThreshold;

		// CPGs
		vector<vector<int>> defaultCPGConnections;
		defaultCPGConnections.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGConnections[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons) {
					defaultCPGConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons - 1;
					cout << "It happened to the CPGs" << endl;
				}
				else {
					defaultCPGConnections[i][j] = s_amountInputNeurons + i;
				}
			}
		}



		individualGenome->g_patternGeneratorConnections = defaultCPGConnections;
		//	cout << "CPGs just saved in an individual genome" << endl;

		vector<vector<float>> defaultCPGWeights;
		defaultCPGWeights.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGWeights[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				defaultCPGWeights[i][j] = 0.1;
			}
		}
		individualGenome->g_initialPatternGenerationWeights = defaultCPGWeights;
		individualGenome->g_patternGenerationWeights = defaultCPGWeights;

		vector<vector<float>> defaultPatternGenerationTime;
		defaultPatternGenerationTime.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationTime[i].resize(2);
			defaultPatternGenerationTime[i][0] = 0.5;
			defaultPatternGenerationTime[i][1] = 0.5;
		}
		individualGenome->g_initialPatternGenerationTime = defaultPatternGenerationTime;
		individualGenome->g_patternGenerationTime = defaultPatternGenerationTime;

		vector<int> defaultPatternGenerationType;
		defaultPatternGenerationType.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationType[i] = 1;
		}
		individualGenome->g_patternGenerationType = defaultPatternGenerationType;

		// initialize other parameters
		individualGenome->g_individualAge = 0;

		if (s_useSensors == true) {
			individualGenome->g_useSensors = true;
			if (s_useTactileSensing == true) {
				individualGenome->g_useTactileSensors = true;
			}
			else {
				individualGenome->g_useTactileSensors = false;
			}
			if (s_useVestibularSystem == true){
				individualGenome->g_useVestibularSystem = true;
			}
			else {
				individualGenome->g_useVestibularSystem = false;
			}
			if (s_useProprioception == true) {
				individualGenome->g_useProprioception = true;
			}
			else {
				individualGenome->g_useProprioception = false;
			}
		}
		else {
			individualGenome->g_useSensors = false;
			individualGenome->g_useProprioception = false;
			individualGenome->g_useTactileSensors = false;
			individualGenome->g_useVestibularSystem = false;
		}

		// specify where they start at
		individualGenome->g_proprioceptionStartsAt = s_amountSenseParts;
		individualGenome->g_tactileSensingStartsAt = 0;
		individualGenome->g_vestibularSystemStartsAt = s_amountSenseParts + s_amountJoints;

		// resize how acquired activationLevels 
		individualGenome->g_acquiredActivationLevels.resize(s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators);
		individualGenome->g_jointActivity.resize(i_jointAmount);

		//individualGenome->
		individualGenome->g_jointHandles = i_jointHandles;
		// morphology saver
		individualGenome->g_objectAmount = i_objectAmount;
		individualGenome->g_objectMass = i_objectMass;
		individualGenome->g_objectOrientation = i_objectOrientation;
		individualGenome->g_objectParent = i_objectParent;
		individualGenome->g_objectPosition = i_objectPosition;
		individualGenome->g_tactileObjects = i_tactileObjects;
		individualGenome->g_objectSizes = i_objectSizes;
		individualGenome->g_objectType = i_objectType;

		individualGenome->g_jointSizes = i_jointSizes;
		individualGenome->g_jointAmount = i_jointAmount;
		if (individualGenome->g_jointAmount < 0) {
			individualGenome->g_jointAmount = 0;
		}
		individualGenome->g_jointMass = i_jointMass;
		individualGenome->g_jointOrientation = i_jointOrientation;
		individualGenome->g_jointParent = i_jointParent;
		individualGenome->g_jointPosition = i_jointPosition;
		individualGenome->g_jointType = i_objectType;
		individualGenome->g_jointMaxForce = i_jointMaxForce;
		individualGenome->g_jointMaxVelocity = i_jointMaxVelocity;
		individualGenome->g_jointMaxMinAngles = i_jointMaxMinAngles;

		defaultSenseInput.resize(s_totalInput);

		for (int i = 0; i < s_totalInput; i++) {
			defaultSenseInput[i] = 0.0;
		}
		// mutationRate
		individualGenome->g_mutationRate = s_initialMutationRate;

		individualGenome->g_senseInput = defaultSenseInput;

		vector<vector<vector<float>>> defaultLStateProperties;
		// five different states
		defaultLStateProperties.resize(5);
		defaultLStateProperties[0].resize(1);
		defaultLStateProperties[1].resize(2);
		defaultLStateProperties[2].resize(1);
		defaultLStateProperties[3].resize(1);
		defaultLStateProperties[4].resize(1);
		for (int i = 0; i < defaultLStateProperties.size(); i++) {
			for (size_t j = 0; j < defaultLStateProperties[i].size(); j++)
			{
				//				cout << "1." << i << "." << j << endl;
				defaultLStateProperties[i][j].resize(14);
				for (size_t k = 0; k < 14; k++)
				{
					defaultLStateProperties[i][j][k] = 0.0f;
				}
				// set properties manually
				defaultLStateProperties[i][j][0] = 0.05; // length
				defaultLStateProperties[i][j][1] = 0.05; // width
				defaultLStateProperties[i][j][2] = 0.05; // height
				defaultLStateProperties[i][j][3] = 0; // x
				defaultLStateProperties[i][j][4] = 0; // y
				defaultLStateProperties[i][j][5] = 0.1; // z
				defaultLStateProperties[i][j][6] = 0.0; // rotX
				defaultLStateProperties[i][j][7] = 0.0; // rotY
				defaultLStateProperties[i][j][8] = 0; // rotZ
				defaultLStateProperties[i][j][9] = 0.0; // mass
				defaultLStateProperties[i][j][10] = 0.0;
				defaultLStateProperties[i][j][11] = 0.0; // color 1
				defaultLStateProperties[i][j][12] = 0.5; // color 2
				defaultLStateProperties[i][j][13] = 1.0; // color 3

			}
		}

		vector<bool> defaultLeafState;
		defaultLeafState.resize(5);
		for (int i = 0; i < 5; i++) {
			defaultLeafState[i] = false;
		}
		defaultLeafState[4] = true;
		//		cout << "test access of defaultStateProperties [0][1][0]: " << defaultLStateProperties[1][1][0] << endl;

		individualGenome->g_lStateProperties = defaultLStateProperties;

		// saving the tactile objects if present
		if (s_useTactileSensing == true) {
			// separate ;
			string partNames = s_specifiedPartNames;
			stringstream partString(partNames);
			int partCount = 0;
			int tactileParts = 0;
			//			cout << "Lines here please: " << endl;
			while (partString.good()) {
				string singlePart;
				getline(partString, singlePart, ';');
				partCount++;
				//i_tactileObjects.resize(partCount);
				int tempHandle;
				const char* charPart = singlePart.c_str();
				tempHandle = simGetObjectHandle(charPart);
				//				cout << "tempHandle " << tempHandle << ", charPartHandle " << simGetObjectHandle(charPart)
				//					<< ", Charpart: " << charPart << ", objectAmount " << currentGenome.g_objectAmount << endl;
				for (int i = 0; i < currentGenome.g_objectAmount; i++) {
					if (currentGenome.g_objectHandles[i] == tempHandle) {
						//						cout << "The tactile handle is found and saved in the individual" << endl;
						tactileParts++;
						currentGenome.g_tactileObjects.resize(tactileParts);
						if (currentGenome.g_tactileObjects.size() < tactileParts) {
							//							cout << "Something went wrong with resizing the tactileObjects" << endl;
						}
						//						cout << "CONFIRMING TACTILE SIZE: " << currentGenome.g_tactileObjects.size() << endl;
						currentGenome.g_tactileHandles.resize(tactileParts);
						currentGenome.g_tactileObjects[tactileParts - 1] = i;
						currentGenome.g_tactileHandles[tactileParts - 1] = tempHandle;
					}
				}
				if (tactileParts < partCount){
					cout << "tactileParts = " << tactileParts << ", partCount = " << partCount << endl;
					cout << "Note: more tactile object were given than were identified" << endl;
					cout << " -> check if the given object strings correspond with the tactile sensor object names of your agent" << endl;
				}
			}

			// save handles and objects in genome
			individualGenome->g_tactileObjects = currentGenome.g_tactileObjects;
		}

		// additional functionality is to be updated
		storedIndividualGenomes.push_back(move(individualGenome));
	}
	//	printLStateProperties(storedIndividualGenomes[0]);
	vector<unique_ptr<DefaultGenome>>::iterator it;
	//	vector<CIndividualGenome>::iterator it;

	for (it = storedIndividualGenomes.begin(); it != storedIndividualGenomes.end(); ++it) {
		//	it->printIndividualInfo();
	}
}

void CENN::printMorphologyParameters(DefaultGenome individual){
	//void CENN::printMorphologyParameters(CIndividualGenome individual){
		//after l system or other morphology is used you can save the morphology using this function
	// not being used, function can be deleted
	// mainHandle
	mainHandleName = simGetObjectName(mainHandle[0]);
	cout << "The agent's main handle name = " << simGetObjectName(mainHandle[0]) << endl;
	int amountObjectsInTree[1];
	simGetObjectsInTree(mainHandle[0], sim_object_shape_type, 0, amountObjectsInTree);
	cout << "amountObjectsInTree = " << amountObjectsInTree[0] << endl;
	int amountJointsInTree[1];
	simGetObjectsInTree(mainHandle[0], sim_object_joint_type, 0, amountJointsInTree);
	cout << "amountJointsInTree = " << amountJointsInTree[0] << endl;

	cout << "Object Handles" << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << individual.g_objectHandles[i] << ", ";
	} cout << endl;
	cout << "ObjectPositions: " << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << "::";
		for (int j = 0; j < 3; j++) {
			cout << individual.g_objectPosition[i][j] << ", ";
		} cout << endl;
	} cout << endl;
	
	cout << "Object Orientations: " << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << "::";
		for (int j = 0; j < 3; j++) {
			cout << individual.g_objectOrientation[i][j] << ", ";
		} cout << endl;
	} cout << endl;

	cout << "Object Sizes: " << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << "::";
		for (int j = 0; j < 3; j++) {
			cout << individual.g_objectSizes[i][j] << ", ";
		} cout << endl;
	} cout << endl;

	cout << "Object Mass: " << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << individual.g_objectMass[i] << ", ";
	} cout << endl;

	cout << "Object Parents: " << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << individual.g_objectParent[i] << ", ";
	} cout << endl;

	cout << "Object Type: " << endl;
	for (int i = 0; i < amountObjectsInTree[0]; i++) {
		cout << individual.g_objectType[i] << ", ";
	} cout << endl;


	cout << "Joint Handles" << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << individual.g_jointHandles[i] << ", ";
	} cout << endl;
	cout << "JointPositions: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << "::";
		for (int j = 0; j < 3; j++) {
			cout << individual.g_jointPosition[i][j] << ", ";
		} cout << endl;
	} cout << endl;

	cout << "Joint Orientations: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << "::";
		for (int j = 0; j < 3; j++) {
			cout << individual.g_jointOrientation[i][j] << ", ";
		} cout << endl;
	} cout << endl;

	cout << "Joint Sizes: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << "::";
		for (int j = 0; j < 3; j++) {
			cout << individual.g_jointSizes[i][j] << ", ";
		} cout << endl;
	} cout << endl;

	cout << "Joint Mass: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << individual.g_jointMass[i] << ", ";
	} cout << endl;

	cout << "Joint Parents: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << individual.g_jointParent[i] << ", ";
	} cout << endl;

	cout << "Joint Type: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << individual.g_jointType[i] << ", ";
	} cout << endl;
	cout << "Joint MaxMinAngles: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		for (int j = 0; j < 2; j++) {
			cout << individual.g_jointMaxMinAngles[i][j] << ", ";
		}
	} cout << endl;
	cout << "Joint maxVelocity: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << individual.g_jointMaxVelocity[i] << ", ";
	}	cout << endl;
	cout << "Joint maxForce: " << endl;
	for (int i = 0; i < amountJointsInTree[0]; i++) {
		cout << individual.g_jointMaxForce[i] << ", ";
	} cout << endl;


	for (int i = 0; i < amountJointsInTree[0]; i++) {

	}


	//	cout << "s_amountJoints = " << s_amountJoints << endl;
	individual.g_objectAmount = amountObjectsInTree[0];
	individual.g_jointAmount = amountJointsInTree[0];
	cout << "objAmount and JointAmount = " << individual.g_objectAmount << " and " << individual.g_jointAmount << endl; 
	cout << "in currentGenome this is: " << currentGenome.g_objectAmount << " and " << currentGenome.g_jointAmount << endl;



	// also for debugging
	// load the amount of joints and save them in CENN so they can be easily accessed. 
	//int jointAm[1];
/*	int	objAm[1];
	simGetObjectsInTree(mainHandle, 1, sim_handle_all, objAm);
	cout << "objAmount = " << objAm[0] << endl;
	for (size_t i = 0; i < objAm[0]; i++)
	{

	}
	simAddObjectToSelection(sim_handle_tree, mainHandle);
	int selectionSize = simGetObjectSelectionSize();
	vector<int> objectHandles;
	objectHandles.resize(selectionSize);
	int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason

	//s_objectAmount = selectionSize;
	//	simGetObjectSelection(objectHandles);
	simGetObjectSelection(tempObjectHandles);
	int jointCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		objectHandles[i] = tempObjectHandles[i];
		if (simGetObjectType(objectHandles[i]) == 1) {
			currentGenome.g_jointHandles.resize(jointCounter + 1);
			if (s_jointControl == 0) {
				currentGenome.g_jointActivity.resize(jointCounter + 1);
			}
			else if (s_jointControl == 1){
				currentGenome.g_jointActivity.resize((jointCounter + 1) * 2);
			}
			else if (s_jointControl == 2){
				currentGenome.g_jointActivity.resize((jointCounter + 1) * 3);
			}
			else {
				cout << "ERROR: Joint control could not be specified" << endl;
			}
			currentGenome.g_jointHandles[jointCounter] = objectHandles[i];
			jointCounter++;
		}
	}
	int objectCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		if (simGetObjectType(objectHandles[i]) == 0) {
			currentGenome.g_objectHandles.resize(objectCounter + 1);
			currentGenome.g_objectHandles[objectCounter] = tempObjectHandles[i];
			objectCounter++;
		}
	}
	cout << "g_objectHandles.size = " << currentGenome.g_objectHandles.size() << endl;

	currentGenome.g_jointAmount = jointCounter;
	jointCounter = 0;
	objectCounter = 0;
	simRemoveObjectFromSelection(sim_handle_all, NULL);
	/**/
	cout << endl << endl;
}


void CENN::printLStateProperties(DefaultGenome individual){
//	void CENN::printLStateProperties(CIndividualGenome individual){

	cout << "lStateProperties: " << endl;
	cout << "Size: " << individual.g_lStateProperties.size() << endl;

	for (size_t i = 0; i < individual.g_lStateProperties.size(); i++) {
		cout << "Size[" << i << "]: " << individual.g_lStateProperties[i].size() << endl;
		for (size_t j = 0; j < individual.g_lStateProperties[i].size(); j++) {
			cout << "Size["<< i<< "][" << j<< "]: " << individual.g_lStateProperties[i][j].size() << endl;
			for (size_t k = 0; k < individual.g_lStateProperties[i][j].size(); k++) {
				cout << individual.g_lStateProperties[i][j][k] << " :" << i << j << k << ":, ";
			} cout << endl;
		} cout << endl;
	} cout << "done! " << endl;
}


void CENN::resetDefaultPlantNN() {
//	CIndividualGenome *individualGenome;
	
		// give every genome a creature blueprint, should have a if createCreature is used function
		//		currentGenomestoreAgentBlueprint(mainHandleName, s_objectPosition, s_objectOrientation, s_objectSizes, s_objectMass, s_objectParent, s_objectType, 
		//			s_jointPosition, s_jointOrientation, s_jointSizes, s_jointMass, s_jointParent, s_jointType, s_jointMaxMinAngles, s_jointMaxForce, s_jointMaxVelocity);
		//		currentGenomeamountInterNeurons = 2;
		//		currentGenomeamountOutputNeurons = 8;
		// create a default neat based NN? not now, simpler even;

		//++++++++++++++++++++++++++++++++++++++++++++++++

		// plant pars are based on amount joints available
		//	int countJoints = 0;
		//	simAddObjectToSelection(sim_handle_tree, mainHandle);
		//	for (int j = 0; j < simGetObjectSelectionSize(); j++) {
		//		if (simGetObject)
		//	}
		//	s_amountInputNeurons = currentGenome.
		s_amountJoints = currentGenome.g_jointAmount; 
//		cout << "amount joints: " << s_amountJoints << endl;
//		cout << "jointHandlessize = " << currentGenome.g_jointHandles.size() << endl;
		//	addNeuron2(3);
		cout << "amount of joints = " << s_amountJoints << endl;
		cout << "amountObjects = " << currentGenome.g_objectAmount << " or " << i_objectAmount << endl;

		s_amountSenseParts = 0;
		s_totalInput = currentGenome.g_jointHandles.size();
		s_amountJoints = currentGenome.g_jointHandles.size();
		currentGenome.g_amountSenseParts = s_amountSenseParts;
		s_amountInputNeurons = currentGenome.g_jointHandles.size();
		cout << endl << "amountInput: " << s_amountInputNeurons << endl << endl;
		currentGenome.g_amountInputNeurons = s_amountInputNeurons;
//		cout << "s_amountInputNeurons = " << s_amountInputNeurons << endl;
		currentGenome.g_amountInterNeurons = s_amountInterNeurons;
		s_amountOutputNeurons = currentGenome.g_jointHandles.size();
		currentGenome.g_amountOutputNeurons = s_amountOutputNeurons;
		currentGenome.g_amountPatternGenerators = s_amountPatternGenerators;
//		cout << "s_amountPatternGenerators = " << s_amountPatternGenerators << endl;
//		cout << "s_amountInterNeurons = " << s_amountInterNeurons << endl;
//		cout << "s_amountOutputNeurons = " << s_amountOutputNeurons << endl; 

		int defaultAmountNeurons = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators;
		currentGenome.g_amountNeurons = defaultAmountNeurons;
		cout << "1.0" << endl;
		vector<int> defaultInputNeuronConnectionAmount;
		vector<int> defaultInterNeuronConnectionAmount;
		vector<int> defaultOutputNeuronConnectionAmount;
		vector<int> defaultCPGConnectionAmount;
		vector<int> defaultInputNeuronToSense;
		vector<float> defaultSenseInput;

		defaultInputNeuronConnectionAmount.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			defaultInputNeuronConnectionAmount[i] = 1;
		}
		defaultInterNeuronConnectionAmount.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++) {
			defaultInterNeuronConnectionAmount[i] = 5;
		}
		defaultOutputNeuronConnectionAmount.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++) {
			defaultOutputNeuronConnectionAmount[i] = 1;
		}
		defaultCPGConnectionAmount.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++) {
			defaultCPGConnectionAmount[i] = 1;
		}
		defaultInputNeuronToSense.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			if (i < s_totalInput) {
				defaultInputNeuronToSense[i] = i;
			}
			else {
				cout << " - Note: there are more input neurons than inputs meaning that some input neurons are not connected to any sensors" << endl;
			}
		}


		currentGenome.g_inputNeuronConnectionAmount = defaultInputNeuronConnectionAmount;
		currentGenome.g_interNeuronConnectionAmount = defaultInterNeuronConnectionAmount;
		currentGenome.g_outputNeuronConnectionAmount = defaultOutputNeuronConnectionAmount;
		currentGenome.g_patternGeneratorConnectionAmount = defaultCPGConnectionAmount;
		currentGenome.g_inputNeuronToInputSense = defaultInputNeuronToSense;


		// real initialization begins

		// input
		vector<vector<int>> defaultInputNeuronConnections;
		defaultInputNeuronConnections.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronConnections[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "it happened" << endl;
				}
				else {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + i;
				}
		}

		currentGenome.g_inputNeuronConnections = defaultInputNeuronConnections;
		//	cout << "inputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInputNeuronWeights;
		defaultInputNeuronWeights.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronWeights[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
			{
				defaultInputNeuronWeights[i][j] = 0.5;
			}
		}
		currentGenome.g_initialInputNeuronWeights = defaultInputNeuronWeights;
		currentGenome.g_inputNeuronWeights = defaultInputNeuronWeights;

		vector<float> defaultInputThreshold;
		defaultInputThreshold.resize(s_amountInputNeurons);
		for (size_t i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputThreshold[i] = -0.2;
		}
		currentGenome.g_inputActivationThreshold = defaultInputThreshold;

		// inter
		vector<vector<int>> defaultInterNeuronConnections;
		defaultInterNeuronConnections.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronConnections[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				if (i + j >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons +
						s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "It happened again" << endl;
				}
				else {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons + i + j;
				}
			}
		}

		currentGenome.g_interNeuronConnections = defaultInterNeuronConnections;
		//	cout << "interNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInterNeuronWeights;
		defaultInterNeuronWeights.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronWeights[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				defaultInterNeuronWeights[i][j] = 0.5;
			}
		}
		currentGenome.g_initialInterNeuronWeights = defaultInterNeuronWeights;
		currentGenome.g_interNeuronWeights = defaultInterNeuronWeights;

		vector<float> defaultInterThreshold;
		defaultInterThreshold.resize(s_amountInterNeurons);
		for (size_t i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterThreshold[i] = -0.2;
		}
		currentGenome.g_interNeuronActivationThresholds = defaultInterThreshold;
		//output
		vector<vector<int>> defaultOutputNeuronConnections;
		defaultOutputNeuronConnections.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronConnections[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				if (i >= (s_amountJoints)+s_amountPatternGenerators) { // *3 depending on control type 
					defaultOutputNeuronConnections[i][j] = s_amountJoints - 1;
					cout << "It happened to the joints" << endl;
				}
				else {
					defaultOutputNeuronConnections[i][j] = i;
				}
			}
		}

		currentGenome.g_outputNeuronConnections = defaultOutputNeuronConnections;
		//	cout << "outputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultOutputNeuronWeights;
		defaultOutputNeuronWeights.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronWeights[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				defaultOutputNeuronWeights[i][j] = 0.5;
			}
		}
		currentGenome.g_initialOutputNeuronWeights = defaultOutputNeuronWeights;
		currentGenome.g_outputNeuronWeights = defaultOutputNeuronWeights;

		vector<float> defaultOutputThreshold;
		defaultOutputThreshold.resize(s_amountOutputNeurons);
		for (size_t i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputThreshold[i] = 0.2;
		}
		currentGenome.g_outputActivationThreshold = defaultOutputThreshold;

		// CPGs
		vector<vector<int>> defaultCPGConnections;
		defaultCPGConnections.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGConnections[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons) {
					defaultCPGConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons - 1;
					cout << "It happened to the CPGs" << endl;
				}
				else {
					defaultCPGConnections[i][j] = s_amountInputNeurons + i;
				}
			}
		}



		currentGenome.g_patternGeneratorConnections = defaultCPGConnections;
		//	cout << "CPGs just saved in an individual genome" << endl;

		vector<vector<float>> defaultCPGWeights;
		defaultCPGWeights.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGWeights[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				defaultCPGWeights[i][j] = 0.1;
			}
		}
		currentGenome.g_initialPatternGenerationWeights = defaultCPGWeights;
		currentGenome.g_patternGenerationWeights = defaultCPGWeights;

		vector<vector<float>> defaultPatternGenerationTime;
		defaultPatternGenerationTime.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationTime[i].resize(2);
			defaultPatternGenerationTime[i][0] = 0.5;
			defaultPatternGenerationTime[i][1] = 0.5;
		}
		currentGenome.g_initialPatternGenerationTime = defaultPatternGenerationTime;
		currentGenome.g_patternGenerationTime = defaultPatternGenerationTime;

		vector<int> defaultPatternGenerationType;
		defaultPatternGenerationType.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationType[i] = 1;
		}
		currentGenome.g_patternGenerationType = defaultPatternGenerationType;

		// initialize other parameters
		currentGenome.g_individualAge = 0;

		if (s_useSensors == true) {
			currentGenome.g_useSensors = true;
			if (s_useTactileSensing == true) {
				currentGenome.g_useTactileSensors = true;
			}
			else {
				currentGenome.g_useTactileSensors = false;
			}
			if (s_useVestibularSystem == true){
				currentGenome.g_useVestibularSystem = true;
			}
			else {
				currentGenome.g_useVestibularSystem = false;
			}
			if (s_useProprioception == true) {
				currentGenome.g_useProprioception = true;
			}
			else {
				currentGenome.g_useProprioception = false;
			}
		}
		else {
			currentGenome.g_useSensors = false;
			currentGenome.g_useProprioception = false;
			currentGenome.g_useTactileSensors = false;
			currentGenome.g_useVestibularSystem = false;
		}

		// specify where they start at
		currentGenome.g_proprioceptionStartsAt = s_amountSenseParts;
		currentGenome.g_tactileSensingStartsAt = 0;
		currentGenome.g_vestibularSystemStartsAt = s_amountSenseParts + s_amountJoints;

		// resize how acquired activationLevels 
		currentGenome.g_acquiredActivationLevels.resize(s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators);
		currentGenome.g_jointActivity.resize(i_jointAmount);

		//currentGenome.
	//	currentGenome.g_jointHandles = i_jointHandles;
		// morphology saver
		i_objectAmount = currentGenome.g_objectAmount;
		// minor debugging
	//	printMorphologyParameters(currentGenome);
	//	printLStateProperties(currentGenome);



//		currentGenome.g_objectAmount = i_objectAmount;
//		currentGenome.g_objectMass = i_objectMass;
//		currentGenome.g_objectOrientation = i_objectOrientation;
//		currentGenome.g_objectParent = i_objectParent;
//		currentGenome.g_objectPosition = i_objectPosition;
//		currentGenome.g_tactileObjects = i_tactileObjects;
//		currentGenome.g_objectSizes = i_objectSizes;
//		currentGenome.g_objectType = i_objectType;

//		currentGenome.g_jointSizes = i_jointSizes;
//		currentGenome.g_jointAmount = i_jointAmount;
		if (currentGenome.g_jointAmount < 0) {
			currentGenome.g_jointAmount = 0;
		}
//		currentGenome.g_jointMass = i_jointMass;
//		currentGenome.g_jointOrientation = i_jointOrientation;
//		currentGenome.g_jointParent = i_jointParent;
//		currentGenome.g_jointPosition = i_jointPosition;
//		currentGenome.g_jointType = i_objectType;
//		currentGenome.g_jointMaxForce = i_jointMaxForce;
//		currentGenome.g_jointMaxVelocity = i_jointMaxVelocity;
//		currentGenome.g_jointMaxMinAngles = i_jointMaxMinAngles;

		defaultSenseInput.resize(currentGenome.g_jointHandles.size());
		for (int i = 0; i < currentGenome.g_jointHandles.size(); i++) {
			defaultSenseInput[i] = 0.0;
		}
		// mutationRate
		currentGenome.g_mutationRate = s_initialMutationRate;
		currentGenome.g_senseInput = defaultSenseInput;
		vector<vector<vector<float>>> defaultLStateProperties;
		// five different states
	//	cout << "next is commented out " << endl;
	//	printMorphologyParameters(currentGenome);
		/*

		defaultLStateProperties.resize(6);
		defaultLStateProperties[0].resize(1);
		defaultLStateProperties[1].resize(2);
		defaultLStateProperties[2].resize(1);
		defaultLStateProperties[3].resize(1);
		defaultLStateProperties[4].resize(1);
		defaultLStateProperties[5].resize(1);
		
		for (int i = 0; i < defaultLStateProperties.size(); i++) {
			for (size_t j = 0; j < defaultLStateProperties[i].size(); j++)
			{
			//	cout << "1." << i << "." << j << endl;
				defaultLStateProperties[i][j].resize(11);
				for (size_t k = 0; k < 11; k++)
				{
					defaultLStateProperties[i][j][k] = 0.0f;
				}
				// set properties manually
				defaultLStateProperties[i][j][0] = 0.1; // length
				defaultLStateProperties[i][j][1] = 0.1; // width
				defaultLStateProperties[i][j][2] = 0.1; // height
				defaultLStateProperties[i][j][3] = 0; // x
				defaultLStateProperties[i][j][4] = 0; // y
				defaultLStateProperties[i][j][5] = 0.1; // z
				defaultLStateProperties[i][j][6] = 0.1; // rotX
				defaultLStateProperties[i][j][7] = 0.1; // rotY
				defaultLStateProperties[i][j][8] = 0; // rotZ
				defaultLStateProperties[i][j][9] = 0.0; // mass
				defaultLStateProperties[i][j][10] = 0.0;
			}
		}

		vector<bool> defaultLeafState;
		defaultLeafState.resize(6);
		for (int i = 0; i < 6; i++) {
			defaultLeafState[i] = false;
		}
		defaultLeafState[4] = true;
		currentGenome.g_lStateProperties = defaultLStateProperties;
		*/

		// saving the tactile objects if present
		if (s_useTactileSensing == true) {
			// separate ;
			string partNames = s_specifiedPartNames;
			stringstream partString(partNames);
			int partCount = 0;
			int tactileParts = 0;
		while (partString.good()) {
				string singlePart;
				getline(partString, singlePart, ';');
				partCount++;
				//i_tactileObjects.resize(partCount);
				int tempHandle;
				const char* charPart = singlePart.c_str();
				tempHandle = simGetObjectHandle(charPart);
			for (int i = 0; i < currentGenome.g_objectAmount; i++) {
					if (currentGenome.g_objectHandles[i] == tempHandle) {
						tactileParts++;
						currentGenome.g_tactileObjects.resize(tactileParts);
						if (currentGenome.g_tactileObjects.size() < tactileParts) {
							cout << "Something went wrong with resizing the tactileObjects" << endl;
						}
						//						cout << "CONFIRMING TACTILE SIZE: " << currentGenome.g_tactileObjects.size() << endl;
						currentGenome.g_tactileHandles.resize(tactileParts);
						currentGenome.g_tactileObjects[tactileParts - 1] = i;
						currentGenome.g_tactileHandles[tactileParts - 1] = tempHandle;
					}
				}
				if (tactileParts < partCount){
					cout << "tactileParts = " << tactileParts << ", partCount = " << partCount << endl;
					cout << "Note: more tactile object were given than were identified" << endl;
					cout << " -> check if the given object strings correspond with the tactile sensor object names of your agent" << endl;
				}
			}

			// save handles and objects in genome
			currentGenome.g_tactileObjects = currentGenome.g_tactileObjects;
		}
		
		// additional functionality is to be updated
	//	currentGenome.printIndividualInfo();
}


void CENN::initializer(int simType) {
	// set random seed
//	srand(0);
	if (simType == 1) {
		useDefaultPlantPars();
	}
	else if (simType == 2) {
		useDefaultPlantPars();
	}
	else {
		useDefaultPars();
	}

	initializeEnvironment(); // not set yet
	initializeAgents2();	 // identifies created robot
	initializePopulation2(s_initialPopulationSize); // creates genomes and corresponding joint. The parameters of each individual of the population will stay in memory

	//simStopSimulation();
	_initialize == false;
}

void CENN::initialize() {
//	float position[3] = { 0.0, 0.0, 0.1 };

	environment.initialize();
//	unique_ptr<Population> pop(new Population(50));
//	Population *pop = new Population(50);
	int populationSize = 100; 
	simulationSetup mode; 
//	amountIncrements = 5; 
	vector<int> indNumbers;

	if (simSet != RECALLROTATOR) {
		bool fileExists = false;
		cout << "sceneNum = " << sceneNum << endl;
		ifstream file("interfaceFiles\\settings" + to_string(sceneNum) + ".csv");
		if (file.good()) {
			cout << "settings found" << endl;
			bool genBool = false;
			bool popBool = false;
			bool popInds = false;
			bool indCountBool = false;
			int popCounter = 0;

			string value;
			list<string> values;

			while (file.good()) {
				getline(file, value, ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
				if (value.find('\n') != string::npos) {
					split_line(value, "\n", values);
				}
				else {
					values.push_back(value);
				}
			}
			file.close();

			list<string>::const_iterator it = values.begin();
			for (it = values.begin(); it != values.end(); it++) {
				//		cout << "IT";
				string tmp = *it;
				//		cout << tmp << endl; 
				if (genBool == true){
					generation = atoi(tmp.c_str());
					cout << "generation found : " << generation << endl;
					genBool = false;
				}
				else if (indCountBool == true) {
					indCounter = atoi(tmp.c_str());
					indCountBool = false;
				}
				else if (popBool == true) {
					populationSize = atoi(tmp.c_str());
					popCounter = populationSize;
					cout << "populationSize found : " << populationSize << endl;
					popBool = false;
				}
				else if (popInds == true) {
					//		cout << "individuals found" << endl; 
					indNumbers.push_back(atoi(tmp.c_str()));
					popCounter--;
					if (popCounter == 0){
						popInds = false;
					}
				}
				if (tmp == "#generation") {
					genBool = true;
				}
				else if (tmp == "#indCounter") {
					indCountBool = true;
				}

				else if (tmp == "#populationSize") {
					popBool = true;
				}
				else if (tmp == "#individuals"){
					popInds = true;
				}
				fileExists = true;
			}
			indCounter = generation*populationSize;
		}
		else {
			/*	ofstream settingsFile;
				settingsFile.open("interfaceFiles\\settings" + to_string(sceneNum) + ".csv");
				settingsFile << ",#generation,0," << endl;
				settingsFile.close();*/ /*
				ifstream file("interfaceFiles\\EvoSettings" + to_string(sceneNum) + ".csv");
				if (file.good()) {
				cout << "EVO settings found" << endl;
				bool modeBool = false;
				bool popSizeBool = false;
				bool mrBool = false;
				bool customIndBool = false;
				bool runBool = false;
				string value;
				list<string> values;

				while (file.good()) {
				getline(file, value, ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
				if (value.find('\n') != string::npos) {
				split_line(value, "\n", values);
				}
				else {
				values.push_back(value);
				}
				}
				file.close();

				list<string>::const_iterator it = values.begin();
				for (it = values.begin(); it != values.end(); it++) {
				//		cout << "IT";
				string tmp = *it;
				//		cout << tmp << endl;
				if (modeBool == true){
				if (tmp == "EVOLUTION") {
				mode = EVOLUTION;
				}
				else if (tmp == "COEVOLUTION"){
				mode = COEVOLUTION;
				}
				else if (tmp == "ROTATE"){
				mode = ROTATE;
				}
				else if (tmp == "RECALLCUSTOM") {
				mode = RECALLCUSTOM;
				cout << "setting mode to recall custom" << endl;
				}
				else if (tmp == "RECALLROTATOR") {
				mode = RECALLROTATOR;
				cout << "setting mode to recall rotator" << endl;
				}
				else {
				cout << "mode not found in file settings" << endl;
				}
				modeBool = false;
				simSet = mode;
				}
				else if (popSizeBool == true) {
				populationSize = atoi(tmp.c_str());
				popSizeBool = false;
				}
				else if (mrBool == true) {

				}
				else if (customIndBool == true) {
				customIndividual = atoi(tmp.c_str());
				customIndBool = false;
				}
				else if (runBool == true) {
				sceneNum = atoi(tmp.c_str());
				runBool = false;
				}
				if (tmp == "#mode") {
				modeBool = true;
				}
				else if (tmp == "#populationSize") {
				popSizeBool = true;
				}
				else if (tmp == "#customInd"){
				customIndBool = true;
				}
				else if (tmp == "#run") {
				runBool = true;
				}
				}
				} /**/
		}

	}
	populations.push_back(unique_ptr<Population>(new Population(populationSize)));
	cout << "population created" << endl;
	populations[0]->popIndNumbers = indNumbers;
	if (generation != 0 && simSet != RECALLROTATOR) {
		cout << "loading genomes" << endl;
		populations[0]->loadPopulationGenomes(sceneNum);
		cout << "population loaded" << endl;
	}
	newGenerations = 0;
	
//	pop->populationGenomes[0].createMorpholgy();
	//
//	pop->populationGenomes[0].setJointsToDefault_Control();
	// handle 
//	pop->populationGenomes[0].handleControl();
	// start of simulation
//	Evaluator *evaluator = new Evaluator(populations);
	
	// start of simulation
/*	MorpologyConstructor *morpologyConstructor = new MorphologyConstructor;
	for (int i = 0; i < init�alPopulationSize; i++) {
		morphologyConstructor->create(populations[0]->populationGenomes[0]);
	}

	// handle 
	Control *control = new Control;
	for (int i = 0; i < init�alPopulationSize; i++) {
		control(currentIndividual);
		//evaluator->evaluate(pop->populationGenomes[i]);
	}

	// end of simulation
	for (int i = 0; i < init�alPopulationSize; i++) {
		evaluate(currentIndividual);
		//evaluator->evaluate(pop->populationGenomes[i]);
	}

	// create offspring

	//Genome consists of: 
	/*
		
	*/

	// create one morphology
	// first call createCreature, then lSystem
//	createMorphology(populations[0]->populationGenomes);
//	populations[0]->populationGenomes.createMorphology();
//	populations[0]->createMorph();
//	populations[0]->evaluate();
//	populations[0]->end();
//	cout << "checking sizes: pop = " << populations.size() << endl;
//	cout << "individualGenome = " << populations[0]->populationGenomes.size() << endl;
//	cout << "morphologies: " << populations[0]->populationGenomes[0].morphologies.size() << endl;
//	cout << "controls: " << populations[0]->populationGenomes[0].controls.size() << endl;
//	cout << "baseMorphology " << populations[0]->populationGenomes[0].morphologies[0].baseMorphology.size();
//  cout << "baseMorphPars " << populations[0]->populationGenomes[0].morphologies[0].baseMorphology[0].v_createCreature() << endl;
//	populations[0]->populationGenomes[0].createMorphology(0);

}

int CENN::createWalls(float density) {
//	simCreatePureShape(0, )
	return 1;
}
void CENN::setScene(){
	if (simGetObjectHandle("light0") != -1) {
		sceneNum = 0;
	}
	else if (simGetObjectHandle("light1") != -1) {
		sceneNum = 1;
	}
	else if (simGetObjectHandle("light2") != -1) {
		sceneNum = 2;
	}
	else if (simGetObjectHandle("light3") != -1) {
		sceneNum = 3;
	}
	else if (simGetObjectHandle("light4") != -1) {
		sceneNum = 4;
	}
	else if (simGetObjectHandle("light5") != -1) {
		sceneNum = 5;
	}
	else if (simGetObjectHandle("light6") != -1) {
		sceneNum = 6;
	}
	else if (simGetObjectHandle("light7") != -1) {
		sceneNum = 7;
	}
	else if (simGetObjectHandle("light8") != -1) {
		sceneNum = 8;
	}
	else if (simGetObjectHandle("light8") != -1) {
		sceneNum = 8;
	}
	else if (simGetObjectHandle("light9") != -1) {
		sceneNum = 9;
	}
	else if (simGetObjectHandle("light10") != -1) {
		sceneNum = 10;
	}
	else if (simGetObjectHandle("light11") != -1) {
		sceneNum = 11;
	}
	else if (simGetObjectHandle("light12") != -1) {
		sceneNum = 12;
	}
	else if (simGetObjectHandle("light13") != -1) {
		sceneNum = 13;
	}
	else if (simGetObjectHandle("light14") != -1) {
		sceneNum = 14;
	}
	else if (simGetObjectHandle("light15") != -1) {
		sceneNum = 15;
	}
	else if (simGetObjectHandle("light16") != -1) {
		sceneNum = 16;
	}
	else if (simGetObjectHandle("light17") != -1) {
		sceneNum = 17;
	}
	else if (simGetObjectHandle("light18") != -1) {
		sceneNum = 18;
	}
	else if (simGetObjectHandle("light19") != -1) {
		sceneNum = 19;
	}

	else if (simGetObjectHandle("light20") != -1) {
		sceneNum = 20;
	}
	else if (simGetObjectHandle("light21") != -1) {
		sceneNum = 21;
	}
	else if (simGetObjectHandle("light22") != -1) {
		sceneNum = 22;
	}
	else if (simGetObjectHandle("light23") != -1) {
		sceneNum = 23;
	}
	else {
		sceneNum = -1;
	//	cout << "NO OBJECT FOUND" << endl;
	}
}

void CENN::startOfSimulation(){
	int simType = simSet;
	growTimeKeeper = 0;
//	if (simType == ROTATE) {
		resetTheSun();
//	}
	if (_initialize == true) {
		
//		if ()
	//	populations[0]->populationGenomes[0].init();
		_initialize = false;
		srand(sceneNum + indCounter);
		if (simType == EVOLUTION) {
			simSetBooleanParameter(sim_boolparam_display_enabled, false);
		}
	}
	else {
//		populations[0]->populationGenomes[0].init();
	}
	
	if (simType == RECALLCUSTOM) {
	//	populations[0]->populationGenomes[0]->init_noMorph();
		populations[0]->loadIndividual(customIndividual, sceneNum);
		populations[0]->createNewIndividual();
		for (int i = 0; i < amountIncrements; i++) {
			populations[0]->growNew(1);
		}
	//	populations[0]->populationGenomes[0]->morph->recallAndCreate();
	//	populations[0]->populationGenomes[0]->morph->evaluate(sceneNum);
	//	populations[0]->createIndividual(0);
	//	populations[0]->createIndividual(0);
	//	populations[0]->createNewIndividual();
	//	populations[0]->selectNewIndividual(0);
	//	cout << "selected new individual" << endl; 
	//	populations[0]->populationGenomes[0]->morph->recallAndCreate();
	//	cout << "new individual created " << endl; 
	}
	else if (simType == RECALLROTATOR) {
		cout << "recalling rotator" << endl; 
		populations[0]->loadIndividualGenome(customIndividual, sceneNum);
		cout << "individual loaded" << endl; 
		cout << "creating new individual" << endl; 
		populations[0]->createNewIndividual();
		cout << "individual created" << endl; 
		for (int i = 0; i < amountIncrements; i++) {
			populations[0]->growNew(1);
		}
	}
	else if (simType == ROTATE) {
		/*if (generation == 1001) {
			simSetBooleanParameter(sim_boolparam_display_enabled, true);
			float bestIndFitness = 0;
			int bestIndividual = 0;
			for (int i = 0; i < populations[0]->populationFitness.size(); i++)
			{
				cout << "individual " << i << " has fitness of " << populations[0]->populationFitness[i] << endl;
				if (populations[0]->populationFitness[i] > bestIndFitness) {
					bestIndFitness = populations[0]->populationFitness[i];
					bestIndividual = populations[0]->popIndNumbers[i];
				}
			}
			cout << "bestIndividual = " << bestIndividual << endl;
			cout << "bestIndFitness = " << bestIndFitness << endl;
			populations[0]->populationGenomes[0]->init_noMorph();
			populations[0]->loadIndividual(bestIndividual, sceneNum);
			populations[0]->populationGenomes[0]->morph->recallAndCreate();
			populations[0]->populationGenomes[0]->morph->evaluate(sceneNum);
			simPauseSimulation();
			
		}*/
		if (indCounter < populations[0]->populationGenomes.size()) {
			currentInd = indCounter;
			populations[0]->initializeIndividual(currentInd);
		//	populations[0]->growInd(amountIncrements, currentInd);
		//	populations[0]->evaluateIndividual(currentInd, sceneNum);
			populations[0]->popIndNumbers.push_back(indCounter);
			for (int i = 0; i < amountIncrements; i++) {
				populations[0]->growInd(1, indCounter);
			}
			indCounter++;
		}
		else if (indCounter >= populations[0]->populationGenomes.size()) {
			currentInd = rand() % populations[0]->populationGenomes.size();
			populations[0]->selectNewIndividual(currentInd); // before this, the old stored class of newGenome needs to be deleted, else memory will leak
			populations[0]->mutateNewIndividual();
			populations[0]->createNewIndividual();
			for (int i = 0; i < amountIncrements; i++) {
				populations[0]->growNew(1);
			}
		//	populations[0]->createPopulation();
		//	populations[0]->growNew(amountIncrements);
		//	float fitness = populations[0]->evaluateNewIndividual(indCounter, sceneNum);
			indCounter++;
		}
	}
	else if (simType == EVOLUTION) { // by default do an evolutionary run. 
		//checkMemoryLeak();
	
/*		if (generation == 1001) {
			simSetBooleanParameter(sim_boolparam_display_enabled, true);
			float bestIndFitness = 0;
			int bestI = 0;
			int bestIndividual = 0;
			for (int i = 0; i < populations[0]->populationFitness.size(); i++)
			{
				cout << "individual " << i <<" has fitness of " << populations[0]->populationFitness[i] << endl; 
				if (populations[0]->populationFitness[i] > bestIndFitness) {
					bestIndFitness = populations[0]->populationFitness[i];
					bestIndividual = populations[0]->popIndNumbers[i];
					bestI = i; 
				}
			}
			
			cout << "bestIndividual = " << bestIndividual << endl; 
			cout << "bestIndFitness = " << bestIndFitness << endl; 
			populations[0]->populationGenomes[0]->init_noMorph();
			populations[0]->loadIndividual(bestIndividual, sceneNum);
			populations[0]->populationGenomes[0]->morph->recallAndCreate();
			populations[0]->populationGenomes[0]->morph->evaluate(sceneNum);
			
			simPauseSimulation();
			simQuitSimulator(false);
		} */
		if (indCounter < populations[0]->populationGenomes.size()) {
			currentInd = indCounter;
			populations[0]->initializeIndividual(currentInd);
		//	populations[0]->growInd(amountIncrements, currentInd);
		//	populations[0]->evaluateIndividual(currentInd, sceneNum);
			populations[0]->popIndNumbers.push_back(indCounter);
			for (int i = 0; i < amountIncrements; i++) {
				populations[0]->growInd(1, indCounter);
			}
			indCounter++;
		}
		else if (indCounter >= populations[0]->populationGenomes.size()) {
		//	cout << "creating new individual" << endl; 
			currentInd = rand() % populations[0]->populationGenomes.size();
		//	cout << "selecting new individual" << endl; 
		//	cout << "selected individual had number " << populations[0]->popIndNumbers[currentInd] << endl; 
			populations[0]->selectNewIndividual(currentInd); // before this, the old stored class of newGenome needs to be deleted, else memory will leak
		//	cout << "mutating new individual" << endl; 
			populations[0]->mutateNewIndividual();
			cout << "mutation done " << endl; 
		//	cout << "creating new individual" << endl; 
			populations[0]->createNewIndividual();
		//	cout << "growing new individual for " << amountIncrements << "increments" << endl; ;
			//populations[0]->createPopulation();
		//	populations[0]->growNew(amountIncrements);
			for (int i = 0; i < amountIncrements; i++) {
				populations[0]->growNew(1);
			}
//			float fitness = populations[0]->evaluateNewIndividual(indCounter, sceneNum);
			indCounter++;
		}
		
	} 
	else if (simType == OBSTACLEEVOLUTION) {
		createWalls(1.5); 
		if (indCounter < populations[0]->populationGenomes.size()) {
			currentInd = indCounter;
			populations[0]->initializeIndividual(currentInd);
			populations[0]->evaluateIndividual(currentInd, sceneNum);
			populations[0]->popIndNumbers.push_back(indCounter);
			indCounter++;
		}
		else if (indCounter >= populations[0]->populationGenomes.size()) {
			currentInd = rand() % populations[0]->populationGenomes.size();
			populations[0]->selectNewIndividual(currentInd); // before this, the old stored class of newGenome needs to be deleted, else memory will leak
			populations[0]->mutateNewIndividual();
			populations[0]->createNewIndividual();
			indCounter++;
		}
	}
	else if (simType == COEVOLUTION) {
		if (indCounter < populations[0]->populationGenomes.size()) {
			currentInd = indCounter;
			cout << "about to co-evolve" << endl; 
			populations[0]->initializeIndividual(currentInd);
			cout << "first creature evolved" << endl; 
		//	populations[0]->evaluateIndividual(currentInd);
		//	populations[0]->popIndNumbers.push_back(indCounter);
			populations[0]->popIndNumbers.push_back(indCounter);
			for (int i = 0; i < amountIncrements; i++) {
				populations[0]->growInd(1, indCounter);
			}

			indCounter++;
		}
		else if (indCounter >= populations[0]->populationGenomes.size()) {
			cout << "new genome" << endl; 
			currentInd = rand() % populations[0]->populationGenomes.size();
			populations[0]->selectNewIndividual(currentInd); // before this, the old stored class of newGenome needs to be deleted, else memory will leak
			cout << "selected new individual" << endl;
			populations[0]->mutateNewIndividual();
			populations[0]->createNewIndividual();
			populations[0]->createCoPopulationSameGenome(3.5);
			for (int i = 0; i < amountIncrements; i++) {
				populations[0]->growCoSame(1);
				populations[0]->growNew(1);
			}
			indCounter++;
		}
	}

	
	

//	cout << "NOTHING ELSE IS HAPPENING SO THE DAMN PROGRAM SHOULD NOT CRASH" << endl; 

	/*_initialize = true;
	if (_initialize == true) {
		populations[0]->populationGenomes[0].init();
		_initialize = false; 
	}
	else {
		//populations[0]->populationGenomes[0].mutate();
		populations[0]->populationGenomes[0].create();
	} */
/*	if (_initialize == true)
	{
		float position[3] = { 0.1, 0.1, 0.1 };
	}
	else if (indCounter < s_initialPopulationSize - 1)  {
		indCounter++;
		
		currentInd = indCounter;
		currentGenome = storedIndividualGenomes[currentInd];
		for (int i = 0; i < currentGenome.g_jointAmount; i++) {
			simSetJointForce(currentGenome.g_jointHandles[i], 10);
		}
		
		if (nNHandlingType == 1) {
			resetDefaultPlantNN();
		}
		else if (nNHandlingType == 2) {
			modularLSystem();
			resetDefaultPlantNN();
		}
	}
	else {
		currentInd = rand() % s_initialPopulationSize;
		indCounter++;
		for (int i = 0; i < currentGenome.g_jointAmount; i++) {
			simSetJointForce(currentGenome.g_jointHandles[i], 100);
		}

		if (sexualReproduction == true) {

		}
		else { 
			currentGenome = storedIndividualGenomes[currentInd];
		}
		if (nNHandlingType == 1) {
			mutateLProperties(0.2);//0.05);

			currentGenome.g_leafSunContact.clear();
			rayHandles.clear();
			currentGenome.g_leafHandles.clear();
			currentGenome.g_leafJointHandles.clear();


			lSystem();
			int collectionHandle = simCreateCollection("agent1", 1);
			for (int i = 0; i < currentGenome.g_objectHandles.size(); i++) {			
				simAddObjectToCollection(collectionHandle, currentGenome.g_objectHandles[i], sim_handle_single, 0);
			}
//			simAddObjectToCollection(collectionHandle, currentGenome.g_objectHandles[i], sim_handle_tree)

		//	createMorphologyWithLeafs();
			lightHandle = simGetObjectHandle("Omnidirectional_light");
			cout << "currentGenome.g_leafHandles.size() = " << currentGenome.g_leafHandles.size() << endl;
			leafToSunRay();
			
			resetDefaultPlantNN();
			//	initializeAgents2();	 // identifies created robot
		//	identifyRobotMorphology();
		//	defaultPlantNN();
		}
		else if (nNHandlingType == 2) {
			mutateModularLProperties(0.2);
			modularLSystem();
			resetDefaultPlantNN();
		}
	}
//	int parA[1];
//	simGetObjectIntParameter(mainHandle, 3019, parA);
//	cout << "parA = " << parA[0] << endl; 
//	int visib[1];
//	simGetObjectIntParameter(mainHandle, 10, visib);
//	cout << "Visibility of mainHandle = " << visib[0] << endl;
	// simPauseSimulation();
	// method 2 doesn't use callNNPars and storeNNPars */
}

void CENN::leafToSunRay() {
//	assert(currentGenome.g_leafHandles.size() == currentGenome.g_leafSunContact.size());
	currentGenome.g_leafSunContact.resize(currentGenome.g_leafHandles.size());
	for (int i = 0; i < currentGenome.g_leafHandles.size(); i++) {
		currentGenome.g_leafSunContact[i] = createRay(lightHandle, currentGenome.g_leafHandles[i]);
	}
//	if (currentGenome.g_leafSunContact.size() > 0) {
//		simPauseSimulation();
//	}
}

void CENN::checkLError() {
	cout << "objAmount: " << currentGenome.g_objectAmount << endl;
	cout << "currentGenome.g_lStateProperties.size: " << currentGenome.g_lStateProperties.size() << endl;

//	for (int i = 0; i < currentGenome.g_lStateProperties[i].size(); i++) {
//		if (currentGenome.g_lStateProperties[i].size)
//		for (int j = 0; j < currentGenome.g_lStateProperties[i][j].size(); j++) {
//		}
//	}

	for (int i = 0; i < currentGenome.g_lStateProperties.size(); i++) {
		cout << "currentGenome.g_lStateProperties[" << i << "].size: " << currentGenome.g_lStateProperties[i].size() << endl;
		for (int j = 0; j < currentGenome.g_lStateProperties[i].size(); j++) {
			cout << "j: " << j << endl;
			cout << "currentGenome.g_lStateProperties[" << i << "][" << j << "].size: " << currentGenome.g_lStateProperties[i][j].size() << endl;
		}
	}

	vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
	for (size_t currentState = 0; currentState < a.size(); currentState++)
	{
		for (int k = 0; k < a[currentState].size(); k++) {
			cout << "Pars[" << currentState << "][" << k << "]: " << a[currentState][k][0] << ", " << a[currentState][k][1] << ", " << a[currentState][k][2] << ", " << a[currentState][k][3] << ", " 
				<< a[currentState][k][4] << ", " << a[currentState][k][5] << ", " << a[currentState][k][6] << ", " << a[currentState][k][7] << ", " 
				<< a[currentState][k][8] << ", " << a[currentState][k][9] << ", " << a[currentState][k][10] << endl;
		}
	}

	cout << "done checking" << endl << endl;
}

void CENN::modularLGrowth(int amountNewObj, int currentState, int targetState, int objNum) {
	//	cout << "lGrowth is called" << endl;
	//	currentGenome.g_objectType
	if (currentGenome.g_objectType[objNum] == currentState) {
		for (int j = 0; j < currentGenome.g_lStateProperties[currentState].size(); j++) {
			//	cout << "1." << objNum << "." << j << endl;
		//	currentGenome.g_objectAmount++;
		//	currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
			currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
			if (currentGenome.g_lStateProperties[currentState][j][10] >= -0.5 && currentGenome.g_lStateProperties[currentState][j][10] < 0.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 0;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 0.5 && currentGenome.g_lStateProperties[currentState][j][10] < 1.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 1;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 1.5 && currentGenome.g_lStateProperties[currentState][j][10] < 2.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 2;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 2.5 && currentGenome.g_lStateProperties[currentState][j][10] < 3.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 3;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 3.5 && currentGenome.g_lStateProperties[currentState][j][10] < 4.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 4;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 4.5 && currentGenome.g_lStateProperties[currentState][j][10] < 5.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 5;
			}
			//			massObjects.resize(amountParts);
			int newP = currentGenome.g_objectAmount;

			currentGenome.g_jointAmount++;
			int newJoint = currentGenome.g_jointAmount - 1;
			//		if (toTargetState[j] != true) {
			//			currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = currentState;
			//		}

			//			createCylinder(0.01, 0.01, 0.2, 0.0, 0.0, 0.2, angleDif0 + (angleDif0 * j), angleDif1 + (angleDif1 * j),
			//				angleDif2 + (angleDif2 * j), 1, parent, newP);
			vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;

			//			createCube(a[currentState][j][0], a[currentState][j][1], a[currentState][j][2], a[currentState][j][3], a[currentState][j][4],
			//				a[currentState][j][5], a[currentState][j][6], a[currentState][j][7], a[currentState][j][8], a[currentState][j][9], objNum, newP);
			//			createJointAndCube(a[currentState][j][0], a[currentState][j][1], a[currentState][j][2], a[currentState][j][3], a[currentState][j][4],
			//				a[currentState][j][5], a[currentState][j][6], a[currentState][j][7], a[currentState][j][8], a[currentState][j][9], objNum, newP, newJoint);
			createCubeModule(a[currentState][j][0], a[currentState][j][1], a[currentState][j][2], a[currentState][j][3], a[currentState][j][4],
				a[currentState][j][5], a[currentState][j][6], a[currentState][j][7], a[currentState][j][8], a[currentState][j][9], objNum, newP, newJoint);
		}
		//	vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
		//	for (int k = 0; k < a[currentState].size(); k++) {
		//		cout << "Creating Cube" << endl;
		//		cout << "stateSize = " << a[currentState].size() << endl;
		//		createCube(a[currentState][k][0], a[currentState][k][1], a[currentState][k][2], a[currentState][k][3], a[currentState][k][4],
		//			a[currentState][k][5], a[currentState][k][6], a[currentState][k][7], a[currentState][k][8], a[currentState][k][9], objNum, newP);
		//	}



		//				for (int k = 0; k < currentGenome.g_lStateChanger.size(); k++) {
		//					currentGenome.g_lStateChanger[k] += currentGenome.g_lStateSequencer[k];
		//				}
		//					0.01, 0.01, 0.2, 0.0, 0.0, 0.2, angleDif0 + (angleDif0 * j), angleDif1 + (angleDif1 * j),
		//					angleDif2 + (angleDif2 * j), 1, parent, newP);


		/*	float newObjPos[3];
		float compPos[3];
		for (int k = 0; k < (amountParts-1); k++) {
		simGetObjectPosition(partHandles[k], partHandles[amountParts - 1], compPos);
		for (int l = 0; l < 3; l++) {
		if (compPos[l] > -0.003 && compPos[l] < 0.003) {
		//simRemoveObject(partHandles[amountParts - 1]);
		simGetObjectPosition(partHandles[amountParts - 1], NULL, newObjPos);
		cout << "new object too close to old object" << endl;
		newObjPos[0] += 0.01;
		newObjPos[1] += 0.01;
		newObjPos[2] += 0.01;
		simSetObjectPosition(partHandles[amountParts - 1], NULL, newObjPos);
		}
		}
		} */



		//int collision = simCheckCollisionEx(partHandles[amountParts-1], sim_handle_all, NULL);
		//cout << collision;
		///if (collision == true) {
		//	simRemoveObject(partHandles[amountParts - 1]);
		//	cout << "object removed" << endl;
		//}

		/*
		amountParts += 3;
		cout << "amountParts = " << amountParts << endl;
		partHandles.resize(amountParts);
		partStates.resize(amountParts);
		int parent = i;
		int new1 = amountParts - 3;
		int new2 = amountParts - 2;
		int new3 = amountParts -1;
		partStates[amountParts - 3] = 2;
		partStates[amountParts - 2] = 2;
		partStates[amountParts - 1] = 1;
		createCylinder(0.1, 0.1, 0.1, 0.1, 0.0, 0.2, 0.1, 0.2, 1.0, 1, parent, new1);
		simSetObjectParent(partHandles[amountParts - 3], partHandles[parent], TRUE);
		nameObject("part", amountParts - 3);

		createCylinder(0.1, 0.1, 0.1, 0.0, 0.0, 0.2, 0.3, 0.1, 0.5, 1, parent, new2);
		simSetObjectParent(partHandles[amountParts - 2], partHandles[parent], TRUE);
		nameObject("part", amountParts - 2);

		createCylinder(0.1, 0.1, 0.1, 0.0, 0.0, 0.2, 0.2, 0.5, 0.3, 1, parent, new3);
		simSetObjectParent(partHandles[amountParts - 1], partHandles[parent], TRUE);
		nameObject("part", amountParts - 1);
		/**/
	}
	// cout << "Done with LGrowth" << endl;
}

void CENN::lGrowth(int amountNewObj, int currentState, int targetState, int parentNum) {
	cout << "lGrowth is called" << endl;
	//	currentGenome.g_objectType
	int parentObjectHandle = currentGenome.g_objectHandles[parentNum];
	if (currentGenome.g_objectType[parentNum] == currentState) {
		for (int j = 0; j < currentGenome.g_lStateProperties[currentState].size(); j++) {
			cout << "1." << parentNum << "." << j << endl;
			currentGenome.g_objectAmount++;
			currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
			currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
			if (currentGenome.g_lStateProperties[currentState][j][10] >= -0.5 && currentGenome.g_lStateProperties[currentState][j][10] < 0.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 0;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 0.5 && currentGenome.g_lStateProperties[currentState][j][10] < 1.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 1;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 1.5 && currentGenome.g_lStateProperties[currentState][j][10] < 2.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 2;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 2.5 && currentGenome.g_lStateProperties[currentState][j][10] < 3.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 3;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 3.5 && currentGenome.g_lStateProperties[currentState][j][10] < 4.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 4;
			}
			if (currentGenome.g_lStateProperties[currentState][j][10] >= 4.5 && currentGenome.g_lStateProperties[currentState][j][10] < 5.5) {
				currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = 5;
			}
			//			massObjects.resize(amountParts);
			int newP = currentGenome.g_objectAmount - 1;

		//		if (toTargetState[j] != true) {
			//			currentGenome.g_objectType[currentGenome.g_objectAmount - 1] = currentState;
			//		}

			//			createCylinder(0.01, 0.01, 0.2, 0.0, 0.0, 0.2, angleDif0 + (angleDif0 * j), angleDif1 + (angleDif1 * j),
			//				angleDif2 + (angleDif2 * j), 1, parent, newP);
			vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;

			//			createCube(a[currentState][j][0], a[currentState][j][1], a[currentState][j][2], a[currentState][j][3], a[currentState][j][4],
			//				a[currentState][j][5], a[currentState][j][6], a[currentState][j][7], a[currentState][j][8], a[currentState][j][9], objNum, newP);
			//			createJointAndCube(a[currentState][j][0], a[currentState][j][1], a[currentState][j][2], a[currentState][j][3], a[currentState][j][4],
			//				a[currentState][j][5], a[currentState][j][6], a[currentState][j][7], a[currentState][j][8], a[currentState][j][9], objNum, newP, newJoint);
			
			if (currentGenome.g_objectType[currentGenome.g_objectAmount - 1] != 4 && currentGenome.g_objectType[currentGenome.g_objectAmount - 1] != 3) {
				currentGenome.g_jointAmount++;
				int newJoint = currentGenome.g_jointAmount - 1;
				assert(parentNum <= currentGenome.g_objectHandles.size());
				cout << "parentNum = " << parentNum;
				createJointAndCubeST(a[currentState][j][0], a[currentState][j][1], a[currentState][j][2], a[currentState][j][3], a[currentState][j][4],
					a[currentState][j][5], a[currentState][j][6], a[currentState][j][7], a[currentState][j][8], a[currentState][j][9], parentNum, newP, newJoint);
			}
			else {
				// alternative, add artificial leaf
				int handle = createArtificialLeaf(0.05, 0.05, 0.02, 0, 0, 0, 0, 0, 0, 0, parentNum, newP);
				if (handle > 0) {
					cout << "handle is " << handle << endl;
					currentGenome.g_objectHandles[currentGenome.g_objectHandles.size() - 1] = handle;
					// add a visionSensor
					int visionSensorHandle = createVisionSensor(handle);
					currentGenome.g_leafHandles.push_back(visionSensorHandle);
					// set visionsensor on top of object
					visionHandles.push_back(visionSensorHandle);
					
				}
				else {
					assert(currentGenome.g_objectHandles.size() == currentGenome.g_objectAmount);
					currentGenome.g_objectHandles.resize(currentGenome.g_objectHandles.size() - 1);
					currentGenome.g_objectAmount--;
					cout << "handle is " << handle << endl; 
				}
			}
		}
		//	vector<vector<vector<float>>> a = currentGenome.g_lStateProperties;
		//	for (int k = 0; k < a[currentState].size(); k++) {
		//		cout << "Creating Cube" << endl;
		//		cout << "stateSize = " << a[currentState].size() << endl;
		//		createCube(a[currentState][k][0], a[currentState][k][1], a[currentState][k][2], a[currentState][k][3], a[currentState][k][4],
		//			a[currentState][k][5], a[currentState][k][6], a[currentState][k][7], a[currentState][k][8], a[currentState][k][9], objNum, newP);
		//	}



		//				for (int k = 0; k < currentGenome.g_lStateChanger.size(); k++) {
		//					currentGenome.g_lStateChanger[k] += currentGenome.g_lStateSequencer[k];
		//				}
		//					0.01, 0.01, 0.2, 0.0, 0.0, 0.2, angleDif0 + (angleDif0 * j), angleDif1 + (angleDif1 * j),
		//					angleDif2 + (angleDif2 * j), 1, parent, newP);


		/*	float newObjPos[3];
		float compPos[3];
		for (int k = 0; k < (amountParts-1); k++) {
		simGetObjectPosition(partHandles[k], partHandles[amountParts - 1], compPos);
		for (int l = 0; l < 3; l++) {
		if (compPos[l] > -0.003 && compPos[l] < 0.003) {
		//simRemoveObject(partHandles[amountParts - 1]);
		simGetObjectPosition(partHandles[amountParts - 1], NULL, newObjPos);
		cout << "new object too close to old object" << endl;
		newObjPos[0] += 0.01;
		newObjPos[1] += 0.01;
		newObjPos[2] += 0.01;
		simSetObjectPosition(partHandles[amountParts - 1], NULL, newObjPos);
		}
		}
		} */



		//int collision = simCheckCollisionEx(partHandles[amountParts-1], sim_handle_all, NULL);
		//cout << collision;
		///if (collision == true) {
		//	simRemoveObject(partHandles[amountParts - 1]);
		//	cout << "object removed" << endl;
		//}

		/*
		amountParts += 3;
		cout << "amountParts = " << amountParts << endl;
		partHandles.resize(amountParts);
		partStates.resize(amountParts);
		int parent = i;
		int new1 = amountParts - 3;
		int new2 = amountParts - 2;
		int new3 = amountParts -1;
		partStates[amountParts - 3] = 2;
		partStates[amountParts - 2] = 2;
		partStates[amountParts - 1] = 1;
		createCylinder(0.1, 0.1, 0.1, 0.1, 0.0, 0.2, 0.1, 0.2, 1.0, 1, parent, new1);
		simSetObjectParent(partHandles[amountParts - 3], partHandles[parent], TRUE);
		nameObject("part", amountParts - 3);

		createCylinder(0.1, 0.1, 0.1, 0.0, 0.0, 0.2, 0.3, 0.1, 0.5, 1, parent, new2);
		simSetObjectParent(partHandles[amountParts - 2], partHandles[parent], TRUE);
		nameObject("part", amountParts - 2);

		createCylinder(0.1, 0.1, 0.1, 0.0, 0.0, 0.2, 0.2, 0.5, 0.3, 1, parent, new3);
		simSetObjectParent(partHandles[amountParts - 1], partHandles[parent], TRUE);
		nameObject("part", amountParts - 1);
		/**/
	}
	// cout << "Done with LGrowth" << endl;
}

void CENN::nameObject(string part, int handleNum) {
	string num = to_string(handleNum);
	//	string objName = "part";
	part.append(num);
	const char * setName = part.c_str();
	simSetObjectName(currentGenome.g_objectHandles[handleNum], setName);
}

/*
void DefaultGenome::printIndividualInfo() {
//	void CIndividualGenome::printIndividualInfo() {
		cout << "individual: " << individualNumber << endl;
	cout << "amountInputNeurons: " << g_amountInputNeurons << endl;
	cout << "amountinterNeurons: " << g_amountInterNeurons << endl;
	cout << "amountOutputNeurons: " << g_amountOutputNeurons << endl;
	cout << "amountCPGs: " << g_amountPatternGenerators << endl;
	for (int i = 0; i < g_amountInputNeurons; i++) {
		cout << "inputNeuronConnectionAmount " << i << " = " << g_inputNeuronConnectionAmount[i] << ", ";
	} cout << endl;
	cout << "CONFIRMING TACTILE SIZE: " << g_tactileObjects.size() << endl;
	
} */

void CENN::resetTheSun() {
	float sunPosition[3];
	int sunHandle = simGetObjectHandle(("light" + to_string(sceneNum)).c_str());
	simGetObjectPosition(sunHandle, -1, sunPosition);
	sunPosition[0] = -2;
	sunPosition[1] = 5;
	sunPosition[2] = 5;
	simSetObjectPosition(sunHandle, -1, sunPosition);
}

void CENN::rotateTheSun() {
	float sunPosition[3];
	int sunHandle = simGetObjectHandle(("light" + to_string(sceneNum)).c_str());
	simGetObjectPosition(sunHandle, -1, sunPosition);
	sunPosition[1] = sunPosition[1] - 1;
	simSetObjectPosition(sunHandle, -1, sunPosition);
}

void CENN::handleSimulation(){
	//cout << "growtime1 = " << growTimeKeeper << endl;
	growTimeKeeper += simGetSimulationTimeStep();
	simulationTime += simGetSimulationTimeStep();
	//cout << "growtime = " << growTimeKeeper << endl; 
	switch (simSet) {
	case (EVOLUTION) :
		if (indCounter - 1  < populations[0]->populationGenomes.size()) {
			if (growTimeKeeper >= 0.2) {
				//	cout << "rotating individual" << endl; 
				rotateTheSun();
				populations[0]->adjustFitnessIndividual(indCounter - 1, sceneNum);
				growTimeKeeper = 0;
			}
		}
		else {
			if (growTimeKeeper >= 0.2) {
				rotateTheSun();
				populations[0]->adjustNewFitness(sceneNum);
				growTimeKeeper = 0;
			}
			//	populations[0]->controlPopulation();
		}
		if (simulationTime >= 2.0) {
			if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
				float fitness = populations[0]->evaluateNewRotatedIndividual(indCounter - 1, sceneNum);
			}
			else{
				float fitness = populations[0]->evaluateRotatedIndividual(indCounter - 1, sceneNum);
			}
			simulationTime = 0;
			growTimeKeeper = 0;
			simStopSimulation();
		}
		break;
	case (ROTATE) :
	//	cout << "case is rotate" << endl; 
		if (indCounter - 1  < populations[0]->populationGenomes.size()) {
			if (growTimeKeeper >= 0.2) {
			//	cout << "rotating individual" << endl; 
				rotateTheSun();
				populations[0]->rotateInd(indCounter-1);
				populations[0]->adjustFitnessIndividual(indCounter-1, sceneNum); 
				growTimeKeeper = 0;
			}
		}
		else {
			if (growTimeKeeper >= 0.2) {
				rotateTheSun();
				populations[0]->rotateNewInd();
				populations[0]->adjustNewFitness(sceneNum);
				growTimeKeeper = 0;
			}
		}
		if (simulationTime >= 2.0) {
			if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
				cout << "evaluating new ind" << endl; 
				float fitness = populations[0]->evaluateNewRotatedIndividual(indCounter - 1, sceneNum);
				cout << "evaluated new ind" << endl; 

			}
			else{
				float fitness = populations[0]->evaluateRotatedIndividual(indCounter - 1, sceneNum);
			}
			simulationTime = 0; 
			growTimeKeeper = 0;
			simStopSimulation();
		}
		break;
	case (DYNAMIC) :
		//	cout << "case is rotate" << endl; 
		if (indCounter - 1  < populations[0]->populationGenomes.size()) {
			populations[0]->controlCreated(indCounter - 1);
			if (growTimeKeeper >= 0.2) {
				//	cout << "rotating individual" << endl; 
				rotateTheSun();
				populations[0]->adjustFitnessIndividual(indCounter - 1, sceneNum);
				growTimeKeeper = 0;
			}
		}
		else {
			populations[0]->controlNew();
			if (growTimeKeeper >= 0.2) {
				rotateTheSun();
				populations[0]->adjustNewFitness(sceneNum);
				growTimeKeeper = 0;
			}
		}
		if (simulationTime >= 2.0) {
			if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
				cout << "evaluating new ind" << endl;
				float fitness = populations[0]->evaluateNewRotatedIndividual(indCounter - 1, sceneNum);
				cout << "evaluated new ind" << endl;
			}
			else{
				float fitness = populations[0]->evaluateRotatedIndividual(indCounter - 1, sceneNum);
			}
			simulationTime = 0;
			growTimeKeeper = 0;
			simStopSimulation();
		}
		break;

	case (GROWEVOLUTION): 
		if (indCounter - 1  < populations[0]->populationGenomes.size()) {
			populations[0]->controlCreated(indCounter - 1);
			if (growTimeKeeper >= 0.1) {

				populations[0]->growInd(1, indCounter - 1);
				growTimeKeeper = 0;
			}
		}
		else {
			populations[0]->controlNew();

			if (growTimeKeeper >= 0.1) {
				populations[0]->growNew(1);
				growTimeKeeper = 0; 
			}
		//	populations[0]->controlPopulation();
		}
		if (simGetSimulationTime() >= 0.55) {
			if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
				float fitness = populations[0]->evaluateNewIndividual(indCounter - 1, sceneNum);
			}
			else{
				float fitness = populations[0]->evaluateIndividual(indCounter - 1, sceneNum);
			}
			simStopSimulation();
		}
		break;
	case (RECALLROTATOR) :
		if (growTimeKeeper >= 0.2) {
			rotateTheSun();
			populations[0]->rotateNewInd();
			populations[0]->adjustNewFitness(sceneNum);
			growTimeKeeper = 0;
		}
		if (simulationTime >= 2.0) {
			simulationTime = 0;
			growTimeKeeper = 0;
			simStopSimulation();
		}
		break; 
	case (RECALLCUSTOM) :
		break; 
	case (COEVOLUTION) :
		if (indCounter - 1  < populations[0]->populationGenomes.size()) {
			if (growTimeKeeper >= 0.2) {
				//	cout << "rotating individual" << endl; 
				cout << "I think the error is here" << endl; 
				rotateTheSun();
				populations[0]->adjustFitnessIndividual(indCounter - 1, sceneNum);
				growTimeKeeper = 0;
				cout << "no, it's not" << endl; 
			}
		}
		else {
			if (growTimeKeeper >= 0.2) {
				rotateTheSun();
				populations[0]->adjustNewFitness(sceneNum);
				growTimeKeeper = 0;
			}
			//	populations[0]->controlPopulation();
		}
		if (simulationTime >= 2.0) {
			if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
				float fitness = populations[0]->evaluateNewRotatedIndividual(indCounter - 1, sceneNum);
			}
			else{
				float fitness = populations[0]->evaluateRotatedIndividual(indCounter - 1, sceneNum);
			}
			simulationTime = 0;
			growTimeKeeper = 0;
			simStopSimulation();
		}
		break;
		//if (indCounter - 1 < populations[0]->populationGenomes.size()) {
		//	if (growTimeKeeper >= 0.2) {
		//		populations[0]->growInd(1, indCounter - 1);
		//		growTimeKeeper = 0;
		////		cout << "should set time to zero" << endl; 
		//	}
		//}
		//else {
		//	if (growTimeKeeper >= 0.2) {
		////		populations[0]->growNew(1);
		////		populations[0]->growCoSame(1);
		//		growTimeKeeper = 0;
		//	}
		//}
		//if (simGetSimulationTime() >= 0.1) {
		//	simStopSimulation();
		//}
		//break; 
	}
	
	

	
//	assert(amountVisionSensors == visionHandles.size());
	//if (GetAsyncKeyState(VK_SHIFT)) {
	//	cout << "Space pressed" << endl;
	//	populations[0]->populationGenomes[0].increment();
//	}
	

//	populations[0]->populationGenomes[0].increment();
	
//	simPauseSimulation();


	/*	if (visionHandles.size() > 0) {
		for (int i = 0; i < visionHandles.size(); i++) {
			simSetExplicitHandling(visionHandles[i], 0);
			float* data;
			int* dataSize;
			simHandleVisionSensor(visionHandles[i], &data, &dataSize);
			simGetVisionSensorImage(visionHandles[i]);
			simGetVisionSensorCharImage(visionHandles[i], NULL, NULL);
			float *colors2;
			colors2 = simGetVisionSensorImage(visionHandles[i]);
			simSetVisionSensorImage(visionHandles[i], colors2);
			simReleaseBuffer((char*)data);
			simReleaseBuffer((char*)dataSize);
		}
	}*/
	
//	if (eNNMethod == 2) {
//	float force[3];
//	float velocity[3];
/*	if (currentGenome.g_jointAmount != 0) {
		simGetJointForce(currentGenome.g_jointHandles[0], force);
		cout << "jointForce = " << force[0] << endl;
		simGetJointForce(currentGenome.g_jointHandles[1], force);
		cout << "jointForce = " << force[0] << endl;
		simGetJointForce(currentGenome.g_jointHandles[2], force);
		cout << "jointForce = " << force[0] << endl; 
		simGetJointTargetVelocity(currentGenome.g_jointHandles[0], velocity);
		cout << "jointVelocity = " << force[0] << endl;
		simGetJointTargetVelocity(currentGenome.g_jointHandles[0], velocity);
		cout << "jointVelocity = " << force[0] << endl;
		simGetJointTargetVelocity(currentGenome.g_jointHandles[0], velocity);
		cout << "jointVelocity = " << force[0] << endl;
	} /**/

	if (nNHandlingType == 0 && _initialize == false) {
		//	cout << "handle is called" << endl;
		updateAcquiredActivationLevels2();
		//	cout << "Acq [12-13] = " << currentGenome.g_acquiredActivationLevels[12] << ", " << currentGenome.g_acquiredActivationLevels[13];
		updateJointForces2(s_jointControl);
	}
	if (nNHandlingType == 1 && _initialize == false) {
		//	cout << "handle is called" << endl;
		updateAcquiredActivationLevels2();
		//	cout << "Acq [12-13] = " << currentGenome.g_acquiredActivationLevels[12] << ", " << currentGenome.g_acquiredActivationLevels[13];
		updateJointForces2(s_jointControl);
	
		// handle of joint
		int jointHandle = simGetObjectHandle("blabla");
		simSetJointForce(jointHandle, 100);
		
		//int par[5];
//		simGetObjectIntParameter(currentGenome.g_objectHandles[2], 3019, par);
//		cout << "respondable mask = " << par[0] << ", " << par[1] << endl;
//		simGetObjectIntParameter(currentGenome.g_objectHandles[3], 3019, par);
//		cout << "respondable mask = " << par[0] << ", " << par[1] << endl;
//		simSetObjectIntParameter(currentGenome.g_objectHandles[4], 3019, 0);
//		simSetObjectIntParameter(currentGenome.g_objectHandles[5], 3019, 0);
//		simSetObjectIntParameter(currentGenome.g_objectHandles[6], 3019, 0);
//		simSetObjectIntParameter(currentGenome.g_objectHandles[7], 3019, 0);

		// check for collisions and if collided destory parent joint
	//	for (int i = 2; i < currentGenome.g_objectAmount; i++) {
	//		if (simCheckCollision(currentGenome.g_objectHandles[i], sim_handle_all) == 0) {
			//	int jointHandle;
			//	simGetObjectParent(currentGenome.g_objectHandles[i]);
			//	simSetJointMode(jointHandle, sim_jointmode_passive, NULL);
			//	cout << "jointHandle: " << jointHandle << endl;
	//			simRemoveObject(currentGenome.g_objectHandles[i]);
	//		}
	//	}
	}



	else if (nNHandlingType == 1 && _initialize == false) {
		//	cout << "handle is called" << endl;
		updateAcquiredActivationLevels2();
		//	cout << "Acq [12-13] = " << currentGenome.g_acquiredActivationLevels[12] << ", " << currentGenome.g_acquiredActivationLevels[13];
		updateJointForces2(s_jointControl);
	}
//	}
//	if (nNHandlingType == 1 && _initialize == false) {
	//	cout << "initialize is false" << endl;
//	}
//	cout << "HandleTime!" << endl;
}


// METHOD 2 ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CENN::proprioception2() {
//	cout << "jointAmount: " << currentGenome.g_jointAmount << endl;
	for (int i = 0; i < currentGenome.g_jointAmount; i++) {
//		cout << "i: " << i << endl;
		float jointPos[3];
//		cout << "jointAmount and handleSize: " << currentGenome.g_jointAmount << ", " << currentGenome.g_jointHandles.size() << endl;
		simGetJointPosition(currentGenome.g_jointHandles[i], jointPos);
//		cout << "herp" << endl;
//		cout << "propStart: " << currentGenome.g_proprioceptionStartsAt << endl;
//		cout << "sensinputSize: " << currentGenome.g_senseInput.size();
//		cout << "sensin: " << currentGenome.g_senseInput[currentGenome.g_proprioceptionStartsAt + i] << endl;
		currentGenome.g_senseInput[currentGenome.g_proprioceptionStartsAt + i] = jointPos[0];
//		cout << "derp" << endl;
		//cout << "j" << i << ":" << jointPos[0] << ",";
	}
}

void CENN::vestibularSystem2() {
	// vestibular system = sensory neurons 8 - 25 

	//cout << "cathandle = " << catHandle << endl;
	simGetObjectOrientation(mainHandle[0], -1, headOrientation);
	simGetObjectPosition(mainHandle[0], -1, headPosition);

	for (int i = 0; i < 3; i++) {
		errorHeadPosition[i] = headPosition[i] - previousHeadPosition[i];
		headAcceleration[i] = errorHeadPosition[i] / simGetSimulationTimeStep();
	}
	for (int i = 0; i < 3; i++) {
		previousHeadPosition[i] = headPosition[i];
	}

	for (int i = 0; i < 3; i++) {
		currentGenome.g_senseInput[i + currentGenome.g_vestibularSystemStartsAt] = headAcceleration[i] * 0.1;
		currentGenome.g_senseInput[i + currentGenome.g_vestibularSystemStartsAt + 3] = headAcceleration[i] * -0.1;
	}

	for (int i = 0; i < 3; i++) {
		errorHeadRotation[i] = headRotation[i] - previousHeadRotation[i];
		headRotAcceleration[i] = errorHeadRotation[i];
	}
	for (int i = 0; i < 3; i++) {
		previousHeadRotation[i] = headRotation[i];
	}
	for (int i = 0; i < 3; i++) {
		currentGenome.g_senseInput[i + currentGenome.g_vestibularSystemStartsAt + 6] = headRotAcceleration[i] * 0.1;
		currentGenome.g_senseInput[i + currentGenome.g_vestibularSystemStartsAt + 9] = headRotAcceleration[i] * -0.1;
	}

	normalizedHeadOrientation[0] = headOrientation[0]; //x
	normalizedHeadOrientation[1] = -headOrientation[0]; //x 
	normalizedHeadOrientation[2] = headOrientation[1]; //y
	normalizedHeadOrientation[3] = -headOrientation[1]; //y
	normalizedHeadOrientation[4] = headOrientation[2]; //z
	normalizedHeadOrientation[5] = -headOrientation[2]; //z
	for (int i = 0; i < 6; i++){
		currentGenome.g_senseInput[i + currentGenome.g_vestibularSystemStartsAt + 12] = normalizedHeadOrientation[i];
	}
}

void CENN::tactileSensing2() {
	//	cout << "--";
	// if object collides make the input 1.0 else 0.0
	//	cout << "size h = " << currentGenome.g_tactileHandles.size() << endl;
	for (int i = 0; i < currentGenome.g_tactileHandles.size(); i++) {
		//cout << "handleName: " << simGetObjectName(currentGenome.g_tactileHandles[i]) << endl;
		bool collisionI = simCheckCollision(floorHandle, currentGenome.g_tactileHandles[i]);
		if (collisionI == true) {
			//	cout << "WOW, so much collision!" << endl;
			currentGenome.g_senseInput[i] = 1.0;
		}
		else {
			currentGenome.g_senseInput[i] = 0.0;
		}
	}
	//	cout << "size = " << currentGenome.g_tactileObjects.size() << endl;
	//	currentGenome.g_tactileObjects.resize(1);
	//	cout << "size rz = " << currentGenome.g_tactileObjects.size() << endl;
	//cout << "inpNtSsize = " << currentGenome.g_senseInput.size() << endl;
	// identify the handles

	// check collisions
	//	simCheckCollision();
}

void CENN::updateAcquiredActivationLevels2(){
	// update NN
	if (s_useSensors == true) {
		if (s_useTactileSensing == true) {
			tactileSensing2();
		}
		if (s_useProprioception == true) {
			proprioception2();
//			cout << "Proprioception success" << endl;
		}
		if (s_useVestibularSystem == true) {
			vestibularSystem2();
		}
	}

	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		currentGenome.g_acquiredActivationLevels[i] = currentGenome.g_senseInput[currentGenome.g_inputNeuronToInputSense[i]];
	}

//	cout << "Updating 1.0" << endl;
	vector<float> bufferAcquiredActivationLevels;
	bufferAcquiredActivationLevels.resize(currentGenome.g_amountNeurons);
	for (int i = 0; i < (currentGenome.g_amountNeurons); i++) {
		bufferAcquiredActivationLevels[i] = currentGenome.g_acquiredActivationLevels[i];
	}
	for (int i = 0; i < (currentGenome.g_amountNeurons); i++) {
		if (currentGenome.g_acquiredActivationLevels[i] < -1000) {
//			cout << "ERROR: acquiredActivationLevel " << i << " below -1000" << endl;
		}
	}
	for (int i = 0; i < (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons); i++) {
		if (bufferAcquiredActivationLevels[i] < -1000) {
//			cout << "ERROR: bufferAcquiredActivationLevel " << i << " below -1000!!!" << endl;
		}
	}
//	cout << "Updating 1.1" << endl;
	// check inputNeurons
	for (int i = 0; i < (currentGenome.g_amountInputNeurons); i++) {//amountInterNeurons; i++) { // less than 300 because of the acquired activation level length (inter + output neurons)
		// if the threshold is lower then the acquired activation, the weights of that particular neuron [i] are activated by the for loops below
		//	cout << "Input Threshold = " << currentGenome.g_inputActivationThreshold[i] << ", currentGenome.g_acquiredActivationLevels = " << currentGenome.g_acquiredActivationLevels[i] << ", ";
		if (currentGenome.g_inputActivationThreshold[i] < currentGenome.g_acquiredActivationLevels[i]) {
			// finding the amount of connections of each neuron
			//		cout << "Input Activity present" << endl;
			//		cout << "currentGenome.g_inputNeuronConnectionAmount = " << currentGenome.g_inputNeuronConnectionAmount[i] << endl;
			for (int j = 0; j < currentGenome.g_inputNeuronConnectionAmount[i]; j++){
				// adding all the values in the connections corresponding to the neuron number and connection number
				//	cout << "InputNeuron " << i << " activated";
				int con = currentGenome.g_inputNeuronConnections[i][j];
				bufferAcquiredActivationLevels[con] += (currentGenome.g_inputNeuronWeights[i][j] / 4);// *acquiredActivationLevel[i];
				//	cout << "bufferAcq: " << bufferAcquiredActivationLevels[con] << endl;
			}
		}
		else if (currentGenome.g_inputActivationThreshold[i] > currentGenome.g_acquiredActivationLevels[i]) {
			for (int k = 0; k < currentGenome.g_inputNeuronConnectionAmount[i]; k++){
				int con = currentGenome.g_inputNeuronConnections[i][k];
				bufferAcquiredActivationLevels[con] += 0;
			}
		}
	}
//	cout << "Updating 1.2" << endl;
	// check interNeurons

	for (int i = 0; i < (currentGenome.g_amountInterNeurons); i++) {
		if (currentGenome.g_interNeuronActivationThresholds[i] < currentGenome.g_acquiredActivationLevels[i + currentGenome.g_amountInputNeurons]) {
			for (int j = 0; j < currentGenome.g_interNeuronConnectionAmount[i]; j++){
				int con = currentGenome.g_interNeuronConnections[i][j];
				bufferAcquiredActivationLevels[con] += (currentGenome.g_interNeuronWeights[i][j] / 4);
			//	cout << "bufferAcq Inter: " << bufferAcquiredActivationLevels[con] << "connectedTo: " << con << endl;
			}
		}
		else if (currentGenome.g_interNeuronActivationThresholds[i] > currentGenome.g_acquiredActivationLevels[i + currentGenome.g_amountInputNeurons]) {
			for (int k = 0; k < currentGenome.g_interNeuronConnectionAmount[i]; k++){
				int con = currentGenome.g_interNeuronConnections[i][k];
				bufferAcquiredActivationLevels[con] += 0;
			}
		}
	}
//	cout << "Updating 1.3" << endl;
	//check output neurons
	// something is not right here

//	cout << "amount out: " << currentGenome.g_amountOutputNeurons << endl;
//	cout << "threshold size: " << currentGenome.g_outputActivationThreshold.size() << endl;
//	cout << "acquiredActivationLevels.size():  " << currentGenome.g_acquiredActivationLevels.size() << endl;
//	cout << "input: " << currentGenome.g_amountInputNeurons << endl;
//	cout << "inter: " << currentGenome.g_amountInterNeurons << endl;
//	cout << "connectionAmount: " << currentGenome.g_outputNeuronConnectionAmount.size(); cout << ", " << currentGenome.g_outputNeuronConnectionAmount[0]; cout << ", "
//		<< currentGenome.g_outputNeuronConnections[0].size() << endl;
//	cout << "connection[0][0]: " << currentGenome.g_outputNeuronConnections[0][0] << endl;
//	cout << "connection[8].size(): " << currentGenome.g_outputNeuronConnections[8].size() << endl;
//	cout << "jointActivitySize: " << currentGenome.g_jointActivity.size() << endl;

	for (int i = 0; i < (currentGenome.g_amountOutputNeurons); i++) {
//			cout << "i = " << i << endl;
//			cout << "threshold i = " << currentGenome.g_outputActivationThreshold[i] << endl;
//			cout << "acq i = " << currentGenome.g_acquiredActivationLevels[i + currentGenome.g_amountInputNeurons + currentGenome.g_amountInterNeurons] << endl;
		if (currentGenome.g_outputActivationThreshold[i] < currentGenome.g_acquiredActivationLevels[i + currentGenome.g_amountInputNeurons + currentGenome.g_amountInterNeurons]) {
			//		cout << "output Activation Threshold < acquired @ " << i << endl;
//			cout << "Updating 1.3.1" << endl;
			for (int j = 0; j < currentGenome.g_outputNeuronConnectionAmount[i]; j++){
				int con = currentGenome.g_outputNeuronConnections[i][j];
				currentGenome.g_jointActivity[con] += (currentGenome.g_outputNeuronWeights[i][j] / 4);
				//			cout << "jointActivity present but currentGenome.g_out = " << currentGenome.g_outputNeuronWeights[i][j] << endl;
//				cout << "Updating 1.3.2" << endl;
			}
		}
		else if (currentGenome.g_outputActivationThreshold[i] > currentGenome.g_acquiredActivationLevels[i + currentGenome.g_amountInputNeurons + currentGenome.g_amountInterNeurons]) {
//			cout << "else" << endl;
			for (int k = 0; k < currentGenome.g_outputNeuronConnectionAmount[i]; k++){
				int con = currentGenome.g_outputNeuronConnections[i][k];
				currentGenome.g_jointActivity[con] += 0;
			}
		}
	}
//	cout << "Updating 1.4" << endl;
	// check pattern generators

	//	cout << "bufferAc " << bufferAcquiredActivationLevel[0] << ", cpgTimer = " << cpgTimer[0] << endl;
	for (int i = 0; i < (currentGenome.g_amountNeurons); i++) {
		currentGenome.g_acquiredActivationLevels[i] = bufferAcquiredActivationLevels[i];
		if (currentGenome.g_acquiredActivationLevels[i] > 1) {
			currentGenome.g_acquiredActivationLevels[i] = 1;
		}
		if (currentGenome.g_acquiredActivationLevels[i] < -1) {
			currentGenome.g_acquiredActivationLevels[i] = -1;
		}
		currentGenome.g_acquiredActivationLevels[i] = currentGenome.g_acquiredActivationLevels[i] * leakyIntegrationFactor;
	}
//	cout << "Updating 1.5" << endl;
	for (int i = 0; i < (currentGenome.g_amountNeurons); i++) {
		if (bufferAcquiredActivationLevels[i] < -1000) {
//			cout << "ERROR: bufferAcquiredActivationLevel below -1000" << endl;
		}
	}

	for (int i = 0; i < (currentGenome.g_amountNeurons); i++) {
		if (currentGenome.g_acquiredActivationLevels[i] < -1000) {
//			cout << "ERROR: acquiredActivationLevel below -1000" << endl;
		}
	}
}

void CENN::updateJointForces2(int jointControl){
	// takes the values of the output neurons connected to specific joints
	// float jointPos
//	if (jointControl == 0 || jointControl == 1 || jointControl == 2) {
		//cout << "currentGenome.g_jointActivity[i] = " << endl;
	for (int i = 0; i < currentGenome.g_jointHandles.size(); i++){
		//cout << i << " ,"; 
		simSetJointTargetVelocity(currentGenome.g_jointHandles[i], 10.0);
		simSetJointForce(currentGenome.g_jointHandles[i], 10);

	//	float mass[1];
	//	simGetObjectFloatParameter(currentGenome.g_objectHandles[i], 3005, mass);
	//	cout << "objMass = " << mass[0] << endl; 
		float jointPos[1];
	//	simSetJointMode(currentGenome.g_jointHandles[i], sim_jointmode_passive, 0);
	//	simGetJointPosition(currentGenome.g_jointHandles[i], jointPos);
	//	simSetJointForce(currentGenome.g_jointHandles[i], jointPos[0] * 100);
	//	simSetJointTargetVelocity(currentGenome.g_jointHandles[i], 10);
			//			cout << "joint position = " << jointPos[0] << endl;
			if (jointPos[0] < -0.5 || jointPos[0] > 0.5) {
		//		simSetJointTargetPosition(currentGenome.g_jointHandles[i], 0);
		//		simSetJointTargetVelocity(currentGenome.g_jointHandles[i], 0.1);
		//		simSetJointForce(currentGenome.g_objectHandles[i], 0.01);
			}
		//	else {
	//			simSetJointTargetPosition(currentGenome.g_jointHandles[i], currentGenome.g_jointActivity[i]);
	//			simSetJointTargetVelocity(currentGenome.g_jointHandles[i], currentGenome.g_jointActivity[i] * 100);
	//			simSetJointForce(currentGenome.g_objectHandles[i], 0.0001);
		//	}
		//	float force[3] = { 0, 0, 0 };
		//	simGetJointTargetVelocity(currentGenome.g_jointHandles[i], force);
		//	cout << i << ":" << force[0] << ", ";
			//cout << currentGenome.g_jointActivity[i] << ", ";
		} //cout << endl;
//	}
}

void CENN::addNeuron2(int neuronType){
	switch (neuronType) {
	case 0:
		// add inputNeuron
		currentGenome.g_amountInputNeurons++;
		currentGenome.g_inputNeuronConnectionAmount.resize(currentGenome.g_amountInputNeurons);
		// set the connection amout to 1 by default
	//	currentGenome.g_inputNeuronConnectionAmount[i_amountInputNeurons - 1] = 1;
		// because an input neuron is added on the end of the inputNeuron chain, 
		// all connections of the interNeurons and CPGs need to be incremented
		currentGenome.g_inputNeuronConnections.resize(currentGenome.g_amountInputNeurons);
		currentGenome.g_inputNeuronWeights.resize(currentGenome.g_amountInputNeurons);
		currentGenome.g_inputNeuronConnectionAmount[currentGenome.g_amountInputNeurons - 1] = 1;
		currentGenome.g_inputNeuronConnections[currentGenome.g_amountInputNeurons - 1].resize(currentGenome.g_inputNeuronConnectionAmount[currentGenome.g_amountInputNeurons - 1]);
		currentGenome.g_inputNeuronWeights[currentGenome.g_amountInputNeurons - 1].resize(currentGenome.g_inputNeuronConnectionAmount[currentGenome.g_amountInputNeurons - 1]);
		incrementConnections(currentGenome.g_amountInputNeurons - 1);
		
		currentGenome.g_inputActivationThreshold.resize(currentGenome.g_amountInputNeurons);
		currentGenome.g_inputActivationThreshold[currentGenome.g_amountInputNeurons - 1] = 0.1;
		// set the amount of connections of the new neuron to 1 for now
		
		for (int i = 0; i < currentGenome.g_inputNeuronConnectionAmount[currentGenome.g_amountInputNeurons - 1]; i++) {
			// set random connection and weights
			currentGenome.g_inputNeuronConnections[currentGenome.g_amountInputNeurons - 1][i] = rand() %
				(currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
			currentGenome.g_inputNeuronWeights[currentGenome.g_amountInputNeurons - 1][i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}		
		break;
	case 1:
		// add interNeuron
		currentGenome.g_amountInterNeurons++;
		currentGenome.g_interNeuronConnectionAmount.resize(currentGenome.g_amountInterNeurons);
		// because an inter neuron is added on the end of the interNeuron chain, 
		// all connections of the interNeurons and CPGs need to be incremented
		currentGenome.g_interNeuronConnections.resize(currentGenome.g_amountInterNeurons);
		currentGenome.g_interNeuronWeights.resize(currentGenome.g_amountInterNeurons);
		currentGenome.g_interNeuronConnectionAmount[currentGenome.g_amountInterNeurons - 1] = 1;
		currentGenome.g_interNeuronConnections[currentGenome.g_amountInterNeurons - 1].resize(currentGenome.g_interNeuronConnectionAmount[currentGenome.g_amountInterNeurons - 1]);
		currentGenome.g_interNeuronWeights[currentGenome.g_amountInterNeurons - 1].resize(currentGenome.g_interNeuronConnectionAmount[currentGenome.g_amountInterNeurons - 1]);

		incrementConnections(currentGenome.g_amountInterNeurons + currentGenome.g_amountInputNeurons - 1);
		currentGenome.g_interNeuronActivationThresholds.resize(currentGenome.g_amountInterNeurons);
		currentGenome.g_interNeuronActivationThresholds[currentGenome.g_amountInterNeurons - 1] = 0.1;
		// set the amount of connections of the new neuron to 1 for now

		for (int i = 0; i < currentGenome.g_interNeuronConnectionAmount[currentGenome.g_amountInterNeurons - 1]; i++) {
			// set random connection and weights
			currentGenome.g_interNeuronConnections[currentGenome.g_amountInterNeurons - 1][i] = rand() %
				(currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
			currentGenome.g_interNeuronWeights[currentGenome.g_amountInterNeurons - 1][i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
		break;
	case 2:
		// add outputNeuron
		currentGenome.g_amountOutputNeurons++;
		currentGenome.g_outputNeuronConnectionAmount.resize(currentGenome.g_amountOutputNeurons);
		// because an output neuron is added on the end of the outputNeuron chain, 
		// all connections of the outputNeurons and CPGs need to be incremented
		currentGenome.g_outputNeuronConnections.resize(currentGenome.g_amountOutputNeurons);
		currentGenome.g_outputNeuronWeights.resize(currentGenome.g_amountOutputNeurons);
		currentGenome.g_outputActivationThreshold.resize(currentGenome.g_amountOutputNeurons);
		currentGenome.g_outputActivationThreshold[currentGenome.g_amountOutputNeurons - 1] = 0.1;
		// set the amount of connections of the new neuron to 1 for now
		currentGenome.g_outputNeuronConnectionAmount[currentGenome.g_amountOutputNeurons - 1] = 1;
		currentGenome.g_outputNeuronConnections[currentGenome.g_amountOutputNeurons - 1].resize(currentGenome.g_outputNeuronConnectionAmount[currentGenome.g_amountOutputNeurons-1]);
		currentGenome.g_outputNeuronWeights[currentGenome.g_amountOutputNeurons - 1].resize(currentGenome.g_outputNeuronConnectionAmount[currentGenome.g_amountOutputNeurons-1]);
		for (int i = 0; i < currentGenome.g_outputNeuronConnectionAmount[currentGenome.g_amountOutputNeurons - 1]; i++) {
			// set random connection and weights
			currentGenome.g_outputNeuronConnections[currentGenome.g_amountOutputNeurons - 1][i] = rand() % currentGenome.g_jointAmount;
			currentGenome.g_outputNeuronWeights[currentGenome.g_amountOutputNeurons - 1][i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
		break;
	case 3:
		// add CPG
		currentGenome.g_amountPatternGenerators++;
		currentGenome.g_patternGeneratorConnectionAmount.resize(currentGenome.g_amountPatternGenerators);
		// because an inter neuron is added on the end of the patternGenerator chain, 
		// all connections of the patternGenerators and CPGs need to be incremented
		currentGenome.g_patternGeneratorConnections.resize(currentGenome.g_amountPatternGenerators);
		currentGenome.g_patternGenerationWeights.resize(currentGenome.g_amountPatternGenerators);
		currentGenome.g_patternGeneratorConnectionAmount[currentGenome.g_amountPatternGenerators - 1] = 1;
		currentGenome.g_patternGeneratorConnections[currentGenome.g_amountPatternGenerators - 1].resize(currentGenome.g_patternGeneratorConnectionAmount[currentGenome.g_amountPatternGenerators - 1]);
		currentGenome.g_patternGenerationWeights[currentGenome.g_amountPatternGenerators - 1].resize(currentGenome.g_patternGeneratorConnectionAmount[currentGenome.g_amountPatternGenerators - 1]);
		incrementConnections(currentGenome.g_amountInterNeurons + currentGenome.g_amountInputNeurons + currentGenome.g_amountPatternGenerators - 1);
		// set the amount of connections of the new neuron to 1 for now
		for (int i = 0; i < currentGenome.g_patternGeneratorConnectionAmount[currentGenome.g_amountPatternGenerators - 1]; i++) {
			// set random connection and weights
			currentGenome.g_patternGeneratorConnections[currentGenome.g_amountPatternGenerators - 1][i] = rand() %
				(currentGenome.g_amountInputNeurons + currentGenome.g_amountPatternGenerators + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators); // in this case they are able to connect to inputs and to themselves
			currentGenome.g_patternGenerationWeights[currentGenome.g_amountPatternGenerators - 1][i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
		break;
	}
}

void CENN::removeNeuron2(int neuronType) {

	int randomInputNeuron = -1;
	int randomInterNeuron = -1;
	int randomOutputNeuron = -1;
	int randomPatternGenerator = -1;

	switch (neuronType) {

	case 0:
		// remove random inputNeuron
		currentGenome.g_amountInputNeurons--;
		randomInputNeuron = rand() % currentGenome.g_amountInputNeurons;
	
		currentGenome.g_inputNeuronConnectionAmount.erase(currentGenome.g_inputNeuronConnectionAmount.begin() + randomInputNeuron);
		currentGenome.g_inputNeuronConnections.erase(currentGenome.g_inputNeuronConnections.begin() + randomInputNeuron);
	
		currentGenome.g_inputNeuronWeights.erase(currentGenome.g_inputNeuronWeights.begin() + randomInputNeuron);
		currentGenome.g_inputActivationThreshold.erase(currentGenome.g_inputActivationThreshold.begin() + randomInputNeuron);
		decrementConnections(randomInputNeuron);
		break;
	case 1:
		// add interNeuron
		currentGenome.g_amountInterNeurons--;
		randomInterNeuron = rand() % currentGenome.g_amountInterNeurons;
	
		currentGenome.g_interNeuronConnectionAmount.erase(currentGenome.g_interNeuronConnectionAmount.begin() + randomInterNeuron);
		currentGenome.g_interNeuronConnections.erase(currentGenome.g_interNeuronConnections.begin() + randomInterNeuron);

		currentGenome.g_interNeuronWeights.erase(currentGenome.g_interNeuronWeights.begin() + randomInterNeuron);
		currentGenome.g_interNeuronActivationThresholds.erase(currentGenome.g_interNeuronActivationThresholds.begin() + randomInterNeuron);

		decrementConnections(randomInterNeuron + currentGenome.g_amountInputNeurons);

		break;
	case 2:
		currentGenome.g_amountOutputNeurons--;
		randomOutputNeuron = rand() % currentGenome.g_amountOutputNeurons;
		currentGenome.g_outputNeuronConnectionAmount.erase(currentGenome.g_outputNeuronConnectionAmount.begin() + randomOutputNeuron);
		currentGenome.g_outputNeuronConnections.erase(currentGenome.g_outputNeuronConnections.begin() + randomOutputNeuron);
		currentGenome.g_outputNeuronWeights.erase(currentGenome.g_outputNeuronWeights.begin() + randomOutputNeuron);
		currentGenome.g_outputActivationThreshold.erase(currentGenome.g_outputActivationThreshold.begin() + randomOutputNeuron);

		break;
	case 3:
		currentGenome.g_amountPatternGenerators--;
		randomPatternGenerator = rand() % currentGenome.g_amountPatternGenerators;
		currentGenome.g_patternGeneratorConnectionAmount.erase(currentGenome.g_patternGeneratorConnectionAmount.begin() + randomPatternGenerator);
		currentGenome.g_patternGeneratorConnections.erase(currentGenome.g_patternGeneratorConnections.begin() + randomPatternGenerator);
		currentGenome.g_patternGenerationWeights.erase(currentGenome.g_patternGenerationWeights.begin() + randomPatternGenerator);
		decrementConnections(randomPatternGenerator + currentGenome.g_amountInputNeurons + currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons);
		break;
	}
	cout << "Neuron Removed Succesfully" << endl;
}

void CENN::removeConnection2() {
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		currentGenome.g_inputNeuronConnectionAmount[i] = currentGenome.g_inputNeuronConnectionAmount[i]--;
		if (currentGenome.g_inputNeuronConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_inputNeuronConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_inputNeuronConnectionAmount[i] < 0) {
			currentGenome.g_inputNeuronConnectionAmount[i] = 0;
		}
		int randomDeletion = rand() % currentGenome.g_inputNeuronConnectionAmount[i];
		currentGenome.g_inputNeuronConnections[i].erase(currentGenome.g_inputNeuronConnections[i].begin() + (randomDeletion));
		currentGenome.g_inputNeuronWeights[i].erase(currentGenome.g_inputNeuronWeights[i].begin() + (randomDeletion));
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		currentGenome.g_interNeuronConnectionAmount[i] = currentGenome.g_interNeuronConnectionAmount[i]--;
		if (currentGenome.g_interNeuronConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_interNeuronConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_interNeuronConnectionAmount[i] < 0) {
			currentGenome.g_interNeuronConnectionAmount[i] = 0;
		}
		int randomDeletion = rand() % currentGenome.g_interNeuronConnectionAmount[i];
		currentGenome.g_interNeuronConnections[i].erase(currentGenome.g_interNeuronConnections[i].begin() + (randomDeletion));
		currentGenome.g_interNeuronWeights[i].erase(currentGenome.g_interNeuronWeights[i].begin() + (randomDeletion));
	}

	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		currentGenome.g_outputNeuronConnectionAmount[i] = currentGenome.g_outputNeuronConnectionAmount[i]--;
		if (currentGenome.g_outputNeuronConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_outputNeuronConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_outputNeuronConnectionAmount[i] < 0) {
			currentGenome.g_outputNeuronConnectionAmount[i] = 0;
		}
		int randomDeletion = rand() % currentGenome.g_outputNeuronConnectionAmount[i];
		currentGenome.g_outputNeuronConnections[i].erase(currentGenome.g_outputNeuronConnections[i].begin() + (randomDeletion));
		currentGenome.g_outputNeuronWeights[i].erase(currentGenome.g_outputNeuronWeights[i].begin() + (randomDeletion));
	}

	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		currentGenome.g_patternGeneratorConnectionAmount[i] = currentGenome.g_patternGeneratorConnectionAmount[i]--;
		if (currentGenome.g_patternGeneratorConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_patternGeneratorConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_patternGeneratorConnectionAmount[i] < 0) {
			currentGenome.g_patternGeneratorConnectionAmount[i] = 0;
		}
		int randomDeletion = rand() % currentGenome.g_patternGeneratorConnectionAmount[i];
		currentGenome.g_patternGeneratorConnections[i].erase(currentGenome.g_patternGeneratorConnections[i].begin() + (randomDeletion));
		currentGenome.g_patternGenerationWeights[i].erase(currentGenome.g_patternGenerationWeights[i].begin() + (randomDeletion));
	}
	cout << "Succesfully removed a connection" << endl;
}

void CENN::addConnection2() {
//	cout << "1" << endl;
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
	//	cout << "1.1." << i << endl;
		currentGenome.g_inputNeuronConnectionAmount[i] = currentGenome.g_inputNeuronConnectionAmount[i]++;
		if (currentGenome.g_inputNeuronConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_inputNeuronConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_inputNeuronConnectionAmount[i] < 0) {
			currentGenome.g_inputNeuronConnectionAmount[i] = 0;
		}
		currentGenome.g_inputNeuronConnections[i].resize(currentGenome.g_inputNeuronConnectionAmount[i]);
		int j = currentGenome.g_inputNeuronConnectionAmount[i] - 1;
		currentGenome.g_inputNeuronConnections[i][j] = rand() % (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
		currentGenome.g_inputNeuronWeights[i].resize(currentGenome.g_inputNeuronConnectionAmount[i]);
		currentGenome.g_inputNeuronWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
	//	cout << "1.2." << i << endl;

		currentGenome.g_interNeuronConnectionAmount[i] = currentGenome.g_interNeuronConnectionAmount[i]++;
		if (currentGenome.g_interNeuronConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_interNeuronConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_interNeuronConnectionAmount[i] < 0) {
			currentGenome.g_interNeuronConnectionAmount[i] = 0;
		}
		int j = currentGenome.g_interNeuronConnectionAmount[i] -1;
		currentGenome.g_interNeuronConnections[i].resize(currentGenome.g_interNeuronConnectionAmount[i]);
		currentGenome.g_interNeuronConnections[i][j] = 1; // rand() % (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
		currentGenome.g_interNeuronWeights[i].resize(currentGenome.g_interNeuronConnectionAmount[i]);
		currentGenome.g_interNeuronWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
	//	cout << "1.3." << i << endl;

		currentGenome.g_outputNeuronConnectionAmount[i] = currentGenome.g_outputNeuronConnectionAmount[i]++;
		if (currentGenome.g_outputNeuronConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_outputNeuronConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_outputNeuronConnectionAmount[i] < 0) {
			currentGenome.g_outputNeuronConnectionAmount[i] = 0;
		}
		int j = currentGenome.g_outputNeuronConnectionAmount[i] -1;
		currentGenome.g_outputNeuronConnections[i].resize(currentGenome.g_outputNeuronConnectionAmount[i]);
		currentGenome.g_outputNeuronConnections[i][j] = rand() % currentGenome.g_jointAmount;
		currentGenome.g_outputNeuronWeights[i].resize(currentGenome.g_outputNeuronConnectionAmount[i]);
		currentGenome.g_outputNeuronWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}

	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		currentGenome.g_patternGeneratorConnectionAmount[i] = currentGenome.g_patternGeneratorConnectionAmount[i]++;
		if (currentGenome.g_patternGeneratorConnectionAmount[i] > maxAmountConnections) {
			currentGenome.g_patternGeneratorConnectionAmount[i] = maxAmountConnections;
		}
		else if (currentGenome.g_patternGeneratorConnectionAmount[i] < 0) {
			currentGenome.g_patternGeneratorConnectionAmount[i] = 0;
		}
		int j = currentGenome.g_patternGeneratorConnectionAmount[i] -1;
		currentGenome.g_patternGeneratorConnections[i].resize(currentGenome.g_patternGeneratorConnectionAmount[i]);
		currentGenome.g_patternGeneratorConnections[i][j] = (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
		currentGenome.g_patternGenerationWeights[i].resize(currentGenome.g_patternGeneratorConnectionAmount[i]);
		currentGenome.g_patternGenerationWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}
}

void CENN::mutation2() {
	currentGenome.g_mutationRate;

//	cout << "Adding a connection" << endl;
	addConnection2();
//	cout << "Removing a connection" << endl;
	removeConnection2();
//	cout << "Adding a Neuron" << endl;
//	addNeuron2(0);
//	addNeuron2(1);
//	addNeuron2(2);
//	addNeuron2(3);
//	cout << "removing a neuron" << endl;
//	removeNeuron2(0);
//	removeNeuron2(1);
//	removeNeuron2(2);
//	removeNeuron2(3);

//	if (nNHandlingType == 1){
//		printLStateProperties(currentGenome);
//		mutateLProperties(0.95);
//		printLStateProperties(currentGenome);
	//	extremeMutateLProperties(0.5);
	//	extremeMutateLProperties(0.5);
//	}

//	cout << "mutating weights and connections" << endl;

	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_inputNeuronConnectionAmount[i]; j++){
			currentGenome.g_inputNeuronConnections[i][j] = rand() % (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
		}
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		for (int j = 0; j < currentGenome.g_interNeuronConnectionAmount[i]; j++){
			currentGenome.g_interNeuronConnections[i][j] = rand() % (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
		}
	}
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_outputNeuronConnectionAmount[i]; j++){
			currentGenome.g_outputNeuronConnections[i][j] = rand() % currentGenome.g_jointAmount;
		}
	}
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		for (int j = 0; j < currentGenome.g_patternGeneratorConnectionAmount[i]; j++){
			currentGenome.g_patternGeneratorConnections[i][j] = (currentGenome.g_amountInterNeurons + currentGenome.g_amountOutputNeurons + currentGenome.g_amountPatternGenerators) + (currentGenome.g_amountInputNeurons);
		}
	}
	// mutate weights
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_inputNeuronConnectionAmount[i]; j++){
			currentGenome.g_inputNeuronWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		for (int j = 0; j < currentGenome.g_interNeuronConnectionAmount[i]; j++){
			currentGenome.g_interNeuronWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
	}
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_outputNeuronConnectionAmount[i]; j++){
			currentGenome.g_outputNeuronWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
	}
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		for (int j = 0; j < currentGenome.g_patternGeneratorConnectionAmount[i]; j++){
			currentGenome.g_patternGenerationWeights[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		}
	}
	// mutate thresholds
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		currentGenome.g_interNeuronActivationThresholds[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		currentGenome.g_interNeuronActivationThresholds[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		currentGenome.g_outputActivationThreshold[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
	}
	// mutate CPG
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++){
		for (int j = 0; j < currentGenome.g_patternGenerationTime[i].size(); j++) {
			currentGenome.g_patternGenerationTime[i][j] = 0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - 0.0)));
		}
	} 
//	addNeuron2(0);
}

void CENN::genomeSaver2() {
	// NN parameters
	//cout << "Creature morphology is being saved" << endl;

	ofstream savedGenome;
	ostringstream genomeFileName;
	genomeFileName << "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\population\\NN" << indCounter << ".csv";
	savedGenome.open(genomeFileName.str()); //ios::out | ios::ate | ios::app);
	savedGenome << "This file consists of the morphological and NN parameters of an individual\n";
	savedGenome << "parent of this individual = " << "reserved" << endl;
	savedGenome << "fitness of this individual = " << individualFitness << endl;
	savedGenome << "Individual belongs to species: " << "reserved" << endl;
	savedGenome << "Individual is located in island Population: " << "reserved" << endl;
	savedGenome << "tactile sensor object names: " << "reserved" << endl;
	savedGenome << "These are the NN properties: " << endl;
	savedGenome << ",#amountNeurons," << currentGenome.g_amountNeurons << "," << endl;
	savedGenome << ",#amountInputNeurons," << currentGenome.g_amountInputNeurons << "," << endl;
	savedGenome << ",#amountInterNeurons," << currentGenome.g_amountInterNeurons << "," << endl;
	savedGenome << ",#amountOutputNeurons," << currentGenome.g_amountOutputNeurons << "," << endl;
	savedGenome << ",#amountCPGs," << currentGenome.g_amountPatternGenerators << "," << endl;
	savedGenome << ",#inputNeuronConnectionAmount,";
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		savedGenome << currentGenome.g_inputNeuronConnectionAmount[i] << ",";
	} savedGenome << endl;
	savedGenome << ",#interNeuronConnectionAmount,";
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		savedGenome << currentGenome.g_interNeuronConnectionAmount[i] << ",";
	} savedGenome << endl;
	savedGenome << ",#outputNeuronConnectionAmount,";
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		savedGenome << currentGenome.g_outputNeuronConnectionAmount[i] << ",";
	} savedGenome << endl;
	savedGenome << ",#patternGeneratorConnectionAmount,";
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		savedGenome << currentGenome.g_patternGeneratorConnectionAmount[i] << ",";
	} savedGenome << endl;

	savedGenome << ",#inputNeuronConnections,";
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_inputNeuronConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_inputNeuronConnections[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#interNeuronConnections,";
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		for (int j = 0; j < currentGenome.g_interNeuronConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_interNeuronConnections[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#outputNeuronConnections,";
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_outputNeuronConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_outputNeuronConnections[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#paternGeneratorNeuronConnections,";
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		cout << ", " << currentGenome.g_patternGeneratorConnectionAmount[i];
		for (int j = 0; j < currentGenome.g_patternGeneratorConnectionAmount[i]; j++) {
			cout << "pgCon [" << i << j << "] = " << currentGenome.g_patternGeneratorConnections[i][j] << ", ";
			savedGenome << currentGenome.g_patternGeneratorConnections[i][j] << ",";
		}
	} savedGenome << endl;

	savedGenome << ",#inputNeuronWeights,";
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_inputNeuronConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_inputNeuronWeights[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#interNeuronWeights,";
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		for (int j = 0; j < currentGenome.g_interNeuronConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_interNeuronWeights[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#outputNeuronWeights,";
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_outputNeuronConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_outputNeuronWeights[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#paternGeneratorNeuronWeights,";
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		for (int j = 0; j < currentGenome.g_patternGeneratorConnectionAmount[i]; j++) {
			savedGenome << currentGenome.g_patternGenerationWeights[i][j] << ",";
		}
	} savedGenome << endl;

	savedGenome << ",#inputNeuronThresholds,";
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		savedGenome << currentGenome.g_inputActivationThreshold[i] << ",";
	} savedGenome << endl;
	savedGenome << ",#interNeuronThresholds,";
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		savedGenome << currentGenome.g_interNeuronActivationThresholds[i] << ",";
	} savedGenome << endl;
	savedGenome << ",#outputNeuronThresholds,";
	for (int i = 0; i < currentGenome.g_amountOutputNeurons; i++) {
		savedGenome << currentGenome.g_outputActivationThreshold[i] << ",";
	} savedGenome << endl;

	savedGenome << ",#initialPatternGeneratorTime,";
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		for (int j = 0; j < currentGenome.g_patternGenerationTime.size(); j++){
			savedGenome << currentGenome.g_patternGenerationTime[i][j] << ",";
		}
	} savedGenome << endl;
	savedGenome << ",#patternGeneratorType,";
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		savedGenome << currentGenome.g_patternGenerationType[i] << ",";
	} savedGenome << endl;

	savedGenome << "\nThese are the Object Properties:\n";
	savedGenome << "\n,amountObj," << currentGenome.g_objectAmount << "," << endl;
	savedGenome << "\n,#objPos,";
//	cout << "objPos: " << i_objectPosition[8][0] << ", " << i_objectPosition[8][1] << ", " << i_objectPosition[8][2] << endl;
	for (int i = 0; i < currentGenome.g_objectPosition.size(); i++) {
		for (int j = 0; j < currentGenome.g_objectPosition[i].size(); j++) {
			savedGenome << currentGenome.g_objectPosition[i][j] << ",";
		}
	}
	
	cout << "object position saved" << endl;
	savedGenome << "\n,#objOr,";
	
	for (int i = 0; i < currentGenome.g_objectOrientation.size(); i++) {
		for (int j = 0; j < currentGenome.g_objectOrientation[i].size(); j++) {
			savedGenome << currentGenome.g_objectOrientation[i][j] << ",";
		}
	}
	
	cout << "object orientation saved" << endl;
	savedGenome << "\n,#objSize,";
	for (int i = 0; i < currentGenome.g_objectSizes.size(); i++) {
		for (int j = 0; j < 3; j++) {
			savedGenome << currentGenome.g_objectSizes[i][j] << ",";
		}
	}

	cout << "object size saved" << endl;
	savedGenome << "\n,#objMass,";
	for (int i = 0; i < currentGenome.g_objectMass.size(); i++) {
		savedGenome << currentGenome.g_objectMass[i] << ",";
	}

	cout << "object mass saved" << endl;
	savedGenome << "\n,#objPar,";
	for (int i = 0; i < currentGenome.g_objectParent.size(); i++) {
		savedGenome << currentGenome.g_objectParent[i] << ",";
	}

	cout << "object parent saved" << endl;
	savedGenome << "\n,#objType,";
	for (int i = 0; i < currentGenome.g_objectType.size(); i++) {
		savedGenome << currentGenome.g_objectType[i] << ",";
	}

	savedGenome << "\n,object friction,reserved,";
	savedGenome << "\n\nJointProperties:\n";
	savedGenome << "\n,#amountJoints," << currentGenome.g_jointAmount;
	savedGenome << "\n,#jointPos,";
	for (int i = 0; i < currentGenome.g_jointPosition.size(); i++) {
		for (int j = 0; j < currentGenome.g_jointPosition[i].size(); j++) {
			savedGenome << currentGenome.g_jointPosition[i][j] << ",";
		}
	}
	
	cout << "joint position saved" << endl;
	savedGenome << "\n,#jointOr,";
	for (int i = 0; i < currentGenome.g_jointOrientation.size(); i++) {
		for (int j = 0; j < currentGenome.g_jointOrientation[i].size(); j++) {
			savedGenome << currentGenome.g_jointOrientation[i][j] << ",";
		}
	}
	
	
	cout << "joint orientation saved" << endl;
	savedGenome << "\n,#jointSize,";
	for (int i = 0; i < currentGenome.g_jointSizes.size(); i++) {
		for (int j = 0; j < currentGenome.g_jointSizes[i].size(); j++) {
			savedGenome << currentGenome.g_jointSizes[i][j] << ",";
		}
	}
	
	cout << "joint size saved" << endl;
savedGenome << "\n,#jointMass,";
for (int i = 0; i < currentGenome.g_jointMass.size(); i++) {
	savedGenome << currentGenome.g_jointMass[i] << ",";
}
cout << "joint mass saved" << endl;
savedGenome << "\n,#jointPar,";
for (int i = 0; i < currentGenome.g_jointParent.size(); i++) {
	savedGenome << currentGenome.g_jointParent[i] << ",";
}
cout << "joint parent saved" << endl;
savedGenome << "\n,#jointType,";
for (int i = 0; i < currentGenome.g_jointType.size(); i++) {
	savedGenome << currentGenome.g_jointType[i] << ",";
}
cout << "joint type saved" << endl;
savedGenome << "\n,#jointMaxMinAngles,";
for (int i = 0; i < currentGenome.g_jointMaxMinAngles.size(); i++) {
	for (int j = 0; j < 2; j++) {
		savedGenome << currentGenome.g_jointMaxMinAngles[i][j] << ",";
	}
}
cout << "jointMaxMinAngles Saved" << endl;
savedGenome << "\n,#jointMaxVelocities,";
for (int i = 0; i < currentGenome.g_jointMaxVelocity.size(); i++) {
	savedGenome << currentGenome.g_jointMaxVelocity[i] << ",";
}
cout << "jointMaxVelocities saved" << endl;
savedGenome << "\n,#jointMaxForce,";
for (int i = 0; i < currentGenome.g_jointMaxForce.size(); i++) {
	savedGenome << currentGenome.g_jointMaxForce[i] << ",";
}
cout << "jointMaxForces saved" << endl;
savedGenome << "\n\nEnd of construction info" << endl;
savedGenome.close();
cout << "genome saved" << endl;
/**/
}

void CENN::resetParameters2() {
	for (int i = 0; i < currentGenome.g_jointActivity.size(); i++){
		currentGenome.g_jointActivity[i] = 0;
	}
	for (int i = 0; i < currentGenome.g_amountSenseParts; i++){
		currentGenome.g_senseInput[i] = 0;
	}
}

float randFloat(float lower, float upper){
	float value = lower + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upper - lower)));
	return value;
}



void CENN::mutateLProperties(float mutRate){
	// plant properties
	vector<vector<vector<float>>> mutatedLStateProperties;
	mutatedLStateProperties = currentGenome.g_lStateProperties;
	// five different states
	//	mutatedLStateProperties.resize(6);

	//	cout << "increasing size" << endl;
	// increase size
	for (int k = 0; k < mutatedLStateProperties.size() -1; k++) {
		if (randFloat(0, 1.0) < mutRate) {

			int tempSize = mutatedLStateProperties[k].size() + 1;
			if (tempSize < mutatedLStateProperties.size()) {
				mutatedLStateProperties[k].resize(tempSize);
				mutatedLStateProperties[k][(tempSize - 1)].resize(14);
				for (int i = 0; i < mutatedLStateProperties[k].size(); i++) {
					mutatedLStateProperties[k][tempSize - 1][0] = 0.1;
					mutatedLStateProperties[k][tempSize - 1][1] = 0.1;
					mutatedLStateProperties[k][tempSize - 1][2] = 0.1;
					mutatedLStateProperties[k][tempSize - 1][3] = 0.0;
					mutatedLStateProperties[k][tempSize - 1][4] = 0.0;
					mutatedLStateProperties[k][tempSize - 1][5] = mutatedLStateProperties[k][tempSize - 1][2];
					mutatedLStateProperties[k][tempSize - 1][6] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415);
					mutatedLStateProperties[k][tempSize - 1][7] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415);
					mutatedLStateProperties[k][tempSize - 1][8] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415);
					mutatedLStateProperties[k][tempSize - 1][9] = mutatedLStateProperties[k][tempSize - 1][9] * mutatedLStateProperties[k][tempSize - 1][1] * mutatedLStateProperties[k][tempSize - 1][2];
					mutatedLStateProperties[k][tempSize - 1][10] = rand() % mutatedLStateProperties.size();
					mutatedLStateProperties[k][tempSize - 1][11] = randFloat(0.0, 1.0);
					mutatedLStateProperties[k][tempSize - 1][12] = randFloat(0.0, 1.0);
					mutatedLStateProperties[k][tempSize - 1][13] = randFloat(0.0, 1.0);

					//	cout << "Hello?" << endl;
				}
			}
		}
	}
	if (randFloat(0, 1.0) < mutRate) {

		int tempSize = mutatedLStateProperties[4].size() + 1;
		if (tempSize < mutatedLStateProperties.size()) {
			mutatedLStateProperties[4].resize(tempSize);
			mutatedLStateProperties[4][(tempSize - 1)].resize(14);
			for (int i = 0; i < mutatedLStateProperties[4].size(); i++) {
				mutatedLStateProperties[4][tempSize - 1][0] = 0.1;
				mutatedLStateProperties[4][tempSize - 1][1] = 0.1;
				mutatedLStateProperties[4][tempSize - 1][2] = 0.1;
				mutatedLStateProperties[4][tempSize - 1][3] = 0.0;
				mutatedLStateProperties[4][tempSize - 1][4] = 0.0;
				mutatedLStateProperties[4][tempSize - 1][5] = mutatedLStateProperties[4][tempSize - 1][2];
				mutatedLStateProperties[4][tempSize - 1][6] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415);
				mutatedLStateProperties[4][tempSize - 1][7] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415);
				mutatedLStateProperties[4][tempSize - 1][8] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415);
				mutatedLStateProperties[4][tempSize - 1][9] = mutatedLStateProperties[4][tempSize - 1][9] * mutatedLStateProperties[4][tempSize - 1][1] * mutatedLStateProperties[4][tempSize - 1][2];
				mutatedLStateProperties[4][tempSize - 1][10] = rand() % mutatedLStateProperties.size();
				mutatedLStateProperties[4][tempSize - 1][11] = randFloat(0.0, 1.0);
				mutatedLStateProperties[4][tempSize - 1][12] = randFloat(0.0, 1.0);
				mutatedLStateProperties[4][tempSize - 1][13] = randFloat(0.0, 1.0);

				//	cout << "Hello?" << endl;
			}
		}
	}
	//	cout << "decreasing size" << endl;
	// decrease size
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int tempSize = mutatedLStateProperties[k].size() - 1;
			if (tempSize > 0){
				mutatedLStateProperties[k].resize(tempSize);
			}
		}
	}

	//	cout << "about to cross state" << endl;
	// copy properties across state
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int randState = rand() % mutatedLStateProperties[k].size();
			for (int l = 0; l < mutatedLStateProperties[k].size(); l++) {
				mutatedLStateProperties[l] = mutatedLStateProperties[randState];
			}
		}
	}

	//	cout << "about to randomize" << endl;
	// randomize size: if this happens, copy all lstates of some
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int randState = rand() % mutatedLStateProperties[k].size();
			mutatedLStateProperties[k][0] = mutatedLStateProperties[k][randState];
			int tempSize = rand() % 4 + 1;
			if (tempSize > 0){
				mutatedLStateProperties[k].resize(tempSize);
			}
			for (int l = 0; l < mutatedLStateProperties[k].size(); l++) {
				mutatedLStateProperties[k][l] = mutatedLStateProperties[k][0];
			}
		}
	}

	//	cout << "about to randomly mutate pars" << endl;
//	if (randFloat(0, 1.0) < mutRate) {
		for (int i = 0; i < mutatedLStateProperties.size(); i++) {
			//	cout << "i: " << i << endl;
			for (size_t j = 0; j < mutatedLStateProperties[i].size(); j++)
			{
				//	cout << "j: " << j << endl; 
				// randomly mutate properties
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][0] = randFloat(0.01, 0.15); // length
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][1] = randFloat(0.01, 0.15); // width
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][2] = randFloat(0.01, 0.15); // height
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][3] = randFloat(0.00, 0.00); // x
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][4] = randFloat(0.00, 0.00); // y
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][5] = mutatedLStateProperties[i][j][2];
					//		mutatedLStateProperties[i][j][5] = mutatedLStateProperties[i][j][2] + 0.1; // z
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][6] = randFloat(-2.0 * 3.1415, 2.0 * 3.1415); // rotX
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][7] = randFloat(-2.0 * 3.1415, 2.0* 3.1415); // rotY
				}
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][8] = randFloat(-2.0 * 3.1415, 2.0* 3.1415); // rotZ
				}
				//	if (randFloat(0, 1.0) < mutRate) {
				//		mutatedLStateProperties[i][j][9] = randFloat(0.01, 0.5); // mass
				//	}
				mutatedLStateProperties[i][j][9] = mutatedLStateProperties[i][j][9] * mutatedLStateProperties[i][j][1] * mutatedLStateProperties[i][j][2];
				if (randFloat(0, 1.0) < mutRate) {
					mutatedLStateProperties[i][j][10] = randFloat(0.0, (mutatedLStateProperties.size() - 1));
				}
	//		}
		}
	} /**/
	//	cout << "done randomizing" << endl;
	currentGenome.g_lStateProperties = mutatedLStateProperties;
}


void CENN::mutateModularLProperties(float mutRate){
	// plant properties
	vector<vector<vector<float>>> mutatedLStateProperties;
	mutatedLStateProperties = currentGenome.g_lStateProperties;
	// five different states
	//	mutatedLStateProperties.resize(6);

	//	cout << "increasing size" << endl;
	// increase size
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {

			int tempSize = mutatedLStateProperties[k].size() + 1;
			if (tempSize < 6) {
				mutatedLStateProperties[k].resize(tempSize);
				mutatedLStateProperties[k][(tempSize - 1)].resize(14);
				for (int i = 0; i < mutatedLStateProperties[k].size(); i++) {
					mutatedLStateProperties[k][tempSize - 1][0] = 0.1;
					mutatedLStateProperties[k][tempSize - 1][1] = 0.1;
					mutatedLStateProperties[k][tempSize - 1][2] = 0.1;
					mutatedLStateProperties[k][tempSize - 1][3] = 0.0;
					mutatedLStateProperties[k][tempSize - 1][4] = 0.0;
					mutatedLStateProperties[k][tempSize - 1][5] = mutatedLStateProperties[k][tempSize - 1][2];
					mutatedLStateProperties[k][tempSize - 1][6] = 0; // (rand() % 3 - 1) // * 3.1415;
					mutatedLStateProperties[k][tempSize - 1][7] = 0; // (rand() % 3 - 1) // 3.1415;
					mutatedLStateProperties[k][tempSize - 1][8] = 0; // (rand() % 3 - 1) //* 3.1415;
					mutatedLStateProperties[k][tempSize - 1][9] = mutatedLStateProperties[k][tempSize - 1][9] * mutatedLStateProperties[k][tempSize - 1][1] * mutatedLStateProperties[k][tempSize - 1][2];
					mutatedLStateProperties[k][tempSize - 1][10] = rand() % mutatedLStateProperties.size();
					mutatedLStateProperties[k][tempSize - 1][11] = randFloat(0.0, 1.0);
					mutatedLStateProperties[k][tempSize - 1][12] = randFloat(0.0, 1.0);
					mutatedLStateProperties[k][tempSize - 1][13] = randFloat(0.0, 1.0);

					//	cout << "Hello?" << endl;
				}
			}
		}
	}
	//	cout << "decreasing size" << endl;
	// decrease size
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int tempSize = mutatedLStateProperties[k].size() - 1;
			if (tempSize > 0){
				mutatedLStateProperties[k].resize(tempSize);
			}
		}
	}

	//	cout << "about to cross state" << endl;
	// copy properties across state
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int randState = rand() % mutatedLStateProperties[k].size();
			for (int l = 0; l < mutatedLStateProperties[k].size(); l++) {
				mutatedLStateProperties[l] = mutatedLStateProperties[randState];
			}
		}
	}

	//	cout << "about to randomize" << endl;
	// randomize size: if this happens, copy all lstates of some
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int randState = rand() % mutatedLStateProperties[k].size();
			mutatedLStateProperties[k][0] = mutatedLStateProperties[k][randState];
			int tempSize = rand() % 4 + 1;
			if (tempSize > 0){
				mutatedLStateProperties[k].resize(tempSize);
			}
			for (int l = 0; l < mutatedLStateProperties[k].size(); l++) {
				mutatedLStateProperties[k][l] = mutatedLStateProperties[k][0];
			}
		}
	}

	//	cout << "about to randomly mutate pars" << endl;
	for (int i = 0; i < mutatedLStateProperties.size(); i++) {
		//	cout << "i: " << i << endl;
		for (size_t j = 0; j < mutatedLStateProperties[i].size(); j++)
		{
			//	cout << "j: " << j << endl; 
			// randomly mutate properties
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][0] = 0.1; // randFloat(0.01, 0.15); // length
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][1] = 0.1; // randFloat(0.01, 0.15); // width
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][2] = 0.1; // randFloat(0.01, 0.15); // height
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][3] = randFloat(0.00, 0.00); // x
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][4] = randFloat(0.00, 0.00); // y
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][5] = mutatedLStateProperties[i][j][2];
				//		mutatedLStateProperties[i][j][5] = mutatedLStateProperties[i][j][2] + 0.1; // z
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][6] = (rand() % 3 - 1) * 3.1415; // rotX
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][7] = (rand() % 3 - 1) * 3.1415; // rotY
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][8] = (rand() % 3 - 1) * 3.1415; // rotZ
			}
			//	if (randFloat(0, 1.0) < mutRate) {
			//		mutatedLStateProperties[i][j][9] = randFloat(0.01, 0.5); // mass
			//	}
			mutatedLStateProperties[i][j][9] = mutatedLStateProperties[i][j][9] * mutatedLStateProperties[i][j][1] * mutatedLStateProperties[i][j][2];
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][10] = randFloat(0.0, (mutatedLStateProperties.size() - 1));
			}
		}
	} /**/
	//	cout << "done randomizing" << endl;
	currentGenome.g_lStateProperties = mutatedLStateProperties;
}


void CENN::extremeMutateLProperties(float mutRate){
	// plant properties
	cout << "Making sure everything is correct" << endl;
	vector<vector<vector<float>>> mutatedLStateProperties;
	mutatedLStateProperties = currentGenome.g_lStateProperties;
	// five different states
//	mutatedLStateProperties.resize(6);
	cout << "randFloat = " << randFloat(0, 1.0) << endl;
	// increase size
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			cout << "higher than mutrate" << endl;
			int tempSize = mutatedLStateProperties[k].size() + 1;
			if (tempSize < 6) {
				mutatedLStateProperties[k].resize(tempSize);
				mutatedLStateProperties[k][(tempSize - 1)].resize(11);
				for (int i = 0; i < mutatedLStateProperties[k].size(); i++) {
			//		mutatedLStateProperties[k][tempSize - 1][0] = 0.1;
			//		mutatedLStateProperties[k][tempSize - 1][1] = 0.1;
			//		mutatedLStateProperties[k][tempSize - 1][2] = 0.1;
			//		mutatedLStateProperties[k][tempSize - 1][3] = 0.01;
			//		mutatedLStateProperties[k][tempSize - 1][4] = 0.0;
			//		mutatedLStateProperties[k][tempSize - 1][5] = 0.01;
			//		mutatedLStateProperties[k][tempSize - 1][6] = 0.0;
			//		mutatedLStateProperties[k][tempSize - 1][7] = 0.0;
			//		mutatedLStateProperties[k][tempSize - 1][8] = 0.0;
			//		mutatedLStateProperties[k][tempSize - 1][9] = 0.0;
			//		mutatedLStateProperties[k][tempSize - 1][10] = rand() % 5;
			//		cout << "Hello?" << endl;
				}
			}
		}
	}
	// increase size
	for (int k = 0; k < mutatedLStateProperties.size(); k++) {
		if (randFloat(0, 1.0) < mutRate) {
			int tempSize = mutatedLStateProperties[k].size() - 1;
			if (tempSize > 0){
				mutatedLStateProperties[k].resize(tempSize);
			}
		}
	}


	//cout << "2.0" << endl;
	for (int i = 0; i < mutatedLStateProperties.size(); i++) {
		for (size_t j = 0; j < mutatedLStateProperties[i].size(); j++)
		{
		//	cout << "1." << i << "." << j << endl;
			mutatedLStateProperties[i][j].resize(11);
			for (size_t k = 0; k < 11; k++)
			{
				mutatedLStateProperties[i][j][k] = 0.0f;
			}
			// set properties manually
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][0] = randFloat(0.01, 1.0); // length
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][1] = randFloat(0.01, 1.0); // width
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][2] = randFloat(0.01, 1.0); // height
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][3] = randFloat(0.00, 0.00); // x
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][4] = randFloat(0.00, 0.00); // y
			}
		//	if (randFloat(0, 1.0) > mutRate) {
				mutatedLStateProperties[i][j][5] = mutatedLStateProperties[i][j][2];
				//		mutatedLStateProperties[i][j][5] = mutatedLStateProperties[i][j][2] + 0.1; // z

		//	}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][6] = randFloat(-5.0, 5.0); // rotX
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][7] = randFloat(-5.0, 5.0); // rotY
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][8] = randFloat(-5.0, 5.0); // rotZ
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][9] = randFloat(0.01, 1.0); // mass
			}
			if (randFloat(0, 1.0) < mutRate) {
				mutatedLStateProperties[i][j][10] = randFloat(0.0, 5.0);
			}
		}
	}
	currentGenome.g_lStateProperties = mutatedLStateProperties;
}


void CENN::initializeAgents2() {
	cout << "-1" << endl;
	//	loadCreature(3);
	//	cout << "initialize agent function is called. " << endl;
	//	CCreatureCreator* createCreature;
	//	createCreature = new CCreatureCreator;
	//	createCreature->createCreature(3);
	//	mainHandle = createCreature->creatureHandle;
	assert(mainHandle[0] > 0);
	//	cout << "mainhandle = " << mainHandle; 
	mainHandleName = simGetObjectName(mainHandle[0]);
//	cout << "mainHandleName: " << mainHandleName << endl; 
	// check if the creature corresponds with the amount of joints etc
	int jointCount[1];
	simGetObjectsInTree(mainHandle[0], 1, 1, jointCount);
	//	cout << "s_amountJoints = " << s_amountJoints << endl;
	//	cout << "jointCount = " << jointCount[0] << endl;
	if (jointCount[0] == s_amountJoints) {
		cout << "Amount joints in ENN corresponds with creature" << endl;
	}
	
	else {
	//	cout << "ERROR: Amount of joints in ENN does not correspond with the amount of neurons in creature" << endl;
	//	cout << "Amount joints = " << currentGenome.g_jointAmount << ", and amount inputNeurons = " << endl;
	}

	// also for debugging
	// load the amount of joints and save them in CENN so they can be easily accessed. 
	//int jointAm[1];
	cout << "1.0" << endl;
	int	objAm[1];
	simGetObjectsInTree(mainHandle[0], 1, sim_handle_all, objAm);
	for (size_t i = 0; i < objAm[0]; i++)
	{

	}
	simAddObjectToSelection(sim_handle_tree, mainHandle[0]);
	int selectionSize = simGetObjectSelectionSize();
	int tempObjectHandles[500]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason
	vector<int> objectHandles;
	objectHandles.resize(selectionSize);
	//s_objectAmount = selectionSize;
	simGetObjectSelection(tempObjectHandles);
	int jointCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		objectHandles[i] = tempObjectHandles[i];
		if (simGetObjectType(objectHandles[i]) == 1) {
			currentGenome.g_jointHandles.resize(jointCounter + 1);
			if (s_jointControl == 0) {
				currentGenome.g_jointActivity.resize(jointCounter + 1);
			}
			else if (s_jointControl == 1){
				currentGenome.g_jointActivity.resize((jointCounter + 1) * 2);
			}
			else if (s_jointControl == 2){
				currentGenome.g_jointActivity.resize((jointCounter + 1) * 3);
			}
			else {
				cout << "ERROR: Joint control could not be specified" << endl;
			}
			currentGenome.g_jointHandles[jointCounter] = objectHandles[i];
			jointCounter++;
		}
	}
	cout << "2.0" << endl;
	int objectCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		if (simGetObjectType(objectHandles[i]) == 0) {
			currentGenome.g_objectHandles.resize(objectCounter + 1);
			currentGenome.g_objectHandles[objectCounter] = tempObjectHandles[i];
			objectCounter++;
		}
	}
	currentGenome.g_jointAmount = jointCounter;
	jointCounter = 0;
	objectCounter = 0;
	simRemoveObjectFromSelection(sim_handle_all, NULL);
}

void CENN::identifyRobotMorphology() {
	//loadCreature(3);
	//	cout << "initialize agent function is called. " << endl;
	//	CCreatureCreator* createCreature;
	//	createCreature = new CCreatureCreator;
	//	createCreature->createCreature(3);
	//	mainHandle = createCreature->creatureHandle;

	mainHandleName = simGetObjectName(mainHandle[0]);
	cout << "The agent's main handle name = " << simGetObjectName(mainHandle[0]) << endl;
	// check if the creature corresponds with the amount of joints etc
	int jointCount[1];
	simGetObjectsInTree(mainHandle[0], 1, 1, jointCount);
	//	cout << "s_amountJoints = " << s_amountJoints << endl;
	//	cout << "jointCount = " << jointCount[0] << endl;
	if (jointCount[0] == currentGenome.g_jointAmount) {
		cout << "Amount joints in ENN corresponds with creature" << endl;
	}

	else {
		//	cout << "ERROR: Amount of joints in ENN does not correspond with the amount of neurons in creature" << endl;
		//	cout << "Amount joints = " << currentGenome.g_jointAmount << ", and amount inputNeurons = " << endl;
	}

	// also for debugging
	// load the amount of joints and save them in CENN so they can be easily accessed. 
	//int jointAm[1];
	int	objAm[1];
	simGetObjectsInTree(mainHandle[0], 1, sim_handle_all, objAm);
	cout << "objAmount = " << objAm[0] << endl;
	for (size_t i = 0; i < objAm[0]; i++)
	{

	}
	simAddObjectToSelection(sim_handle_tree, mainHandle[0]);
	int selectionSize = simGetObjectSelectionSize();
	vector<int> objectHandles;
	objectHandles.resize(selectionSize);
	int tempObjectHandles[1024]; // temporarily stores the object Handles as they cannot be inserted directly in a vector for some reason

	//s_objectAmount = selectionSize;
//	simGetObjectSelection(objectHandles);
	simGetObjectSelection(tempObjectHandles);
	int jointCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		objectHandles[i] = tempObjectHandles[i];
		if (simGetObjectType(objectHandles[i]) == 1) {
			currentGenome.g_jointHandles.resize(jointCounter + 1);
			if (s_jointControl == 0) {
				currentGenome.g_jointActivity.resize(jointCounter + 1);
			}
			else if (s_jointControl == 1){
				currentGenome.g_jointActivity.resize((jointCounter + 1) * 2);
			}
			else if (s_jointControl == 2){
				currentGenome.g_jointActivity.resize((jointCounter + 1) * 3);
			}
			else {
				cout << "ERROR: Joint control could not be specified" << endl;
			}
			currentGenome.g_jointHandles[jointCounter] = objectHandles[i];
			jointCounter++;
		}
	}
	int objectCounter = 0;
	for (size_t i = 0; i < selectionSize; i++)
	{
		if (simGetObjectType(objectHandles[i]) == 0) {
			currentGenome.g_objectHandles.resize(objectCounter + 1);
			currentGenome.g_objectHandles[objectCounter] = tempObjectHandles[i];
			objectCounter++;
		}
	}
	cout << "g_objectHandles.size = " << currentGenome.g_objectHandles.size() << endl;
	
	currentGenome.g_jointAmount = jointCounter;
	jointCounter = 0;
	objectCounter = 0;
	simRemoveObjectFromSelection(sim_handle_all, NULL);
}


void CENN::growStem(int partNum, int parent) {

	currentGenome.g_objectAmount++;
	currentGenome.g_objectHandles.resize(currentGenome.g_objectAmount);
	currentGenome.g_objectType.resize(currentGenome.g_objectAmount);
	//cout << "Stem is growing";
	//massObjects.resize(amountParts);
	currentGenome.g_objectType[partNum] = 0;
	currentGenome.g_objectType[parent] = 2;
	createCube(0.1, 0.1, 0.1, 0.0, 0.0, 0.1, /*rot x,y,z: */ 0.0, 0.0, 0.0, 1, parent, partNum);
	string num = to_string(partNum);
	string objName = "part";
	objName.append(num);
	const char * setName = objName.c_str();
	simSetObjectName(currentGenome.g_objectHandles[partNum], setName);
	//set parent
	simSetObjectParent(currentGenome.g_objectHandles[partNum], currentGenome.g_objectHandles[parent], TRUE);
	//simPauseSimulation();
}

void CENN::useDefaultGenome() {
	DefaultGenome *individualGenome;
//	CIndividualGenome *individualGenome;
	for (int i = 0; i < s_initialPopulationSize; i++) {
		populationFitness.resize(s_initialPopulationSize);

		unique_ptr<DefaultGenome> individualGenome(new DefaultGenome);
//		individualGenome = new CIndividualGenome;
		individualGenome->individualNumber = i;
		// give every genome a creature blueprint, should have a if createCreature is used function
		//		individualGenome->storeAgentBlueprint(mainHandleName, s_objectPosition, s_objectOrientation, s_objectSizes, s_objectMass, s_objectParent, s_objectType, 
		//			s_jointPosition, s_jointOrientation, s_jointSizes, s_jointMass, s_jointParent, s_jointType, s_jointMaxMinAngles, s_jointMaxForce, s_jointMaxVelocity);
		//		individualGenome->amountInterNeurons = 2;
		//		individualGenome->amountOutputNeurons = 8;
		// create a default neat based NN? not now, simpler even;

		//++++++++++++++++++++++++++++++++++++++++++++++++
		individualGenome->g_amountSenseParts = s_amountSenseParts;
		individualGenome->g_amountInputNeurons = s_amountInputNeurons;
		individualGenome->g_amountInterNeurons = s_amountInterNeurons;
		individualGenome->g_amountOutputNeurons = s_amountOutputNeurons;
		individualGenome->g_amountPatternGenerators = s_amountPatternGenerators;
		cout << "s_amountPatternGenerators = " << s_amountPatternGenerators << endl;

		int defaultAmountNeurons = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators;
		individualGenome->g_amountNeurons = defaultAmountNeurons;

		vector<int> defaultInputNeuronConnectionAmount;
		vector<int> defaultInterNeuronConnectionAmount;
		vector<int> defaultOutputNeuronConnectionAmount;
		vector<int> defaultCPGConnectionAmount;
		vector<int> defaultInputNeuronToSense;
		vector<float> defaultSenseInput;

		defaultInputNeuronConnectionAmount.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			defaultInputNeuronConnectionAmount[i] = 1;
		}
		defaultInterNeuronConnectionAmount.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++) {
			defaultInterNeuronConnectionAmount[i] = 5;
		}
		defaultOutputNeuronConnectionAmount.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++) {
			defaultOutputNeuronConnectionAmount[i] = 1;
		}
		defaultCPGConnectionAmount.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++) {
			defaultCPGConnectionAmount[i] = 1;
		}
		defaultInputNeuronToSense.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++) {
			if (i < s_totalInput) {
				defaultInputNeuronToSense[i] = i;
			}
			else {
				cout << " - Note: there are more input neurons than inputs meaning that some input neurons are not connected to any sensors" << endl;
			}
		}


		individualGenome->g_inputNeuronConnectionAmount = defaultInputNeuronConnectionAmount;
		individualGenome->g_interNeuronConnectionAmount = defaultInterNeuronConnectionAmount;
		individualGenome->g_outputNeuronConnectionAmount = defaultOutputNeuronConnectionAmount;
		individualGenome->g_patternGeneratorConnectionAmount = defaultCPGConnectionAmount;
		individualGenome->g_inputNeuronToInputSense = defaultInputNeuronToSense;


		// real initialization begins

		// input
		vector<vector<int>> defaultInputNeuronConnections;
		defaultInputNeuronConnections.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronConnections[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "it happened" << endl;
				}
				else {
					defaultInputNeuronConnections[i][j] = s_amountInputNeurons + i;
				}
		}

		individualGenome->g_inputNeuronConnections = defaultInputNeuronConnections;
		//	cout << "inputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInputNeuronWeights;
		defaultInputNeuronWeights.resize(s_amountInputNeurons);
		for (int i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputNeuronWeights[i].resize(defaultInputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInputNeuronConnectionAmount[i]; j++)
			{
				defaultInputNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialInputNeuronWeights = defaultInputNeuronWeights;
		individualGenome->g_inputNeuronWeights = defaultInputNeuronWeights;

		vector<float> defaultInputThreshold;
		defaultInputThreshold.resize(s_amountInputNeurons);
		for (size_t i = 0; i < s_amountInputNeurons; i++)
		{
			defaultInputThreshold[i] = -0.2;
		}
		individualGenome->g_inputActivationThreshold = defaultInputThreshold;

		// inter
		vector<vector<int>> defaultInterNeuronConnections;
		defaultInterNeuronConnections.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronConnections[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators) {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons +
						s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators - 1;
					cout << "It happened again" << endl;
				}
				else {
					defaultInterNeuronConnections[i][j] = s_amountInputNeurons + i + j;
				}
			}
		}

		individualGenome->g_interNeuronConnections = defaultInterNeuronConnections;
		//	cout << "interNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultInterNeuronWeights;
		defaultInterNeuronWeights.resize(s_amountInterNeurons);
		for (int i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterNeuronWeights[i].resize(defaultInterNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultInterNeuronConnectionAmount[i]; j++)
			{
				defaultInterNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialInterNeuronWeights = defaultInterNeuronWeights;
		individualGenome->g_interNeuronWeights = defaultInterNeuronWeights;

		vector<float> defaultInterThreshold;
		defaultInterThreshold.resize(s_amountInterNeurons);
		for (size_t i = 0; i < s_amountInterNeurons; i++)
		{
			defaultInterThreshold[i] = -0.2;
		}
		individualGenome->g_interNeuronActivationThresholds = defaultInterThreshold;
		//output
		vector<vector<int>> defaultOutputNeuronConnections;
		defaultOutputNeuronConnections.resize(s_amountOutputNeurons);
		cout << "amountOutputNeurons = " << s_amountOutputNeurons << endl;
		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronConnections[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				if (i >= (s_amountJoints)+s_amountPatternGenerators) { // *3 depending on control type 
					defaultOutputNeuronConnections[i][j] = s_amountJoints - 1;
					cout << "It happened to the joints" << endl;
				}
				else {
					defaultOutputNeuronConnections[i][j] = i;
				}
			}
		}

		individualGenome->g_outputNeuronConnections = defaultOutputNeuronConnections;
		//	cout << "outputNeuronConnections just saved in an individual genome" << endl;

		vector<vector<float>> defaultOutputNeuronWeights;
		defaultOutputNeuronWeights.resize(s_amountOutputNeurons);
		for (int i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputNeuronWeights[i].resize(defaultOutputNeuronConnectionAmount[i]);

			for (int j = 0; j < defaultOutputNeuronConnectionAmount[i]; j++)
			{
				defaultOutputNeuronWeights[i][j] = 0.5;
			}
		}
		individualGenome->g_initialOutputNeuronWeights = defaultOutputNeuronWeights;
		individualGenome->g_outputNeuronWeights = defaultOutputNeuronWeights;

		vector<float> defaultOutputThreshold;
		defaultOutputThreshold.resize(s_amountOutputNeurons);
		for (size_t i = 0; i < s_amountOutputNeurons; i++)
		{
			defaultOutputThreshold[i] = 0.2;
		}
		individualGenome->g_outputActivationThreshold = defaultOutputThreshold;

		// CPGs
		vector<vector<int>> defaultCPGConnections;
		defaultCPGConnections.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGConnections[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				if (i >= s_amountInterNeurons + s_amountOutputNeurons) {
					defaultCPGConnections[i][j] = s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons - 1;
					cout << "It happened to the CPGs" << endl;
				}
				else {
					defaultCPGConnections[i][j] = s_amountInputNeurons + i;
				}
			}
		}



		individualGenome->g_patternGeneratorConnections = defaultCPGConnections;
		//	cout << "CPGs just saved in an individual genome" << endl;

		vector<vector<float>> defaultCPGWeights;
		defaultCPGWeights.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultCPGWeights[i].resize(defaultCPGConnectionAmount[i]);

			for (int j = 0; j < defaultCPGConnectionAmount[i]; j++)
			{
				defaultCPGWeights[i][j] = 0.1;
			}
		}
		individualGenome->g_initialPatternGenerationWeights = defaultCPGWeights;
		individualGenome->g_patternGenerationWeights = defaultCPGWeights;
	
		vector<vector<float>> defaultPatternGenerationTime;
		defaultPatternGenerationTime.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationTime[i].resize(2);
			defaultPatternGenerationTime[i][0] = 0.5;
			defaultPatternGenerationTime[i][1] = 0.5;
		}
		individualGenome->g_initialPatternGenerationTime = defaultPatternGenerationTime;
		individualGenome->g_patternGenerationTime = defaultPatternGenerationTime;

		vector<int> defaultPatternGenerationType;
		defaultPatternGenerationType.resize(s_amountPatternGenerators);
		for (int i = 0; i < s_amountPatternGenerators; i++)
		{
			defaultPatternGenerationType[i] = 1;
		}
		individualGenome->g_patternGenerationType = defaultPatternGenerationType;

		// initialize other parameters
		individualGenome->g_individualAge = 0;

		if (s_useSensors == true) {
			individualGenome->g_useSensors = true;
			if (s_useTactileSensing == true) {
				individualGenome->g_useTactileSensors = true;
			}
			else {
				individualGenome->g_useTactileSensors = false;
			}
			if (s_useVestibularSystem == true){
				individualGenome->g_useVestibularSystem = true;
			}
			else {
				individualGenome->g_useVestibularSystem = false;
			}
			if (s_useProprioception == true) {
				individualGenome->g_useProprioception = true;
			}
			else {
				individualGenome->g_useProprioception = false;
			}
		}
		else {
			individualGenome->g_useSensors = false;
			individualGenome->g_useProprioception = false;
			individualGenome->g_useTactileSensors = false;
			individualGenome->g_useVestibularSystem = false;
		}

		// specify where they start at
		individualGenome->g_proprioceptionStartsAt = s_amountSenseParts;
		individualGenome->g_tactileSensingStartsAt = 0;
		individualGenome->g_vestibularSystemStartsAt = s_amountSenseParts + s_amountJoints;

		// resize how acquired activationLevels 
		individualGenome->g_acquiredActivationLevels.resize(s_amountInputNeurons + s_amountInterNeurons + s_amountOutputNeurons + s_amountPatternGenerators);
		individualGenome->g_jointActivity.resize(i_jointAmount);

		//individualGenome->
		individualGenome->g_jointHandles = i_jointHandles;
		// morphology saver
		individualGenome->g_objectAmount = i_objectAmount;
		individualGenome->g_objectMass = i_objectMass;
		individualGenome->g_objectOrientation = i_objectOrientation;
		individualGenome->g_objectParent = i_objectParent;
		individualGenome->g_objectPosition = i_objectPosition;
		individualGenome->g_tactileObjects = i_tactileObjects;
		individualGenome->g_objectSizes = i_objectSizes;
		individualGenome->g_objectType = i_objectType;

		individualGenome->g_jointSizes = i_jointSizes;
		individualGenome->g_jointAmount = i_jointAmount;
		if (individualGenome->g_jointAmount < 0) {
			individualGenome->g_jointAmount = 0;
		}
		individualGenome->g_jointMass = i_jointMass;
		individualGenome->g_jointOrientation = i_jointOrientation;
		individualGenome->g_jointParent = i_jointParent;
		individualGenome->g_jointPosition = i_jointPosition;
		individualGenome->g_jointType = i_objectType;
		individualGenome->g_jointMaxForce = i_jointMaxForce;
		individualGenome->g_jointMaxVelocity = i_jointMaxVelocity;
		individualGenome->g_jointMaxMinAngles = i_jointMaxMinAngles;

		defaultSenseInput.resize(s_totalInput);
		for (int i = 0; i < s_totalInput; i++) {
			defaultSenseInput[i] = 0.0;
		}
		// mutationRate
		individualGenome->g_mutationRate = s_initialMutationRate;

		individualGenome->g_senseInput = defaultSenseInput;

		// plant properties
		cout << "Making sure everything is correct" << endl; 
		vector<vector<vector<float>>> defaultLStateProperties;
		// five different states
		defaultLStateProperties.resize(5);
		defaultLStateProperties[0].resize(1);
		defaultLStateProperties[1].resize(2);
		defaultLStateProperties[2].resize(1);
		defaultLStateProperties[3].resize(1);
		defaultLStateProperties[4].resize(1);
		for (int i = 0; i < defaultLStateProperties.size(); i++) {
			for (size_t j = 0; j < defaultLStateProperties[i].size(); j++)
			{
			//	cout << "1." << i << "." << j << endl;
				defaultLStateProperties[i][j].resize(11);
				for (size_t k = 0; k < 11; k++)
				{
					defaultLStateProperties[i][j][k] = 0.0f;
				}
				// set properties manually
				defaultLStateProperties[i][j][0] = 0.1; // length
				defaultLStateProperties[i][j][1] = 0.1; // width
				defaultLStateProperties[i][j][2] = 0.1; // height
				defaultLStateProperties[i][j][3] = 0; // x
				defaultLStateProperties[i][j][4] = 0; // y
				defaultLStateProperties[i][j][5] = 0.1; // z
				defaultLStateProperties[i][j][6] = 0.1; // rotX
				defaultLStateProperties[i][j][7] = 0.1; // rotY
				defaultLStateProperties[i][j][8] = 0; // rotZ
				defaultLStateProperties[i][j][9] = 0.0; // mass
				defaultLStateProperties[i][j][10] = 0.0;
				defaultLStateProperties[i][j][11] = 0.0;
				defaultLStateProperties[i][j][12] = 0.5;
				defaultLStateProperties[i][j][13] = 0.9;
			}
		}

		vector<bool> defaultLeafState; 
		defaultLeafState.resize(5);
		for (int i = 0; i < 5; i++) {
			defaultLeafState[i] = false; 
		}
		defaultLeafState[4] = true;
		cout << "test access of defaultStateProperties [0][1][0]: " << defaultLStateProperties[1][1][0] << endl;
		
		individualGenome->g_lStateProperties = defaultLStateProperties;

		// saving the tactile objects if present
		if (s_useTactileSensing == true) {
			// separate ;
			string partNames = s_specifiedPartNames;
			stringstream partString(partNames);
			int partCount = 0;
			int tactileParts = 0;
			cout << "Lines here please: " << endl;
			while (partString.good()) {
				string singlePart;
				getline(partString, singlePart, ';');
				partCount++;
				//i_tactileObjects.resize(partCount);
				int tempHandle;
				const char* charPart = singlePart.c_str();
				tempHandle = simGetObjectHandle(charPart);
				cout << "tempHandle " << tempHandle << ", charPartHandle " << simGetObjectHandle(charPart)
					<< ", Charpart: " << charPart << ", objectAmount " << currentGenome.g_objectAmount << endl;
				for (int i = 0; i < currentGenome.g_objectAmount; i++) {
					if (currentGenome.g_objectHandles[i] == tempHandle) {
						cout << "The tactile handle is found and saved in the individual" << endl;
						tactileParts++;
						currentGenome.g_tactileObjects.resize(tactileParts);
						if (currentGenome.g_tactileObjects.size() < tactileParts) {
							cout << "Something went wrong with resizing the tactileObjects" << endl;
						}
						//						cout << "CONFIRMING TACTILE SIZE: " << currentGenome.g_tactileObjects.size() << endl;
						currentGenome.g_tactileHandles.resize(tactileParts);
						currentGenome.g_tactileObjects[tactileParts - 1] = i;
						currentGenome.g_tactileHandles[tactileParts - 1] = tempHandle;
					}
				}
				if (tactileParts < partCount){
					cout << "tactileParts = " << tactileParts << ", partCount = " << partCount << endl;
					cout << "Note: more tactile object were given than were identified" << endl;
					cout << " -> check if the given object strings correspond with the tactile sensor object names of your agent" << endl;
				}
			}

			// save handles and objects in genome
			individualGenome->g_tactileObjects = currentGenome.g_tactileObjects;
		}

		// additional functionality is to be updated
		storedIndividualGenomes.push_back(move(individualGenome));
	}
//	vector<unique_ptr<DefaultGenome>>::iterator it;
//	vector<CIndividualGenome>::iterator it;

//	for (it = storedIndividualGenomes.begin(); it != storedIndividualGenomes.end(); ++it) {
	//	it->printIndividualInfo();
//	}
}

void CENN::checkMemoryLeak() {
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	cout << "Checked Leaks!" << endl << endl;
}

void CENN::initializePopulation2(int initPopSize) {
	//if (useTemplate == true) {
	if (nNHandlingType == 1){
		defaultPlantNN();
	}
	else if (nNHandlingType == 2) {
		defaultModuleNN();
	}
	else {
		initializeNN(nNTemplate);
		useDefaultGenome();
	}
	// defining every individual separately
}


// METHOD 2 END ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CENN::endOfSimulation(){
	cout << "endofsim" << endl; 
	cout << "generation = " << generation << endl; 
	cout << "newGeneration = " << newGenerations << endl;
	switch (simSet) {
	case (EVOLUTION) : {
		
		if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
			// The newGenome pointer has been moved and replaces one pointer of the individuals in the population. Make sure tha
			// the fitness of all genomes are actually 0.
			for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
				populations[0]->populationGenomes[i]->genomeFitness = 0; 
			}
//			cout << "delete 3D model" << endl;
//			populations[0]->newGenome->deleteCreated();
//			cout << "deleting new Genome" << endl;
//			delete populations[0]->newGenome;
		}
		else {
//			cout << "delete 3D model of popGenome" << endl;
			populations[0]->populationGenomes[indCounter - 1]->genomeFitness = 0;
		}
//		cout << "indCounter = " << indCounter << endl;
		if (indCounter % populations[0]->populationGenomes.size() == 0 && indCounter != 0) {
			populations[0]->savePopFitness(generation, populations[0]->populationFitness, sceneNum);
			generation++;
			newGenerations++;
		}
		
		if (indCounter >= populations[0]->populationGenomes.size()) {

			//	populations[0]->populationGenomes[0].checkGenome();
			//	populations[0]->newGenome[0].checkGenome();
			//	float fitness = populations[0]->evaluateNewIndividual();
		}

		break;
	}
	case (ROTATE) : {
		cout << "ROTATE SAVE CALLED " << endl; 
		if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
			for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
				populations[0]->populationGenomes[i]->genomeFitness = 0;
			}
		//	populations[0]->evaluateNewIndividual();
		}
		else {
			//			cout << "delete 3D model of popGenome" << endl;
			/*populations[0]->evaluateIndividual(indCounter - 1);
			populations[0]->populationGenomes[indCounter - 1]->deleteCreated();*/
		}
		//		cout << "indCounter = " << indCounter << endl;
		if (indCounter % populations[0]->populationGenomes.size() == 0 && indCounter != 0) {
	//		cout << "saving pop Fitness" << endl; 
			populations[0]->savePopFitness(generation, populations[0]->populationFitness, sceneNum);
			generation++;
			newGenerations++;
		}

		if (indCounter >= populations[0]->populationGenomes.size()) {
		}
	}
	case (DYNAMIC) : {
		if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
		}
		else {
		}
		if (indCounter % populations[0]->populationGenomes.size() == 0 && indCounter != 0) {
			populations[0]->savePopFitness(generation, populations[0]->populationFitness, sceneNum);
			generation++;
			newGenerations++;
		}

		if (indCounter >= populations[0]->populationGenomes.size()) {
		}
	}
	case (RECALLCUSTOM) :
		break;
	case (RECALLROTATOR) :
		break; 
	case (COEVOLUTION) : {
		if (indCounter - 1 >= populations[0]->populationGenomes.size()) {
		//	float fitness = populations[0]->evaluateNewIndividual(indCounter - 1, sceneNum);
		//	cout << "delete 3D model" << endl;
		//	populations[0]->newGenome->deleteCreated();
		//	populations[0]->deleteCoCreated();
		//	cout << "deleting new Genome" << endl;
//			delete populations[0]->newGenome;
		}
		else {
		//	cout << "delete 3D model of popGenome" << endl;
		//	populations[0]->populationGenomes[indCounter - 1]->deleteCreated();
		}
		cout << "indCounter = " << indCounter << endl;
		if (indCounter % populations[0]->populationGenomes.size() == 0 && indCounter != 0) {
			populations[0]->savePopFitness(generation, populations[0]->populationFitness, sceneNum);
			generation++;
			newGenerations++;
		//	cout << "savedPopFitness" << endl;
		}
		
		if (indCounter > populations[0]->populationGenomes.size()) {
		//	cout << "evaluating new genome" << endl;
		
			//	populations[0]->populationGenomes[0].checkGenome();
			//	populations[0]->newGenome[0].checkGenome();
			//	float fitness = populations[0]->evaluateNewIndividual();
		}
		break;
	}
	}
	if (simSet != RECALLROTATOR) {
		if (newGenerations == 4&& indCounter > 1) { // close v-rep every 20 generations
			cout << "saving" << endl;
			string lightName = "light" + to_string(sceneNum);
			int light_handle = simGetObjectHandle(lightName.c_str());
			simRemoveObject(light_handle);
			cout << "removed lightHandle" << endl;

			ofstream settingsFile;
			settingsFile.open("interfaceFiles\\settings" + to_string(sceneNum) + ".csv");
			settingsFile << ",#generation," << generation << "," << endl;
			settingsFile << ",#indCounter," << indCounter << "," << endl;
			settingsFile << ",#populationSize," << populations[0]->populationGenomes.size() << "," << endl;
			settingsFile << ",#individuals,";
			cout << "saved standard stuff" << endl;
			for (int i = 0; i < populations[0]->popIndNumbers.size(); i++) {
				settingsFile << populations[0]->popIndNumbers[i] << ",";
			} settingsFile << endl;

			settingsFile.close();

			cout << "saved settingsFile" << endl;
			float bestIndFitness = -1000;
			int bestIndividual = 0;
			for (int i = 0; i < populations[0]->populationFitness.size(); i++)
			{
				cout << "individual " << i << " has fitness of " << populations[0]->populationFitness[i] << endl;
				if (populations[0]->populationFitness[i] > bestIndFitness) {
					bestIndFitness = populations[0]->populationFitness[i];
					bestIndividual = populations[0]->popIndNumbers[i];
				}
			}

			simCloseScene();
			simLoadScene("plantScene.ttt");
			cout << "bestIndividual = " << bestIndividual << endl;
			cout << "bestIndFitness = " << bestIndFitness << endl;
			populations[0]->selectNewIndividual(currentInd);
			populations[0]->newGenome->init_noMorph();
			populations[0]->loadBaseMorphology(bestIndividual, sceneNum);
			populations[0]->newGenome->morph->recallAndCreate();
			populations[0]->newGenome->morph->evaluate(sceneNum);
			string modelName = "interfaceFiles\\morphologies" + to_string(sceneNum) + " ind" + to_string(bestIndividual) + "scene" + to_string(sceneNum) + "gen" + to_string(generation) + ".ttt";
			simSaveScene(modelName.c_str());
			populations[0]->newGenome->morph->deleteCreatedMorph();
			simCloseScene();
			simLoadScene("plantScene.ttt");
			float secondBestIndFitness = 0;
			int secondBestIndividual = 0;
			for (int i = 0; i < populations[0]->populationFitness.size(); i++)
			{
				if (populations[0]->populationFitness[i] > secondBestIndFitness && populations[0]->populationFitness[i] < bestIndFitness) {
					secondBestIndFitness = populations[0]->populationFitness[i];
					secondBestIndividual = populations[0]->popIndNumbers[i];
				}
			}

			cout << "secondBestIndividual = " << secondBestIndividual << endl;
			cout << "secondBestIndFitness = " << secondBestIndFitness << endl;
			populations[0]->newGenome->init_noMorph();
			populations[0]->loadBaseMorphology(secondBestIndividual, sceneNum);
			populations[0]->newGenome->morph->recallAndCreate();
			populations[0]->newGenome->morph->evaluate(sceneNum);
			string secModelName = "interfaceFiles\\morphologies" + to_string(sceneNum) + " indsecBest" + to_string(secondBestIndividual) + "scene" + to_string(sceneNum) + "gen" + to_string(generation) + ".ttt";
			simSaveScene(secModelName.c_str());
			simQuitSimulator(false);

			simCloseScene();
			simLoadScene("plantScene.ttt");
			float worstIndFitness = 10000;
			int worstIndividual = 0;
			for (int i = 0; i < populations[0]->populationFitness.size(); i++)
			{
				if (populations[0]->populationFitness[i] < worstIndFitness) {
					worstIndFitness = populations[0]->populationFitness[i];
					worstIndividual = populations[0]->popIndNumbers[i];
				}
			}

			cout << "worstIndividual = " << worstIndividual << endl;
			cout << "worstIndFitness = " << worstIndFitness << endl;
			populations[0]->newGenome->init_noMorph();
			populations[0]->loadBaseMorphology(worstIndividual, sceneNum);
			populations[0]->newGenome->morph->recallAndCreate();
			populations[0]->newGenome->morph->evaluate(sceneNum);
			string worstModelName = "interfaceFiles\\morphologies" + to_string(sceneNum) + "worst" + to_string(worstIndividual) + "scene" + to_string(sceneNum) + "gen" + to_string(generation) + ".ttt";
			simSaveScene(worstModelName.c_str());
			simQuitSimulator(false);
			/**/
		}
	}
	//if (simLoadScene("plantScene.ttt") == -1){
	//	cout << "FAILEDASKJNBDBSADSAI&!T%!)(/#%!)#%!#)#!";
	//}
	//else {
	//	cout << "SUCCESS" << endl << endl << endl << endl;
//	}
	
	// delete newGenome
	

	// delete all rays
	// following if and for loop only need to be implemented when creating the entire genome. 
	/*
	for (int i = 0; i < rayHandles.size(); i++) {
		simRemoveDrawingObject(rayHandles[i]);
	}

	assert(currentGenome.g_leafSunContact.size() == currentGenome.g_leafHandles.size());

	cout << endl << "SIMULATION STOPPED!!" << endl << endl << endl;
	if (evaluationMethod == 0) {
		if (_initialize == true) {
			_initialize = false;
		}
		else {
		//	evaluateIndividual();
			// temp
			currentGenome.currentGenomeChecker();
			// temp end
		
			genomeSaver2();
		//	evaluateIndividual();
		//	genomeSaver();
		//	genomeChecker(currentInd);
		//	resetParameters();
		}
	}
	else if (evaluationMethod == 1) {
		if (_initialize == true) {
			_initialize = false;
			
		}
		// do some plant evaluations
	}
	cout << "Current individual = " << currentInd << endl;
//	genomeChecker(currentInd);
//	genomeSaver2();

	float fitnessVal = 0;

	if (indCounter > s_initialPopulationSize) {
		cout << "evaluating Individual" << endl;
		// manual evaluation, can be put in a class in the future, should return fitness value
		for (int i = 0; i < currentGenome.g_leafSunContact.size(); i++) {
			if (currentGenome.g_leafSunContact[i] == true) {
				cout << "leaf " << i << " is true " << endl;
				fitnessVal = fitnessVal + 1.0;
			}
		}
		fitnessVal += (currentGenome.g_objectAmount * 0.01);
		cout << "FitnessVal: " << fitnessVal << endl;
	}
	if (indCounter < s_initialPopulationSize) {
		populationFitness[currentInd] = fitnessVal;
	}
	else {
		cout << "no?" << endl;
		int randInd = rand() % s_initialPopulationSize;
		if (fitnessVal >= populationFitness[randInd]) {
			cout << "Fitness is equal or higher!! " << fitnessVal << endl;
			populationFitness[randInd] = fitnessVal;
			// replace the random individual with the new genome
			//storeNNPars(storedIndividualGenomes[randInd], randInd);
			for (int i = 0; i < storedIndividualGenomes.size(); i++) {
				cout << "i: " << i << endl;
				// make sure the amount of objects = 1??
				currentGenome.g_objectAmount = 1;
				currentGenome.g_jointAmount = 0;
				storedIndividualGenomes[randInd] = currentGenome;
			}
			//				currentGenome.g_objectAmount = 1;
			//				currentGenome.g_jointAmount = 0;
			//				storedIndividualGenomes[randInd] = currentGenome;
		}
	}
	currentGenome.g_leafSunContact.clear();
	rayHandles.clear();
	currentGenome.g_leafHandles.clear();
	currentGenome.g_leafJointHandles.clear();

		
		/*
		float fitnessVal;
		float highestObject = 0;
		float objectPos[3];
		cout << "objectAmount = " << currentGenome.g_objectAmount << endl;
		if (currentGenome.g_objectAmount != currentGenome.g_objectHandles.size()) {
			cout << "objectAmount and handles.size() not the same!?? " << endl;
		}
		cout << "handleSize = " << currentGenome.g_objectHandles.size() << endl;
		for (int i = 0; i < currentGenome.g_objectAmount; i++) {
			simGetObjectPosition(currentGenome.g_objectHandles[i], NULL, objectPos);
			if (objectPos[1] > highestObject){
				highestObject = objectPos[1];
			}
		}
		// add amount of objects as a fitness value. 
		int amountObjects = currentGenome.g_objectAmount;
		fitnessVal = highestObject + (0.02* amountObjects);

		cout << "Highest Object = " << highestObject << endl; 
		cout << "FitnessVal: " << fitnessVal << endl;
		if (indCounter < s_initialPopulationSize) {
			populationFitness[currentInd] = highestObject;
		}
		
		else {
			cout << "no?" << endl;
			int randInd = rand() % s_initialPopulationSize;
			if (fitnessVal > populationFitness[randInd]) {
				cout << "Fitness is higher!! " << fitnessVal << endl;
				populationFitness[randInd] = fitnessVal;
				// replace the random individual with the new genome
				//storeNNPars(storedIndividualGenomes[randInd], randInd);
				for (int i = 0; i < storedIndividualGenomes.size(); i++) {
					cout << "i: " << i << endl;
					// make sure the amount of objects = 1??
					currentGenome.g_objectAmount = 1;
					currentGenome.g_jointAmount = 0;
					storedIndividualGenomes[randInd] = currentGenome;
				}
//				currentGenome.g_objectAmount = 1;
//				currentGenome.g_jointAmount = 0;
//				storedIndividualGenomes[randInd] = currentGenome;
			}
		} */

//	} /**/



	// temp, replace all stored with current
	/*if (indCounter > s_initialPopulationSize) {
		for (int i = 0; i < storedIndividualGenomes.size(); i++) {
			cout << "i: " << i << endl;
			// make sure the amount of objects = 1??
			currentGenome.g_objectAmount = 1;
			currentGenome.g_jointAmount = 0;
			storedIndividualGenomes[i] = currentGenome;
		}
//		printMorphologyParameters(storedIndividualGenomes[0]);
	} /**/
}

void CENN::evaluateIndividual() {
	float mainPos[3];
	simGetObjectPosition(mainHandle[0], -1, mainPos);
	individualFitness = mainPos[0];
	cout << "The fitness of individual " << indCounter << " = " << individualFitness << endl;
	if (indCounter < s_initialPopulationSize) {
		populationFitness[currentInd] = individualFitness;
	}
	else {
		int randInd = rand() % s_initialPopulationSize;
		if (individualFitness > populationFitness[randInd]) {
			populationFitness[randInd] = individualFitness;
			// replace the random individual with the new genome
			//storeNNPars(storedIndividualGenomes[randInd], randInd);
		}
	}
}

/*
void CENN::storeNNPars(CIndividualGenome indGen, int indNum) {
	CIndividualGenome individualGenome = storedIndividualGenomes[indNum];


	// give every genome a creature blueprint, should have a if createCreature is used function
	//		individualGenome->storeAgentBlueprint(mainHandleName, s_objectPosition, s_objectOrientation, s_objectSizes, s_objectMass, s_objectParent, s_objectType, 
	//			s_jointPosition, s_jointOrientation, s_jointSizes, s_jointMass, s_jointParent, s_jointType, s_jointMaxMinAngles, s_jointMaxForce, s_jointMaxVelocity);
	//		individualGenome->amountInterNeurons = 2;
	//		individualGenome->amountOutputNeurons = 8;
	// create a default neat based NN? not now, simpler even;

	//++++++++++++++++++++++++++++++++++++++++++++++++
	individualGenome.g_mutationRate = i_mutationRate;

	individualGenome.g_amountSenseParts = i_amountSenseParts;
	individualGenome.g_amountInputNeurons = i_amountInputNeurons;
	individualGenome.g_amountInterNeurons = i_amountInterNeurons;
	individualGenome.g_amountOutputNeurons = i_amountOutputNeurons;
	individualGenome.g_amountPatternGenerators = i_amountPatternGenerators;

	int defaultAmountNeurons = i_amountInputNeurons + i_amountInterNeurons + i_amountOutputNeurons + i_amountPatternGenerators;
	individualGenome.g_amountNeurons = i_amountNeurons;	// needs recalculation before saving

	individualGenome.g_inputNeuronConnectionAmount = i_inputNeuronConnectionAmount;
	individualGenome.g_interNeuronConnectionAmount = i_interNeuronConnectionAmount;
	individualGenome.g_outputNeuronConnectionAmount = i_outputNeuronConnectionAmount;
	individualGenome.g_patternGeneratorConnectionAmount = i_patternGeneratorConnectionAmount;

	individualGenome.g_inputNeuronToInputSense = i_inputNeuronToInputSense;

	individualGenome.g_inputNeuronConnections = i_inputNeuronConnections;
	individualGenome.g_initialInputNeuronWeights = i_initialInputNeuronWeights;
	individualGenome.g_inputNeuronWeights = i_inputNeuronWeights;
	individualGenome.g_inputActivationThreshold = i_inputActivationThreshold;

	individualGenome.g_interNeuronConnections = i_interNeuronConnections;
	individualGenome.g_initialInterNeuronWeights = i_initialInterNeuronWeights;
	individualGenome.g_interNeuronWeights = i_interNeuronWeights;

	individualGenome.g_interNeuronActivationThresholds = i_interNeuronActivationThresholds;
	individualGenome.g_outputNeuronConnections = i_outputNeuronConnections;

	individualGenome.g_initialOutputNeuronWeights = i_initialOutputNeuronWeights;
	individualGenome.g_outputNeuronWeights = i_outputNeuronWeights;
	individualGenome.g_outputActivationThreshold = i_outputActivationThreshold;

	individualGenome.g_patternGeneratorConnections = i_patternGeneratorConnections;
	individualGenome.g_initialPatternGenerationWeights = i_initialPatternGenerationWeights;
	individualGenome.g_patternGenerationWeights = i_patternGenerationWeights;
	individualGenome.g_initialPatternGenerationTime = i_initialPatternGenerationTime;
	individualGenome.g_patternGenerationTime = i_patternGenerationTime;
	individualGenome.g_patternGenerationType = i_patternGenerationType;

	individualGenome.g_individualAge = i_individualAge;
	individualGenome.g_useSensors = i_useSensors;
	individualGenome.g_useTactileSensors = i_useTactileSensors;
	individualGenome.g_useProprioception = i_useProprioception;
	individualGenome.g_useVestibularSystem = i_useVestibularSystem;

	// morphology saver
	individualGenome.g_objectAmount = i_objectAmount;
	individualGenome.g_objectMass = i_objectMass;
	individualGenome.g_objectOrientation = i_objectOrientation;
	individualGenome.g_objectParent = i_objectParent;
	individualGenome.g_objectPosition = i_objectPosition;
	individualGenome.g_tactileObjects = i_tactileObjects;
	individualGenome.g_objectSizes = i_objectSizes;
	individualGenome.g_objectType = i_objectType;

	individualGenome.g_jointSizes = i_jointSizes;
	individualGenome.g_jointAmount = i_jointAmount;
	individualGenome.g_jointMass = i_jointMass;
	individualGenome.g_jointOrientation = i_jointOrientation;
	individualGenome.g_jointParent = i_jointParent;
	individualGenome.g_jointPosition = i_jointPosition;
	individualGenome.g_jointType = i_objectType;
	individualGenome.g_jointMaxForce = i_jointMaxForce;
	individualGenome.g_jointMaxVelocity = i_jointMaxVelocity;
	individualGenome.g_jointMaxMinAngles = i_jointMaxMinAngles;

	individualGenome.g_senseInput = i_senseInput;

	individualGenome.g_tactileObjects = i_tactileObjects;

	// additional functionality is to be updated
	storedIndividualGenomes[indNum] = individualGenome;
} */


void CENN::genomeChecker(int genomeNumb) {
	// checks if the genome is structured correctly
	cout << "Checking Genome" << endl;
	if (genomeNumb > s_initialPopulationSize) {
		cout << "ERROR: The Genome being checked is not initialized" << endl;
	}
	else
	{
		//storedIndividualGenomes[genomeNumb].amountInputNeurons
	}

	assert(currentGenome.g_objectAmount == currentGenome.g_objectHandles.size());

	if (currentGenome.g_objectAmount != currentGenome.g_objectHandles.size()) {
		cout << "g_objectHandles.size() is not correct" << endl;
		cout << "g_objectHandles.size() = " << currentGenome.g_objectHandles.size() << endl;
		cout << "g_objectAmount = " << currentGenome.g_objectAmount << endl;
	}
	if (currentGenome.g_objectAmount != currentGenome.g_objectPosition.size()) {
		cout << "ERROR at object positions" << endl;
	}
	if (currentGenome.g_objectAmount != currentGenome.g_objectType.size()){
		cout << "g_objectType.size() is not correct" << endl;
	}
	if (currentGenome.g_objectAmount != currentGenome.g_objectMass.size()){
		cout << "ERROR at objectMass" << endl;
	}
	if (currentGenome.g_objectAmount != currentGenome.g_objectOrientation.size()){
		cout << "ERROR at orientation" << endl;
	}
	if (currentGenome.g_objectAmount != currentGenome.g_objectParent.size()){
		cout << "ERROR at parent" << endl;
	}

	if (currentGenome.g_objectAmount != currentGenome.g_objectSizes.size()){
		cout << "ERROR at Sizes" << endl;
	}
	if (currentGenome.g_jointAmount != currentGenome.g_jointHandles.size()) {
		cout << "ERROR: jointHandles.size() = " << currentGenome.g_jointHandles.size() << ", While jointAmount = " << currentGenome.g_jointAmount << endl;
	}
}

void genomeLoader() {
	// can be used to load a specific saved genome from a .csv file. 
}



void CENN::initializeEnvironment() {
	floorHandle = simGetObjectHandle("DefaultFloor");
}

/*
void CIndividualGenome::storeAgentBlueprint(string mainHandleName, vector<vector<float>> objectPosition, vector<vector<float>> objectOrientation, 
	vector<vector<float>> objectSize, vector<float> objectMass, vector<int> objectParent, vector <int> objectType, vector<vector<float>> jointPosition, 
	vector<vector<float>> jointOrientation,vector<vector<float>> jointSize, vector<float> jointMass, vector<int> jointParent, vector <int> jointType, 
	vector<vector<float>> jointMaxMinAngles, vector<float> jointMaxVelocity, vector<float> jointMaxForce) {
	CAgentBlueprint* agentBlueprint;

	agentBlueprint = new CAgentBlueprint;

	agentBlueprint->getConstructionBlueprint(mainHandleName, objectPosition, objectOrientation, objectSize, objectMass, objectParent, objectType, jointPosition,
		jointOrientation, jointSize, jointMass, jointParent, jointType, jointMaxMinAngles, jointMaxVelocity, jointMaxForce);
}
*/
/*
void CAgentBlueprint::getConstructionBlueprint(string mainHandleName, vector<vector<float>> objectPosition, vector<vector<float>> objectOrientation,
	vector<vector<float>> objectSize, vector<float> objectMass, vector<int> objectParent, vector <int> objectType, vector<vector<float>> jointPosition,
	vector<vector<float>> jointOrientation, vector<vector<float>> jointSize, vector<float> jointMass, vector<int> jointParent, vector <int> jointType,
	vector<vector<float>> jointMaxMinAngles, vector<float> jointMaxVelocity, vector<float> jointMaxForce){
	// get blueprint from the simulation. All individuals of the initial population will get the same construction info. 
	// first get the mainObject
	b_mainHandleName = mainHandleName;
	const char * b_mainHandleNameChar = mainHandleName.c_str();
	a_objectOrientation = objectOrientation; // store initial Robot values. 
	a_objectPosition = objectPosition;
	a_objectSizes = objectSize;
	a_objectMass = objectMass;
	a_objectParent = objectParent;
	a_objectType = objectType;

	a_jointOrientation = jointOrientation; // store initial Robot values. 
	a_jointPosition = jointPosition;
	a_jointSizes = jointSize;
	a_jointMass = jointMass;
	a_jointParent = jointParent;
	a_jointType = jointType;
	a_jointMaxMinAngles = jointMaxMinAngles;
	a_jointMaxVelocity = jointMaxVelocity;
	a_jointMaxForce = jointMaxForce;

	//a_jointAngles = jointAngles; 
	
	/*
		int b_mainHandle = simGetObjectHandle(b_mainHandleNameChar);
	int objectCount[1];
	simGetObjectsInTree(b_mainHandle, sim_handle_all, 1, objectCount);
	agentAmountObjects = objectCount[0] + 1;
	cout << "amount objects in tree = " << agentAmountObjects << endl;
	int objIndex[10];
//	vector<int> objIndex;
//	objIndex.resize(agentAmountObjects);
	simAddObjectToSelection(sim_handle_tree, b_mainHandle);
	simGetObjectSelection(objIndex);
	//objIndex = simGetObjectsInTree(b_mainHandle, sim_handle_all, 1, objectCount);
	
	objectType.resize(agentAmountObjects);
	objectPositions.resize(agentAmountObjects);
	objectOrientations.resize(agentAmountObjects);
	objectSize.resize(agentAmountObjects);
	objectParents.resize(agentAmountObjects);
	objectPositions.resize(agentAmountObjects);
	
	for (int i = 0; i < agentAmountObjects; i++) {
		cout << "Object index = " << objIndex[i] << ", ";
		objectType[i] = simGetObjectType(objIndex[i]);
		cout << "the Object Type = " << objectType[i] << endl; // 0 = shape, 1 = joint
		if (objectType[i] == 1) {
			objectSize[i].resize(3);
			float size[3];
			simGetObjectSizeValues(objIndex[i], size);
			objectSize[i][0] = size[0];
			objectSize[i][1] = size[1];
			objectSize[i][2] = size[2];
		}
		else {
			objectSize[i].resize(3);
			float size[3];
			simGetObjectSizeValues(objIndex[i], size);
			objectSize[i][0] = size[0];
			objectSize[i][1] = size[1];
			objectSize[i][2] = size[2];
			cout << "sizes are : " << size[0] << ", " << size[1] << ", " << size[2] << endl;
			cout << "The size factor = " << simGetObjectSizeFactor(objIndex[i]) << endl;

		}



	} cout << endl;
	simRemoveObjectFromSelection(sim_handle_all, sim_handle_all);
	*   /
}
*/

void CENN::initializeNN(string nNtemplate){
	ifstream calledENNUI("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\NNTemplate.csv");
	if (calledENNUI.fail()){
		cout << "NNTemplate not found, creating default NN" << endl;
	} 
}

void CENN::incrementConnections(int connectPos) {
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_inputNeuronConnections[i].size(); j++) {
			if (currentGenome.g_inputNeuronConnections[i][j] >= connectPos) {
				currentGenome.g_inputNeuronConnections[i][j]++;
			}
		}
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		for (int j = 0; j < currentGenome.g_interNeuronConnections[i].size(); j++) {
			if (currentGenome.g_interNeuronConnections[i][j] >= connectPos) {
				currentGenome.g_interNeuronConnections[i][j]++;
			}
		}
	}
	cout << "2.1";
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		for (int j = 0; j < currentGenome.g_patternGeneratorConnections[i].size(); j++) {
			if (currentGenome.g_patternGeneratorConnections[i][j] >= connectPos) {
				currentGenome.g_patternGeneratorConnections[i][j]++;
			}
		}
	}
}

void CENN::decrementConnections(int connectPos) {
	for (int i = 0; i < currentGenome.g_amountInputNeurons; i++) {
		for (int j = 0; j < currentGenome.g_inputNeuronConnections[i].size(); j++) {
			if (currentGenome.g_inputNeuronConnections[i][j] >= connectPos) {
				currentGenome.g_inputNeuronConnections[i][j]--;
			}
		}
	}
	for (int i = 0; i < currentGenome.g_amountInterNeurons; i++) {
		for (int j = 0; j < currentGenome.g_interNeuronConnections[i].size(); j++) {
			if (currentGenome.g_interNeuronConnections[i][j] >= connectPos) {
				currentGenome.g_interNeuronConnections[i][j]--;
			}
		}
	}
	for (int i = 0; i < currentGenome.g_amountPatternGenerators; i++) {
		for (int j = 0; j < currentGenome.g_patternGeneratorConnections[i].size(); j++) {
			if (currentGenome.g_patternGeneratorConnections[i][j] >= connectPos) {
				currentGenome.g_patternGeneratorConnections[i][j]--;
			}
		}
	}
}

void CENN::initializeENNPars()
{
	cout << "NN parameters are being identified" << endl;

	ifstream calledENNUI("C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\ENNUI.csv");
	if (calledENNUI.fail()){
		cout << "ENNUI not found" << endl;
	}

	string value;
	list<string> values;
	vector<string> storedStrings;
	int amountStoredStrings = 100;
	storedStrings.resize(amountStoredStrings);
	storedStrings = {
		"#amountJoints",				// #0
		"#jointControl",				// #1
		"#initialPopulationSize",		// #2
		"#islandPopulations",			// #3
		"#useSensors",					// #4
		"#useCPGs",						// #5
		"#useProprioception",			// #6
		"#useVestibularSystem",			// #7
		"#useTactileSensing",			// #8
		"#tactileSensingType",			// #9
		"#amountSenseParts",			// #10
		"#specifiedPartNames",			// #11
		"#reproductionType",			// #12
		"#genomeType",					// #13
		"#evolvableMutationRate",		// #14
		"#initialMutationRate",			// #15
		"#amountInterNeurons",			// #16
		"#amountCPGs",					// #17
		"#amountInputNeurons",			// #18
		"#amountOutputNeurons",			// #19
		"#crossover",					// #20
		"#amountCrossover",				// #21
		"#evolvableCrossover",			// #22
		"#variableGenomeSize",			// #23
		"#geneDuplication",				// #24
		"#evolvingMorphology",			// #25
		"#evolveDimensions",			// #26
		"#deleteRandomParts",			// #27
		"#addRandomParts",				// #28
		"#lEvolution"					// #29
	};

	//int amountStordStrings = 100;

	vector<bool> readActivators;
	readActivators.resize(amountStoredStrings);
	for (int i = 0; i < amountStoredStrings; i++) {
		readActivators[i] = false;
	}

	vector<string> lines;
	lines.resize(1);
	int lineCounter = 0;

	while (calledENNUI.good())
	{
		getline(calledENNUI, value, '\n');
		//	cout << "current value = " << value << endl;
		lineCounter++;
		//	cout << "line counter = " << lineCounter << endl;
		lines.resize(lineCounter);
		lines[lineCounter - 1] = value;
	}

	int sepCounter = 0;
	vector<vector<string>> storedSepValues;
	storedSepValues.resize(lines.size());

	string sepValue;
	for (int i = 0; i < lines.size(); i++) {
		istringstream line(lines[i]);
		while (getline(line, sepValue, ',')) {
			sepCounter++;
			storedSepValues[i].resize(sepCounter);
			storedSepValues[i][sepCounter - 1] = sepValue;
			//		cout << "storedSepValues[i][sepCounter-1] = " << storedSepValues[i][sepCounter - 1] << endl;

			//		cout << "stored string value 1 = " << storedStrings[1]<< endl;
			//			cout << "sepValue = " << sepValue << endl;

			for (int j = 0; j < storedSepValues[i].size(); j++) {
				if (storedSepValues[i][j] == storedStrings[j]) {
					switch (j)
						case 0:
							cout << "0!!!!!!!!!!!!!!!" << endl;
							break;
				}
			}
		}
		sepCounter = 0;
	}/**/
	for (int i = 0; i < storedSepValues.size(); i++) {
		for (int j = 0; j < storedSepValues[i].size(); j++) {
			for (int k = 0; k < storedStrings.size(); k++) {
				if (storedSepValues[i][j] == storedStrings[k]) {
					//	cout << "found: " << storedStrings[k] << endl;
					//	cout << storedStrings[k] << " = " << /* lines[i] << endl;// */ storedSepValues[i][j+1] << endl;

					switch (k) {
					case 0:
						s_amountJoints = atoi(storedSepValues[i][j + 1].c_str());
						cout << "case 0: " << storedStrings[k] << " = " << storedSepValues[i][j + 1] << " = " << s_amountJoints << endl;
						break;
					case 1: // jointControl
						s_jointControl = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 2: // initialPopulationSize
						s_initialPopulationSize = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 3: // islandPopulations
						if (storedSepValues[i][j + 1] == "True") {
							s_islandPopulations = true;
						}
						else {
							s_islandPopulations = false;
						}
						break;
					case 4: // useSensors
						if (storedSepValues[i][j + 1] == "True") {
							s_useSensors = true;
						}
						else
						{
							s_useSensors = false;
						}
						break;
					case 5: // useCPGs
						if (storedSepValues[i][j + 1] == "True") {
							s_usePatternGenerators = true;
						}
						else
						{
							s_usePatternGenerators = false;
						}
						break;
					case 6: // useProprioception
						if (storedSepValues[i][j + 1] == "True") {
							s_useProprioception = true;
						}
						else
						{
							s_useProprioception = false;
						}
						break;
					case 7: // useVestibularSystem
						if (storedSepValues[i][j + 1] == "True") {
							s_useVestibularSystem = true;
						}
						else
						{
							s_useVestibularSystem = false;
						}
						break;
					case 8: // useTactileSensing
						if (storedSepValues[i][j + 1] == "True") {
							s_useTactileSensing = true;
						}
						else
						{
							s_useTactileSensing = false;
						}
						break;
					case 9: // tactileSensingType
						s_tactileSensingType = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 10: // amountSenseParts
						s_amountSenseParts = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 11: // specifiedPartNames
						s_specifiedPartNames = storedSepValues[i][j + 1];
						break;
					case 12: // reproductionType
						s_reproductionType = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 13: //genomeType
						s_genomeType = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 14: //evolvableMutationRate
						s_evolvableMutationRate = atof(storedSepValues[i][j + 1].c_str());
						break;
					case 15: //initialMutationRate
						s_initialMutationRate = atof(storedSepValues[i][j + 1].c_str());
						break;
					case 16: // amountInterNeurons
						s_amountInterNeurons = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 17: // amountCPGs
						s_amountPatternGenerators = atoi(storedSepValues[i][j + 1].c_str());
						cout << "s_amountCpsaijdnisand = " << s_amountPatternGenerators << endl;
						break;
					case 18: // amountInputNeurons
						s_amountInputNeurons = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 19: // amountOutputNeurons
						s_amountOutputNeurons = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 20: // crossover
						if (storedSepValues[i][j + 1] == "True") {
							s_useCrossover = true;
						}
						else
						{
							s_useCrossover = false;
						}
						break;
					case 21: // amountCrossover
						s_maxAmountCrossover = atoi(storedSepValues[i][j + 1].c_str());
						break;
					case 22: // evolvableCrossover
						if (storedSepValues[i][j + 1] == "True") {
							s_evolvableCrossover = true;
						}
						else
						{
							s_evolvableCrossover = false;
						}
						break;
					case 23: // variableGenomeSize
						if (storedSepValues[i][j + 1] == "True") {
							s_variableGenomeSize = true;
						}
						else
						{
							s_variableGenomeSize = false;
						}
						break;
					case 24: // geneDuplication
						if (storedSepValues[i][j + 1] == "True") {
							s_geneDuplication = true;
						}
						else
						{
							s_geneDuplication = false;
						}
						break;
					case 25: // evolvingMorphology
						if (storedSepValues[i][j + 1] == "True") {
							s_evolvingMorphology = true;
						}
						else
						{
							s_evolvingMorphology = false;
						}
						break;
					case 26: // evolveDimensions
						if (storedSepValues[i][j + 1] == "True") {
							s_evolvingDimensions = true;
						}
						else
						{
							s_evolvingDimensions = false;
						}
						break;
					case 27: // deleteRandomParts
						if (storedSepValues[i][j + 1] == "True") {
							s_deleteRandomParts = true;
						}
						else
						{
							s_deleteRandomParts = false;
						}
						break;
					case 28: // addRandomParts
						if (storedSepValues[i][j + 1] == "True") {
							s_addRandomParts = true;
						}
						else
						{
							s_addRandomParts = false;
						}
						break;
					case 29: // lEvolution
						if (storedSepValues[i][j + 1] == "True") {
							s_lEvolution = true;
						}
						else
						{
							s_lEvolution = false;
						}
						break;
					}
				}
			}
		}
	}
}