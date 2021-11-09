//
// Created by ebb505 on 12/03/2021.
//
#include "simulatedER/Organ.h"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

#define ISROBOTSTATIC 0

/// \todo EB: Do i need this?
using namespace are::sim;
//namespace cop = coppelia;



void Organ::IsOrganColliding(const std::vector<int>& skeletonHandles, const std::vector<Organ>& organList)
{
    int8_t collisionResult;
    // Check collision with skeleton
    for (int oH : objectHandles) {
        for (int skeletonHandle : skeletonHandles) {
            collisionResult = simCheckCollision(oH, skeletonHandle);
            if (collisionResult == 1) { // Collision detected!
                organColliding = true;
                return;
            }
        }
    }
    // Check collision with other organs
    for (int oH : objectHandles) {
        for (auto &organComp : organList) {
            if(organComp.isOrganRemoved() && organComp.isOrganChecked()) // Prevent comparing to organs already removed.
                continue;
            if(organComp.getOrganHandle() == organHandle) // Prevent comparing organ with itself.
                continue;
            for (auto &i : organComp.objectHandles) {
                collisionResult = simCheckCollision(oH, i);
                if (collisionResult == 1) { // Collision detected!
                    organColliding = true;
                    return;
                }
            }
        }
    }
}

void Organ::isOrganGoodOrientation()
{
    float diffPosZ;
    diffPosZ = connectorPos[2] - organPos[2];
    /// \todo EB: remove this hard-coded value
    organGoodOrientation = (diffPosZ > -0.015) && (diffPosZ < 0.015); // Is organ pointing downwards?
}

void Organ::isGripperCollision(int gripperHandle, const std::vector<int>& skeletonHandles, const std::vector<Organ>& organList)
{
    float gripperPosition[3];
    float gripperOrientation[3];
    /// \todo EB: would it be better to use the frame of the organ? It's easy change
    gripperPosition[0] = connectorPos[0]; gripperPosition[1] = connectorPos[1]; gripperPosition[2] = connectorPos[2];
    gripperOrientation[0] = connectorOri[0]; gripperOrientation[1] = connectorOri[1]; gripperOrientation[2] = connectorOri[2];
    simSetObjectPosition(gripperHandle, -1, gripperPosition);
    simSetObjectOrientation(gripperHandle, -1, gripperOrientation);

    // Orientation
    gripperOrientation[0] = 0.0; gripperOrientation[1] = M_PI_2; gripperOrientation[2] = 0.0;
    simSetObjectOrientation(gripperHandle, gripperHandle, gripperOrientation);
    gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = M_PI;
    simSetObjectOrientation(gripperHandle, gripperHandle, gripperOrientation);

    // Move relative to gripper.
    gripperPosition[0] = -0.035;
    gripperPosition[1] = 0.0;
    /// \todo EB: This offset should be somewhere else or constant.
    if(organType == 1) // Wheels
        gripperPosition[2] = -0.13;
    else if(organType == 2) // Sensors
        gripperPosition[2] = -0.15;
    else if(organType == 3) // Joints
        gripperPosition[2] = -0.195;
    else if(organType == 4) // Caster
        gripperPosition[2] = -0.1;
    else
        assert(false);

    simSetObjectPosition(gripperHandle, gripperHandle, gripperPosition);
    gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = 1.5708;
    simSetObjectOrientation(gripperHandle, gripperHandle, gripperOrientation);
    int8_t collisionResult;
    // Check collision with skeleton (s)
    for(int skeletonHandle : skeletonHandles) {
        collisionResult = simCheckCollision(gripperHandle, skeletonHandle);
        if (collisionResult == 1) // Collision detected!
            return;
    }
    /// \todo EB: Wheels are ignored in the following loop.
    // Check collision with other organs
    for (auto &organComp : organList) {
        if(organComp.isOrganRemoved() && organComp.isOrganChecked())
            continue;
        for (auto &i : organComp.objectHandles) {
            collisionResult = simCheckCollision(gripperHandle, i);
            if (collisionResult == 1) // Collision detected!
                return;
        }
    }
    organGripperAccess = true;
    gripperPosition[0] = 1.0; gripperPosition[1] = 1.0; gripperPosition[2] = 1.0;
    simSetObjectPosition(gripperHandle, -1, gripperPosition);
}

void Organ::isOrganInsideMainSkeleton(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    // Transform organPos from m to voxels
    int xPos = static_cast<int>(std::round(organPos[0]/VOXEL_REAL_SIZE));
    int yPos = static_cast<int>(std::round(organPos[1]/VOXEL_REAL_SIZE));
    int zPos = static_cast<int>(std::round(organPos[2]/VOXEL_REAL_SIZE));
    zPos -= MATRIX_HALF_SIZE;
    uint8_t voxelValue;
    voxelValue = skeletonMatrix.getVoxel(xPos,yPos,zPos);
    if(voxelValue == FILLEDVOXEL) {// Organ centre point inside of skeleton
        organInsideSkeleton = true;
        return;
    }
    else if(voxelValue == EMPTYVOXEL) {
        /// \todo EB: This temporary fixes the issue of the joint colliding with the head organ!
        if(xPos <= xHeadUpperLimit && xPos >= xHeadLowerLimit &&
           yPos <= yHeadUpperLimit && yPos >= xHeadLowerLimit) {
            organInsideSkeleton = true;
            return;
        }
        else {
            return;
        }
    }
    else
        assert(false);
}

void Organ::testOrgan(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int gripperHandle, const std::vector<int>& skeletonHandles,
                      const std::vector<Organ>& organList)
{
    IsOrganColliding(skeletonHandles, organList);
    isOrganGoodOrientation();
    isGripperCollision(gripperHandle, skeletonHandles, organList);
    isOrganInsideMainSkeleton(skeletonMatrix);
}

void Organ::repressOrgan()
{
    if(organInsideSkeleton || organColliding || !organGoodOrientation || !organGripperAccess){
        simRemoveObject(simGetObjectParent(organHandle)); // Remove force sensor.
        simRemoveModel(organHandle); // Remove model.
        simRemoveModel(graphicConnectorHandle);
        organRemoved = true;
    }
    else{
        organRemoved = false;
    }
}

void Organ::createOrgan(int skeletonHandle)
{
    organChecked = true;
    /// \todo EB: It might be worth to have this as a separate parameters (?)
    std::string modelsPath = are::settings::getParameter<are::settings::String>(parameters,"#organsPath").value;
    int version = are::settings::getParameter<are::settings::Integer>(parameters,"#organsVersion").value;

    if(version != 2 || version != 3){
        std::cout << "Version of organs not set. Set to default valuee of 3." << std::endl;
        version = 3;
    }
    std::stringstream vers;
    vers << version;

    if(organType == 0) // Brain
        modelsPath += "C_HeadV" + vers.str() + ".ttm";
    else if(organType == 1) // Wheels
        modelsPath += "C_WheelV" + vers.str() + ".ttm";
    else if(organType == 2) // Sensors
        modelsPath += "C_SensorV" + vers.str() + ".ttm";
    else if(organType == 3) // Joints
        modelsPath += "C_Joint.ttm";
    else if(organType == 4) // Caster
        modelsPath+= "C_Caster.ttm";
    else
        assert(false);

    organHandle = simLoadModel(modelsPath.c_str());
    assert(organHandle != -1);

    /// \todo: EB: Maybe we should move this to a method
    // Get object handles for collision detection
    simAddObjectToSelection(sim_handle_tree, organHandle);
    int selectionSize = simGetObjectSelectionSize();
    // store all these objects (max 10 shapes)
    int shapesStorage[10]; // stores up to 10 shapes
    simGetObjectSelection(shapesStorage);
    for (int i = 0; i < selectionSize; i++) {
        char* componentName;
        componentName = simGetObjectName(shapesStorage[i]);
        if('P' == componentName[0]){
            objectHandles.push_back(shapesStorage[i]);
        }
        simReleaseBuffer(componentName);
    }
    simRemoveObjectFromSelection(sim_handle_all, organHandle);

    // For force sensor
    int fsParams[5];
    fsParams[0] = 0; fsParams[1] = 1; fsParams[2] = 1; fsParams[3] = 0; fsParams[4] = 0;
    float fsFParams[5];
    // EB: Calibrating this values is very important. They define when a sensor is broken.
    fsFParams[0] = 0.005; fsFParams[1] = 1000000; fsFParams[2] = 10000000; fsFParams[3] = 0; fsFParams[4] = 0;
    int forceSensor = simCreateForceSensor(3, fsParams, fsFParams, nullptr);
    // Set organPos
    float tempOrganPos[3];
    tempOrganPos[0] = organPos.at(0); tempOrganPos[1] = organPos.at(1); tempOrganPos[2] = organPos.at(2);
    simSetObjectPosition(forceSensor,-1,tempOrganPos);
    simSetObjectPosition(organHandle, -1, tempOrganPos);

    // Create connector and offset by some distance
    /// \todo EB: This offset shouldn't be here.
    float tempConnectorPos[3];
    tempConnectorPos[0] = tempOrganPos[0]; tempConnectorPos[1] = tempOrganPos[1]; tempConnectorPos[2] = tempOrganPos[2];
    if(organType == 0) // Brain
        tempConnectorPos[2] = tempOrganPos[2] + 0.02;
    else if(organType == 1){ // Wheels
        tempConnectorPos[1] = organPos[1] - 0.025;
    }
    else if(organType == 2) { // Sensors
        tempConnectorPos[2] = organPos[2] + 0.02;
    }else if(organType == 3) // Joints
        tempConnectorPos[2] = tempOrganPos[2] + 0.035;
    else if(organType == 4)  // Caster
        tempConnectorPos[2] = tempOrganPos[2] + 0.02;
    else
        assert(false);

    connectorHandle = simCreateDummy(0.01,nullptr);
    simSetObjectParent(connectorHandle, organHandle, 1);
    simSetObjectPosition(connectorHandle,-1,tempConnectorPos);

    // Set organOri
    float tempOrganOri[3];
    tempOrganOri[0] = organOri.at(0); tempOrganOri[1] = organOri.at(1); tempOrganOri[2] = organOri.at(2);
    simSetObjectOrientation(forceSensor, -1, tempOrganOri);
    simSetObjectOrientation(organHandle, -1, tempOrganOri);
    simSetObjectOrientation(connectorHandle,-1,tempOrganOri);
    // If the organ is not brain rotate along z-axis relative to the organ itself.
    if(organType != 0){
        tempOrganOri[0] = 0.0; tempOrganOri[1] = 0.0; tempOrganOri[2] = organOri.at(2);
        simSetObjectOrientation(forceSensor, forceSensor, tempOrganOri);
        simSetObjectOrientation(organHandle, organHandle, tempOrganOri);
    }

    /// \todo EB: These two lines work but I don't understand why with the previous method no.
    simGetObjectOrientation(organHandle, -1, tempOrganOri);
    simSetObjectOrientation(connectorHandle,-1,tempOrganOri);
    // Set parents
    simSetObjectParent(forceSensor,skeletonHandle,1);
    simSetObjectParent(organHandle, forceSensor, 1);
    /// \todo There might be a bug somewhere! The lines commented work well for ARE-Puck and ARE-Potato. The uncommented lines work well with evolved morphologies...
    // This moves the organ slightly away from the surface. This parameters were calibrated through visual inspection
    /// \todo: EB: We might not need this in the future
    tempOrganPos[0] = 0.0; tempOrganPos[1] = 0.0; tempOrganPos[2] = 0.0;

    if(organType == 0) // Brain
        tempOrganPos[2] = 0.0;
    else if(organType == 1) // Wheels
        tempOrganPos[2] = -0.03;
    else if(organType == 2) { // Sensors
        tempOrganPos[2] = -0.03;
    }else if(organType == 3) // Joints
        tempOrganPos[2] = -0.045;
    else if(organType == 4) { // Caster
        tempOrganPos[0] = 0.01;
        tempOrganPos[2] = -0.03;
    } else
        assert(false);

    simSetObjectPosition(forceSensor, forceSensor, tempOrganPos);
    simGetObjectPosition(organHandle, -1, tempOrganPos);
    organPos.at(0) = tempOrganPos[0];
    organPos.at(1) = tempOrganPos[1];
    organPos.at(2) = tempOrganPos[2];
    // Position of connector after rotation
    float newConnectorPos[3];
    simGetObjectPosition(connectorHandle, -1, newConnectorPos);
    float newConnectorOri[3];
    simGetObjectOrientation(connectorHandle, -1, newConnectorOri);

    /// \todo EB: Resize vector might be better way to do this.
    connectorPos.push_back(newConnectorPos[0]);
    connectorPos.push_back(newConnectorPos[1]);
    connectorPos.push_back(newConnectorPos[2]);
    connectorOri.push_back(newConnectorOri[0]);
    connectorOri.push_back(newConnectorOri[1]);
    connectorOri.push_back(newConnectorOri[2]);

#ifndef ISROBOTSTATIC
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
    simSetObjectInt32Parameter(organHandle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
    simSetObjectInt32Parameter(organHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
    if(organType != 0)
        createMaleConnector(skeletonHandle);

}

void Organ::createMaleConnector(int skeletonHandle)
{
    float tempConnectorPosition[3];
    float tempConnectorOrientation[3];
    int tempConnectorHandle;

    tempConnectorPosition[0] = connectorPos.at(0);
    tempConnectorPosition[1] = connectorPos.at(1);
    tempConnectorPosition[2] = connectorPos.at(2);

    tempConnectorOrientation[0] = connectorOri.at(0);
    tempConnectorOrientation[1] = connectorOri.at(1);
    tempConnectorOrientation[2] = connectorOri.at(2);

    std::string modelsPath = are::settings::getParameter<are::settings::String>(parameters,"#organsPath").value;
    modelsPath += "C_MaleConnectorV2.ttm";

    tempConnectorHandle = simLoadModel(modelsPath.c_str());
    assert(tempConnectorHandle != -1);

    simSetObjectPosition(tempConnectorHandle, -1, tempConnectorPosition);
    simSetObjectOrientation(tempConnectorHandle, -1, tempConnectorOrientation);

    /// \todo EB: We need to find a better way align origins of connectors and connectors! This distance only applies for the second male conenctor
    tempConnectorPosition[0] = 0.00;
    tempConnectorPosition[1] = 0.0;
    tempConnectorPosition[2] = 0.005;

    tempConnectorOrientation[0] = 3.14159;
    tempConnectorOrientation[1] = 0.0;
    tempConnectorOrientation[2] = 0.0;

    simSetObjectPosition(tempConnectorHandle, tempConnectorHandle, tempConnectorPosition);
    simSetObjectOrientation(tempConnectorHandle, tempConnectorHandle, tempConnectorOrientation);

    simSetObjectParent(tempConnectorHandle, skeletonHandle, 1);
    graphicConnectorHandle = tempConnectorHandle;
}
