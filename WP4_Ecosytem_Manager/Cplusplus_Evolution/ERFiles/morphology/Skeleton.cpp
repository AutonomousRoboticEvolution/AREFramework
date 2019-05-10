#include <iostream>
#include "Skeleton.h"

void Skeleton::createSkeleton() {
    switch(skeletonType){
        case BARS:
            createBars();
            break;
        case CUBES:
            createCube();
            std::cout << "WARNING: CUBES does not exist!" << std::endl;
            break;
        case LSYSTEM:
            std::cout << "WARNING: LSYSTEM does not exist!" << std::endl;
            break;
        case CPPN:
            std::cout << "WARNING: CPPN does not exist!" << std::endl;
            break;
        default:
            std::cout << "WARNING: Skeleton does not exist!" << std::endl;
            break;
    }
}

void Skeleton::createBars() {
    std::vector<int> primitiveHandles;
    int skeletonHandle;
    int tempHandle;
    float columnWidth = 0.015;
    float columnHeight = 0.010; // Wheel not touching floor decrease height
    float magnitude;
    float angle;
    float primitiveSize[3];
    float primitivePosition[3];
    float primitiveOrientation[3];
    float primitiveColour[3];
    //TODO remove this variable from here
    float brainPos[] = {0.0, 0.0, 0.0};

    for(int i = 1; i < organs.size(); i++){
        // Horizontal bar
        magnitude = sqrt(pow(organs[i].coordinates[0] / 100 - brainPos[0], 2) + pow(organs[i].coordinates[1] / 100 - brainPos[1], 2)) + columnWidth;
        angle = atan2(organs[i].coordinates[1] / 100 - brainPos[1], organs[i].coordinates[0] / 100 - brainPos[0]);

        primitiveSize[0] = magnitude;
        primitiveSize[1] = columnWidth;
        primitiveSize[2] = columnHeight;
        tempHandle = simCreatePureShape(0, 8, primitiveSize, 1, NULL);
        primitiveHandles.push_back(tempHandle);

        primitivePosition[0] = brainPos[0] + (organs[i].coordinates[0] / 100 - brainPos[0]) / 2;
        primitivePosition[1] = brainPos[1] + (organs[i].coordinates[1] / 100 - brainPos[1]) / 2;
        primitivePosition[2] = 0;
        simSetObjectPosition(tempHandle, -1, primitivePosition);

        primitiveOrientation[0] = 0;
        primitiveOrientation[1] = 0;
        primitiveOrientation[2] = angle;
        simSetObjectOrientation(tempHandle, -1, primitiveOrientation);

        primitiveColour[0] = 0;
        primitiveColour[1] = 1;
        primitiveColour[2] = 0;
        simSetShapeColor(tempHandle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

        // Vertical bar
        primitiveSize[0] = columnWidth;
        primitiveSize[1] = columnHeight;
        primitiveSize[2] = abs(organs[i].coordinates[2] / 100 - brainPos[2] - 0.025);
        tempHandle = simCreatePureShape(2, 8, primitiveSize, 1, NULL);
        primitiveHandles.push_back(tempHandle);

        primitivePosition[0] = organs[i].coordinates[0] / 100;
        primitivePosition[1] = organs[i].coordinates[1] / 100;
        primitivePosition[2] = brainPos[2] + (organs[i].coordinates[2] / 100 - brainPos[2] - 0.025) / 2;
        simSetObjectPosition(tempHandle, -1, primitivePosition);

        primitiveOrientation[0] = 0;
        primitiveOrientation[1] = 0;
        primitiveOrientation[2] = angle;
        simSetObjectOrientation(tempHandle, -1, primitiveOrientation);

        primitiveColour[0] = 0;
        primitiveColour[1] = 1;
        primitiveColour[2] = 0;
        simSetShapeColor(tempHandle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

        // Chamfer
        primitiveSize[0] = columnWidth;
        primitiveSize[1] = columnWidth;
        primitiveSize[2] = columnWidth;
        tempHandle = simCreatePureShape(1, 8, primitiveSize, 1, NULL);
        primitiveHandles.push_back(tempHandle);

        primitivePosition[0] = organs[i].coordinates[0] / 100 - cos(angle) * 0.005;
        primitivePosition[1] = organs[i].coordinates[1] / 100 - sin(angle) * 0.005;
        primitivePosition[2] = columnWidth/2.0;
        simSetObjectPosition(tempHandle, -1, primitivePosition);

        primitiveOrientation[0] = 0.0;
        primitiveOrientation[1] = 0.0; //0.785398
        primitiveOrientation[2] = angle;
        simSetObjectOrientation(tempHandle, -1, primitiveOrientation);

        primitiveColour[0] = 0;
        primitiveColour[1] = 1;
        primitiveColour[2] = 0;
        simSetShapeColor(tempHandle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);
    }
    int* array = primitiveHandles.data();
    this->skeletonHandle = simGroupShapes(array, primitiveHandles.size());
    simSetObjectName(this->skeletonHandle, "robotShape");
}

void Skeleton::createCube() {
    int cubeHandle = -1;
    float cubeSize[3] = {0.05, 0.05, 0.05};
    cubeHandle = simCreatePureShape(0, 8, cubeSize, 1, NULL);
}
