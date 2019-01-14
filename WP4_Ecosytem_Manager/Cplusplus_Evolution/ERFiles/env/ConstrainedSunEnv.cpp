#include "ConstrainedSunEnv.h"
#include <cmath>


ConstrainedSunEnv::ConstrainedSunEnv()
{
}


ConstrainedSunEnv::~ConstrainedSunEnv()
{
}

void ConstrainedSunEnv::init() {
	if (type == 0) {
		envObjectHandles.clear(); // every time the environment is created, it removes all objects and creates them again
		BasicSunEnv::init();
		// create four walls: 
		float wallSize[3] = { 0.15,0.01,0.12 };
		float wallPos[4][3] = {
			{ wallSize[0] / 2, 0, wallSize[2] / 2 },
			{ 0, wallSize[0] / 2, wallSize[2] / 2 },
			{ -wallSize[0] / 2, 0, wallSize[2] / 2 },
			{ 0, -wallSize[0] / 2, wallSize[2] / 2 }
		};
		float wallOr[4][3] = {
			{ 0,0,0.5 * M_PI },
			{ 0,0,0 },
			{ 0,0, 0.5 * M_PI },
			{ 0,0, M_PI }
		};
		float transparency[1] = { 0.5 };
		for (int i = 0; i < 4; i++) {
			envObjectHandles.push_back(simCreatePureShape(0, 24, wallSize, 1, NULL)); // they are static
			simSetObjectOrientation(envObjectHandles[i], -1, wallOr[i]);
			simSetObjectPosition(envObjectHandles[i], -1, wallPos[i]);
			string name = "col" + to_string(i);
			simSetObjectName(envObjectHandles[i], name.c_str());
			simSetShapeColor(envObjectHandles[i], NULL, sim_colorcomponent_transparency, transparency);
			simSetObjectSpecialProperty(envObjectHandles[i], sim_objectspecialproperty_collidable);
			simSetObjectSpecialProperty(envObjectHandles[i], sim_objectspecialproperty_measurable);
			simSetObjectSpecialProperty(envObjectHandles[i], sim_objectspecialproperty_detectable_all);


		}
		settings->envObjectHandles = envObjectHandles;
	} 
	else if (type == 1) {
		envObjectHandles.clear(); // every time the environment is created, it removes all objects and creates them again
		// sun
		lightHandle = simGetObjectHandle("Light");
		iteration = 0;
		step = 0;
		float pos[3] = { 0, 0, 0.5 };
		simSetObjectPosition(lightHandle, -1, pos);
		float orien[3] = {0,0,0};
		simSetObjectOrientation(lightHandle, -1, orien);
	
		BasicSunEnv::init();
		maxTime = 30;
		// create four walls: 
		float wallSize[3] = { 0.15,0.01,0.2 };
		float wallPos[4][3] = {
			{ wallSize[0] / 2, 0, 0 },
			{ 0, wallSize[0] / 2, 0 },
			{ -wallSize[0] / 2, 0, 0 },
			{ 0, -wallSize[0] / 2, 0 }
		};
		float wallOr[4][3] = {
			{ 0,0,0.5 * M_PI },
			{ 0,0,0 },
			{ 0,0, 0.5 * M_PI },
			{ 0,0, M_PI }
		};
		float transparency[1] = { 0.5 };
		for (int i = 0; i < 4; i++) {
			envObjectHandles.push_back(simCreatePureShape(0, 24, wallSize, 1, NULL)); // they are static
			simSetObjectOrientation(envObjectHandles[i], lightHandle, wallOr[i]);
			simSetObjectPosition(envObjectHandles[i], lightHandle, wallPos[i]);
			string name = "col" + to_string(i);
			simSetObjectName(envObjectHandles[i], name.c_str());
			simSetShapeColor(envObjectHandles[i], NULL, sim_colorcomponent_transparency, transparency);
			simSetObjectParent(envObjectHandles[i], lightHandle, true);
			simSetObjectSpecialProperty(envObjectHandles[i], sim_objectspecialproperty_collidable);
			simSetObjectSpecialProperty(envObjectHandles[i], sim_objectspecialproperty_measurable);
			simSetObjectSpecialProperty(envObjectHandles[i], sim_objectspecialproperty_detectable_all);

		}
		settings->envObjectHandles = envObjectHandles;
	}
}

float ConstrainedSunEnv::fitnessFunction(MorphologyPointer morph) {
	float fitness = BasicSunEnv::fitnessFunction(morph);
	//vector<float> nullVector;
	//if (morph->modular = false) {
	//	fitness = 0; // no fixed morphology that can absorb light
	//}
	//else {
	//	vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
	//	for (int i = 0; i < createdModules.size(); i++) {
	//		if (createdModules[i]->moduleID == 2) {
	//			float addedFitness = 0;
	//			if (createdModules[i]->broken == false) {
	//				vector<float> fourPoints = createdModules[i]->updateModule(nullVector);
	//				if (fourPoints.size() > 0) {
	//						//	float objectMatrix[12];
	//					//	simGetObjectMatrix(createdModules[i]->objectHandles[1], -1, objectMatrix);
	//					//	createdModules[i]->objectHandles[3];
	//					//					addedFitness += (checkSunInterSection(fourPoints, objectMatrix, createdModules));
	//					addedFitness += checkAngle(fourPoints, createdModules[i]);
	//					if (addedFitness > 0) {
	//						createdModules[i]->addEnergy(addedFitness * 1.1);
	//					}

	//				}
	//				/*	if (createdModules[i]->broken == false) {
	//						float addedFitness = 0;
	//						vector<float> fourPoints = createdModules[i]->updateModule(nullVector);
	//						float objectMatrix[12];
	//						simGetObjectMatrix(createdModules[i]->objectHandles[3], -1, objectMatrix);
	//						createdModules[i]->objectHandles[3];
	//						addedFitness += (checkSunInterSection(fourPoints, objectMatrix, createdModules));
	//						fitness += addedFitness;
	//					}*/
	//			}
	//		}
	//	}
	//}
	return fitness;
}

float ConstrainedSunEnv::updateEnv(MorphologyPointer morph) {
//	BasicSunEnv::updateEnv(createdModules);
	if (type == 1) {
		// move the walls with the sun.
		step += 1;
		float pos[3] = { step*0.002f, 0, 0.5 };
		simSetObjectPosition(lightHandle, -1, pos);
		iteration = 0;
	}

	return BasicSunEnv::updateEnv(morph);
	
}


/* This function is different from the checkSunInterSection function in BasicSunEnv as the 
* objects created in the environment are also checked
*/
float ConstrainedSunEnv::checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules) {
	vector<vector<float>> points;
	float fitness = 0;
	for (int n = 0; n < (fourPoints.size() / 3); n++) {
		points.resize(n + 1);
		for (int i = 0; i < 3; i++) {
			points[n].push_back(fourPoints[i + (n * 3)]);
		}
	}
	int sunHandle = simGetObjectHandle("Light");
	float sunPos[3];
	simGetObjectPosition(sunHandle, -1, sunPos);
	float red[3] = { 1,0,0 };
	float objectVector[3] = { objectMatrix[2], objectMatrix[6], objectMatrix[10] };
	float middleRayVector[3];

	// check intersection between five specified points of an object and see if it collides. 
	// Add a fitness value if one of the points does not collide
	// Multiply the fitness value by the z vector in respect to the sun/light
	for (int i = 0; i < points.size(); i++) {
		vector<float> normalizedRayVector;
		float ax = sunPos[0] - points[i][0];
		float ay = sunPos[1] - points[i][1];
		float az = sunPos[2] - points[i][2];
		float a = sqrt(((ax * ax) + (ay * ay) + (az * az)));
		ax = ax / a;
		ay = ay / a;
		az = az / a;
		//		cout << "magnitude = " << sqrt(((ax * ax) + (ay * ay) + (az * az))) << endl; 
		if (i == 0) {
			middleRayVector[0] = ax;
			middleRayVector[1] = ay;
			middleRayVector[2] = az;
		}

		normalizedRayVector.push_back(ax);
		normalizedRayVector.push_back(ay);
		normalizedRayVector.push_back(az);
		//float blue[3] = { 0,0,1 };
		//int rayHandle = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 1, 0.0,-1, 2, blue, NULL, NULL, blue); // startEndLine );
		//float ab[6] = { points[i][0], points[i][1], points[i][2], sunPos[0], sunPos[1], sunPos[2] };
		//simAddDrawingObjectItem(rayHandle, ab);
		bool collision = false;
		for (int j = 0; j < createdModules.size(); j++) {
			for (int k = 0; k < createdModules[j]->objectHandles.size(); k++) {
				if (collision != true) {
					collision = checkCollisionHandleRay(createdModules[j]->objectHandles[k], points[i], normalizedRayVector);
				}
				else {
					return 0;
				}
			}
		}
		for (int j = 0; j < envObjectHandles.size(); j++) {
			if (collision != true) {
				collision = checkCollisionHandleRay(envObjectHandles[j], points[i], normalizedRayVector);
			}
			else {
				return 0;
			}
		}
		fitness += 1.0;
	}
	//	cout << "objectVector = " << objectVector[0] << ", " << objectVector[1] << ", " << objectVector[2] << endl;
	//	cout << "middleRayVector = " << middleRayVector[0] << ", " << middleRayVector[1] << ", " << middleRayVector[2] << endl;
	float zDif;
	if (objectVector[0] < 0 && objectVector[1] < 0 && middleRayVector[0] < 0 && middleRayVector[1] < 0) {
		zDif = 1 - middleRayVector[2] + objectVector[2];
	}
	else if (objectVector[0] >= 0 && objectVector[1] >= 0 && middleRayVector[0] >= 0 && middleRayVector[1] >= 0) {
		zDif = 1 - middleRayVector[2] + objectVector[2];
	}
	//	else if (objectVector[0] < 0 && objectVector[1] >= 0 && middleRayVector[0] >= 0 && middleRayVector[1] >= 0) {
	//		zDif = 1 - (1 + (1 - middleRayVector[2])) + objectVector[2];
	//	}
	//	else if (objectVector[0] < 0 && objectVector[1] < 0 && middleRayVector[0] >= 0 && middleRayVector[1] < 0) {
	//		zDif = 1 - (1 + (1 - middleRayVector[2])) + objectVector[2];
	//	}
	//	else if (objectVector[0] >= 0 && objectVector[1] < 0 && middleRayVector[0] < 0 && middleRayVector[1] < 0) {
	//		zDif = 1 - (1 + (1 - middleRayVector[2])) + objectVector[2];
	//	}
	else {
		zDif = 1 - (1 + (1 - middleRayVector[2])) + objectVector[2];
	}

	if (zDif > 0) {
		fitness = fitness * zDif;
	}
	else {
		return 0;
	}

	//float addedVector[3] = { objectVector[0] + middleRayVector[0], objectVector[1] + middleRayVector[1], objectVector[2] + middleRayVector[2] };
	//float a = sqrt(((addedVector[0] * addedVector[0]) + (addedVector[1]* addedVector[1]) + (addedVector[2]* addedVector[2])));
	//addedVector[0] = addedVector[0] / a;
	//addedVector[1] = addedVector[1] / a;
	//addedVector[2] = addedVector[2] / a;

	//float zDif = addedVector[2] / 2;
	//fitness = fitness * zDif;
	return fitness;
}