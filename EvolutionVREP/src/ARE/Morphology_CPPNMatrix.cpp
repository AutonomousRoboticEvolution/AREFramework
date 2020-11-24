#include "ARE/Morphology_CPPNMatrix.h"
#include "v_repLib.h"
#include <stdio.h>
#include "misc/coppelia_communication.hpp"

#define HANDMADEROBOT 0
#define ISCLUSTER 0
#define ISROBOTSTATIC 1

using namespace are;
namespace cop = coppelia;

void Morphology_CPPNMatrix::create()
{
    mainHandle = -1;
    int meshHandle = -1;

    //simSetBooleanParameter(sim_boolparam_display_enabled, false); // To turn off display
    manScore = 0;
    numSkeletonVoxels = 0;
    createHead();
    PolyVox::RawVolume<AREVoxel> areMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    PolyVox::RawVolume<uint8_t > skeletonMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    // Decoding CPPN
#ifndef HANDMADEROBOT
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif HANDMADEROBOT == 0
    genomeDecoder(areMatrix, nn);
#endif
    generateSkeleton(areMatrix, skeletonMatrix, VoxelType::BONE); // Generate skeleton without modifications
    createSkeletonBase(skeletonMatrix, true);
    emptySpaceForHead(skeletonMatrix);
    skeletonRegionCounter(skeletonMatrix);
    removeSkeletonRegions(skeletonMatrix);
#ifndef HANDMADEROBOT
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif HANDMADEROBOT == 1
    createAREPuck(skeletonMatrix);
#elif HANDMADEROBOT == 2
    createAREPotato(skeletonMatrix);
#elif HANDMADEROBOT == 3
    createARETricyle(skeletonMatrix);
#endif

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
        
        findSkeletonSurface(skeletonMatrix);
        generateOrgans(nn);

        meshHandle = simCreateMeshShape(2, 20.0f * 3.1415f / 180.0f, listVertices.at(0).data(), listVertices.at(0).size(), listIndices.at(0).data(),
                                        listIndices.at(0).size(), nullptr);
        if (meshHandle == -1) {
            std::cerr << "Importing mesh NOT succesful! " << __func__  << std::endl;
        }
        usleep(1000);
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
            brainHandle = simCreatePureShape(0,0,brainSize,0.250,NULL);
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
            for(auto & i : _organSpec){
#ifndef HANDMADEROBOT
                std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif HANDMADEROBOT == 0
                setOrganOrientation(nn, i); // Along z-axis relative to the organ itself
#endif
                createOrgan(i, mainHandle);
                i.organChecked = true;
                // If organ is not brain
                if(i.organType != 0){
                    createMaleConnector(i, mainHandle);
                    testComponents(skeletonMatrix,i);
                    geneRepression(i);
                }
                /// Cap the number of organs.
                short int goodOrganCounter = 0;
                for(auto & i : _organSpec){
                    if(!i.organRemoved && i.organChecked && i.organType != 4)
                        goodOrganCounter++;

                }
                if(goodOrganCounter > 8) /// \todo EB: Move this constant elsewhere!
                    break;

            }

            testRobot(skeletonMatrix);
            manufacturabilityScore();
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
    if(settings::getParameter<settings::Boolean>(parameters,"#isExportModel").value){
        int loadInd = 0; /// \todo EB: We might need to remove this or change it!
        exportMesh(loadInd, listVertices.at(0),listIndices.at(0));
        exportRobotModel(loadInd);
    }
    // Segmented robots
    if((indVerResult || convexDecompositionSuccess) && settings::getParameter<settings::Boolean>(parameters,"#isSegmentedRobot").value){
        // Since the list of organ is going to increase, it's better to cap it to prevent accessing elements out of range.
        int originalSize = _organSpec.size();
        for(int i = 0; i < originalSize; i++){
        //for(auto & i : _organSpec){
            if (_organSpec.at(i).organType == 3 && !_organSpec.at(i).organRemoved && _organSpec.at(i).organChecked) {

                int childSkeletonHandle;
                float sizeCuboid[3] = {0.025, 0.025, 0.025};
                childSkeletonHandle = simCreatePureShape(0, 0, sizeCuboid, 250, NULL);
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
                simSetObjectPosition(childSkeletonHandle, _organSpec.at(i).objectHandles[1], position);
                simSetObjectOrientation(childSkeletonHandle,_organSpec.at(i).objectHandles[1], orientation);
                int handles[2] = {childSkeletonHandle,_organSpec.at(i).objectHandles[1]};

                // Get final position of the cube and use it as reference when creating organ.
                float tempPosition[3];
                simGetObjectPosition(childSkeletonHandle, -1, tempPosition);

                int childSkeletonVisualHandle;
                childSkeletonVisualHandle = simCreatePureShape(0, 0, sizeCuboid, 250, NULL);
                float color[3] = {0,1,0};
                simSetShapeColor(childSkeletonVisualHandle,NULL,sim_colorcomponent_ambient_diffuse,color);

                // Make cube invisible to physics engine
                /// \todo: EB: Do we need to set all the flags? It seems only the last one and first one is needed.
                simSetObjectSpecialProperty(childSkeletonVisualHandle,0); // Non-collidable, non-detectable, etc.
                simSetObjectProperty(childSkeletonVisualHandle,sim_modelproperty_not_dynamic);
                simSetObjectProperty(childSkeletonVisualHandle,sim_modelproperty_not_respondable);
                simSetObjectInt32Parameter(childSkeletonVisualHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes

                simSetObjectPosition(childSkeletonVisualHandle, _organSpec.at(i).objectHandles[1], position);
                simSetObjectOrientation(childSkeletonVisualHandle,_organSpec.at(i).objectHandles[1], orientation);

                _organSpec.at(i).objectHandles[1] = simGroupShapes(handles,2);
                simSetObjectParent(childSkeletonVisualHandle,_organSpec.at(i).objectHandles[1],1);

                // Generate organs
                const auto CHILDORGANNUMBER = 4;
                int organCoord[CHILDORGANNUMBER][3] {{0,0,2},{0,-1,0},{-1,0,0},{2,0,0}};
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
                        continue;
                    }
                }
                for(int j = 0; j < CHILDORGANNUMBER; j++){
                    if(organTypeList.at(j) != -1){
                        OrganSpec _organ;
                        _organ.organType = organTypeList.at(j);
                        _organ.organRemoved = false;
                        _organ.organChecked = false;

                        _organ.organPos.push_back(tempPosition[0]);
                        _organ.organPos.push_back(tempPosition[1]);
                        _organ.organPos.push_back(tempPosition[2]);
                        generateOrientations(organCoord[j][0], organCoord[j][1], organCoord[j][2], _organ);
                        setOrganOrientation(nn, _organ); // Along z-axis relative to the organ itself

                        // Create dummy just to get orientation
                        /// \todo EB: This is not the best way to do it. Find something else!
                        int tempDummy = simCreateDummy(0.01, NULL);
                        float tempOri[3] = {0,0,0};
                        simSetObjectOrientation(tempDummy,_organSpec.at(i).objectHandles[1],tempOri);
                        simGetObjectOrientation(tempDummy,-1,tempOri);
                        _organ.organOri[0] += tempOri[0];
                        _organ.organOri[1] += tempOri[1];
                        _organ.organOri[2] += tempOri[2];
                        simRemoveObject(tempDummy);

                        createOrgan(_organ, _organSpec.at(i).objectHandles[1]);
                        _organ.organChecked = true;
                        createMaleConnector(_organ, _organSpec.at(i).objectHandles[1]);
                        testComponents(skeletonMatrix,_organ);
                        _organ.organGoodOrientation = true; // Ignore orientation as these organs always have good orientations.
                        geneRepression(_organ);
                        _organSpec.push_back(_organ);
                    }
                }
            }
        }
    }
    // Get info from body plan for body plan descriptors or logging.
    if(indVerResult || convexDecompositionSuccess){
        // For the graph descriptor
        getFinalSkeletonVoxels(skeletonMatrix);
        for(auto & i : _organSpec) {
            if(!i.organChecked) // Stop when the organs not checked or generated start.
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

            if(i.organType == 1) { // Wheels
                indDesc.matDesc.graphMatrix[matPosX][matPosY][matPosZ] = 2;
                indDesc.symDesc.setVoxelQuadrant(voxelPosX, voxelPosY, voxelPosZ, 1);
            }
            else if(i.organType == 2) { // Sensors
                indDesc.matDesc.graphMatrix[matPosX][matPosY][matPosZ] = 3;
                indDesc.symDesc.setVoxelQuadrant(voxelPosX, voxelPosY, voxelPosZ, 2);
            }
            else if(i.organType == 3) { // Joints
                indDesc.matDesc.graphMatrix[matPosX][matPosY][matPosZ] = 4;
                indDesc.symDesc.setVoxelQuadrant(voxelPosX, voxelPosY, voxelPosZ, 3);
            }
            else if(i.organType == 4) { // Caster
                indDesc.matDesc.graphMatrix[matPosX][matPosY][matPosZ] = 5;
                indDesc.symDesc.setVoxelQuadrant(voxelPosX, voxelPosY, voxelPosZ, 4);
            }
        }
        indDesc.symDesc.setSymDesc();
        //indDesc.cartDesc.getSkeletonDimmensions(skeletonMatrix);
        indDesc.cartDesc.getRobotDimmensions(_organSpec);
        indDesc.cartDesc.voxelNumber = numSkeletonVoxels;
        indDesc.cartDesc.countOrgans(_organSpec);
        indDesc.cartDesc.setCartDesc();
    }
    cop::retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles);
    // EB: This flag tells the simulator that the shape is convex even though it might not be. Be careful,
    // this might mess up with the physics engine if the shape is non-convex!
    // I set this flag to prevent the warning showing and stopping evolution.
    simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_convex, 1);
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


void Morphology_CPPNMatrix::genomeDecoder(PolyVox::RawVolume<AREVoxel>& areMatrix, NEAT::NeuralNetwork &cppn)
{
    std::vector<double> input{0,0,0,0}; // Vector used as input of the Neural Network (NN).
    AREVoxel areVoxel;
    // Generate voxel matrix
    auto region = areMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                input[0] = static_cast<double>(x);
                input[1] = static_cast<double>(y);
                input[2] = static_cast<double>(z);
                input[3] = static_cast<double>(sqrt(pow(x,2)+pow(y,2)+pow(z,2)));
                // Set inputs to NN
                cppn.Input(input);
                // Activate NN
                cppn.Activate();
                // Take output from NN and store it.
                areVoxel.bone = EMPTYVOXEL;
                areVoxel.wheel = EMPTYVOXEL;
                areVoxel.sensor = EMPTYVOXEL;
                areVoxel.joint = EMPTYVOXEL;
                areVoxel.caster = EMPTYVOXEL;

                if(cppn.Output()[1] > 0) {
                    areVoxel.bone = FILLEDVOXEL;
                }
                if(cppn.Output()[2] > 0){
                    areVoxel.wheel = FILLEDVOXEL;
                }
                if(cppn.Output()[3] > 0) {
                    areVoxel.sensor = FILLEDVOXEL;
                }
                if(cppn.Output()[4] > 0) { /// \todo EB WARNING! Verify the order
                    areVoxel.caster = FILLEDVOXEL;
                }
                if(cppn.Output()[5] > 0) { /// \todo EB WARNING! Verify the order
                    areVoxel.joint = FILLEDVOXEL;
                }

                areMatrix.setVoxel(x, y, z, areVoxel);
            }
        }
    }
}

void Morphology_CPPNMatrix::generateSkeleton(PolyVox::RawVolume<AREVoxel> &areMatrix,
        PolyVox::RawVolume<uint8_t> &skeletonMatrix, VoxelType _voxelType)
{
    AREVoxel areVoxel;
    uint8_t uVoxelValue;
    uint8_t voxel;
    auto region = skeletonMatrix.getEnclosingRegion();
    bool isSkeletonConnected = false;

    for(int32_t z = region.getLowerZ()+1; z < region.getLowerZ() + skeletonBaseHeight; z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= xHeadUpperLimit + skeletonBaseThickness && x >= xHeadLowerLimit - skeletonBaseThickness &&
                   y <= yHeadUpperLimit + skeletonBaseThickness && y >= yHeadLowerLimit - skeletonBaseThickness){
                    if(x <= xHeadUpperLimit && x >= xHeadLowerLimit &&
                       y <= yHeadUpperLimit && y >= yHeadLowerLimit){
                        //isSkeletonConnected = false;
                    }
                    else{
                        areVoxel = areMatrix.getVoxel(x, y, z);
                        // If output greater than threshold write voxel.
                        // NOTE: Hard boundaries seem to work better with convex decomposition
                        voxel = areVoxel.bone;

                        if(voxel > 0.5){
                            isSkeletonConnected = true;
                        }
                    }
                }
            }
        }
    }
    if(isSkeletonConnected){
        for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
            for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
                for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                    areVoxel = areMatrix.getVoxel(x, y, z);
                    // If output greater than threshold write voxel.
                    // NOTE: Hard boundaries seem to work better with convex decomposition
                    voxel = areVoxel.bone;

                    if(voxel > 0.5){
                        uVoxelValue = FILLEDVOXEL;
                        numSkeletonVoxels++;
                    }
                    else
                        uVoxelValue = EMPTYVOXEL;

                    skeletonMatrix.setVoxel(x, y, z, uVoxelValue);
                }
            }
        }
    }
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
    OrganSpec _organ;
    _organ.organType = 0; /// \todo EB: This represents wheel. Use enum instead.
    _organ.organPos.push_back(0.0);
    _organ.organPos.push_back(0.0);
    _organ.organPos.push_back(0.06);
    _organ.organOri.push_back(0.0);
    _organ.organOri.push_back(0.0);
    _organ.organOri.push_back(0.0);
    _organSpec.push_back(_organ);
}

void Morphology_CPPNMatrix::createOrgan(Morphology_CPPNMatrix::OrganSpec &organ, int skeletonHandle)
{

    /// \todo EB: It might be worth to have this as a separate parameters (?)
    std::string modelsPath = settings::getParameter<settings::String>(parameters,"#organsPath").value;
    int organHandle = 0;
    if(organ.organType == 0) // Brain
        modelsPath += "C_HeadV3.ttm";
    else if(organ.organType == 1) { // Wheels
#ifndef HANDMADEROBOT
        std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif HANDMADEROBOT == 0
        modelsPath += "C_WheelV2.ttm";
#elif HANDMADEROBOT == 1
        modelsPath += "C_Wheel.ttm";
#elif HANDMADEROBOT == 2
        modelsPath += "C_Wheel.ttm";
#elif HANDMADEROBOT == 3
        modelsPath += "C_Wheel.ttm";
#endif
    }
    else if(organ.organType == 2) // Sensors
        modelsPath += "C_Sensor.ttm";
    else if(organ.organType == 3) // Joints
        modelsPath += "C_Joint.ttm";
    else if(organ.organType == 4) // Caster
        modelsPath+= "C_Caster.ttm";
    else
        assert(false);

    organHandle = simLoadModel(modelsPath.c_str());
    assert(organHandle != -1);
    organ.handle = organHandle;

    /// \todo: EB: Maybe we should move this to a method
    // Get object handles for collision detection
    simAddObjectToSelection(sim_handle_tree, organ.handle);
    int selectionSize = simGetObjectSelectionSize();
    // store all these objects (max 10 shapes)
    int shapesStorage[10]; // stores up to 10 shapes
    simGetObjectSelection(shapesStorage);
    for (int i = 0; i < selectionSize; i++) {
        char* componentName;
        componentName = simGetObjectName(shapesStorage[i]);
        if('P' == componentName[0]){
            organ.objectHandles.push_back(shapesStorage[i]);
        }
        simReleaseBuffer(componentName);
    }
    simRemoveObjectFromSelection(sim_handle_all, organ.handle);


    // For force sensor
    int fsParams[5];
    fsParams[0] = 0; fsParams[1] = 1; fsParams[2] = 1; fsParams[3] = 0; fsParams[4] = 0;
    float fsFParams[5];
    // EB: Calibrating this values is very important. They define when a sensor is broken.
    fsFParams[0] = 0.005; fsFParams[1] = 1000000; fsFParams[2] = 10000000; fsFParams[3] = 0; fsFParams[4] = 0;
    int forceSensor = simCreateForceSensor(3, fsParams, fsFParams, NULL);
    // Set organPos
    float organPos[3];
    organPos[0] = organ.organPos[0]; organPos[1] = organ.organPos[1]; organPos[2] = organ.organPos[2];
    simSetObjectPosition(forceSensor,-1,organPos);
    simSetObjectPosition(organHandle, -1, organPos);

    // Create connector and offset by some distance
    /// \todo EB: This offset shouldn't be here.
    float connectorPos[3];
    connectorPos[0] = organPos[0]; connectorPos[1] = organPos[1]; connectorPos[2] = organPos[2];
    if(organ.organType == 0) // Brain
        connectorPos[2] = organPos[2] + 0.02;
    else if(organ.organType == 1){ // Wheels
#ifndef HANDMADEROBOT
        std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif HANDMADEROBOT == 0
        connectorPos[1] = organPos[1] - 0.015;
        connectorPos[2] = organPos[2] + 0.00;
#elif HANDMADEROBOT == 1
        connectorPos[2] = organPos[2] + 0.02;
#elif HANDMADEROBOT == 2
        connectorPos[2] = organPos[2] + 0.02;
#elif HANDMADEROBOT == 3
        connectorPos[2] = organPos[2] + 0.02;
#endif
    }
    else if(organ.organType == 2) { // Sensors
        connectorPos[0] = organPos[0] + 0.01;
        connectorPos[2] = organPos[2] + 0.02;
    }else if(organ.organType == 3) // Joints
        connectorPos[2] = organPos[2] + 0.035;
    else if(organ.organType == 4)  // Caster
        connectorPos[2] = organPos[2] + 0.02;
    else
        assert(false);

    organ.connectorHandle = simCreateDummy(0.01,NULL);
    simSetObjectParent(organ.connectorHandle,organHandle,1);
    simSetObjectPosition(organ.connectorHandle,-1,connectorPos);

    // Set organOri
    float organOri[3];
    organOri[0] = organ.organOri[0]; organOri[1] = organ.organOri[1]; organOri[2] = organ.organOri[2];
    simSetObjectOrientation(forceSensor, -1, organOri);
    simSetObjectOrientation(organHandle, -1, organOri);
    simSetObjectOrientation(organ.connectorHandle,-1,organOri);
    // If the organ is not brain rotate along z-axis relative to the organ itself.
    if(organ.organType != 0){
        organOri[0] = 0.0; organOri[1] = 0.0; organOri[2] = organ.organOri[3];
        simSetObjectOrientation(forceSensor, forceSensor, organOri);
        simSetObjectOrientation(organHandle, organHandle, organOri);
    }

    /// \todo EB: These two lines work but I don't understand why with the previous method no.
    simGetObjectOrientation(organHandle,-1,organOri);
    simSetObjectOrientation(organ.connectorHandle,-1,organOri);
    // Set parents
    simSetObjectParent(forceSensor,skeletonHandle,1);
    simSetObjectParent(organHandle, forceSensor, 1);
    /// \todo There might be a bug somewhere! The lines commented work well for ARE-Puck and ARE-Potato. The uncommented lines work well with evolved morphologies...
    // This moves the organ slightly away from the surface. This parameters were calibrated through visual inspection
    /// \todo: EB: We might not need this in the future
    organPos[0] = 0.0; organPos[1] = 0.0; organPos[2] = 0.0;

    if(organ.organType == 0) // Brain
        organPos[2] = 0.0;
    else if(organ.organType == 1) // Wheels
        organPos[2] = -0.03;
    else if(organ.organType == 2) { // Sensors
        organPos[0] = -0.01;
        organPos[2] = -0.03;
    }else if(organ.organType == 3) // Joints
        organPos[2] = -0.045;
    else if(organ.organType == 4) { // Caster
        organPos[0] = 0.01;
        organPos[2] = -0.03;
    } else
        assert(false);

    simSetObjectPosition(forceSensor, forceSensor, organPos);
    simGetObjectPosition(organHandle, -1, organPos);
    organ.organPos[0] = organPos[0];
    organ.organPos[1] = organPos[1];
    organ.organPos[2] = organPos[2];
    // Position of connector after rotation
    float newConnectorPos[3];
    simGetObjectPosition(organ.connectorHandle, -1, newConnectorPos);
    float newConnectorOri[3];
    simGetObjectOrientation(organ.connectorHandle, -1, newConnectorOri);

    organ.connectorPos.push_back(newConnectorPos[0]);
    organ.connectorPos.push_back(newConnectorPos[1]);
    organ.connectorPos.push_back(newConnectorPos[2]);
    organ.connectorOri.push_back(newConnectorOri[0]);
    organ.connectorOri.push_back(newConnectorOri[1]);
    organ.connectorOri.push_back(newConnectorOri[2]);

#ifndef ISROBOTSTATIC
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
    simSetObjectInt32Parameter(organ.handle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
    simSetObjectInt32Parameter(organ.handle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif

    usleep(1000);
}

void Morphology_CPPNMatrix::createTemporalGripper(Morphology_CPPNMatrix::OrganSpec &organ)
{
    int gripperHandle;
    float gripperPosition[3];
    float gripperOrientation[3];
    std::string modelsPath = settings::getParameter<settings::String>(parameters,"#organsPath").value;
    modelsPath += "C_Gripper.ttm";
    gripperHandle = simLoadModel(modelsPath.c_str());
    assert(gripperHandle != -1);

    /// \todo EB: would it be better to use the frame of the organ? It's easy change
    gripperPosition[0] = organ.connectorPos[0];
    gripperPosition[1] = organ.connectorPos[1];
    gripperPosition[2] = organ.connectorPos[2];
    gripperOrientation[0] = organ.connectorOri[0];
    gripperOrientation[1] = organ.connectorOri[1];
    gripperOrientation[2] = organ.connectorOri[2];

    simSetObjectPosition(gripperHandle, -1, gripperPosition);
    simSetObjectOrientation(gripperHandle, -1, gripperOrientation);

    // Move relative to gripper.
    gripperPosition[0] = 0.0;
    gripperPosition[1] = 0.0;
    /// \todo EB: This offset should be somewhere else or constant.
    if(organ.organType == 1) { // Wheels
#ifndef HANDMADEROBOT
        std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif HANDMADEROBOT == 0
        gripperPosition[2] = -0.11;
#elif HANDMADEROBOT == 1
        gripperPosition[2] = -0.1;
#elif HANDMADEROBOT == 2
        gripperPosition[2] = -0.1;
#elif HANDMADEROBOT == 3
        gripperPosition[2] = -0.1;
#endif
    }
    else if(organ.organType == 2) // Sensors
        gripperPosition[2] = -0.1;
    else if(organ.organType == 3) // Joints
        gripperPosition[2] = -0.195;
    else if(organ.organType == 4) // Caster
        gripperPosition[2] = -0.1;
    else
        assert(false);

    simSetObjectPosition(gripperHandle, gripperHandle, gripperPosition);
    gripperOrientation[0] = 0.0;
    gripperOrientation[1] = 0.0;
    gripperOrientation[2] = 1.5708;
    simSetObjectOrientation(gripperHandle, gripperHandle, gripperOrientation);

#ifndef ISROBOTSTATIC
    std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
    simSetObjectInt32Parameter(gripperHandle, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
    simSetObjectInt32Parameter(gripperHandle, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
    simSetObjectParent(gripperHandle, mainHandle, 1);

    organ.gripperHandle = gripperHandle;
}

void Morphology_CPPNMatrix::setOrganOrientation(NEAT::NeuralNetwork &cppn, Morphology_CPPNMatrix::OrganSpec &organ)
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

bool Morphology_CPPNMatrix::IsOrganColliding(Morphology_CPPNMatrix::OrganSpec &organ)
{
    int8_t collisionResult;
    // Check collision with skeleton
    for(int handle : organ.objectHandles){
        for(int skeletonHandle : skeletonHandles){
            collisionResult = simCheckCollision(handle,skeletonHandle);
            if(collisionResult == 1) // Collision detected!
                return true;
        }
    }
    /// \todo EB: Wheels are ignored in the following loop.
    // Check collision with other organs
    for(int handle : organ.objectHandles) {
        for (auto &organComp : _organSpec) {
            for (auto &i : organComp.objectHandles) {
                collisionResult = simCheckCollision(handle, i);
                if (collisionResult == 1) // Collision detected!
                    return true;
            }
        }
    }
    return false;
}

bool Morphology_CPPNMatrix::IsOrganInsideSkeleton(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int organHandle)
{
    float position[3];
    simGetObjectPosition(organHandle,-1,position);

    // Transform organPos from m to voxels
    int xPos = static_cast<int>(std::round(position[0]/VOXEL_REAL_SIZE));
    int yPos = static_cast<int>(std::round(position[1]/VOXEL_REAL_SIZE));
    int zPos = static_cast<int>(std::round(position[2]/VOXEL_REAL_SIZE));
    zPos -= MATRIX_HALF_SIZE;
    uint8_t voxelValue;
    voxelValue = skeletonMatrix.getVoxel(xPos,yPos,zPos);
    if(voxelValue == FILLEDVOXEL) // Organ centre point inside of skeleton
        return true;
    else if(voxelValue == EMPTYVOXEL) {
        /// \todo EB: This temporary fixes the issue of the joint colliding with the head organ!
        if(xPos <= xHeadUpperLimit && xPos >= xHeadLowerLimit &&
                yPos <= yHeadUpperLimit && yPos >= xHeadLowerLimit)
            return true;
        else
            return false;
    }
    else
        assert(false);
}

bool Morphology_CPPNMatrix::isOrganGoodOrientation(Morphology_CPPNMatrix::OrganSpec &organ)
{
    float diffPosZ;
    diffPosZ = organ.connectorPos[2] - organ.organPos[2];
    return diffPosZ < 0.005; // Is organ pointing downwards?
}

void Morphology_CPPNMatrix::emptySpaceForHead(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= xHeadUpperLimit && x >= xHeadLowerLimit && y <= yHeadUpperLimit && y >= yHeadLowerLimit){
                    if(skeletonMatrix.getVoxel(x,y,z) == FILLEDVOXEL){
                        skeletonMatrix.setVoxel(x, y, z, EMPTYVOXEL);
                        numSkeletonVoxels--;
                    }
                }
            }
        }
    }
}

void Morphology_CPPNMatrix::testComponents(PolyVox::RawVolume<uint8_t> &skeletonMatrix,Morphology_CPPNMatrix::OrganSpec &organ)
{
    // Manufacturability tests for organs.
    createTemporalGripper(organ);

    organ.organInsideSkeleton = IsOrganInsideSkeleton(skeletonMatrix, organ.handle);
    organ.organColliding = IsOrganColliding(organ);
    organ.organGoodOrientation = isOrganGoodOrientation(organ);
    organ.organGripperAccess = !isGripperColliding(organ.gripperHandle);

    removeGripper(organ.gripperHandle);
}

void Morphology_CPPNMatrix::removeRobot()
{
    // Remove robot method.
    // If the robot fails a single test its novelty is pensalised
    if(!robotManRes.noCollisions || !robotManRes.noBadOrientations || !robotManRes.isGripperAccess){
        /// \todo EB: is this a good way to remove robot?
        indDesc.cartDesc.robotWidth = 0.0;
        indDesc.cartDesc.robotDepth = 0.0;
        indDesc.cartDesc.robotHeight = 0.0;
        indDesc.cartDesc.voxelNumber = 0.0;
        indDesc.cartDesc.wheelNumber = 0.0;
        indDesc.cartDesc.sensorNumber = 0.0;
        indDesc.cartDesc.casterNumber = 0.0;
    }
}

void Morphology_CPPNMatrix::geneRepression(Morphology_CPPNMatrix::OrganSpec &organ)
{
    if(organ.organInsideSkeleton){
        if(organ.organType == 1)
            robotManRes.wheelsRepressed++;
        else if(organ.organType == 2)
            robotManRes.sensorsRepressed++;
        else if(organ.organType == 3)
            robotManRes.jointsRepressed++;
        else if(organ.organType == 4)
            robotManRes.casterRepressed++;
        else
            assert(false);

        simRemoveObject(simGetObjectParent(organ.handle)); // Remove force sensor.
        simRemoveModel(organ.handle); // Remove model.
        simRemoveModel(organ.gripperHandle);
        simRemoveModel(organ.graphicConnectorHandle);
        organ.organRemoved = true;
    }
    else{
        if(organ.organColliding){
            if(organ.organType == 1)
                robotManRes.wheelsRepressed++;
            else if(organ.organType == 2)
                robotManRes.sensorsRepressed++;
            else if(organ.organType == 3)
                robotManRes.jointsRepressed++;
            else if(organ.organType == 4)
                robotManRes.casterRepressed++;
            else
                assert(false);

            simRemoveObject(simGetObjectParent(organ.handle)); // Remove force sensor.
            simRemoveModel(organ.handle); // Remove model.
            simRemoveModel(organ.gripperHandle);
            simRemoveModel(organ.graphicConnectorHandle);
            organ.organRemoved = true;
        }
        else{
            if(!organ.organGoodOrientation){
                if(organ.organType == 1)
                    robotManRes.wheelsRepressed++;
                else if(organ.organType == 2)
                    robotManRes.sensorsRepressed++;
                else if(organ.organType == 3)
                    robotManRes.jointsRepressed++;
                else if(organ.organType == 4)
                    robotManRes.casterRepressed++;
                else
                    assert(false);

                simRemoveObject(simGetObjectParent(organ.handle)); // Remove force sensor.
                simRemoveModel(organ.handle); // Remove model.
                simRemoveModel(organ.gripperHandle);
                simRemoveModel(organ.graphicConnectorHandle);
                organ.organRemoved = true;
            }
            else{
                if(!organ.organGripperAccess){
                    if(organ.organType == 1)
                        robotManRes.wheelsRepressed++;
                    else if(organ.organType == 2)
                        robotManRes.sensorsRepressed++;
                    else if(organ.organType == 3)
                        robotManRes.jointsRepressed++;
                    else if(organ.organType == 4)
                        robotManRes.casterRepressed++;
                    else
                        assert(false);

                    simRemoveObject(simGetObjectParent(organ.handle)); // Remove force sensor.
                    simRemoveModel(organ.handle); // Remove model.
                    simRemoveModel(organ.gripperHandle);
                    simRemoveModel(organ.graphicConnectorHandle);
                    organ.organRemoved = true;
                }
                else{
                    organ.organRemoved = false;
                }
            }
        }
    }
}

void Morphology_CPPNMatrix::manufacturabilityScore()
{
    const double rewardScalar = 1.0;
    const double punishScalar = 1.0;
    const double voxelsScalar = 0.0;
    manScore = 0;
    /** Reward **/
    double rewardValue = (double) (_organSpec.size() - 1) / MAX_NUM_ORGANS; // Ignore head organ
    /** Punish **/
    double punishValue = 0;
    for(auto & i : _organSpec) {
        if(i.organType != 0){
            if (i.organInsideSkeleton || i.organColliding)
                punishValue += 1.0 / 3.0;
            if (!i.organGoodOrientation)
                punishValue += 1.0 / 3.0;
            if (!i.organGripperAccess)
                punishValue += 1.0 / 3.0;
        }
    }
    punishValue = (double) punishValue / MAX_NUM_ORGANS;
    /** Voxels **/
    double voxelsValue = 0;
    voxelsValue = 1 - (numSkeletonVoxels/VOXELS_NUMBER);
    /** Sum **/
    manScore = rewardScalar * rewardValue - punishScalar * punishValue + voxelsScalar * voxelsValue;
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

void Morphology_CPPNMatrix::tempVisualizeMatrix(NEAT::NeuralNetwork &neuralNetwork, VoxelType _voxelType, float posX, float posY, float posZ)
{
    // Create voxel-matrix
    PolyVox::RawVolume<AREVoxel> areMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    PolyVox::RawVolume<uint8_t > wheelMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    // Decode genome
    genomeDecoder(areMatrix, neuralNetwork);
    /// Post-processing functions
    generateSkeleton(areMatrix, wheelMatrix, _voxelType);

    // Marching cubes - we might not need this step at the beginning.
    auto mesh = PolyVox::extractMarchingCubesMesh(&wheelMatrix, wheelMatrix.getEnclosingRegion());
    // I'm not sure if we need this step.
    auto decodedMesh = PolyVox::decodeMesh(mesh);
    // Get vertices and indices.
    std::vector<simFloat> vertices;
    std::vector<simInt> indices;
    vertices.reserve(decodedMesh.getNoOfVertices());
    indices.reserve(decodedMesh.getNoOfIndices());
    getIndicesVertices(decodedMesh,vertices,indices);
    // Generate mesh file (stl). We don't have to generate the mesh file here but we can use the function elsewhere.
    //exportMesh(vertices,indices);
    // Import mesh to V-REP
    if (!vertices.empty()) {
        mainHandle = simCreateMeshShape(2, 20.0f * 3.1415f / 180.0f, vertices.data(), vertices.size(), indices.data(),
                                        indices.size(), nullptr);
        if (mainHandle == -1) {
            std::cout << "Importing mesh NOT succesful!" << std::endl;
        }
        std::ostringstream name;
        name << "VoxelBone" << id;
        simSetObjectName(mainHandle, name.str().c_str());
        // Set starting point
        simFloat objectPosition[3];
        simGetObjectPosition(mainHandle, -1, objectPosition);
        // Shifting origin of mesh in V-REP
        objectPosition[0] = posX + objectPosition[0];
        objectPosition[1] = posY + objectPosition[1];
        objectPosition[2] = posZ + objectPosition[2];
        simSetObjectPosition(mainHandle, -1, objectPosition);

    } else {
        //TODO no mesh data, CPPN generated no volume.
    }
}

void Morphology_CPPNMatrix::exploreSkeleton(PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                          PolyVox::RawVolume<bool> &visitedVoxels, int32_t posX, int32_t posY,
                                          int32_t posZ, int surfaceCounter)
{
    visitedVoxels.setVoxel(posX, posY, posZ, true); // Cell visited
    uint8_t voxel;
    // Explore neighbourhood.
    for (int dz = -1; dz <= 1; dz+=1) {
        if (posZ + dz > -MATRIX_HALF_SIZE && posZ + dz < MATRIX_HALF_SIZE) {
            voxel = skeletonMatrix.getVoxel(posX, posY, posZ + dz);
            if (!visitedVoxels.getVoxel(posX, posY, posZ + dz) && voxel == FILLEDVOXEL) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX, posY, posZ + dz, surfaceCounter);
            }
            else if(!visitedVoxels.getVoxel(posX, posY, posZ + dz) && voxel == EMPTYVOXEL) {
                std::vector<int> newCoord{posX, posY, posZ, 0, 0, dz};
                skeletonSurfaceCoord[surfaceCounter-1].push_back(newCoord);
            }
        }
    }
    for (int dy = -1; dy <= 1; dy+=1) {
        if (posY + dy > -MATRIX_HALF_SIZE && posY + dy < MATRIX_HALF_SIZE) {
            voxel = skeletonMatrix.getVoxel(posX, posY + dy, posZ);
            if (!visitedVoxels.getVoxel(posX, posY + dy, posZ) && voxel == FILLEDVOXEL) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX, posY + dy, posZ, surfaceCounter);
            }
            else if(!visitedVoxels.getVoxel(posX, posY + dy, posZ) && voxel == EMPTYVOXEL) {
                std::vector<int> newCoord{posX, posY, posZ, 0, dy, 0};
                skeletonSurfaceCoord[surfaceCounter-1].push_back(newCoord);
            }
        }
    }
    for (int dx = -1; dx <= 1; dx+=1) {
        if (posX + dx > -MATRIX_HALF_SIZE && posX + dx < MATRIX_HALF_SIZE) {
            voxel = skeletonMatrix.getVoxel(posX + dx, posY, posZ);
            if (!visitedVoxels.getVoxel(posX + dx, posY, posZ) && voxel == FILLEDVOXEL) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX + dx, posY, posZ, surfaceCounter);
            }
            else if(!visitedVoxels.getVoxel(posX + dx, posY, posZ) && voxel == EMPTYVOXEL) {
                std::vector<int> newCoord{posX, posY, posZ, dx, 0, 0};
                skeletonSurfaceCoord[surfaceCounter-1].push_back(newCoord);
            }
        }
    }
}

void Morphology_CPPNMatrix::findSkeletonSurface(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    // This matrix stores the visited elements.
    PolyVox::RawVolume<bool > visitedVoxels(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    uint8_t voxel;
    int surfaceCounter = 0;
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 2) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 2) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 2) {
                voxel = skeletonMatrix.getVoxel(x, y, z);
                if(voxel == FILLEDVOXEL && !visitedVoxels.getVoxel(x, y, z)){
                    surfaceCounter++;
                    skeletonSurfaceCoord.resize(surfaceCounter);
                    exploreSkeleton(skeletonMatrix, visitedVoxels, x, y, z, surfaceCounter);
                }
            }
        }
    }
}

void Morphology_CPPNMatrix::generateOrgans(NEAT::NeuralNetwork &cppn)
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
            else if(cppn.Output()[4] > 0.99) { // Caster
                if(settings::getParameter<settings::Boolean>(parameters,"#isCaster").value) // For debugging only
                    organType = 3;
            }
            else if(cppn.Output()[5] > 0.99) { // Joint
                if(settings::getParameter<settings::Boolean>(parameters,"#isJoint").value) // For debugging only
                    organType = 4;
            }
            // Create organ if any
            if(organType > 0){
                OrganSpec _organ;
                _organ.organType = organType;
                _organ.organRemoved = false;
                _organ.organChecked = false;
                tempPos[0] = (float) input[0] * VOXEL_REAL_SIZE;
                tempPos[1] = (float) input[1] * VOXEL_REAL_SIZE;
                tempPos[2] = (float) input[2] * VOXEL_REAL_SIZE;
                tempPos[2] += MATRIX_HALF_SIZE * VOXEL_REAL_SIZE;

                _organ.organPos.push_back(tempPos[0]);
                _organ.organPos.push_back(tempPos[1]);
                _organ.organPos.push_back(tempPos[2]);
                generateOrientations(skeletonSurfaceCoord[m][n].at(3), skeletonSurfaceCoord[m][n].at(4), skeletonSurfaceCoord[m][n].at(5), _organ);
                _organSpec.push_back(_organ);
            }
        }
    }
}

void Morphology_CPPNMatrix::generateOrientations(int x, int y, int z, OrganSpec& _organ)
{
    if ((x < 0) && (y < 0) && (z > 0)){
        _organ.organOri.push_back(-2.5261);
        _organ.organOri.push_back(+0.5235);
        _organ.organOri.push_back(+2.1861);
    }
    else if ((x < 0) && (y == 0) && (z > 0)){
        _organ.organOri.push_back(+3.1415);
        _organ.organOri.push_back(+0.7853);
        _organ.organOri.push_back(-3.1415);
    }
    else if ((x < 0) && (y > 0) && (z > 0)){
        _organ.organOri.push_back(+2.5261);
        _organ.organOri.push_back(+0.6981);
        _organ.organOri.push_back(-2.1862);
    }
    else if ((x == 0) && (y < 0) && (z > 0)){
        _organ.organOri.push_back(-2.3561);
        _organ.organOri.push_back(+0.5960);
        _organ.organOri.push_back(+1.5707);
    }
    else if ((x == 0) && (y == 0) && (z > 0)){
        _organ.organOri.push_back(-3.1415);
        _organ.organOri.push_back(+0.000);
        _organ.organOri.push_back(+0.0000);
    }
    else if ((x == 0) && (y > 0) && (z > 0)){
        _organ.organOri.push_back(+2.3561);
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(-1.5708);
    }
    else if ((x > 0) && (y < 0) && (z > 0)){
        _organ.organOri.push_back(-2.5261);
        _organ.organOri.push_back(-0.5236);
        _organ.organOri.push_back(+0.9552);
    }
    else if ((x > 0) && (y == 0) && (z > 0)){
        _organ.organOri.push_back(-3.1415);
        _organ.organOri.push_back(-0.7854);
        _organ.organOri.push_back(+0.0000);
    }
    else if ((x > 0) && (y > 0) && (z > 0)){
        _organ.organOri.push_back(+2.5132);
        _organ.organOri.push_back(-0.5235);
        _organ.organOri.push_back(-0.9552);
    }
    else if ((x < 0) && (y < 0) && (z == 0)){
        _organ.organOri.push_back(-1.5708);
        _organ.organOri.push_back(+0.7853);
        _organ.organOri.push_back(+1.5707);
    }
    else if ((x < 0) && (y == 0) && (z == 0)){
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(+1.5707);
        _organ.organOri.push_back(+0.0000);
    }
    else if ((x < 0) && (y > 0) && (z == 0)){
        _organ.organOri.push_back(+1.5707);
        _organ.organOri.push_back(+0.7853);
        _organ.organOri.push_back(-1.5708);
    }
    else if ((x == 0) && (y < 0) && (z == 0)){
        _organ.organOri.push_back(-1.5708);
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(+1.5708);
    }
    else if ((x == 0) && (y > 0) && (z == 0)){
        _organ.organOri.push_back(+1.5707);
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(-1.5708);
    }
    else if ((x > 0) && (y < 0) && (z == 0)) {
        _organ.organOri.push_back(+1.5707);
        _organ.organOri.push_back(-0.7854);
        _organ.organOri.push_back(+1.5708);
    }
    else if ((x > 0) && (y == 0) && (z == 0)) {
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(-1.5708);
        _organ.organOri.push_back(+3.1415);
    }
    else if ((x > 0) && (y > 0) && (z == 0)) {
        _organ.organOri.push_back(+1.5708);
        _organ.organOri.push_back(-0.7854);
        _organ.organOri.push_back(-1.5708);
    }
    else if ((x < 0) && (y < 0) && (z < 0)) {
        _organ.organOri.push_back(-0.6154);
        _organ.organOri.push_back(+0.5235);
        _organ.organOri.push_back(+0.9552);
    }
    else if ((x < 0) && (y == 0) && (z < 0)) {
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(+0.78563);
        _organ.organOri.push_back(+0.0000);
    }
    else if ((x < 0) && (y > 0) && (z < 0)){
        _organ.organOri.push_back(+0.6154);
        _organ.organOri.push_back(+0.5235);
        _organ.organOri.push_back(-0.9552);
    }
    else if ((x == 0) && (y < 0) && (z < 0)){
        _organ.organOri.push_back(-0.7851);
        _organ.organOri.push_back(+0.5235);
        _organ.organOri.push_back(+1.5708);
    }
    else  if ((x == 0) && (y == 0) && (z < 0)){
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(+0.0000);
    }
    else if ((x == 0) && (y > 0) && (z < 0)){
        _organ.organOri.push_back(+0.7853);
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(-1.5708);
    }
    else if ((x > 0) && (y < 0) && (z < 0)){
        _organ.organOri.push_back(-0.6154);
        _organ.organOri.push_back(-0.5236);
        _organ.organOri.push_back(+2.1861);
    }
    else if ((x > 0) && (y == 0) && (z < 0)){
        _organ.organOri.push_back(+0.0000);
        _organ.organOri.push_back(-0.7854);
        _organ.organOri.push_back(-3.1415);
    }
    else if ((x > 0) && (y > 0) && (z < 0)) {
        _organ.organOri.push_back(+0.6154);
        _organ.organOri.push_back(-0.5236);
        _organ.organOri.push_back(-2.1862);
    }
    else {
        _organ.organOri.push_back(+0.6154);
        _organ.organOri.push_back(-0.5236);
        _organ.organOri.push_back(-2.1862);
        std::cerr << "We shouldn't be here: " << __func__ << " " << x << " "
        << y << " " << z << std::endl;
    }

}

void Morphology_CPPNMatrix::skeletonRegionCounter(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    // This matrix stores the visited elements.
    PolyVox::RawVolume<bool > visitedVoxels(PolyVox::Region(PolyVox::Vector3DInt32(-MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE, -MATRIX_HALF_SIZE), PolyVox::Vector3DInt32(MATRIX_HALF_SIZE, MATRIX_HALF_SIZE, MATRIX_HALF_SIZE)));
    uint8_t voxel;
    int regionCounter = 0;
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                voxel = skeletonMatrix.getVoxel(x, y, z);
                if(voxel == FILLEDVOXEL && !visitedVoxels.getVoxel(x, y, z)){
                    regionCounter++;
                    skeletonRegionCoord.resize(regionCounter);
                    std::vector<int> newCoord{x, y, z};
                    skeletonRegionCoord[regionCounter-1].push_back(newCoord);
                    exploreSkeletonRegion(skeletonMatrix, visitedVoxels, x, y, z, regionCounter);
                }
            }
        }
    }
}

void Morphology_CPPNMatrix::exploreSkeletonRegion(PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                            PolyVox::RawVolume<bool> &visitedVoxels, int32_t posX, int32_t posY,
                                            int32_t posZ, int regionCounter)
{
    visitedVoxels.setVoxel(posX, posY, posZ, true); // Cell visited
    uint8_t voxel;
    // Explore neighbourhood.
    for (int dz = -1; dz <= 1; dz+=1) {
        for (int dy = -1; dy <= 1; dy+=1) {
            for (int dx = -1; dx <= 1; dx+=1) {
                if (posX + dx > -MATRIX_HALF_SIZE && posX + dx < MATRIX_HALF_SIZE &&
                    posY + dy > -MATRIX_HALF_SIZE && posY + dy < MATRIX_HALF_SIZE &&
                    posZ + dz > -MATRIX_HALF_SIZE && posZ + dz < MATRIX_HALF_SIZE) {
                    voxel = skeletonMatrix.getVoxel(posX + dx, posY + dy, posZ + dz);
                    if (!visitedVoxels.getVoxel(posX + dx, posY + dy, posZ + dz) && voxel > 120) {
                        std::vector<int> newCoord{posX + dx, posY + dy, posZ + dz};
                        skeletonRegionCoord[regionCounter-1].push_back(newCoord);
                        exploreSkeletonRegion(skeletonMatrix, visitedVoxels, posX + dx, posY + dy, posZ + dz, regionCounter);
                    }
                }
            }
        }
    }
}

void Morphology_CPPNMatrix::removeSkeletonRegions(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    int regionConnected = -1;
    if(skeletonRegionCoord.size() > 1){
        for(int i = 0; i < skeletonRegionCoord.size(); i++){
            for(int j = 0; j < skeletonRegionCoord[i].size(); j++){
                if(skeletonRegionCoord[i][j][0] <= xHeadUpperLimit + skeletonBaseThickness &&
                skeletonRegionCoord[i][j][0] >= xHeadLowerLimit - skeletonBaseThickness &&
                skeletonRegionCoord[i][j][1] <= yHeadUpperLimit + skeletonBaseThickness &&
                skeletonRegionCoord[i][j][1] >= yHeadLowerLimit - skeletonBaseThickness &&
                skeletonRegionCoord[i][j][2] <= -6 + skeletonBaseHeight){ /// \todo :EB make this a constant!
                    regionConnected = i;
                    break;
                }
            }
        }
    }

    if(skeletonRegionCoord.size() > 1){
        for(int i = 0; i < skeletonRegionCoord.size(); i++){
            if(regionConnected != i){
                for(int j = 0; j < skeletonRegionCoord[i].size(); j++){
                    skeletonMatrix.setVoxel(skeletonRegionCoord[i][j][0], skeletonRegionCoord[i][j][1], skeletonRegionCoord[i][j][2], 0);
                }
            }
        }
    }
}

bool Morphology_CPPNMatrix::isGripperColliding(int gripperHandle)
{
    int8_t collisionResult;
    // Check collision with skeleton
    collisionResult = simCheckCollision(gripperHandle,mainHandle);
    if(collisionResult == 1) // Collision detected!
        return true;

    /// \todo EB: Wheels are ignored in the following loop.
    // Check collision with other organs
    for (auto &organComp : _organSpec) {
        for (auto &i : organComp.objectHandles) {
            collisionResult = simCheckCollision(gripperHandle, i);
            if (collisionResult == 1) // Collision detected!
                return true;
        }
    }
    return false;
}

void Morphology_CPPNMatrix::createMaleConnector(Morphology_CPPNMatrix::OrganSpec &organ, int skeletonHandle)
{
    float connectorPosition[3];
    float connectorOrientation[3];
    int connectorHandle;

    connectorPosition[0] = organ.connectorPos[0];
    connectorPosition[1] = organ.connectorPos[1];
    connectorPosition[2] = organ.connectorPos[2];

    connectorOrientation[0] = organ.connectorOri[0];
    connectorOrientation[1] = organ.connectorOri[1];
    connectorOrientation[2] = organ.connectorOri[2];

    std::string modelsPath = settings::getParameter<settings::String>(parameters,"#organsPath").value;
    modelsPath += "C_MaleConnectorV2.ttm";

    connectorHandle = simLoadModel(modelsPath.c_str());
    assert(connectorHandle != -1);

    simSetObjectPosition(connectorHandle, -1, connectorPosition);
    simSetObjectOrientation(connectorHandle, -1, connectorOrientation);

    /// \todo EB: We need to find a better way align origins of connectors and connectors! This distance only applies for the second male conenctor
    connectorPosition[0] = 0.00;
    connectorPosition[1] = 0.0;
    connectorPosition[2] = 0.005;

    connectorOrientation[0] = 3.14159;
    connectorOrientation[1] = 0.0;
    connectorOrientation[2] = 0.0;

    simSetObjectPosition(connectorHandle, connectorHandle, connectorPosition);
    simSetObjectOrientation(connectorHandle, connectorHandle, connectorOrientation);

    simSetObjectParent(connectorHandle, skeletonHandle, 1);
    organ.graphicConnectorHandle = connectorHandle;
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

void Morphology_CPPNMatrix::testRobot(PolyVox::RawVolume<uint8_t>& skeletonMatrix)
{
    // Manufacturability tests for organs.
    for(auto & i : _organSpec) {
        /// \todo EB: Ignore the brain. We might need to change this!
        if(i.organType != 0){
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

void Morphology_CPPNMatrix::createSkeletonBase(PolyVox::RawVolume<uint8_t> &skeletonMatrix, bool isMainSkeleton)
{
    auto region = skeletonMatrix.getEnclosingRegion();
    if(isMainSkeleton){
        for(int32_t z = region.getLowerZ()+1; z < region.getLowerZ() + skeletonBaseHeight; z += 1) {
            for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
                for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                    if(x <= xHeadUpperLimit + skeletonBaseThickness && x >= xHeadLowerLimit - skeletonBaseThickness &&
                       y <= yHeadUpperLimit + skeletonBaseThickness && y >= yHeadLowerLimit - skeletonBaseThickness){ // Additive condition
                        if(x <= xHeadUpperLimit && x >= xHeadLowerLimit &&
                           y <= yHeadUpperLimit && y >= yHeadLowerLimit){ // Substractive condition
                            if(skeletonMatrix.getVoxel(x, y, z) != EMPTYVOXEL){
                                skeletonMatrix.setVoxel(x, y, z, EMPTYVOXEL);
                                numSkeletonVoxels--;
                            }
                        }
                        else{
                            if(skeletonMatrix.getVoxel(x, y, z) != FILLEDVOXEL){
                                skeletonMatrix.setVoxel(x, y, z, FILLEDVOXEL);
                                numSkeletonVoxels++;
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
            for (int32_t y = region.getLowerY() + 1; y < region.getUpperY(); y += 1) {
                for (int32_t x = region.getLowerX() + 1; x < region.getUpperX(); x += 1) {
                    skeletonMatrix.setVoxel(x, y, z, FILLEDVOXEL);
                    numSkeletonVoxels++; /// EB:  Do we need this here?
                }
            }
        }
    }

}

void Morphology_CPPNMatrix::removeGripper(int gripperHandle)
{
    simRemoveModel(gripperHandle);
}

void Morphology_CPPNMatrix::createAREPuck(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    int listVoxels[11][3] = {
            {0,4,-5}, {0,-4,-5}, {0,5,-5}, {0,-5,-5}, // Wheels skeleton
            {3,0,-4}, {3,0,-3},  // Back sensor skeleton
            {-4,0,-5}, {-4,2,-5}, {-4,-2,-5}, {-4,1,-5}, {-4,-1,-5},  // Front sensors skeleton
            };
    int listOrgans[7][3] = {
            {0,5,-5}, {0,-5,-5}, // Wheels positions
            {3,0,-3},  // Back sensor position
            {-4,0,-5}, {-4,2,-5}, {-4,-2,-5},   // Front sensors skeleton
            {3,0,-5},  // Back caster position
    };
    for(int i=0; i < 11; i++){
        skeletonMatrix.setVoxel(listVoxels[i][0], listVoxels[i][1], listVoxels[i][2], FILLEDVOXEL);
    }
    float tempPos[3];
    for(int i = 0; i < 7; i++){
        OrganSpec _organ;
        int oriX = 0; int oriY = 0; int oriZ = 0;
        if(i==0){
            _organ.organType = 1;
            oriX = -1;
        } else if(i==1){
            _organ.organType = 1;
            oriX = -1;
        }else if(i==2){
            _organ.organType = 2;
            oriX = 1;
        }else if(i==3){
            oriX = -1;
            _organ.organType = 2;
        } else if(i==4){
            oriX = -1;
            oriY = 1;
            _organ.organType = 2;
        }else if(i==5){
            oriX = -1;
            oriY = -1;
            _organ.organType = 2;
        } else if(i==6){
            oriX = 1;
            _organ.organType = 4;
        }
        tempPos[0] = (float) listOrgans[i][0] * VOXEL_REAL_SIZE;
        tempPos[1] = (float) listOrgans[i][1] * VOXEL_REAL_SIZE;
        tempPos[2] = (float) listOrgans[i][2] * VOXEL_REAL_SIZE;
        tempPos[2] += MATRIX_HALF_SIZE * VOXEL_REAL_SIZE;

        _organ.organPos.push_back(tempPos[0]);
        _organ.organPos.push_back(tempPos[1]);
        _organ.organPos.push_back(tempPos[2]);
        generateOrientations(oriX, oriY, oriZ, _organ);
        if(i==1){
            _organ.organOri.push_back(3.15149);
        }
        _organSpec.push_back(_organ);
    }
}

void Morphology_CPPNMatrix::createAREPotato(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    int listVoxels[7][3] = {
            {-4,0,-5}, // Wheel
            {-4,-2,-5}, {-4,-1,-5},   // Skeleton
            {-4,-2,-4}, // Sensor
            {4,2,-5}, // Caster
            {1,-4,-5}, // Skeleton
            {1,-5,-5}, // Wheel
    };
    int listOrgans[4][3] = {
            {-4,0,-5}, // Wheel
            {1,-5,-5}, // Wheel
            {-4,-2,-4}, // Sensor
            {4,2,-5}, // Caster
    };
    for(int i=0; i < 7; i++){
        skeletonMatrix.setVoxel(listVoxels[i][0], listVoxels[i][1], listVoxels[i][2], FILLEDVOXEL);
    }
    float tempPos[3];
    for(int i = 0; i < 4; i++){
        OrganSpec _organ;
        int oriX = 0; int oriY = 0; int oriZ = 0;
        if(i==0){
            _organ.organType = 1;
            oriX = -1;
        } else if(i==1){
            _organ.organType = 1;
            oriX = 1;
        }else if(i==2){
            _organ.organType = 2;
            oriX = -1;
            oriY = -1;
        }else if(i==3){
            oriX = 1;
            _organ.organType = 4;
        }
        tempPos[0] = (float) listOrgans[i][0] * VOXEL_REAL_SIZE;
        tempPos[1] = (float) listOrgans[i][1] * VOXEL_REAL_SIZE;
        tempPos[2] = (float) listOrgans[i][2] * VOXEL_REAL_SIZE;
        tempPos[2] += MATRIX_HALF_SIZE * VOXEL_REAL_SIZE;

        _organ.organPos.push_back(tempPos[0]);
        _organ.organPos.push_back(tempPos[1]);
        _organ.organPos.push_back(tempPos[2]);
        generateOrientations(oriX, oriY, oriZ, _organ);

        _organSpec.push_back(_organ);
    }
}

void Morphology_CPPNMatrix::createARETricyle(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    int listVoxels[6][3] = {
            {-4,3,-5}, // Sensor 1
            {-4,-3,-5}, // Sensor 2
            {-3,4,-5}, // Caster 1
            {-3,-4,-5}, // Caster 2
            {4,0,-4}, // Joint 1
            {4,0,-5}, // Joint 1
    };
    int listOrgans[5][3] = {
            {-4,3,-5}, // Sensor 1
            {-4,-3,-5}, // Sensor 2
            {-3,4,-5}, // Caster 1
            {-3,-4,-5}, // Caster 2
            {4,0,-4}, // Joint 1
    };
    for(int i=0; i < 6; i++){
        skeletonMatrix.setVoxel(listVoxels[i][0], listVoxels[i][1], listVoxels[i][2], FILLEDVOXEL);
    }
    float tempPos[3];
    for(int i = 0; i < 5; i++){
        OrganSpec _organ;
        int oriX = 0; int oriY = 0; int oriZ = 0;
        if(i==0){
            _organ.organType = 2;
            oriX = -1;
            oriY = 1;
        } else if(i==1){
            _organ.organType = 2;
            oriX = -1;
            oriY = -1;
        }else if(i==2){
            _organ.organType = 4;
            oriY = 1;
        }else if(i==3){
            oriY = -1;
            _organ.organType = 4;
        }else if(i==4) {
            oriX = 1;
            _organ.organType = 3;
        }
        tempPos[0] = (float) listOrgans[i][0] * VOXEL_REAL_SIZE;
        tempPos[1] = (float) listOrgans[i][1] * VOXEL_REAL_SIZE;
        tempPos[2] = (float) listOrgans[i][2] * VOXEL_REAL_SIZE;
        tempPos[2] += MATRIX_HALF_SIZE * VOXEL_REAL_SIZE;

        _organ.organPos.push_back(tempPos[0]);
        _organ.organPos.push_back(tempPos[1]);
        _organ.organPos.push_back(tempPos[2]);
        generateOrientations(oriX, oriY, oriZ, _organ);

        _organSpec.push_back(_organ);
    }
}

void Morphology_CPPNMatrix::getFinalSkeletonVoxels(PolyVox::RawVolume<uint8_t>& skeletonMatrix)
{
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(skeletonMatrix.getVoxel(x,y,z) == FILLEDVOXEL){
                    indDesc.matDesc.graphMatrix[x + region.getUpperX() - 1][y + region.getUpperY() - 1][z + region.getUpperZ() - 1] = 1;
                    indDesc.symDesc.setVoxelQuadrant(x, y, z, 0);
                }
            }
        }
    }
}
