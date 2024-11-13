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
    std::string manual_design = settings::getParameter<settings::String>(parameters,"#manualDesignFile").value;
    bool growing_decoding = settings::getParameter<settings::Boolean>(parameters,"#growingDecoding").value;
    bool use_quadric = settings::getParameter<settings::Boolean>(parameters,"#useQuadric").value;
    int meshHandle = -1;
    mainHandle = -1;
    bool convexDecompositionSuccess = false;
    std::vector<int> gripperHandles;
    createGripper(gripperHandles);
    //simSetBooleanParameter(sim_boolparam_display_enabled, false); // To turn off display
    numSkeletonVoxels = 0;
    createHead();
    PolyVox::RawVolume<uint8_t> skeletonMatrix(PolyVox::Region(PolyVox::Vector3DInt32(-mc::matrix_size/2, -mc::matrix_size/2, -mc::matrix_size/2), PolyVox::Vector3DInt32(mc::matrix_size/2, mc::matrix_size/2, mc::matrix_size/2)));

    if(manual_design != "None"){
        generateFromManualDesign(skeletonMatrix,organList,list_of_voxels);
    }else{
        GenomeDecoder genomeDecoder;
        if(use_quadric){
            genomeDecoder.superquadricsDecoder(quadric,nn2_cppn,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);
        }
        else{
            if(growing_decoding)
                genomeDecoder.genomeDecoderGrowth(nn2_cppn,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);
            else
                genomeDecoder.genomeDecoder(nn2_cppn,skeletonMatrix,skeletonSurfaceCoord,numSkeletonVoxels);
        }

    }

    // Create mesh for skeleton
    auto mesh = PolyVox::extractMarchingCubesMesh(&skeletonMatrix, skeletonMatrix.getEnclosingRegion());
    auto decodedMesh = PolyVox::decodeMesh(mesh);

    skeletonListVertices.reserve(decodedMesh.getNoOfVertices());
    skeletonListIndices.reserve(decodedMesh.getNoOfIndices());
    bool indVerResult = false;
    indVerResult = getIndicesVertices(decodedMesh);
    // Import mesh to V-REP
    if (indVerResult) {
        if(manual_design == "None")
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


        convexDecompositionSuccess = convex_decomposition(mainHandle,meshHandle,numSkeletonVoxels,skeletonHandles);
        if(convexDecompositionSuccess)
            check_repress_organs(skeletonMatrix,gripperHandles);
        else // Stop generating body plan if convex decomposition fails
            std::cerr << "Not generating robot because convex decomposition failed. Stopping simulation." << std::endl;
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

//    usleep(1000000);
    if(settings::getParameter<settings::Boolean>(parameters,"#isCPPNGenome").value && manual_design == "None")
        retrieve_matrices_from_cppn();
    //Create morphological descriptors
    if(indVerResult || convexDecompositionSuccess){
        feat_desc.create(skeletonMatrix,organList);
        matrix_desc.create(skeletonMatrix,organList);
        organ_mat_desc.create(skeletonMatrix,organList); //todo remove this one or put an option for either matrix or organ_mat
    }
    //create blueprint
    if(settings::getParameter<settings::Boolean>(parameters,"#saveBlueprint").value)
        blueprint.createBlueprint(organList);
    destroyGripper(gripperHandles);
    destroy_physical_connectors();
    // Export model
    if(settings::getParameter<settings::Boolean>(parameters,"#isExportModel").value){
        std::string model_folder = settings::getParameter<settings::String>(parameters,"#modelRepository").value;
        if(model_folder.empty() || model_folder == "None")
            exportRobotModel(morph_id);
        else
            exportRobotModel(morph_id,model_folder);
    }

    retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles,camera_handle);
    // EB: This flag tells the simulator that the shape is convex even though it might not be. Be careful,
    // this might mess up with the physics engine if the shape is non-convex!
    // I set this flag to prevent the warning showing and stopping evolution.
    simSetObjectInt32Parameter(mainHandle, sim_shapeintparam_convex, 1);
}

bool Morphology_CPPNMatrix::convex_decomposition(int mainHandle, int meshHandle, int numSkeletonVoxels, std::vector<int> &skeletonHandles){
    bool convexDecompositionSuccess = false;
    try {
        int convexHandle, brainHandle;
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
    return convexDecompositionSuccess;
}

void Morphology_CPPNMatrix::check_repress_organs(const PolyVox::RawVolume<uint8_t> &skeletonMatrix, const std::vector<int> &gripperHandles){
    std::string manual_design = settings::getParameter<settings::String>(parameters,"#manualDesignFile").value;
    int joints_number = 0;
    // Create organs
    for(Organ &organ : organList){
        // Limit number of legs to 4
        if(organ.getOrganType() == 3 && joints_number == 4 && manual_design == "None"){
            organ.set_organ_removed(true);
            organ.set_organ_checked(true);
            continue;
        }

        if(manual_design == "None")
            if(organ.getOrganType() != 0)
                setOrganOrientation(organ); // Along z-axis relative to the organ itself
        organ.createOrgan(mainHandle);
        if(organ.getOrganType() != 0){
            if(organ.getOrganType() == 1)
                organ.testOrgan(skeletonMatrix, gripperHandles.at(0), skeletonHandles, organList);
            else if(organ.getOrganType() == 2)
                organ.testOrgan(skeletonMatrix, gripperHandles.at(1), skeletonHandles, organList);
            else if(organ.getOrganType() == 3)
                organ.testOrgan(skeletonMatrix, gripperHandles.at(2), skeletonHandles, organList);
            else if(organ.getOrganType() == 4)
                organ.testOrgan(skeletonMatrix, gripperHandles.at(3), skeletonHandles, organList);
            organ.repressOrgan();
        }
        // Count number of good organs.
        if(!organ.isOrganRemoved() && organ.isOrganChecked() && organ.getOrganType() == 3)
            joints_number++;
        // Cap the number of all organs to 8.
        short int goodOrganCounter = 0;
        for(auto & j : organList){
            if(!j.isOrganRemoved() && j.isOrganChecked())
                goodOrganCounter++;

        }
        if(goodOrganCounter >= 8) /// \todo EB: Move this constant elsewhere!
            break;
    }
    testRobot();
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
//    std::vector<float> orientation = settings::getParameter<settings::Sequence<float>>(parameters,"#robotOrientation").value;
//    simSetObjectOrientation(mainHandle,-1,orientation.data());
}

void Morphology_CPPNMatrix::setPosition(float x, float y, float z)
{
    simFloat robotPos[3];
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
    nn2_cppn.step(input);
    output = nn2_cppn.outf();
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
        for (int n = 0; n < skeletonSurfaceCoord[m].size(); n+=1) { /// \todo EB: Define this constant elsewhere!
            double x = static_cast<double>(skeletonSurfaceCoord[m][n].at(0))/static_cast<double>(mc::real_matrix_size/2);
            double y = static_cast<double>(skeletonSurfaceCoord[m][n].at(1))/static_cast<double>(mc::real_matrix_size/2);
            double z = static_cast<double>(skeletonSurfaceCoord[m][n].at(2))/static_cast<double>(mc::real_matrix_size/2);
            input[0] = x;
            input[1] = y;
            input[2] = z;
            input[3] = sqrt(x*x+y*y+z*z);
            organ_type = get_organ_from_cppn(input);
            // Create organ if any
            if(organ_type > 0){
                std::vector<float> tempPosVector(3);
                tempPosVector.at(0) = static_cast<float>(skeletonSurfaceCoord[m][n].at(0) * mc::voxel_real_size);
                tempPosVector.at(1) = static_cast<float>(skeletonSurfaceCoord[m][n].at(1) * mc::voxel_real_size);
                tempPosVector.at(2) = static_cast<float>(skeletonSurfaceCoord[m][n].at(2) * mc::voxel_real_size);
                tempPosVector.at(2) += mc::matrix_size/2 * mc::voxel_real_size;
                std::vector<float> tempOriVector(3);
                generateOrientations(skeletonSurfaceCoord[m][n].at(3), skeletonSurfaceCoord[m][n].at(4), skeletonSurfaceCoord[m][n].at(5), tempOriVector);
                Organ organ(organ_type, tempPosVector, tempOriVector, parameters);
                organList.push_back(organ);
            }
        }
    }
}

void Morphology_CPPNMatrix::exportRobotModel(int indNum, const std::string &folder)
{
    simSetObjectProperty(mainHandle,sim_objectproperty_selectmodelbaseinstead);

    std::stringstream filepath;
    filepath << folder << "/model_" << indNum << ".ttm";

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
    if(settings::getParameter<settings::Boolean>(parameters,"#saveBlueprint").value)
    {
        std::stringstream sst_blueprint;
        sst_blueprint << "/blueprint_" << indNum << ".csv";
        std::ofstream ofs_blueprint(folder + "/" + sst_blueprint.str());
        if(!ofs_blueprint)
        {
            std::cerr << "unable to open : " << folder + "/" + sst_blueprint.str() << std::endl;
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
        filepath_mesh << folder << "/mesh_" << indNum << ".stl";

        //fileformat: the fileformat to export to:
        //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
        simExportMesh(3, filepath_mesh.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

        delete[] verticesMesh;
        delete[] verticesSizesMesh;
        delete[] indicesMesh;
        delete[] indicesSizesMesh;
    }
}

void Morphology_CPPNMatrix::testRobot()
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
    nn2_cppn.step(input);
    output = nn2_cppn.outf();
    for(auto& o: output)
        if(std::isnan(o))
            o = 0;
    // Is there an organ?
    organ_type = -1;
    int max_element = -1;
    max_element = std::max_element(output.begin()+2, output.end()) - output.begin();
    if(max_element == 2){ // Wheel
        // These if statements should be always true but they are here for debugging.
        //if(settings::getParameter<settings::Boolean>(parameters,"#isWheel").value) // For debugging only
            organ_type = 1;
    }
    else if(max_element == 3) { // Sensor
       // if(settings::getParameter<settings::Boolean>(parameters,"#isSensor").value) // For debugging only
            organ_type = 2;
    }
    else if(max_element == 4) { // Joint
      //  if(settings::getParameter<settings::Boolean>(parameters,"#isJoint").value) // For debugging only
            organ_type = 3;
    }
    else if(max_element == 5) { // Caster
//        if(settings::getParameter<settings::Boolean>(parameters,"#isCaster").value) // For debugging only
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


void Morphology_CPPNMatrix::generateFromManualDesign(skeleton_matrix_t &skeleton_matrix, std::vector<Organ> &organs_list, const std::vector<std::vector<int>> &list_of_voxels){


    const int &xhul =  mc::xHeadUpperLimit;
    const int &xhll =  mc::xHeadLowerLimit;
    const int &yhul =  mc::yHeadUpperLimit;
    const int &yhll =  mc::yHeadLowerLimit;
    const float vrs =  mc::voxel_real_size;
    const float hms =  mc::matrix_size/2;
    std::vector<std::vector<int>> organ_voxel_coords;

    //Creating the skeleton matrix
    //Convert list of voxel to skeleton matrix 11*11*11
    for(const auto& voxel: list_of_voxels){
        if(voxel[3] == 1 && //there is a shunk of plastic
                ((voxel[0] >= xhul || xhll >= voxel[0]) || (voxel[1] >= yhul || yhll >= voxel[1]))) // and it is outside of the brain slot
            skeleton_matrix.setVoxel(voxel[0],voxel[1],voxel[2]-5,morph_const::filled_voxel);
        else skeleton_matrix.setVoxel(voxel[0],voxel[1],voxel[2]-5,morph_const::empty_voxel);
        if(voxel[3] > 1)
            organ_voxel_coords.push_back(voxel);
    }


    //Generating the skeleton surface coordinate list
    matrix_int_t skeleton_surface;
    GenomeDecoder::findSkeletonSurface(skeleton_matrix,skeleton_surface);


    //Filling the organs list.
    //Browse through the skeleton surface coordinate
    for(int m = 0; m < skeleton_surface.size(); m++) {
        for(int n = 0; n < skeleton_surface[m].size(); n++) {
            std::vector<int> index_to_keep;
            //For this particular surface coordinate, check if there is an organ there.
            for(int i = 0; i < organ_voxel_coords.size(); i++){
                const auto& organ_coord = organ_voxel_coords[i];
                const auto& surface_coord = skeleton_surface[m][n];
                //If there is an organ here add a new organ in the organs list with the coordinate of this surface voxel, and go to the next voxel.
                if(organ_coord[0] == surface_coord[0] + surface_coord[3] &&
                   organ_coord[1] == surface_coord[1] + surface_coord[4] &&
                   organ_coord[2]-5 == surface_coord[2] + surface_coord[5]){
                    std::vector<float> position = {static_cast<float>(surface_coord[0])*vrs,
                                                   static_cast<float>(surface_coord[1])*vrs,
                                                   static_cast<float>(surface_coord[2] + hms)*vrs};
                    std::vector<float> orientation(3);
                    generateOrientations(surface_coord.at(3), surface_coord.at(4), surface_coord.at(5), orientation);
                    Organ new_organ(organ_coord[3]-1,position,orientation,parameters);
                    organs_list.push_back(new_organ);
                    continue;

                }else index_to_keep.push_back(i); //else mark this organ to be look out on the next surface voxel.
            }
            if(index_to_keep.empty()) break;
            auto tmp = organ_voxel_coords;
            organ_voxel_coords.clear();
            for(const int& i: index_to_keep){
                organ_voxel_coords.push_back(tmp[i]);
            }
        }
    }
}

void Morphology_CPPNMatrix::load_manual_design(const std::string &filename, std::vector<std::vector<int> > &list_of_voxels){
    std::ifstream logFileStream;
    logFileStream.open(filename);
    if(!logFileStream){
        std::cerr << "Morphology_CPPNMatrix -- Unable to open manual design " << filename << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> splitted;
    while(std::getline(logFileStream,line)){
        misc::split_line(line," ",splitted);
        if(splitted[0] == "#")
            continue;
        std::vector<int> voxel = {std::stoi(splitted[0]),std::stoi(splitted[1]),std::stoi(splitted[2])};
        if(splitted[3] == "ffffff") //HEXA for white, bone
            voxel.push_back(1);
        else if(splitted[3] == "ff0000") //HEXA for red, wheel
            voxel.push_back(2);
        else if(splitted[3] == "00ff00") //HEXA for green, sensor
            voxel.push_back(3);
        else if(splitted[3] == "0000ff") //HEXA for blue, leg
            voxel.push_back(4);
        else if(splitted[3] == "000000") //HEXA for black, caster
            voxel.push_back(5);
        list_of_voxels.push_back(voxel);
    }

}
