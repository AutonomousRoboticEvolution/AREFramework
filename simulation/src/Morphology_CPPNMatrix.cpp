#include "simulatedER/Morphology_CPPNMatrix.h"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include <stdio.h>
#include "simulatedER/coppelia_communication.hpp"

#define ISCLUSTER 0
#define ISROBOTSTATIC 0

using namespace are::sim;

void Morphology_CPPNMatrix::create()
{
    int meshHandle = -1;
    mainHandle = -1;
    int convexHandle, brainHandle;
    std::vector<int> gripperHandles;
    createGripper(gripperHandles);
    //simSetBooleanParameter(sim_boolparam_display_enabled, false); // To turn off display
    numSkeletonVoxels = 0;
    createHead();
    PolyVox::RawVolume<AREVoxel> areMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-mc::matrix_size/2, -mc::matrix_size/2, -mc::matrix_size/2), PolyVox::Vector3DInt32(mc::matrix_size/2, mc::matrix_size/2, mc::matrix_size/2)));
    PolyVox::RawVolume<uint8_t> skeletonMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-mc::matrix_size/2, -mc::matrix_size/2, -mc::matrix_size/2), PolyVox::Vector3DInt32(mc::matrix_size/2, mc::matrix_size/2, mc::matrix_size/2)));
    // Decoding CPPN
    GenomeDecoder genomeDecoder;
    if(settings::getParameter<settings::Boolean>(parameters,"#isCPPNGenome").value){
        if(use_neat) genomeDecoder.genomeDecoder(cppn,areMatrix,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);
        else genomeDecoder.genomeDecoder(nn2_cppn,areMatrix,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);
    }
    else
        genomeDecoder.genomeDecoder(matrix_4d,areMatrix,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);

    // Create mesh for skeleton
    auto mesh = PolyVox::extractMarchingCubesMesh(&skeletonMatrix, skeletonMatrix.getEnclosingRegion());
    auto decodedMesh = PolyVox::decodeMesh(mesh);

    skeletonListVertices.reserve(decodedMesh.getNoOfVertices());
    skeletonListIndices.reserve(decodedMesh.getNoOfIndices());
    bool indVerResult = false;
    indVerResult = getIndicesVertices(decodedMesh);
    bool convexDecompositionSuccess = false;
    // Import mesh to V-REP
    if (indVerResult) {
        generateOrgans(skeletonSurfaceCoord);
        skeletonSurfaceCoord.clear();
        skeletonSurfaceCoord.shrink_to_fit();
        meshHandle = simCreateMeshShape(2, 20.0f * 3.1415f / 180.0f, skeletonListVertices.data(), skeletonListVertices.size(), skeletonListIndices.data(),
                                        skeletonListIndices.size(), nullptr);
        if (meshHandle == -1) {
            std::cerr << "Creating mesh NOT succesful! " << __func__  << std::endl;
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
//            intParams[0]: HACD: the minimum number of clusters to be generated (e.g. 1)
//            intParams[1]: HACD: the targeted number of triangles of the decimated mesh (e.g. 500)
//            intParams[2]: HACD: the maximum number of vertices for each generated convex hull (e.g. 100)
//            intParams[3]: HACD: the maximum number of iterations. Use 0 for the default value (i.e. 4).
//            intParams[4]: reserved. Set to 0.
//            intParams[5]: V-HACD: resolution (10000-64000000, 100000 is default).
//            intParams[6]: V-HACD: depth (1-32, 20 is default).
//            intParams[7]: V-HACD: plane downsampling (1-16, 4 is default).
//            intParams[8]: V-HACD: convex hull downsampling (1-16, 4 is default).
//            intParams[9]: V-HACD: max. number of vertices per convex hull (4-1024, 64 is default).
            int conDecIntPams[10] = {1, 100, 20, 1, 0, //HACD
                                              10000, 20, 4, 4, 64}; //V-HACD
//            floatParams[0]: HACD: the maximum allowed concavity (e.g. 100.0)
//            floatParams[1]: HACD: the maximum allowed distance to get convex clusters connected (e.g. 30)
//            floatParams[2]: HACD: the threshold to detect small clusters. The threshold is expressed as a percentage of the total mesh surface (e.g. 0.25)
//            floatParams[3]: reserved. Set to 0.0
//            floatParams[4]: reserved. Set to 0.0
//            floatParams[5]: V-HACD: concavity (0.0-1.0, 0.0025 is default).
//            floatParams[6]: V-HACD: alpha (0.0-1.0, 0.05 is default).
//            floatParams[7]: V-HACD: beta (0.0-1.0, 0.05 is default).
//            floatParams[8]: V-HACD: gamma (0.0-1.0, 0.00125 is default).
//            floatParams[9]: V-HACD: min. volume per convex hull (0.0-0.01, 0.0001 is default).
            float conDecFloatPams[10] = {100, 30, 0.25, 0.0, 0.0,//HACD
                                                  0.0025, 0.05, 0.05, 0.00125, 0.0001};//V-HACD

            convexHandle = simConvexDecompose(meshHandle, 8u | 16u, conDecIntPams, conDecFloatPams);
           if(convexHandle >= 0)
                convexDecompositionSuccess = true;
            //** Compute Mass and Inertia of skeleton. The following method is a "dirty" workaround to have a mass close from the printed skeleton.
            // The issue come from a mismatch between the mass computed by verp and the one expected.
            //Call this to compute the approximate moment of inertia and center of mass
            simComputeMassAndInertia(convexHandle, 84); //kg.m-3
            float skeletonMass = numSkeletonVoxels*0.00116 + 0.114; //real mass of the skeleton
            float mass;
            float inertiaMatrix[9];
            float centerOfMass[3];
            simGetShapeMassAndInertia(convexHandle,&mass, inertiaMatrix, centerOfMass, nullptr);
            simSetShapeMassAndInertia(convexHandle,skeletonMass,inertiaMatrix,centerOfMass,nullptr);
            //*/

            mainHandle = convexHandle;
            // Create brain primitive
            float brainSize[3] = {0.084,0.084,0.11};
            brainHandle = simCreatePureShape(0,0,brainSize,0.503,nullptr); //Head organ weighs 503g
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

        } catch (std::exception &e) {
            //std::clog << "Decomposition failed: why? " << e.what() << __func__ << std::endl;
            convexDecompositionSuccess = false;
        }

        if(convexDecompositionSuccess){
            int joints_number = 0;
            // Create organs
            for(auto & i : organList){
                // Limit number of legs to 4
                if(i.getOrganType() == 3 && joints_number == 4){
                    i.set_organ_removed(true);
                    i.set_organ_checked(true);
                    continue;
                }

                if(i.getOrganType() != 0)
                    setOrganOrientation(i); // Along z-axis relative to the organ itself
                i.createOrgan(mainHandle);
                if(i.getOrganType() != 0){
                    if(i.getOrganType() == 1)
                        i.testOrgan(skeletonMatrix, gripperHandles.at(0), skeletonHandles, organList);
                    else if(i.getOrganType() == 2)
                        i.testOrgan(skeletonMatrix, gripperHandles.at(1), skeletonHandles, organList);
                    else if(i.getOrganType() == 3)
                        i.testOrgan(skeletonMatrix, gripperHandles.at(2), skeletonHandles, organList);
                    else if(i.getOrganType() == 4)
                        i.testOrgan(skeletonMatrix, gripperHandles.at(3), skeletonHandles, organList);
                    i.repressOrgan();
                }
                // Count number of good organs.
                if(!i.isOrganRemoved() && i.isOrganChecked() && i.getOrganType() == 3)
                    joints_number++;
                // Cap the number of all organs to 8.
                short int goodOrganCounter = 0;
                for(auto & j : organList){
                    if(!j.isOrganRemoved() && j.isOrganChecked())
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
    if((indVerResult && convexDecompositionSuccess) && settings::getParameter<settings::Boolean>(parameters,"#isSegmentedRobot").value){
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
                double organCoord[CHILDORGANNUMBER][3] {{0,0,2},{0,-1,0},{-1,0,0},{2,0,0}};

                // SimGroupShapes changes the orientation of the shape this verifies that the organ generation is consistent.
                if(abs(orientation[0]) < 0.01)
                    organCoord[0][2] = -1;

                std::vector<int> organTypeList(CHILDORGANNUMBER,-1);
                // Check each side of the child skeleton
                for(int j = 0; j < CHILDORGANNUMBER; j++){
                    std::vector<double> temp_vector{organCoord[j][0],organCoord[j][1],organCoord[j][2],sqrt(pow(organCoord[j][0],2)+pow(organCoord[j][1],2)+pow(organCoord[j][2],2))};
                    organTypeList.at(j) = get_organ_from_cppn(temp_vector);
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
                        if(organ.getOrganType() !=0)
                            setOrganOrientation(organ); // Along z-axis relative to the organ itself
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

                        if(organ.getOrganType() == 1)
                            organ.testOrgan(skeletonMatrix, gripperHandles.at(0), skeletonHandles, organList);
                        else if(organ.getOrganType() == 2)
                            organ.testOrgan(skeletonMatrix, gripperHandles.at(1), skeletonHandles, organList);
                        else if(organ.getOrganType() == 3)
                            organ.testOrgan(skeletonMatrix, gripperHandles.at(0), skeletonHandles, organList);
                        else if(organ.getOrganType() == 4)
                            organ.testOrgan(skeletonMatrix, gripperHandles.at(0), skeletonHandles, organList);

                        organ.repressOrgan();
                        organList.push_back(organ);
                    }
                }
            }
        }
    }
//    usleep(1000000);
    if(settings::getParameter<settings::Boolean>(parameters,"#isCPPNGenome").value)
        retrieve_matrices_from_cppn();
    // Get info from body plan for body plan descriptors or logging.
    if(indVerResult || convexDecompositionSuccess){
        indDesc.getRobotDimensions(organList);
        indDesc.cartDesc.voxelNumber = numSkeletonVoxels;
        indDesc.countOrgans(organList);
        indDesc.getOrganPositions(organList);
    }
    if(settings::getParameter<settings::Boolean>(parameters,"#saveBlueprint").value)
        blueprint.createBlueprint(organList);
    destroyGripper(gripperHandles);
    destroy_physical_connectors();
    // Export model
    if(settings::getParameter<settings::Boolean>(parameters,"#isExportModel").value)
        exportRobotModel(morph_id);

    retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles,camera_handle);
    // EB: This flag tells the simulator that the shape is convex even though it might not be. Be careful,
    // this might mess up with the physics engine if the shape is non-convex!
    // I set this flag to prevent the warning showing and stopping evolution.
    simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_convex, 1);
}

void Morphology_CPPNMatrix::load(){
    std::stringstream morph_filepath;
    morph_filepath << Logging::log_folder << "/model_" << morph_id << ".ttm";
    std::cout <<  morph_filepath.str() << std::endl;
    int handle = simLoadModel(morph_filepath.str().c_str());

    if(handle == -1)
    {
        std::cerr << "unable to load robot model" << std::endl;
        simChar* lastError = simGetLastError();
        std::cerr << "simGetLastError : " << lastError << std::endl;
        simReleaseBuffer(lastError);
        exit(1);
    }

    mainHandle = handle;
    retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles,camera_handle);
}

void Morphology_CPPNMatrix::createAtPosition(float x, float y, float z)
{
    if(isRobotModel)
        load();
    else
        create();

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose){
        float mass;
        float inertiaMatrix[9];
        float centerOfMass[3];
        simGetShapeMassAndInertia(mainHandle,&mass, inertiaMatrix, centerOfMass, nullptr);
        std::cout << "Mass of the main frame (skeleton + head organ) :" << mass << std::endl;
    }
    setPosition(x,y,z);
    std::vector<float> orientation = settings::getParameter<settings::Sequence<float>>(parameters,"#robotOrientation").value;
    simSetObjectOrientation(mainHandle,-1,orientation.data());
}

void Morphology_CPPNMatrix::setPosition(float x, float y, float z)
{
    float robotPos[3];
    robotPos[0] = x;
    robotPos[1] = y;
    robotPos[2] = z;

    if(simSetObjectPosition(mainHandle, -1, robotPos) < 0){
        std::cerr << "Set object position failed" << std::endl;
        exit(1);
    }
}

bool Morphology_CPPNMatrix::getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<uint8_t>> &decodedMesh)
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
        skeletonListVertices.emplace_back(pos.getX() * mc::shape_scale_value);
        skeletonListVertices.emplace_back(pos.getY() * mc::shape_scale_value);
        skeletonListVertices.emplace_back(pos.getZ() * mc::shape_scale_value);
    }

    // If all vectors are the same, we have an object the size of point. This is considered a failed viability.
    // and it makes the vertex decomposition to crash badly.
    if (pointObject) {
        skeletonListVertices.clear();
        skeletonListIndices.clear();
        return false;
    }

    for (unsigned int i=0; i < n_indices; i++) {
        skeletonListIndices.emplace_back(decodedMesh.getIndex(i));
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

void Morphology_CPPNMatrix::createGripper(std::vector<int>& gripperHandles)
{
    gripperHandles.resize(4);
    float gripperPosition[3];
    float gripperOrientation[3];
    int tempGripperHandle = -1;
    std::string models_path = settings::getParameter<settings::String>(parameters,"#modelsPath").value;
    std::string gripperWheelPath = models_path + "/utils/gripper_w.ttm";
    tempGripperHandle = simLoadModel(gripperWheelPath.c_str());
    assert(tempGripperHandle != -1);
    gripperHandles[0] = tempGripperHandle;
    tempGripperHandle = -1;
    std::string gripperSensorPath = models_path + "/utils/gripper_s.ttm";
    tempGripperHandle = simLoadModel(gripperSensorPath.c_str());
    assert(tempGripperHandle != -1);
    gripperHandles[1] = tempGripperHandle;
    tempGripperHandle = -1;
    std::string gripperLegPath = models_path + "/utils/gripper_l.ttm";
    tempGripperHandle = simLoadModel(gripperLegPath.c_str());
    assert(tempGripperHandle != -1);
    gripperHandles[2] = tempGripperHandle;
    tempGripperHandle = -1;
    std::string gripperCasterPath = models_path + "/utils/gripper_c.ttm";
    tempGripperHandle = simLoadModel(gripperCasterPath.c_str());
    assert(tempGripperHandle != -1);
    gripperHandles[3] = tempGripperHandle;
    gripperHandles.shrink_to_fit();

    gripperPosition[0] = 1.0; gripperPosition[1] = 1.0; gripperPosition[2] = 1.0;
    gripperOrientation[0] = 0.0; gripperOrientation[1] = 0.0; gripperOrientation[2] = 0.0;

    for(const auto & i : gripperHandles){
        simSetObjectPosition(i, -1, gripperPosition);
        simSetObjectOrientation(i, -1, gripperOrientation);
#ifndef ISROBOTSTATIC
        std::cerr << "We shouldn't be here!" << __fun__ << std::endl;
#elif ISROBOTSTATIC == 0
        simSetObjectInt32Parameter(i, sim_shapeintparam_static, 0); // Keeps skeleton fix in the absolute position. For testing purposes
#elif ISROBOTSTATIC == 1
        simSetObjectInt32Parameter(i, sim_shapeintparam_static, 1); // Keeps skeleton fix in the absolute position. For testing purposes
#endif
    }
}

void Morphology_CPPNMatrix::setOrganOrientation(Organ &organ)
{
    // Vector used as input of the Neural Network (NN).
    std::vector<double> input{0,0,0,0};
    std::vector<double> output;
    input[0] = static_cast<int>(std::round(organ.organPos[0]/mc::voxel_real_size));
    input[1] = static_cast<int>(std::round(organ.organPos[1]/mc::voxel_real_size));
    input[2] = static_cast<int>(std::round(organ.organPos[2]/mc::voxel_real_size));
    input[2] -= mc::matrix_size/2;
    input[3] = static_cast<double>(sqrt(pow(input[0],2)+pow(input[1],2)+pow(input[2],2)));
    if(settings::getParameter<settings::Boolean>(parameters,"#isCPPNGenome").value) {
        if (use_neat) {
            // Set inputs to NN
            cppn.Input(input);
            // Activate NN
            cppn.Activate();
            output = cppn.Output();
        } else {
            nn2_cppn.step(input);
            output = nn2_cppn.outf();
        }
    } else{
        int pos_x = input.at(0) + 5;
        int pos_y = input.at(1) + 5;
        int pos_z = input.at(2) + 5;
        // If no thereshold it crashes for joints because the coordinates is out boundaries
        if(pos_x > 10) pos_x = 10; if(pos_y > 10) pos_y = 10; if(pos_z > 10) pos_z = 10;
        int pos_in_vector = pos_x * 121 + pos_y * 11 + pos_z;
        output.push_back(matrix_4d.at(0).at(pos_in_vector));

    }

    for(auto& o: output)
        if(std::isnan(o))
            o = 0;
    float rot;
    rot = output.at(0) * 0.523599; // 30 degrees limit
    organ.organOri.push_back(rot);
}

void Morphology_CPPNMatrix::generateOrgans(std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord)
{
    std::vector<double> input{0,0,0,0}; // Vector used as input of the Neural Network (NN).
    std::vector<double> output;
    int organ_type;
    for(int m = 0; m < skeletonSurfaceCoord.size(); m++) {
        // This sorts the vectors along the z-axis. This allows to generate organs from the bottom of the robot to the top hence reducing the number of useless organs.
        // Solution taken from https://stackoverflow.com/questions/45494567/c-how-to-sort-the-rows-of-a-2d-vector-by-the-values-in-each-rows-column
        std::sort(skeletonSurfaceCoord[m].begin(),
                  skeletonSurfaceCoord[m].end(),
                  [] (const std::vector<int> &a, const std::vector<int> &b)
                  {
                      return a[2] < b[2];
                  });
        // Generate organs every two voxels.
        for (int n = 0; n < skeletonSurfaceCoord[m].size(); n+=1) { /// \todo EB: Define this constant elsewhere!
            input[0] = static_cast<double>(skeletonSurfaceCoord[m][n].at(0));
            input[1] = static_cast<double>(skeletonSurfaceCoord[m][n].at(1));
            input[2] = static_cast<double>(skeletonSurfaceCoord[m][n].at(2));
            input[3] = static_cast<double>(sqrt(pow(skeletonSurfaceCoord[m][n].at(0),2)+pow(skeletonSurfaceCoord[m][n].at(1),2)+pow(skeletonSurfaceCoord[m][n].at(2),2)));
            organ_type = get_organ_from_cppn(input);
            // Create organ if any
            if(organ_type > 0){
                std::vector<float> tempPosVector(3);
                tempPosVector.at(0) = static_cast<float>(input[0] * mc::voxel_real_size);
                tempPosVector.at(1) = static_cast<float>(input[1] * mc::voxel_real_size);
                tempPosVector.at(2) = static_cast<float>(input[2] * mc::voxel_real_size);
                tempPosVector.at(2) += mc::matrix_size/2 * mc::voxel_real_size;
                std::vector<float> tempOriVector(3);
                generateOrientations(skeletonSurfaceCoord[m][n].at(3), skeletonSurfaceCoord[m][n].at(4), skeletonSurfaceCoord[m][n].at(5), tempOriVector);
                Organ organ(organ_type, tempPosVector, tempOriVector, parameters);
                organList.push_back(organ);
            }
        }
    }
}

void Morphology_CPPNMatrix::exportRobotModel(int indNum)
{
    simSetObjectProperty(mainHandle,sim_objectproperty_selectmodelbaseinstead);

    std::stringstream filepath;
    filepath << Logging::log_folder << "/model_" << indNum << ".ttm";

    int p = simGetModelProperty(mainHandle);
    p = (p|sim_modelproperty_not_model)-sim_modelproperty_not_model;
    simSetModelProperty(mainHandle,p);

    int result;
    result = simSaveModel(mainHandle,filepath.str().c_str());
    if(result == -1){
        std::cerr << "Something went wrong when exporting robot model! " << std::endl;
    }
    ///

    // Export blueprint

    std::stringstream sst_blueprint;
    sst_blueprint << "/blueprint_" << indNum << ".csv";
    std::ofstream ofs_blueprint(Logging::log_folder + sst_blueprint.str());
    if(!ofs_blueprint)
    {
        std::cerr << "unable to open : " << Logging::log_folder  + sst_blueprint.str() << std::endl;
        return;
    }
    std::vector<int> tempOrganTypes = blueprint.getOrganTypes();
    std::vector<std::vector<float>> tempOrganPos = blueprint.getOrganPosList();
    std::vector<std::vector<float>> tempOrganOri = blueprint.getOrganOriList();
    for (int i = 0; i < tempOrganTypes.size(); i++) {
        ofs_blueprint << "0" << "," << tempOrganTypes.at(i) << ","
                      << tempOrganPos.at(i).at(0) << "," << tempOrganPos.at(i).at(1) << ","
                      << tempOrganPos.at(i).at(2) << ","
                      << tempOrganOri.at(i).at(0) << "," << tempOrganOri.at(i).at(1) << ","
                      << tempOrganOri.at(i).at(2) << ","
                      << std::endl;
    }

    // Export mesh file
    const auto **verticesMesh = new const simFloat *[2];
    const auto **indicesMesh = new const simInt *[2];
    auto *verticesSizesMesh = new simInt[2];
    auto *indicesSizesMesh = new simInt[2];
    verticesMesh[0] = skeletonListVertices.data();
    verticesSizesMesh[0] = skeletonListVertices.size();
    indicesMesh[0] = skeletonListIndices.data();
    indicesSizesMesh[0] = skeletonListIndices.size();

    std::stringstream filepath_mesh;
    filepath_mesh << Logging::log_folder << "/mesh_" << indNum << ".stl";

    //fileformat: the fileformat to export to:
    //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
    simExportMesh(3, filepath_mesh.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

    delete[] verticesMesh;
    delete[] verticesSizesMesh;
    delete[] indicesMesh;
    delete[] indicesSizesMesh;
}

void Morphology_CPPNMatrix::testRobot(PolyVox::RawVolume<uint8_t>& skeletonMatrix)
{
    // Manufacturability tests for organs.
    for(auto & i : organList) {
        /// \todo EB: Ignore the brain. We might need to change this!
        if(i.getOrganType() != 0){
            if (i.organColliding || i.organInsideSkeleton)
                robotManRes.noCollisions = false;
            if (!i.organGripperAccess)
                robotManRes.isGripperAccess = false;
        }
    }
}

void Morphology_CPPNMatrix::destroyGripper(const std::vector<int>& gripperHandles)
{
    simRemoveModel(gripperHandles[0]);
    simRemoveModel(gripperHandles[1]);
    simRemoveModel(gripperHandles[2]);
    simRemoveModel(gripperHandles[3]);

//    for(auto & i : gripperHandles) {
//        simRemoveModel(i);
//    }
}

void Morphology_CPPNMatrix::destroy_physical_connectors()
{
    for (std::vector<Organ>::iterator it = organList.begin(); it != organList.end(); it++) {
        if (!it->isOrganRemoved() && it->isOrganChecked()) {
            simRemoveModel(it->get_graphical_connector_handle());
        }
    }
}

void Morphology_CPPNMatrix::retrieve_matrices_from_cppn()
{
    std::vector<double> input{0,0,0,0};
    std::vector<double> output;
    matrix_4d.resize(6);
    for(int i = -mc::matrix_size/2 + 1; i < mc::matrix_size/2; i += 1){
        for(int j = -mc::matrix_size/2 + 1; j < mc::matrix_size/2; j += 1){
            for(int k = -mc::matrix_size/2 + 1; k < mc::matrix_size/2; k += 1){
                input[0] = static_cast<double>(i);
                input[1] = static_cast<double>(j);
                input[2] = static_cast<double>(k);
                input[3] = static_cast<double>(sqrt(pow(i,2)+pow(j,2)+pow(k,2)));
                nn2_cppn.step(input);
                output = nn2_cppn.outf();
                for(auto& o: output)
                    if(std::isnan(o))
                        o = 0;

//                matrix_4d.at(0).push_back(output.at(0));
                matrix_4d.at(0).push_back(output.at(0));
                matrix_4d.at(1).push_back((output.at(1) > 0.0) ? 1.0 : -1.0);
                matrix_4d.at(2).push_back(output.at(2));
                matrix_4d.at(3).push_back(output.at(3));
                matrix_4d.at(4).push_back(output.at(4));
                matrix_4d.at(5).push_back(output.at(5));
            }
        }
    }
}

int Morphology_CPPNMatrix::get_organ_from_cppn(std::vector<double> input)
{
    int organ_type = -1;
    std::vector<double> output;
    if(settings::getParameter<settings::Boolean>(parameters,"#isCPPNGenome").value) {
        if (use_neat) {
            // Set inputs to NN
            cppn.Input(input);
            // Activate NN
            cppn.Activate();
            output = cppn.Output();
        } else {
            nn2_cppn.step(input);
            output = nn2_cppn.outf();
            for(auto& o: output)
                if(std::isnan(o))
                    o = 0;

        }
    } else{
        int pos_x = input.at(0) + 5;
        int pos_y = input.at(1) + 5;
        int pos_z = input.at(2) + 5;
        int pos_in_vector = pos_x * 121 + pos_y * 11 + pos_z;
        output.push_back(0); output.push_back(0);
        output.push_back(matrix_4d.at(2).at(pos_in_vector));
        output.push_back(matrix_4d.at(3).at(pos_in_vector));
        output.push_back(matrix_4d.at(4).at(pos_in_vector));
        output.push_back(matrix_4d.at(5).at(pos_in_vector));

    }
    // Is there an organ?
    organ_type = -1;
    int max_element = -1;
    max_element = std::max_element(output.begin()+2, output.end()) - output.begin();
    if(max_element == 2){ // Wheel
        // These if statements should be always true but they are here for debugging.
        if(settings::getParameter<settings::Boolean>(parameters,"#isWheel").value) // For debugging only
            organ_type = 1;
    }
    else if(max_element == 3) { // Sensor
        if(settings::getParameter<settings::Boolean>(parameters,"#isSensor").value) // For debugging only
            organ_type = 2;
    }
    else if(max_element == 4) { // Joint
        if(settings::getParameter<settings::Boolean>(parameters,"#isJoint").value) // For debugging only
            organ_type = 3;
    }
    else if(max_element == 5) { // Caster
        if(settings::getParameter<settings::Boolean>(parameters,"#isCaster").value) // For debugging only
            organ_type = 4;
    } else{
        std::cerr << "We shouldn't be here: " << __func__ << " max_element: " << max_element << std::endl;
        assert(false);
    }
    return organ_type;
}

void Morphology_CPPNMatrix::generateOrientations(int x, int y, int z, std::vector<float> &orientation)
{
    /// \todo: EB: Remove z > 0 amd z < 0 as the organ cannot face these directions.
    // Gives the direction of the organ given the direction of the surface
    if ((x < 0) && (y == 0) && (z == 0)){
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
