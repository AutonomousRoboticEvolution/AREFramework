#include <iostream>
#include "Skeleton.h"

void Skeleton::createSkeleton() {
    switch(skeletonType){
        case BARS:
            createBars();
            break;
        case CUBES:
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
    float parentPosition[3];
    float childPosition[3];
    int temp_primitive_handle;
    float columnWidth = 0.015;
    float columnHeight = 0.010; // Wheel not touching floor decrease height
    float magnitude;
    float angle;
    float primitiveSize[3];
    float primitivePosition[3];
    float primitiveOrientation[3];
    float primitiveColour[3];

    simGetObjectPosition(this->parentHandle,-1,parentPosition);
    simGetObjectPosition(this->childHandle,-1,childPosition);

    // Horizontal bar
    magnitude = sqrt(pow(childPosition[0] / 100 - parentPosition[0], 2) + pow(childPosition[1] / 100 - parentPosition[1], 2)) + columnWidth;
    angle = atan2(childPosition[1] / 100 - parentPosition[1], childPosition[0] / 100 - parentPosition[0]);

    primitiveSize[0] = magnitude;
    primitiveSize[1] = columnWidth;
    primitiveSize[2] = columnHeight;
    temp_primitive_handle = simCreatePureShape(0, 8, primitiveSize, 1, NULL);
    primitiveHandles.push_back(temp_primitive_handle);

    primitivePosition[0] = parentPosition[0] + (childPosition[0] / 100 - parentPosition[0]) / 2;
    primitivePosition[1] = parentPosition[1] + (childPosition[1] / 100 - parentPosition[1]) / 2;
    primitivePosition[2] = 0;
    simSetObjectPosition(temp_primitive_handle, -1, primitivePosition);

    primitiveOrientation[0] = 0;
    primitiveOrientation[1] = 0;
    primitiveOrientation[2] = angle;
    simSetObjectOrientation(temp_primitive_handle, -1, primitiveOrientation);

    primitiveColour[0] = 0;
    primitiveColour[1] = 1;
    primitiveColour[2] = 0;
    simSetShapeColor(temp_primitive_handle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

    // Vertical bar
    primitiveSize[0] = columnWidth;
    primitiveSize[1] = columnHeight;
    primitiveSize[2] = abs(childPosition[2] / 100 - parentPosition[2]);
    temp_primitive_handle = simCreatePureShape(2, 8, primitiveSize, 1, NULL);
    primitiveHandles.push_back(temp_primitive_handle);

    primitivePosition[0] = childPosition[0] / 100;
    primitivePosition[1] = childPosition[1] / 100;
    primitivePosition[2] = parentPosition[2] + (childPosition[2] / 100 - parentPosition[2]) / 2;
    simSetObjectPosition(temp_primitive_handle, -1, primitivePosition);

    primitiveOrientation[0] = 0;
    primitiveOrientation[1] = 0;
    primitiveOrientation[2] = angle;
    simSetObjectOrientation(temp_primitive_handle, -1, primitiveOrientation);

    primitiveColour[0] = 0;
    primitiveColour[1] = 1;
    primitiveColour[2] = 0;
    simSetShapeColor(temp_primitive_handle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

    // Chamfer
    primitiveSize[0] = columnWidth;
    primitiveSize[1] = columnWidth;
    primitiveSize[2] = columnWidth;
    temp_primitive_handle = simCreatePureShape(1, 8, primitiveSize, 1, NULL);
    primitiveHandles.push_back(temp_primitive_handle);

    primitivePosition[0] = childPosition[0] / 100 - cos(angle) * 0.005;
    primitivePosition[1] = childPosition[1] / 100 - sin(angle) * 0.005;
    primitivePosition[2] = columnWidth/2.0;
    simSetObjectPosition(temp_primitive_handle, -1, primitivePosition);

    primitiveOrientation[0] = 0.0;
    primitiveOrientation[1] = 0.0; //0.785398
    primitiveOrientation[2] = angle;
    simSetObjectOrientation(temp_primitive_handle, -1, primitiveOrientation);

    primitiveColour[0] = 0;
    primitiveColour[1] = 1;
    primitiveColour[2] = 0;
    simSetShapeColor(temp_primitive_handle, NULL, sim_colorcomponent_ambient_diffuse, primitiveColour);

    int* array = primitiveHandles.data();
    this->skeletonHandle = simGroupShapes(array, primitiveHandles.size());
}
