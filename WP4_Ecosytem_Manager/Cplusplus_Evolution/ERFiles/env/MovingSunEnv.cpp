#include "MovingSunEnv.h"



MovingSunEnv::MovingSunEnv()
{
	cout << "creating moving sun" << endl;
	maxTime = 20.0;
}


MovingSunEnv::~MovingSunEnv()
{
}

void MovingSunEnv::init() {
//	cout << "movingSunINIT" << endl;
	BasicSunEnv::init(); 
	lightHandle = simGetObjectHandle("Light");
	if (type == 0) {
		iteration = 0;
		step = 0;
		float sf = 1.0;
		float hsf = 0.11;
		float hsf2 = 2.0;
		float vPos = sinf(step * 0.045 * 2) * sf;
		float hPos = (-20 * hsf) + (step * hsf);
		float hPos2 = sinf(step * 0.045 * 2) * hsf2;
		//	cout << "hPos " << hPos << " , step " << step << endl;
		float pos[3] = { hPos, hPos2, vPos };
		simSetObjectPosition(lightHandle, -1, pos);
		iteration = 0;
	}
	else if (type == 1) {
		float pos1[3] = { 0.1, 0.1, 0.1 };
		simSetObjectPosition(lightHandle, -1, pos1);
	}
	maxTime = 20;
}

float MovingSunEnv::fitnessFunction(MorphologyPointer morph) {
	float fitness = BasicSunEnv::fitnessFunction(morph);
	//if (morph->modular = false) {
	//	fitness = 0; // no fixed morphology that can absorb light
	//}
	//else {
	//	fitness = BasicSunEnv::fitnessFunction(morph);
	//}

	//iteration = 0;
	//step = 0;
	return fitness;
}

float MovingSunEnv::updateEnv(MorphologyPointer morph) {
//	BasicSunEnv::updateEnv(createdModules);
	float fitness = 0;
	if (type == 0) {
		if (morph->modular = false) {
			fitness = 0; // no fixed morphology that can absorb light
		}
		else {
			//	vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
			iteration++;
			if (iteration > maxTime) {
				step += 2;
				float sf = 1.0;
				float hsf = 0.11;
				float hsf2 = 2.0;
				float vPos = sinf(step * 0.045 * 2) * sf;
				float hPos = (-18 * hsf) + (step * hsf);
				float hPos2 = sinf(step * 0.045 * 2) * hsf2;
				//		cout << "hPos " << hPos << " , step " << step << endl;
				float pos[3] = { hPos, hPos2, vPos };
				simSetObjectPosition(lightHandle, -1, pos);
				iteration = 0;
			}
		}
	}
	else if (type == 1) {
		float d = 5.0;
		float pos1[3] = { d, d, 0.1 };
		float pos2[3] = { -d, d, 0.1 };
		float pos3[3] = { -d, -d, 0.1 };
		float pos4[3] = { d, -d, 0.1 };
		int currentPos = 0;

		float t = simGetSimulationTime();
		float tFac = maxTime / 4;
		if (t < tFac && currentPos != 1) {
			simSetObjectPosition(lightHandle, -1, pos1);
			currentPos = 1;
		}
		else if (t > tFac && t < (tFac * 2) && currentPos != 2) {
			simSetObjectPosition(lightHandle, -1, pos2);
			currentPos = 2;
		}
		else if (t > (tFac * 2) && (t < tFac * 3) && currentPos != 3) {
			simSetObjectPosition(lightHandle, -1, pos3);
			currentPos = 3;
		}
		else if (t > (tFac * 3) && t < (tFac * 4) && currentPos != 4) {
			simSetObjectPosition(lightHandle, -1, pos4);
			currentPos = 4;
		}
	}
	return BasicSunEnv::updateEnv(morph);
	return fitness;
}


/* This function is different from the checkSunInterSection function in BasicSunEnv as the 
* objects created in the environment are also checked
*/
float MovingSunEnv::checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules) {
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