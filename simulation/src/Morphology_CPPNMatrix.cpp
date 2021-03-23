#include "simulatedER/Morphology_CPPNMatrix.h"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include <stdio.h>
#include "misc/coppelia_communication.hpp"

#define ISCLUSTER 0
#define ISROBOTSTATIC 0

using namespace are::sim;

void Morphology_CPPNMatrix::create()
{
    int meshHandle = -1;
    std::vector<std::vector<std::vector<int>>> skeletonSurfaceCoord;
    mainHandle = -1;
    gripperHandle = -1;
    createGripper();
    //simSetBooleanParameter(sim_boolparam_display_enabled, false); // To turn off display
    numSkeletonVoxels = 0;
    createHead();
    PolyVox::RawVolume<AREVoxel> areMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    PolyVox::RawVolume<uint8_t > skeletonMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    // Decoding CPPN
    GenomeDecoder genomeDecoder;
    genomeDecoder.genomeDecoder(nn,areMatrix,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);

    // Create mesh for skeleton
    auto mesh = PolyVox::extractMarchingCubesMesh(&skeletonMatrix, skeletonMatrix.getEnclosingRegion());
    auto decodedMesh = PolyVox::decodeMesh(mesh);
    std::vector<std::vector<simFloat>> listVertices;
    std::vector<std::vector<simInt>> listIndices;
    listVertices.emplace_back();
    listIndices.emplace_back();
    listVertices.back().reserve(decodedMesh.getNoOfVertices());
    listIndices.back().reserve(decodedMesh.getNoOfIndices());
    bool indVerResult = false;
    indVerResult = getIndicesVertices(decodedMesh,listVertices.back(),listIndices.back());
    bool convexDecompositionSuccess = false;
    // Import mesh to V-REP
    if (indVerResult) {
        generateOrgans(nn,skeletonSurfaceCoord);
        meshHandle = simCreateMeshShape(2, 20.0f * 3.1415f / 180.0f, listVertices.at(0).data(), listVertices.at(0).size(), listIndices.at(0).data(),
                                        listIndices.at(0).size(), nullptr);
        if (meshHandle == -1) {
            std::cerr << "Importing mesh NOT succesful! " << __func__  << std::endl;
        }
        std::ostringstream name;
        name.str("VoxelBone");
        name << id;
        simSetObjectName(meshHandle, name.str().c_str());
        /// \todo EB: Since the bounding box for each shape changes the origin changes as well Therefore, an offset is needed.
        simFloat currentObjectPosition[3];
        simGetObjectPosition(meshHandle,-1,currentObjectPosition);
        simFloat nextObjectPosition[3];
        nextObjectPosition[0] = currentObjectPosition[0] - 0.11879;
        nextObjectPosition[1] = currentObjectPosition[1] - 0.11879;
        nextObjectPosition[2] = currentObjectPosition[2];
        simSetObjectPosition(meshHandle, -1, nextObjectPosition);

        simSetObjectSpecialProperty(meshHandle,0); // Non-collidable, non-detectable, etc.

        // Convex decompositon
        try {
            // Convex decomposition with HACD
            // EB: V-HACD is not a good idea. It crashes randomly. This is an issue with the library itself.
            // http://forum.coppeliarobotics.com/viewtopic.php?f=5&t=8024
            // Convex decomposition parameters
            // EB: Warning, the more triangles are used the more accurate would me the final representation. However,
            // This make the decomposition process slower and more important the likelihood of the decomposition to
            // crash higher. To prevent this I decided to decrease the maximum concavity as mush as possible.
            // EB: IMPORTANT! for the pre-morphogensis stage keep the number of triangles low (100) and high concavity (100).
            // This will speed-up evolution. However...
            // For the morphognesis stage keep the number of triangles high (at least 1200) and low concavity (0.5)
            // This will make a more accurate representation of the skeleton.
            int conDecIntPams[10] = {1, 100, 20, 1, 0, //HACD
                                              10000, 20, 4, 4, 64}; //V-HACD
            float conDecFloatPams[10] = {100, 30, 0.25, 0.0, 0.0,//HACD
                                                  0.0025, 0.05, 0.05, 0.00125, 0.0001};//V-HACD

            int convexHandle;
            convexHandle = simConvexDecompose(meshHandle, 8u | 16u, conDecIntPams, conDecFloatPams);
            mainHandle = convexHandle;
            // Create brain primitive
            float brainSize[3] = {0.084,0.084,0.11};
            int brainHandle;
            brainHandle = simCreatePureShape(0,0,brainSize,0.250,nullptr);
            float brainPos[3] = {0.0,0.0,0.06};
            simSetObjectPosition(brainHandle,-1,brainPos);
            // Group primitives
            int groupHandles[2] = {convexHandle, brainHandle};
            mainHandle = simGroupShapes(groupHandles, 2);
            skeletonHandles.push_back(mainHandle);

            // Set parenthood
            simSetObjectParent(meshHandle,mainHandle, 1);
            simSetObjectSpecialProperty(mainHandle, sim_objectspecialproperty_collidable | sim_objectspecialproperty_measurable |
            sim_objectspecialproperty_detectable_all | sim_objectspecialproperty_renderable); // Detectable, collidable, etc.
#ifndef ISROBOTSTATIC
            std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
            simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
            simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
            simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_respondable, 1);
            //simSetModelProperty(mainHandle,sim_modelproperty_not_visible);
            simSetObjectInt32Parameter(mainHandle,sim_objintparam_visibility_layer, 0); // This hides convex decomposition.
            convexDecompositionSuccess = true;
        } catch (std::exception &e) {
            //std::clog << "Decomposition failed: why? " << e.what() << __func__ << std::endl;
            convexDecompositionSuccess = false;
        }

        if(convexDecompositionSuccess){
            // Recompute mass and inertia to fix object vibration
            // skeleton of PLA is 1.210–1.430 g·cm−3 cit. Wikipedia
            //simComputeMassAndInertia(mainHandle, 1.3f);

            simComputeMassAndInertia(mainHandle, 20.0f);

            // Create organs
            for(auto & i : organList){
                setOrganOrientation(nn, i); // Along z-axis relative to the organ itself
                i.createOrgan(mainHandle);
                if(i.getOrganType() != 0){
                    i.testOrgan(skeletonMatrix, gripperHandle, skeletonHandles, organList);
                    i.repressOrgan();
                }
                /// Cap the number of organs.
                short int goodOrganCounter = 0;
                for(auto & j : organList){
                    if(!j.isOrganRemoved() && j.isOrganChecked() && j.getOrganType() != 4)
                        goodOrganCounter++;

                }
                if(goodOrganCounter > 8) /// \todo EB: Move this constant elsewhere!
                    break;

            }
            testRobot(skeletonMatrix);
        }
        else{
            // Stop generating body plan if convex decomposition fails
            std::cerr << "Not generating robot because convex decomposition failed. Stopping simulation." << std::endl;
        }
    } else {
        // Don't generate mesh if there are no points.
        std::cerr << "Not generating robot because volume is empty.  Stopping simulation." << std::endl;
    }
    // If the robot has no shape, let's fail everything!
    if(!indVerResult || !convexDecompositionSuccess){
        robotManRes.isGripperAccess = false;
        robotManRes.noBadOrientations = false;
        robotManRes.noCollisions = false;
    }
    // Segmented robots
    if((indVerResult || convexDecompositionSuccess) && settings::getParameter<settings::Boolean>(parameters,"#isSegmentedRobot").value){
        // Since the list of organ is going to increase, it's better to cap it to prevent accessing elements out of range.
        int originalSize = organList.size();
        for(int i = 0; i < originalSize; i++){
            if (organList.at(i).getOrganType() == 3 && !organList.at(i).isOrganRemoved() && organList.at(i).isOrganChecked()) {

                int childSkeletonHandle;
                float sizeCuboid[3] = {0.025, 0.025, 0.025};
                childSkeletonHandle = simCreatePureShape(0, 0, sizeCuboid, 250, nullptr);
                simSetObjectSpecialProperty(childSkeletonHandle, sim_objectspecialproperty_collidable | sim_objectspecialproperty_measurable |
                                                                  sim_objectspecialproperty_detectable_all | sim_objectspecialproperty_renderable); // Detectable, collidable, etc.
#ifndef ISROBOTSTATIC
                std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
                simSetObjectInt32Parameter(childSkeletonHandle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
                simSetObjectInt32Parameter(childSkeletonHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
                simSetObjectInt32Parameter(childSkeletonHandle, sim_shapeintparam_respondable, 1);
                simSetObjectInt32Parameter(childSkeletonHandle,sim_objintparam_visibility_layer, 1); // This hides convex decomposition.
                simComputeMassAndInertia(childSkeletonHandle, 0.5f);

                // Allign everything
                /// \todo EB: is this step really needed?
                float position[3] = {0.0,0.0,0.0};
                float orientation[3] = {0.0,0.0,0.0};
                simSetObjectPosition(childSkeletonHandle, -1, position);
                simSetObjectOrientation(childSkeletonHandle, -1, orientation);

                // Move segment relatively to the joint.
                position[2] = -0.045;
                simSetObjectPosition(childSkeletonHandle, organList.at(i).objectHandles[1], position);
                simSetObjectOrientation(childSkeletonHandle,organList.at(i).objectHandles[1], orientation);
                int handles[2] = {childSkeletonHandle,organList.at(i).objectHandles[1]};

                // Get final position of the cube and use it as reference when creating organ.
                float tempPosition[3];
                simGetObjectPosition(childSkeletonHandle, -1, tempPosition);

                int childSkeletonVisualHandle;
                childSkeletonVisualHandle = simCreatePureShape(0, 0, sizeCuboid, 250, nullptr);
                float color[3] = {0,1,0};
                simSetShapeColor(childSkeletonVisualHandle, nullptr, sim_colorcomponent_ambient_diffuse, color);

                // Make cube invisible to physics engine
                /// \todo: EB: Do we need to set all the flags? It seems only the last one and first one is needed.
                simSetObjectSpecialProperty(childSkeletonVisualHandle,0); // Non-collidable, non-detectable, etc.
                simSetObjectProperty(childSkeletonVisualHandle,sim_modelproperty_not_dynamic);
                simSetObjectProperty(childSkeletonVisualHandle,sim_modelproperty_not_respondable);
                simSetObjectInt32Parameter(childSkeletonVisualHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes

                simSetObjectPosition(childSkeletonVisualHandle, organList.at(i).objectHandles.at(1), position);
                simSetObjectOrientation(childSkeletonVisualHandle,organList.at(i).objectHandles.at(1), orientation);

                organList.at(i).objectHandles.at(1) = simGroupShapes(handles,2);
                skeletonHandles.push_back(organList.at(i).objectHandles.at(1));
                simSetObjectParent(childSkeletonVisualHandle,organList.at(i).objectHandles.at(1),1);

                simGetObjectOrientation(organList.at(i).objectHandles.at(1), organList.at(i).getOrganHandle(), orientation);

                // Generate organs
                const auto CHILDORGANNUMBER = 4;
                int organCoord[CHILDORGANNUMBER][3] {{0,0,2},{0,-1,0},{-1,0,0},{2,0,0}};

                // SimGroupShapes changes the orientation of the shape this verifies that the organ generation is consistent.
                if(abs(orientation[0]) < 0.01)
                    organCoord[0][2] = -1;

                std::vector<int> organTypeList(CHILDORGANNUMBER,-1);
                // Check each side of the child skeleton
                for(int j = 0; j < CHILDORGANNUMBER; j++){
                    if(areMatrix.getVoxel(organCoord[j][0],organCoord[j][1],organCoord[j][2]).wheel == FILLEDVOXEL){
                        organTypeList.at(j) = 1;
                        break;
                    } else if(areMatrix.getVoxel(organCoord[j][0],organCoord[j][1],organCoord[j][2]).sensor == FILLEDVOXEL){
                        organTypeList.at(j) = 2;
                        break;
                    } else if(areMatrix.getVoxel(organCoord[j][0],organCoord[j][1],organCoord[j][2]).joint == FILLEDVOXEL){
                        organTypeList.at(j) = 3;
                        break;
                    }  else if(areMatrix.getVoxel(organCoord[j][0],organCoord[j][1],organCoord[j][2]).caster == FILLEDVOXEL){
                        organTypeList.at(j) = 4;
                        break;
                    }
                }

                for(int j = 0; j < CHILDORGANNUMBER; j++){
                    if(organTypeList.at(j) != -1){
                        std::vector<float> tempPosVector(3);
                        tempPosVector.at(0) = tempPosition[0];
                        tempPosVector.at(1) = tempPosition[1];
                        tempPosVector.at(2) = tempPosition[2];
                        std::vector<float> tempOriVector(3);
                        generateOrientations(organCoord[j][0], organCoord[j][1], organCoord[j][2], tempOriVector);
                        Organ organ(organTypeList.at(j), tempPosVector, tempOriVector, parameters);

                        setOrganOrientation(nn, organ); // Along z-axis relative to the organ itself
                        // Create dummy just to get orientation
                        /// \todo EB: This is not the best way to do it. Find something else!
                        int tempDummy = simCreateDummy(0.01, nullptr);
                        float tempOri[3] = {0,0,0};
                        simSetObjectOrientation(tempDummy,organList.at(i).objectHandles[1],tempOri);
                        simGetObjectOrientation(tempDummy,-1,tempOri);
                        organ.organOri[0] += tempOri[0];
                        organ.organOri[1] += tempOri[1];
                        organ.organOri[2] += tempOri[2];
                        simRemoveObject(tempDummy);
                        organ.createOrgan(organList.at(i).objectHandles[1]);
                        organ.testOrgan(skeletonMatrix, gripperHandle, skeletonHandles, organList);
                        organ.organGoodOrientation = true; // Ignore orientation as these organs always have good orientations.
                        organ.repressOrgan();
                        organList.push_back(organ);
                    }
                }
            }
        }
    }
    // Export model
    if(settings::getParameter<settings::Boolean>(parameters,"#isExportModel").value){
        int loadInd = 0; /// \todo EB: We might need to remove this or change it!
        exportMesh(loadInd, listVertices.at(0),listIndices.at(0));
        exportRobotModel(loadInd);
    }
    // Get info from body plan for body plan descriptors or logging.
    if(indVerResult || convexDecompositionSuccess){
        for(auto & i : organList) {
            if(!i.isOrganChecked()) // Stop when the organs not checked or generated start.
                break;
            int voxelPosX = static_cast<int>(std::round(i.connectorPos[0]/VOXEL_REAL_SIZE));
            int voxelPosY = static_cast<int>(std::round(i.connectorPos[1]/VOXEL_REAL_SIZE));
            int voxelPosZ = static_cast<int>(std::round(i.connectorPos[2]/VOXEL_REAL_SIZE));
            int matPosX, matPosY, matPosZ;
            matPosX = voxelPosX + MATRIX_HALF_SIZE;
            matPosY = voxelPosY + MATRIX_HALF_SIZE;
            matPosZ = voxelPosZ + MATRIX_HALF_SIZE;

            if(matPosX > MATRIX_SIZE)
                matPosX = MATRIX_SIZE-1;
            if(matPosX < 0)
                matPosX = 0;
            if(matPosY > MATRIX_SIZE)
                matPosY = MATRIX_SIZE-1;
            if(matPosY < 0)
                matPosY = 0;
            if(matPosZ > MATRIX_SIZE)
                matPosZ = MATRIX_SIZE-1;
            if(matPosZ < 0)
                matPosZ = 0;

        }
        indDesc.cartDesc.getRobotDimmensions(organList);
        indDesc.cartDesc.voxelNumber = numSkeletonVoxels;
        indDesc.cartDesc.countOrgans(organList);
        indDesc.cartDesc.setCartDesc();
    }
    destroyGripper();
    retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles);
    // EB: This flag tells the simulator that the shape is convex even though it might not be. Be careful,
    // this might mess up with the physics engine if the shape is non-convex!
    // I set this flag to prevent the warning showing and stopping evolution.
    simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_convex, 1);
    if(settings::getParameter<settings::Boolean>(parameters,"#isScreenshotEnable").value) {
        robotScreenshot();
    }
}

void Morphology_CPPNMatrix::createAtPosition(float x, float y, float z)
{
    create();
    setPosition(x,y,z);
}

void Morphology_CPPNMatrix::setPosition(float x, float y, float z)
{
    float robotPos[3];
    robotPos[0] = x;
    robotPos[1] = y;
    robotPos[2] = z;

    simSetObjectPosition(mainHandle, -1, robotPos);
}

bool Morphology_CPPNMatrix::getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<uint8_t>> &decodedMesh,
                                         std::vector<simFloat> &vertices, std::vector<simInt> &indices)
{
    const unsigned int n_vertices = decodedMesh.getNoOfVertices();
    const unsigned int n_indices = decodedMesh.getNoOfIndices();

    if (n_vertices <= 0) {
        return false;
    }

    const PolyVox::Vector3DFloat *prev = nullptr;
    bool pointObject = true;
    for (unsigned int i=0; i < n_vertices; i++) {
        const PolyVox::Vector3DFloat &pos = decodedMesh.getVertex(i).position;
        if (pointObject) {
            if (prev != nullptr and (*prev) != pos) pointObject = false;
            prev = &pos;
        }
        vertices.emplace_back(pos.getX() * SHAPE_SCALE_VALUE);
        vertices.emplace_back(pos.getY() * SHAPE_SCALE_VALUE);
        vertices.emplace_back(pos.getZ() * SHAPE_SCALE_VALUE);
    }

    // If all vectors are the same, we have an object the size of point. This is considered a failed viability.
    // and it makes the vertex decomposition to crash badly.
    if (pointObject) {
        vertices.clear();
        indices.clear();
        return false;
    }

    for (unsigned int i=0; i < n_indices; i++) {
        indices.emplace_back(decodedMesh.getIndex(i));
    }

    return true;
}

void Morphology_CPPNMatrix::createHead()
{
    std::vector<float> tempPosVector(3);
    tempPosVector.at(0) = 0.0;
    tempPosVector.at(1) = 0.0;
    tempPosVector.at(2) = 0.06;
    std::vector<float> tempOriVector(3, 0.0);
    Organ organ(0, tempPosVector, tempOriVector, parameters);
    organList.push_back(organ);
}

void Morphology_CPPNMatrix::createGripper()
{
    float gripperPosition[3];
    float gripperOrientation[3];
    std::string modelsPath = settings::getParameter<settings::String>(parameters,"#organsPath").value;
    modelsPath += "C_Gripper.ttm";
    gripperHandle = simLoadModel(modelsPath.c_str());
    assert(gripperHandle != -1);

    gripperPosition[0] = 1.0; gripperPosition[1] = 1.0; gripperPosition[2] = 1.0;
    gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = 0.0;

    simSetObjectPosition(gripperHandle, -1, gripperPosition);
    simSetObjectOrientation(gripperHandle, -1, gripperOrientation);

#ifndef ISROBOTSTATIC
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
    simSetObjectInt32Parameter(gripperHandle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
    simSetObjectInt32Parameter(gripperHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
}

void Morphology_CPPNMatrix::setOrganOrientation(NEAT::NeuralNetwork &cppn, Organ &organ)
{
    // Vector used as input of the Neural Network (NN).
    std::vector<double> input{0,0,0};
    input[0] = static_cast<int>(std::round(organ.organPos[0]/VOXEL_REAL_SIZE));
    input[1] = static_cast<int>(std::round(organ.organPos[1]/VOXEL_REAL_SIZE));
    input[2] = static_cast<int>(std::round(organ.organPos[2]/VOXEL_REAL_SIZE));
    input[2] -= MATRIX_HALF_SIZE;
    // Set inputs to NN
    cppn.Input(input);
    // Activate NN
    cppn.Activate();
    float rotZ;
    rotZ = cppn.Output()[0] * M_2_PI - M_1_PI;
    organ.organOri.push_back(rotZ);
}

void Morphology_CPPNMatrix::exportMesh(int loadInd, std::vector<float> vertices, std::vector<int> indices)
{
#ifndef ISCLUSTER
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISCLUSTER == 0
    const auto **verticesMesh = new const simFloat *[2];
    const auto **indicesMesh = new const simInt *[2];
#elif ISCLUSTER == 1
    auto **verticesMesh = new simFloat *[2];
    auto **indicesMesh = new simInt *[2];
#endif
    auto *verticesSizesMesh = new simInt[2];
    auto *indicesSizesMesh = new simInt[2];
    verticesMesh[0] = vertices.data();
    verticesSizesMesh[0] = vertices.size();
    indicesMesh[0] = indices.data();
    indicesSizesMesh[0] = indices.size();

    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;

    std::stringstream filepath;
    filepath << loadExperiment << "mesh" << loadInd << ".stl";

    //fileformat: the fileformat to export to:
    //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
    simExportMesh(3, filepath.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

    delete[] verticesMesh;
    delete[] verticesSizesMesh;
    delete[] indicesMesh;
    delete[] indicesSizesMesh;
}

void Morphology_CPPNMatrix::generateOrgans(NEAT::NeuralNetwork &cppn, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord)
{
    float tempPos[3];
    std::vector<double> input{0,0,0,0}; // Vector used as input of the Neural Network (NN).
    int organType;
    for(int m = 0; m < skeletonSurfaceCoord.size(); m++) {
        // Generate organs every two voxels.
        for (int n = 0; n < skeletonSurfaceCoord[m].size(); n+=5) { /// \todo EB: Define this constant elsewhere!
            input[0] = static_cast<double>(skeletonSurfaceCoord[m][n].at(0));
            input[1] = static_cast<double>(skeletonSurfaceCoord[m][n].at(1));
            input[2] = static_cast<double>(skeletonSurfaceCoord[m][n].at(2));
            input[3] = static_cast<double>(sqrt(pow(skeletonSurfaceCoord[m][n].at(0),2)+pow(skeletonSurfaceCoord[m][n].at(1),2)+pow(skeletonSurfaceCoord[m][n].at(2),2)));
            // Set inputs to NN
            cppn.Input(input);
            // Activate NN
            cppn.Activate();
            // Is there an organ?
            organType = -1;
            if(cppn.Output()[2] > 0.99){ // Wheel
                // These if statements should be always true but they are here for debugging.
                if(settings::getParameter<settings::Boolean>(parameters,"#isWheel").value) // For debugging only
                    organType = 1;
            }
            else if(cppn.Output()[3] > 0.99) { // Sensor
                if(settings::getParameter<settings::Boolean>(parameters,"#isSensor").value) // For debugging only
                    organType = 2;
            }
            else if(cppn.Output()[4] > 0.99) { // Joint
                if(settings::getParameter<settings::Boolean>(parameters,"#isJoint").value) // For debugging only
                    organType = 3;
            }
            else if(cppn.Output()[5] > 0.99) { // Caster
                if(settings::getParameter<settings::Boolean>(parameters,"#isCaster").value) // For debugging only
                    organType = 4;
            }
            // Create organ if any
            if(organType > 0){
                std::vector<float> tempPosVector(3);
                tempPosVector.at(0) = static_cast<float>(input[0] * VOXEL_REAL_SIZE);
                tempPosVector.at(1) = static_cast<float>(input[1] * VOXEL_REAL_SIZE);
                tempPosVector.at(2) = static_cast<float>(input[2] * VOXEL_REAL_SIZE);
                tempPosVector.at(2) += MATRIX_HALF_SIZE * VOXEL_REAL_SIZE;
                std::vector<float> tempOriVector(3);
                generateOrientations(skeletonSurfaceCoord[m][n].at(3), skeletonSurfaceCoord[m][n].at(4), skeletonSurfaceCoord[m][n].at(5), tempOriVector);
                Organ organ(organType, tempPosVector, tempOriVector, parameters);
                organList.push_back(organ);
            }
        }
    }
}

void Morphology_CPPNMatrix::exportRobotModel(int indNum)
{
    simSetObjectProperty(mainHandle,sim_objectproperty_selectmodelbaseinstead);
    std::string repository = settings::getParameter<settings::String>(parameters, "#repository").value;
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;

    std::stringstream filepath;
    filepath << loadExperiment << "/model" << indNum << ".ttm";

    int p = simGetModelProperty(mainHandle);
    p = (p|sim_modelproperty_not_model)-sim_modelproperty_not_model;
    simSetModelProperty(mainHandle,p);

    int result;
    result = simSaveModel(mainHandle,filepath.str().c_str());
    if(result == -1){
        std::cerr << "Something went wrong when exporting robot model! " << std::endl;
    }
}

void Morphology_CPPNMatrix::robotScreenshot()
{
    std::string modelsPath = are::settings::getParameter<are::settings::String>(parameters,"#organsPath").value;
    modelsPath += "screenshotSensor.ttm";
    int screenShotSensor;
    int generalCamera;
    screenShotSensor = simLoadModel(modelsPath.c_str());
    generalCamera = simGetObjectHandle("Camera0");
    simSetObjectParent(screenShotSensor, generalCamera, 1);
    screenShotSensor = simLoadModel(modelsPath.c_str());
    generalCamera = simGetObjectHandle("Camera1");
    simSetObjectParent(screenShotSensor, generalCamera, 1);
    screenShotSensor = simLoadModel(modelsPath.c_str());
    generalCamera = simGetObjectHandle("Camera2");
    simSetObjectParent(screenShotSensor, generalCamera, 1);
    screenShotSensor = simLoadModel(modelsPath.c_str());
    generalCamera = simGetObjectHandle("Camera3");
    simSetObjectParent(screenShotSensor, generalCamera, 1);
}

void Morphology_CPPNMatrix::testRobot(PolyVox::RawVolume<uint8_t>& skeletonMatrix)
{
    // Manufacturability tests for organs.
    for(auto & i : organList) {
        /// \todo EB: Ignore the brain. We might need to change this!
        if(i.getOrganType() != 0){
            if (i.organColliding || i.organInsideSkeleton)
                robotManRes.noCollisions = false;
            if (!i.organGoodOrientation)
                robotManRes.noBadOrientations = false;
            // is gripper colliding?
            if (!i.organGripperAccess)
                robotManRes.isGripperAccess = false;
        }
    }
}

void Morphology_CPPNMatrix::destroyGripper()
{
    simRemoveModel(gripperHandle);
}

void Morphology_CPPNMatrix::generateOrientations(int x, int y, int z, std::vector<float> &orientation)
{
    if ((x < 0) && (y < 0) && (z > 0)){
        orientation.at(0) = -2.5261;
        orientation.at(1) = +0.5235;
        orientation.at(2) = +2.1861;
    }
    else if ((x < 0) && (y == 0) && (z > 0)){
        orientation.at(0) = +3.1415;
        orientation.at(1) = +0.7853;
        orientation.at(2) = -3.1415;
    }
    else if ((x < 0) && (y > 0) && (z > 0)){
        orientation.at(0) = +2.5261;
        orientation.at(1) = +0.6981;
        orientation.at(2) = -2.1862;
    }
    else if ((x == 0) && (y < 0) && (z > 0)){
        orientation.at(0) = -2.3561;
        orientation.at(1) = +0.5960;
        orientation.at(2) = +1.5707;
    }
    else if ((x == 0) && (y == 0) && (z > 0)){
        orientation.at(0) = -3.1415;
        orientation.at(1) = +0.000;
        orientation.at(2) = +0.0000;
    }
    else if ((x == 0) && (y > 0) && (z > 0)){
        orientation.at(0) = +2.3561;
        orientation.at(1) = +0.0000;
        orientation.at(2) = -1.5708;
    }
    else if ((x > 0) && (y < 0) && (z > 0)){
        orientation.at(0) = -2.5261;
        orientation.at(1) = -0.5236;
        orientation.at(2) = +0.9552;
    }
    else if ((x > 0) && (y == 0) && (z > 0)){
        orientation.at(0) = -3.1415;
        orientation.at(1) = -0.7854;
        orientation.at(2) = +0.0000;
    }
    else if ((x > 0) && (y > 0) && (z > 0)){
        orientation.at(0) = +2.5132;
        orientation.at(1) = -0.5235;
        orientation.at(2) = -0.9552;
    }
    else if ((x < 0) && (y < 0) && (z == 0)){
        orientation.at(0) = -1.5708;
        orientation.at(1) = +0.7853;
        orientation.at(2) = +1.5707;
    }
    else if ((x < 0) && (y == 0) && (z == 0)){
        orientation.at(0) = +0.0000;
        orientation.at(1) = +1.5707;
        orientation.at(2) = +0.0000;
    }
    else if ((x < 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +1.5707;
        orientation.at(1) = +0.7853;
        orientation.at(2) = -1.5708;
    }
    else if ((x == 0) && (y < 0) && (z == 0)){
        orientation.at(0) = -1.5708;
        orientation.at(1) = +0.0000;
        orientation.at(2) = +1.5708;
    }
    else if ((x == 0) && (y > 0) && (z == 0)){
        orientation.at(0) = +1.5707;
        orientation.at(1) = +0.0000;
        orientation.at(2) = -1.5708;
    }
    else if ((x > 0) && (y < 0) && (z == 0)) {
        orientation.at(0) = +1.5707;
        orientation.at(1) = -0.7854;
        orientation.at(2) = +1.5708;
    }
    else if ((x > 0) && (y == 0) && (z == 0)) {
        orientation.at(0) = +0.0000;
        orientation.at(1) = -1.5708;
        orientation.at(2) = +3.1415;
    }
    else if ((x > 0) && (y > 0) && (z == 0)) {
        orientation.at(0) = +1.5708;
        orientation.at(1) = -0.7854;
        orientation.at(2) = -1.5708;
    }
    else if ((x < 0) && (y < 0) && (z < 0)) {
        orientation.at(0) = -0.6154;
        orientation.at(1) = +0.5235;
        orientation.at(2) = +0.9552;
    }
    else if ((x < 0) && (y == 0) && (z < 0)) {
        orientation.at(0) = +0.0000;
        orientation.at(1) = +0.78563;
        orientation.at(2) = +0.0000;
    }
    else if ((x < 0) && (y > 0) && (z < 0)){
        orientation.at(0) = +0.6154;
        orientation.at(1) = +0.5235;
        orientation.at(2) = -0.9552;
    }
    else if ((x == 0) && (y < 0) && (z < 0)){
        orientation.at(0) = -0.7851;
        orientation.at(1) = +0.5235;
        orientation.at(2) = +1.5708;
    }
    else  if ((x == 0) && (y == 0) && (z < 0)){
        orientation.at(0) = +0.0000;
        orientation.at(1) = +0.0000;
        orientation.at(2) = +0.0000;
    }
    else if ((x == 0) && (y > 0) && (z < 0)){
        orientation.at(0) = +0.7853;
        orientation.at(1) = +0.0000;
        orientation.at(2) = -1.5708;
    }
    else if ((x > 0) && (y < 0) && (z < 0)){
        orientation.at(0) = -0.6154;
        orientation.at(1) = -0.5236;
        orientation.at(2) = +2.1861;
    }
    else if ((x > 0) && (y == 0) && (z < 0)){
        orientation.at(0) = +0.0000;
        orientation.at(1) = -0.7854;
        orientation.at(2) = -3.1415;
    }
    else if ((x > 0) && (y > 0) && (z < 0)) {
        orientation.at(0) = +0.6154;
        orientation.at(1) = -0.5236;
        orientation.at(2) = -2.1862;
    }
    else {
        orientation.at(0) = +0.6154;
        orientation.at(1) = -0.5236;
        orientation.at(2) = -2.1862;
        std::cerr << "We shouldn't be here: " << __func__ << " " << x << " "
                  << y << " " << z << std::endl;
    }
}
