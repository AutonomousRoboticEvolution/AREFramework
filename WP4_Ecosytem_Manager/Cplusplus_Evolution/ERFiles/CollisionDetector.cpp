#pragma once

#include "CollisionDetector.h"
#include <iostream>
#include <cmath>


CollisionDetector::CollisionDetector()
{
}


CollisionDetector::~CollisionDetector()
{
}

void cubeDrawing(vector<vector<float>> rotatedPoints, float color[3]) {
	int cubeSegment = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 4, 0.0, -1, 12, color, NULL, NULL, color);

	float lineSegment1[6] = { rotatedPoints[0][0], rotatedPoints[0][1], rotatedPoints[0][2], rotatedPoints[1][0], rotatedPoints[1][1], rotatedPoints[1][2] };
	float lineSegment2[6] = { rotatedPoints[1][0], rotatedPoints[1][1], rotatedPoints[1][2], rotatedPoints[2][0], rotatedPoints[2][1], rotatedPoints[2][2] };
	float lineSegment3[6] = { rotatedPoints[2][0], rotatedPoints[2][1], rotatedPoints[2][2], rotatedPoints[3][0], rotatedPoints[3][1], rotatedPoints[3][2] };
	float lineSegment4[6] = { rotatedPoints[3][0], rotatedPoints[3][1], rotatedPoints[3][2], rotatedPoints[0][0], rotatedPoints[0][1], rotatedPoints[0][2] };
	float lineSegment5[6] = { rotatedPoints[0][0], rotatedPoints[0][1], rotatedPoints[0][2], rotatedPoints[4][0], rotatedPoints[4][1], rotatedPoints[4][2] };
	float lineSegment6[6] = { rotatedPoints[1][0], rotatedPoints[1][1], rotatedPoints[1][2], rotatedPoints[5][0], rotatedPoints[5][1], rotatedPoints[5][2] };
	float lineSegment7[6] = { rotatedPoints[2][0], rotatedPoints[2][1], rotatedPoints[2][2], rotatedPoints[6][0], rotatedPoints[6][1], rotatedPoints[6][2] };
	float lineSegment8[6] = { rotatedPoints[3][0], rotatedPoints[3][1], rotatedPoints[3][2], rotatedPoints[7][0], rotatedPoints[7][1], rotatedPoints[7][2] };
	float lineSegment9[6] = { rotatedPoints[4][0], rotatedPoints[4][1], rotatedPoints[4][2], rotatedPoints[5][0], rotatedPoints[5][1], rotatedPoints[5][2] };
	float lineSegment10[6] = { rotatedPoints[5][0], rotatedPoints[5][1], rotatedPoints[5][2], rotatedPoints[6][0], rotatedPoints[6][1], rotatedPoints[6][2] };
	float lineSegment11[6] = { rotatedPoints[6][0], rotatedPoints[6][1], rotatedPoints[6][2], rotatedPoints[7][0], rotatedPoints[7][1], rotatedPoints[7][2] };
	float lineSegment12[6] = { rotatedPoints[7][0], rotatedPoints[7][1], rotatedPoints[7][2], rotatedPoints[4][0], rotatedPoints[4][1], rotatedPoints[4][2] };

	simAddDrawingObjectItem(cubeSegment, lineSegment1);
	simAddDrawingObjectItem(cubeSegment, lineSegment2);
	simAddDrawingObjectItem(cubeSegment, lineSegment3);
	simAddDrawingObjectItem(cubeSegment, lineSegment4);
	simAddDrawingObjectItem(cubeSegment, lineSegment5);
	simAddDrawingObjectItem(cubeSegment, lineSegment6);
	simAddDrawingObjectItem(cubeSegment, lineSegment7);
	simAddDrawingObjectItem(cubeSegment, lineSegment8);
	simAddDrawingObjectItem(cubeSegment, lineSegment9);
	simAddDrawingObjectItem(cubeSegment, lineSegment10);
	simAddDrawingObjectItem(cubeSegment, lineSegment11);
	simAddDrawingObjectItem(cubeSegment, lineSegment12);

}


float checkArea(float interSection[3], float pps[4][3]) {

	float alphaD = sqrt(powf((pps[0][0] - pps[1][0]), 2) + powf((pps[0][1] - pps[1][1]), 2) + powf((pps[0][2] - pps[1][2]), 2));
	float betaD = sqrt(powf((pps[0][0] - pps[2][0]), 2) + powf((pps[0][1] - pps[2][1]), 2) + powf((pps[0][2] - pps[2][2]), 2));
	cout << "alpha and beta should be the same:" << endl;
	cout << "alphaD = " << alphaD << endl;
	cout << "betaD = " << betaD << endl;
	float areaX = alphaD * betaD;

	float tLine1 = sqrt(powf((pps[0][0] - interSection[0]), 2) + powf((pps[0][1] - interSection[1]), 2) + powf((pps[0][2] - interSection[2]), 2));
	float tLine2 = sqrt(powf((pps[1][0] - interSection[0]), 2) + powf((pps[1][1] - interSection[1]), 2) + powf((pps[1][2] - interSection[2]), 2));
	float tLine3 = sqrt(powf((pps[2][0] - interSection[0]), 2) + powf((pps[2][1] - interSection[1]), 2) + powf((pps[2][2] - interSection[2]), 2));
	float tLine4 = sqrt(powf((pps[3][0] - interSection[0]), 2) + powf((pps[3][1] - interSection[1]), 2) + powf((pps[3][2] - interSection[2]), 2));

	float area1 = 0.5 * tLine1 * tLine2;
	float area2 = 0.5 * tLine2 * tLine3;
	float area3 = 0.5 * tLine3 * tLine4;
	float area4 = 0.5 * tLine4 * tLine1;
	cout << "interSection Point = " << interSection[0] << ", " << interSection[1] << ", " << interSection[2] << endl;
	cout << "triangle areas are: " << area1 << ", " << area2 << ", " << area3 << ", " << area4 << endl;
	float areaBound = area1 + area2 + area3 + area4;

	cout << "areas are: " << endl;
	cout << "	areaX = " << areaX << endl;
	cout << "	areaBound =" << areaBound << endl;
	return (1 / areaX * areaBound);
}


CollisionDetector::CollisionPars CollisionDetector::checkIntersection(float pps[4][3], float color[3], float rayVector[3], float rayOrigin[3], float vectorDirections[3]) {
	float vU[3] = { rayVector[0], rayVector[1], rayVector[2] };

	CollisionPars col;
	float PQ[3]; // vector on a plane
	float PR[3]; // second vector on a plane 
	float planeOriginVector[3] = { ((pps[0][0] + pps[1][0] + pps[2][0] + pps[3][0]) / 4),
		((pps[0][1] + pps[1][1] + pps[2][1] + pps[3][1]) / 4),
		((pps[0][2] + pps[1][2] + pps[2][2] + pps[3][2]) / 4) };

	PQ[0] = (pps[1][0] - pps[0][0]); 	PQ[1] = (pps[1][1] - pps[0][1]);   PQ[2] = (pps[1][2] - pps[0][2]);
	PR[0] = (pps[2][0] - pps[0][0]); 	PR[1] = (pps[2][1] - pps[0][1]); 	PR[2] = (pps[2][2] - pps[0][2]);
	float nV[3] = { (PQ[1] * PR[2]) - (PQ[2] * PR[1]), (PQ[2] * PR[0]) -
		(PQ[0] * PR[2]), (PQ[0] * PR[1]) - (PQ[1] * PR[0]) }; // The Normal Vector of plane  relative to origin {0,0,0}
															  // optional: 
															  // draw nV
	int nVOb = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 4, 0.0, -1, 12, color, NULL, NULL, color);
	float nVOrigin[3] = { 0,0,0 };
	float nVEnd[3] = { nV[0] * 100 * vectorDirections[0], nV[1] * 100 * vectorDirections[1],nV[2] * 100 * vectorDirections[2] };
	float drawnVRay[6] = { planeOriginVector[0], planeOriginVector[1], planeOriginVector[2],
		nVEnd[0] + planeOriginVector[0], nVEnd[1] + planeOriginVector[1], nVEnd[2] + planeOriginVector[2] };
	simAddDrawingObjectItem(nVOb, drawnVRay);

	float vW[3] = { rayOrigin[0] - planeOriginVector[0], rayOrigin[1] - planeOriginVector[1] ,rayOrigin[2] - planeOriginVector[2] }; // plane point 0 minus origin
	float vwRay[6] = { rayOrigin[0], rayOrigin[1], rayOrigin[2], planeOriginVector[0], planeOriginVector[1], planeOriginVector[2] };
	simAddDrawingObjectItem(nVOb, vwRay);
	cout << "vw = " << vW[0] << ", " << vW[1] << ", " << vW[2] << endl;

	float minNDotW = ((-nV[0])*(vW[0]) + (-nV[1])*(vW[1]) + (-nV[2])*(vW[2]));
	float nDotU = ((nV[0])*(vU[0]) + (nV[1])*(vU[1]) + (nV[2])*(vU[2]));
	float s1;
	if ((nDotU < 0.0001 && nDotU > -0.0001)) {
		s1 = 1000;
		cout << "vector does not collide with a plane" << endl; // it's very close to being parallel
		cout << "nDotU = " << nDotU << endl;
	}
	else {
		s1 = minNDotW / nDotU;
		if (s1 < 0) {
			cout << "collision behind ray origin..." << endl;
			s1 = 1000;
		}
		else {
			cout << "collides with the plane!" << endl;
			cout << "nDotU = " << nDotU << endl;
			cout << "s1 = " << s1 << endl;
		}
	}
	float interSectionPos[3] = { (rayVector[0] * s1) + rayOrigin[0], (rayVector[1] * s1) + rayOrigin[1] , (rayVector[2] * s1) + rayOrigin[2] };
	//	int dummyKip = simCreateDummy(0.01, NULL);
	//	simSetObjectPosition(dummyKip, -1, interSectionPos);
	//	simSetObjectName(dummyKip, "dummyKip ");
	float interSect = checkArea(interSectionPos, pps);
	float newinterSectionDistance = sqrtf(powf(sqrtf(powf((interSectionPos[0] + rayOrigin[0]), 2) + powf((interSectionPos[1] + rayOrigin[1]), 2)), 2) + powf((interSectionPos[2] + rayOrigin[2]), 2));

	if (interSect < 1.2) {
		col.collision = true;
		col.collisionDistance = newinterSectionDistance;
		col.scalar = s1;
		int dummyMinZ = simCreateDummy(0.01, NULL);
		simSetObjectPosition(dummyMinZ, -1, interSectionPos);
		simSetObjectName(dummyMinZ, "dummy ");
	}

	return col;
}


bool CollisionDetector::checkCollisionWithRay(int objectHandle, float rayParameters[6]) {
	simGetObjectType(1);

	float objectOrigin[3];
	simGetObjectPosition(objectHandle, -1, objectOrigin);
	float size[3];
	float rotationOrigin[3] = { 0,0,0 };
	simGetObjectFloatParameter(objectHandle, 18, &size[0]);
	simGetObjectFloatParameter(objectHandle, 19, &size[1]);
	simGetObjectFloatParameter(objectHandle, 20, &size[2]);
	for (int i = 0; i < 3; i++) {
		size[i] = size[i] * 2;
	}
	cout << "size = " << size[0] << ", " << size[1] << ", " << size[2] << endl;

	float red[3] = { 1, 0, 0 };
	float blue[3] = { 0, 0, 1 };
	float green[3] = { 0, 1, 0 };

	// create a box from lines; 
	vector<vector<float>> cubeVertex; // 8 points in 3d space
	vector<vector<float>> points;
	points.resize(8);

	float objectMatrix[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	simGetObjectMatrix(objectHandle, -1, objectMatrix);
	cout << "objectMatrix = ";
	for (int i = 0; i < 12; i++) {
		cout << objectMatrix[i] << ", ";
	} cout << endl;

	float gotRotation[3];
	simGetObjectOrientation(objectHandle, -1, gotRotation);
	cout << "gotRotation = ";
	for (int i = 0; i < 3; i++) {
		cout << gotRotation[i] << ", ";
	} cout << endl;

	points[0].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[0].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[0].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[1].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[1].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[1].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[2].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[2].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[2].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[3].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[3].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[3].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[4].push_back(rotationOrigin[0] + (0.5 * size[0]));
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
	points[7].push_back(rotationOrigin[2] - (0.5 * size[2]));

	vector<vector<float>> rotatedPoints;
	rotatedPoints.resize(8);
	for (int i = 0; i < 8; i++) {
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[0]) + (points[i][1] * objectMatrix[1]) + (points[i][2] * objectMatrix[2]));
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[4]) + (points[i][1] * objectMatrix[5]) + (points[i][2] * objectMatrix[6]));
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[8]) + (points[i][1] * objectMatrix[9]) + (points[i][2] * objectMatrix[10]));
		rotatedPoints[i][0] += objectOrigin[0];
		rotatedPoints[i][1] += objectOrigin[1];
		rotatedPoints[i][2] += objectOrigin[2];
	}


	//	cube = simCreatePureShape(0, -1, size, mass, NULL);
//	float quat[4] = { 0.0,0.0,1.0,quatW };
	//simSetObjectQuaternion(cube, -1, quat);


	cubeDrawing(rotatedPoints, red);


	int ray = simAddDrawingObject(sim_drawing_lines + sim_drawing_cyclic, 4, 0.0, -1, 12, blue, NULL, NULL, blue);
	float rayVector[3] = { 0.0,0.0,1 };
	float rayOrigin[3] = { 0,0.0,0.05 };
	float rayEnd[3] = { rayVector[0] * 100, rayVector[1] * 100,rayVector[2] * 100 };
	float drawRay[6] = { rayOrigin[0], rayOrigin[1],rayOrigin[2], rayEnd[0], rayEnd[1], rayEnd[2] };
	simAddDrawingObjectItem(ray, drawRay);


	// ------------------------------------------------------------------------------------------------

	PlanePoints pps[6];
	for (int i = 0; i < 3; i++) { // 1-4-5-8
		pps[0].points[0][i] = rotatedPoints[0][i];
		pps[0].points[1][i] = rotatedPoints[3][i];
		pps[0].points[2][i] = rotatedPoints[4][i];
		pps[0].points[3][i] = rotatedPoints[7][i];
	}
	for (int i = 0; i < 3; i++) { // 2-3-6-7
		pps[1].points[0][i] = rotatedPoints[1][i];
		pps[1].points[1][i] = rotatedPoints[2][i];
		pps[1].points[2][i] = rotatedPoints[5][i];
		pps[1].points[3][i] = rotatedPoints[6][i];
	}
	for (int i = 0; i < 3; i++) { // 1-2-5-6
		pps[2].points[0][i] = rotatedPoints[0][i];
		pps[2].points[1][i] = rotatedPoints[1][i];
		pps[2].points[2][i] = rotatedPoints[4][i];
		pps[2].points[3][i] = rotatedPoints[5][i];
	}
	for (int i = 0; i < 3; i++) { // 3-4-7-8
		pps[3].points[0][i] = rotatedPoints[2][i];
		pps[3].points[1][i] = rotatedPoints[3][i];
		pps[3].points[2][i] = rotatedPoints[6][i];
		pps[3].points[3][i] = rotatedPoints[7][i];
	}
	for (int i = 0; i < 3; i++) { // 1-2-3-4
		pps[4].points[0][i] = rotatedPoints[0][i];
		pps[4].points[1][i] = rotatedPoints[1][i];
		pps[4].points[2][i] = rotatedPoints[3][i];
		pps[4].points[3][i] = rotatedPoints[2][i];
	}
	for (int i = 0; i < 3; i++) { // 5-6-7-8
		pps[5].points[0][i] = rotatedPoints[4][i];
		pps[5].points[1][i] = rotatedPoints[5][i];
		pps[5].points[2][i] = rotatedPoints[7][i];
		pps[5].points[3][i] = rotatedPoints[6][i];
	}

	bool collisionPos[6] = { false, false, false, false, false, false };

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			if (((pps[i].points[0][j] - rayOrigin[i]) * rayVector[i]) >= 0 || ((pps[i].points[1][j] - rayOrigin[i]) * rayVector[i]) >= 0
				|| ((pps[i].points[2][j] - rayOrigin[i]) * rayVector[i]) >= 0 || ((pps[i].points[3][j] - rayOrigin[i]) * rayVector[i]) >= 0) {
				collisionPos[i] = true;
			}
			else {
				collisionPos[i] = false;
				break;
			}
		}
	}


	cout << "collisionPossibility = ";
	for (int i = 0; i < 6; i++) {
		cout << collisionPos[i] << ", ";
	} cout << endl;



	// X -----------------------------------------
	bool collision = false;
	float interSectionDistance = 1000;
	float s1;
	float vectorDirections[6][3] = { { 1,1,1 },
	{ -1,-1,-1 },
	{ -1,-1,-1 },
	{ -1,-1,-1 },
	{ 1,1,1 },
	{ -1,-1,-1 },
	};

	float colors[6][3];
	for (int i = 0; i < 3; i++) {
		colors[0][i] = green[i];
		colors[1][i] = green[i];
		colors[2][i] = red[i];
		colors[3][i] = red[i];
		colors[4][i] = blue[i];
		colors[5][i] = blue[i];
	}

	for (int i = 0; i < 6; i++) {
		//if (i == 3) {
		//	if (collisionPos[i] == true) {
		CollisionPars colPars = checkIntersection(pps[i].points, colors[i], rayVector, rayOrigin, vectorDirections[i]);
		if (colPars.collision == true) {
			collision = true;
			if (colPars.collisionDistance < interSectionDistance) {
				interSectionDistance = colPars.collisionDistance;
				s1 = colPars.scalar;
			}
			//		}
			//	}
		}
	}

	if (collision == false) {
		cout << "no collision" << endl;
		return false;
	}
	else {
		cout << "interSectionDistance = " << interSectionDistance << endl;
		vector<float> jointPosition;
		jointPosition.push_back(rayVector[0] * s1);
		jointPosition.push_back(rayVector[1] * s1);
		jointPosition.push_back(rayVector[2] * s1);
		float interSectionPos[3] = { (rayVector[0] * s1) + rayOrigin[0], (rayVector[1] * s1) + rayOrigin[1], (rayVector[2] * s1) + rayOrigin[2] };
		int dummy = simCreateDummy(0.02, NULL);
		simSetObjectPosition(dummy, -1, interSectionPos);
		simSetObjectName(dummy, "collision");
		return true;
	}
}