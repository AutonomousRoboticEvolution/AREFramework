//
// Created by ebb505 on 12/03/2021.
//
#include "simulatedER/Organ.h"


#define ISROBOTSTATIC 0

/// \todo EB: Do i need this?
using namespace are::sim;
//namespace cop = coppelia;
using mc = are::morph_const;

void organ::generate_orientation(float x, float y, float z, std::vector<float> &orientation)
{
    /// \todo: EB: Remove z > 0 amd z < 0 as the organ cannot face these directions.
    // Gives the direction of the organ given the direction of the surface
    if ((x > 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +M_PI_2; orientation.at(1) = -M_PI_4; orientation.at(2) = -M_PI_2;
    }else if ((x < 0) && (y < 0) && (z == 0)){
        orientation.at(0) = -M_PI_2; orientation.at(1) = +M_PI_4; orientation.at(2) = M_PI_2;
    }else if ((x < 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +M_PI_2; orientation.at(1) = +M_PI_4; orientation.at(2) = -M_PI_2;
    }else if ((x > 0) && (y < 0) && (z == 0)){
        orientation.at(0) =  -M_PI_2; orientation.at(1) = -M_PI_4; orientation.at(2) = M_PI_2;
    }else if ((x < 0) && (y == 0) && (z == 0)){
        orientation.at(0) = +0.0; orientation.at(1) = +M_PI_2; orientation.at(2) = +0.0;
    } else if ((x == 0) && (y < 0) && (z == 0)){
        orientation.at(0) = -M_PI_2; orientation.at(1) = +0.0; orientation.at(2) = +M_PI_2;
    } else if ((x == 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +M_PI_2; orientation.at(1) = +0.0; orientation.at(2) = -M_PI_2;
    } else if ((x > 0) && (y == 0) && (z == 0)) {
        orientation.at(0) = +0.0; orientation.at(1) = -M_PI_2; orientation.at(2) = +M_PI;
    } else {
        orientation.at(0) = +0.6154; orientation.at(1) = -0.5236; orientation.at(2) = -2.1862;
        std::cerr << "We shouldn't be here: " << __func__ << " " << x << " "
                  << y << " " << z << std::endl;
    }
}

void organ::generate_orientation(int x, int y, int z, std::vector<float> &orientation)
{
    /// \todo: EB: Remove z > 0 amd z < 0 as the organ cannot face these directions.
    // Gives the direction of the organ given the direction of the surface
    if ((x > 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +M_PI_2; orientation.at(1) = -M_PI_4; orientation.at(2) = -M_PI_2;
    }else if ((x < 0) && (y < 0) && (z == 0)){
        orientation.at(0) = -M_PI_2; orientation.at(1) = +M_PI_4; orientation.at(2) = M_PI_2;
    }else if ((x < 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +M_PI_2; orientation.at(1) = +M_PI_4; orientation.at(2) = -M_PI_2;
    }else if ((x > 0) && (y < 0) && (z == 0)){
        orientation.at(0) =  -M_PI_2; orientation.at(1) = -M_PI_4; orientation.at(2) = M_PI_2;
    }else if ((x < 0) && (y == 0) && (z == 0)){
        orientation.at(0) = +0.0; orientation.at(1) = +M_PI_2; orientation.at(2) = +0.0;
    } else if ((x == 0) && (y < 0) && (z == 0)){
        orientation.at(0) = -M_PI_2; orientation.at(1) = +0.0; orientation.at(2) = +M_PI_2;
    } else if ((x == 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +M_PI_2; orientation.at(1) = +0.0; orientation.at(2) = -M_PI_2;
    } else if ((x > 0) && (y == 0) && (z == 0)) {
        orientation.at(0) = +0.0; orientation.at(1) = -M_PI_2; orientation.at(2) = +M_PI;
    } else {
        orientation.at(0) = +0.6154; orientation.at(1) = -0.5236; orientation.at(2) = -2.1862;
        std::cerr << "We shouldn't be here: " << __func__ << " " << x << " "
                  << y << " " << z << std::endl;
    }
}

void Organ::createOrgan(int skeletonHandle)
{
    organChecked = true;
    std::string modelsPath = are::settings::getParameter<are::settings::String>(parameters,"#modelsPath").value;

    if(organType == 0) // Brain
        modelsPath += "/organs/head.ttm";
    else if(organType == 1) // Wheels
        modelsPath += "/organs/wheel.ttm";
    else if(organType == 2) // Sensors
        modelsPath += "/organs/sensor.ttm";
    else if(organType == 3) {// Joints
        if(are::settings::getParameter<are::settings::Boolean>(parameters,"#isLeg").value)
            modelsPath += "/organs/leg_2.ttm";
        else
            modelsPath += "/organs/joint.ttm";
    }
    else if(organType == 4) // Caster
        modelsPath+= "/organs/caster.ttm";
    else
        assert(false);

    organHandle = simLoadModel(modelsPath.c_str());
    assert(organHandle != -1);

    /// \todo: EB: Maybe we should move this to a method
    // Get object handles for collision detection
    int selectionSize;
    int* obj_handles = simGetObjectsInTree(organHandle,sim_handle_all,2,&selectionSize);
    for (int i = 0; i < selectionSize; i++) {
        char* componentName;
        componentName = simGetObjectAlias(obj_handles[i],-1);
        if('P' == componentName[0]){
            objectHandles.push_back(obj_handles[i]);
        }
        simReleaseBuffer(componentName);
    }
    simReleaseBuffer(obj_handles);

    // For force sensor
    int fsParams[5];
    fsParams[0] = 0; fsParams[1] = 1; fsParams[2] = 1; fsParams[3] = 0; fsParams[4] = 0;
    double fsFParams[5];
    // EB: Calibrating this values is very important. They define when a sensor is broken.
    fsFParams[0] = 0.005; fsFParams[1] = 1000000; fsFParams[2] = 10000000; fsFParams[3] = 0; fsFParams[4] = 0;
    int forceSensor = simCreateForceSensor(3, fsParams, fsFParams, nullptr);
    // Set organPos
    double tempOrganPos[3];
    tempOrganPos[0] = organPos.at(0); tempOrganPos[1] = organPos.at(1); tempOrganPos[2] = organPos.at(2);
    simSetObjectPosition(forceSensor,-1,tempOrganPos);
    simSetObjectPosition(organHandle, -1, tempOrganPos);

    // Create connector and offset by some distance relative to the organ itself
    /// \todo EB: This offset shouldn't be here.
    double tempConnectorPos[3];
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
    else if(organType == 4) {  // Caster
        tempConnectorPos[0] = tempOrganPos[0] - 0.01;
        tempConnectorPos[2] = tempOrganPos[2] + 0.02;
    }
    else
        assert(false);
    /// \todo EB: Destroy this dummy
    connectorHandle = simCreateDummy(0.01,nullptr);
    simSetObjectParent(connectorHandle, organHandle, 1);
    simSetObjectPosition(connectorHandle,-1,tempConnectorPos);

    // Set organOri
    double tempOrganOri[3];
    tempOrganOri[0] = organOri.at(0); tempOrganOri[1] = organOri.at(1); tempOrganOri[2] = organOri.at(2);
    simSetObjectOrientation(forceSensor, -1, tempOrganOri);
    simSetObjectOrientation(organHandle, -1, tempOrganOri);
    simSetObjectOrientation(connectorHandle,-1,tempOrganOri);
    // If the organ is not brain rotate along x-axis relative to the organ itself.
//    if(organType != 0){
//        tempOrganOri[0] = organOri.at(3); tempOrganOri[1] = 0.0; tempOrganOri[2] = 0.0;
//        simSetObjectOrientation(forceSensor, forceSensor, tempOrganOri);
//        simSetObjectOrientation(organHandle, organHandle, tempOrganOri);
//    }
    /// \todo EB: These two lines work but I don't understand why with the previous method no.
    simGetObjectOrientation(organHandle, -1, tempOrganOri);
    organOri.at(0) = tempOrganOri[0]; organOri.at(1) = tempOrganOri[1]; organOri.at(2) = tempOrganOri[2];
    // Set parents
    simSetObjectParent(forceSensor,skeletonHandle,1);
    simSetObjectParent(organHandle, forceSensor, 1);
    // This moves the organ slightly away from the surface. This parameters were calibrated through visual inspection
    /// \todo: EB: We might not need this in the future
    tempOrganPos[0] = 0.0; tempOrganPos[1] = 0.0; tempOrganPos[2] = 0.0;

    if(organType == 0) // Brain
        tempOrganPos[2] = 0.0;
    else if(organType == 1) { // Wheels
        tempOrganPos[0] = -0.013; // Relative to the floor
        tempOrganPos[2] = -0.035; // Relative to the surface of the skeleton
    } else if(organType == 2) { // Sensors
        tempOrganPos[0] = -0.013; // Relative to the floor
        tempOrganPos[2] = -0.035;
    }else if(organType == 3) { // Joints
        tempOrganPos[0] = -0.014; // Relative to the floor
        tempOrganPos[2] = -0.05;
    }else if(organType == 4) { // Caster
        tempOrganPos[0] = -0.003; // Relative to the floor
        tempOrganPos[2] = -0.035;
    } else
        assert(false);

    simSetObjectPosition(forceSensor, forceSensor, tempOrganPos);
    simGetObjectPosition(organHandle, -1, tempOrganPos);
    organPos.at(0) = tempOrganPos[0];
    organPos.at(1) = tempOrganPos[1];
    organPos.at(2) = tempOrganPos[2];
    // Position of connector after rotation
    double newConnectorPos[3];
    simGetObjectPosition(connectorHandle, -1, newConnectorPos);
    double newConnectorOri[3];
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
    simSetObjectInt32Param(organHandle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
    simSetObjectInt32Param(organHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
    connector_frame_pos.resize(3);
    if(organType != 0)
        createMaleConnector();
    else
        connector_frame_pos = connectorPos;
}

void Organ::createMaleConnector()
{
    double tempConnectorPosition[3];
    double tempConnectorOrientation[3];
    double temp_connector_frame_pos[3];
    int temp_physics_connector_handle;
    int temp_visual_connector_handle = -1;


    tempConnectorPosition[0] = connectorPos.at(0);
    tempConnectorPosition[1] = connectorPos.at(1);
    tempConnectorPosition[2] = connectorPos.at(2);

    tempConnectorOrientation[0] = connectorOri.at(0);
    tempConnectorOrientation[1] = connectorOri.at(1);
    tempConnectorOrientation[2] = connectorOri.at(2);

    std::string physics_path = are::settings::getParameter<are::settings::String>(parameters,"#modelsPath").value;
    std::string visual_path = are::settings::getParameter<are::settings::String>(parameters,"#modelsPath").value;
    visual_path += "utils/male_connector_visual.ttm";
    physics_path += "utils/male_connector_physics.ttm";

    temp_physics_connector_handle = simLoadModel(physics_path.c_str());
    assert(temp_physics_connector_handle != -1);
    temp_visual_connector_handle = simLoadModel(visual_path.c_str());
    assert(temp_visual_connector_handle != -1);

    simSetObjectPosition(temp_physics_connector_handle, -1, tempConnectorPosition);
    simSetObjectOrientation(temp_physics_connector_handle, -1, tempConnectorOrientation);
    simSetObjectPosition(temp_visual_connector_handle, -1, tempConnectorPosition);
    simSetObjectOrientation(temp_visual_connector_handle, -1, tempConnectorOrientation);

    /// \todo EB: We need to find a better way align origins of connectors and connectors! This distance only applies for the second male conenctor
    tempConnectorPosition[0] = 0.00;
    tempConnectorPosition[1] = 0.0;
    tempConnectorPosition[2] = 0.0;

    tempConnectorOrientation[0] = 3.14159;
    tempConnectorOrientation[1] = 0.0;
    tempConnectorOrientation[2] = 0.0;

    simSetObjectPosition(temp_visual_connector_handle, temp_visual_connector_handle, tempConnectorPosition);
    simSetObjectOrientation(temp_visual_connector_handle, temp_visual_connector_handle, tempConnectorOrientation);
    simSetObjectParent(temp_visual_connector_handle, organHandle, 1);

    simSetObjectPosition(temp_physics_connector_handle, temp_physics_connector_handle, tempConnectorPosition);
    simSetObjectOrientation(temp_physics_connector_handle, temp_physics_connector_handle, tempConnectorOrientation);
    simSetObjectParent(temp_physics_connector_handle, organHandle, 1);

    physics_connector_handle = temp_physics_connector_handle;

    simGetObjectPosition(simGetObjectChild(temp_visual_connector_handle,0),-1,temp_connector_frame_pos); // Get the position of the dummy for the blueprint
    connector_frame_pos.at(0) = temp_connector_frame_pos[0];
    connector_frame_pos.at(1) = temp_connector_frame_pos[1];
    connector_frame_pos.at(2) = temp_connector_frame_pos[2] - 0.0098;
}

void Organ::testOrgan(const PolyVox::RawVolume<uint8_t> &skeletonMatrix, int gripperHandle, const std::vector<int>& skeletonHandles,
                      const std::vector<Organ>& organList)
{
    IsOrganColliding(skeletonHandles, organList);
    //isGripperCollision(gripperHandle, skeletonHandles, organList);
    isOrganInsideMainSkeleton(skeletonMatrix);
}

void Organ::repressOrgan()
{

    if(organInsideSkeleton || organColliding){// || !organGripperAccess){
        int parent_handle[1] = {simGetObjectParent(organHandle)};
        simRemoveObjects(parent_handle,1); // Remove force sensor.
        simRemoveModel(organHandle); // Remove model.
        simRemoveModel(physics_connector_handle);
        organRemoved = true;
    }
    else{
        organRemoved = false;
    }
}

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
    // Check connector collision
//    for (int oH : objectHandles) {
        for (auto &organComp : organList) {
            if(organComp.isOrganRemoved() && organComp.isOrganChecked()) // Prevent comparing to organs already removed.
                continue;
            if(organComp.getOrganHandle() == organHandle) // Prevent comparing organ with itself.
                continue;
            //for (auto &i : organComp.objectHandles) {
            collisionResult = simCheckCollision(physics_connector_handle, organComp.get_graphical_connector_handle());
            if (collisionResult == 1) { // Collision detected!
                organColliding = true;
                return;
            }
            //}
//        }
    }
}

void Organ::isGripperCollision(int gripperHandle, const std::vector<int>& skeletonHandles, const std::vector<Organ>& organList)
{
    double gripperPosition[3];
    double gripperOrientation[3];
    /// \todo EB: would it be better to use the frame of the organ? It's easy change
    gripperPosition[0] = connectorPos[0]; gripperPosition[1] = connectorPos[1]; gripperPosition[2] = connectorPos[2];
    gripperOrientation[0] = connectorOri[0]; gripperOrientation[1] = connectorOri[1]; gripperOrientation[2] = connectorOri[2];
    simSetObjectPosition(gripperHandle, -1, gripperPosition);
    simSetObjectOrientation(gripperHandle, -1, gripperOrientation);

    // Orientation
    gripperOrientation[0] = 0.0; gripperOrientation[1] = M_PI_2; gripperOrientation[2] = 0.0;
    if(organType == 1) { // Wheels
        gripperOrientation[0] = 0.0; gripperOrientation[1] = M_PI_2; gripperOrientation[2] = 0.0;
    } else if(organType == 3) { // Joints
        gripperOrientation[0] = 0.0; gripperOrientation[1] = M_PI_2; gripperOrientation[2] = 0.0;
    }
    simSetObjectOrientation(gripperHandle, gripperHandle, gripperOrientation);
    if(organType == 1) { // Wheels
        gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = M_PI_2;
    } else if(organType == 2) { // Sensors
        gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = -M_PI_2;
    } else if(organType == 3) { // Joints
        gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = M_PI;
    } if(organType == 4) { // Caster
        gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = M_PI_2;
    }
    simSetObjectOrientation(gripperHandle, gripperHandle, gripperOrientation);

    // Move relative to gripper.
    /// \todo EB: This offset should be somewhere else or constant.
    if(organType == 1) { // Wheels
        gripperPosition[0] = -0.01; gripperPosition[1] = -0.03;  gripperPosition[2] = -0.229;
    }
    else if(organType == 2) { // Sensors
        gripperPosition[0] = 0.015; gripperPosition[1] = 0.025;  gripperPosition[2] = -0.24;
    }
    else if(organType == 3) { // Joints
        gripperPosition[0] = -0.14; gripperPosition[1] = 0.0; gripperPosition[2] = -0.19;
    }
    else if(organType == 4){ // Caster
        gripperPosition[0] = -0.035; gripperPosition[1] = -0.035;  gripperPosition[2] = -0.17;
    }
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

void Organ::isOrganInsideMainSkeleton(const PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    // Transform organPos from m to voxels
    int xPos = static_cast<int>(std::round(organPos[0]/mc::voxel_real_size));
    int yPos = static_cast<int>(std::round(organPos[1]/mc::voxel_real_size));
    int zPos = static_cast<int>(std::round(organPos[2]/mc::voxel_real_size));
    zPos -= mc::matrix_size/2.;
    uint8_t voxelValue;
    voxelValue = skeletonMatrix.getVoxel(xPos,yPos,zPos);
    if(voxelValue == mc::filled_voxel) {// Organ centre point inside of skeleton
        organInsideSkeleton = true;
        return;
    }
    else if(voxelValue == mc::empty_voxel) {
        /// \todo EB: This temporary fixes the issue of the joint colliding with the head organ!
        if(xPos <= mc::xHeadUpperLimit && xPos >= mc::xHeadLowerLimit &&
           yPos <= mc::yHeadUpperLimit && yPos >= mc::yHeadLowerLimit) {
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
