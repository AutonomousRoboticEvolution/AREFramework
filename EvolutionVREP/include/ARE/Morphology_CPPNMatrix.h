#ifndef MORPHOLOGY_CPPNMATRIX_H
#define MORPHOLOGY_CPPNMATRIX_H

#include "ARE/Morphology.h"

#include "PolyVox/RawVolume.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "eigen3/Eigen/Core"

#include "math.h"

/// \todo EB: Should this be here?
enum VoxelType
{
    BONE,
    WHEEL,
    SENSOR,
    JOINT,
    CASTER
};

namespace are {

class Morphology_CPPNMatrix : public Morphology
{
public:
    Morphology_CPPNMatrix(const settings::ParametersMapPtr &param) : Morphology(param){}

    struct AREVoxel{
        uint8_t bone;
        uint8_t wheel;
        uint8_t sensor;
        uint8_t joint;
        uint8_t caster;
    };

    struct OrganSpec
    {
        int handle;
        int organType;
        int connectorHandle;
        int gripperHandle;
        std::vector<float> organPos;
        std::vector<float> connectorPos;
        std::vector<float> organOri;
        std::vector<float> connectorOri;
        bool organInsideSkeleton;
        bool organColliding;
        bool organGoodOrientation;
        bool organGripperAccess;
        std::vector<int> objectHandles; // For collision detection purpose
    };

    class RobotManRes{
    public:
        bool noCollisions;
        bool noBadOrientations;
        bool isGripperAccess;
        // Constructor
        RobotManRes(){
            noCollisions = true;
            noBadOrientations = true;
            isGripperAccess = true;
        }
        std::vector<bool> getResVector(){
            std::vector<bool> resVector;
            resVector.push_back(noCollisions);
            resVector.push_back(noBadOrientations);
            resVector.push_back(isGripperAccess);
            return resVector;
        }
    };
    RobotManRes robotManRes;

    Morphology::Ptr clone() const override
        {return std::make_shared<Morphology_CPPNMatrix>(*this);}

    void create() override;
    void createAtPosition(float,float,float) override;
    std::vector<double> update() override;
    void setPosition(float,float,float);

    ///////////////////////
    ///// Descriptors /////
    ///////////////////////

    class CartDesc
    {
    private:

    public:
        float robotWidth; // X
        float robotDepth; // Y
        float robotHeight; // Z
        int voxelNumber;
        int wheelNumber;
        int sensorNumber;
        int casterNumber;
        int jointNumber;
        Eigen::VectorXd cartDesc;
        // Constructor
        CartDesc(){
            cartDesc.resize(8);
            robotWidth = 0;
            robotDepth = 0;
            robotHeight = 0;
            voxelNumber = 0;
            wheelNumber = 0;
            sensorNumber = 0;
            casterNumber = 0;
            jointNumber = 0;
        }
        void setCartDesc(){
            cartDesc(0) = robotWidth / MATRIX_SIZE_M;
            cartDesc(1) = robotDepth / MATRIX_SIZE_M;
            cartDesc(2) = robotHeight / MATRIX_SIZE_M;
            cartDesc(3) = (double) voxelNumber / VOXELS_NUMBER;
            cartDesc(4) = (double) wheelNumber / MAX_NUM_ORGANS;
            cartDesc(5) = (double) sensorNumber / MAX_NUM_ORGANS;
            cartDesc(6) = (double) jointNumber / MAX_NUM_ORGANS;
            cartDesc(7) = (double) casterNumber / MAX_NUM_ORGANS;
        }
    };

    class MatDesc{
    public:
        std::vector<std::vector<std::vector<int>>> graphMatrix;
        // Constructor
        MatDesc(){
            graphMatrix.resize(MATRIX_SIZE + 1);
            for(int i = 0; i < MATRIX_SIZE + 1; i++){
                graphMatrix[i].resize(MATRIX_SIZE + 1);
                for(int j = 0; j < MATRIX_SIZE + 1; j++){
                    graphMatrix[i][j].resize(MATRIX_SIZE + 1);
                    for(int k = 0; k < MATRIX_SIZE + 1; k++){
                        graphMatrix[i][j][k] = 0;
                    }
                }
            }
        }
    };

    class CPPNBinaryDesc{
    public:
        std::vector<std::vector<std::vector<bool>>> skeletonMatrix;
        std::vector<std::vector<std::vector<bool>>> wheelMatrix;
        std::vector<std::vector<std::vector<bool>>> sensorMatrix;
        std::vector<std::vector<std::vector<bool>>> jointMatrix;
        std::vector<std::vector<std::vector<bool>>> casterMatrix;
        // Constructor
        CPPNBinaryDesc(){
            skeletonMatrix.resize(MATRIX_SIZE + 1);
            wheelMatrix.resize(MATRIX_SIZE + 1);
            sensorMatrix.resize(MATRIX_SIZE + 1);
            jointMatrix.resize(MATRIX_SIZE + 1);
            casterMatrix.resize(MATRIX_SIZE + 1);
            for(int i = 0; i < 13; i++){
                skeletonMatrix[i].resize(MATRIX_SIZE + 1);
                wheelMatrix[i].resize(MATRIX_SIZE + 1);
                sensorMatrix[i].resize(MATRIX_SIZE + 1);
                jointMatrix[i].resize(MATRIX_SIZE + 1);
                casterMatrix[i].resize(MATRIX_SIZE + 1);
                for(int j = 0; j < MATRIX_SIZE + 1; j++){
                    skeletonMatrix[i][j].resize(MATRIX_SIZE + 1);
                    wheelMatrix[i][j].resize(MATRIX_SIZE + 1);
                    sensorMatrix[i][j].resize(MATRIX_SIZE + 1);
                    jointMatrix[i][j].resize(MATRIX_SIZE + 1);
                    casterMatrix[i][j].resize(MATRIX_SIZE + 1);
                    for(int k = 0; k < MATRIX_SIZE + 1; k++){
                        skeletonMatrix[i][j][k] = false;
                        wheelMatrix[i][j][k] = false;
                        sensorMatrix[i][j][k] = false;
                        jointMatrix[i][j][k] = false;
                        casterMatrix[i][j][k] = false;
                    }
                }
            }
        }
    };

    class IntersectionDesc{
    public:
        std::vector<std::vector<std::vector<bool>>> wheelInter;
        std::vector<std::vector<std::vector<bool>>> sensorInter;
        std::vector<std::vector<std::vector<bool>>> jointInter;
        std::vector<std::vector<std::vector<bool>>> casterInter;
        // Constructor
        IntersectionDesc(){
            wheelInter.resize(MATRIX_SIZE + 1);
            sensorInter.resize(MATRIX_SIZE + 1);
            jointInter.resize(MATRIX_SIZE + 1);
            casterInter.resize(MATRIX_SIZE + 1);
            for(int i = 0; i < MATRIX_SIZE + 1; i++){
                wheelInter[i].resize(MATRIX_SIZE + 1);
                sensorInter[i].resize(MATRIX_SIZE + 1);
                jointInter[i].resize(MATRIX_SIZE + 1);
                casterInter[i].resize(MATRIX_SIZE + 1);
                for(int j = 0; j < MATRIX_SIZE + 1; j++){
                    wheelInter[i][j].resize(MATRIX_SIZE + 1);
                    sensorInter[i][j].resize(MATRIX_SIZE + 1);
                    jointInter[i][j].resize(MATRIX_SIZE + 1);
                    casterInter[i][j].resize(MATRIX_SIZE + 1);
                    for(int k = 0; k < MATRIX_SIZE + 1; k++){
                        wheelInter[i][j][k] = false;
                        sensorInter[i][j][k] = false;
                        jointInter[i][j][k] = false;
                        casterInter[i][j][k] = false;
                    }
                }
            }
        }
    };

    class Descriptors{
    public:
        CartDesc cartDesc;
        MatDesc matDesc;
        CPPNBinaryDesc cppnBinDesc;
        IntersectionDesc interDesc;
    };

    /////////////////////////////
    ///// Create morphology /////
    /////////////////////////////
    /**
     * @brief Decodes the genome (CPPN --> Matrix)
     */
    void genomeDecoder(PolyVox::RawVolume<AREVoxel>& areMatrix, NEAT::NeuralNetwork &cppn);
    /**
     * @brief This method gets all the verices and indices of the mesh generated by PolyVox.
     * This list is imported to V-REP.
     */
    bool getIndicesVertices(PolyVox::Mesh<PolyVox::Vertex<uint8_t>>& decodedMesh, std::vector<float>& vertices, std::vector<int>& indices);
    /**
     * @brief Create the head organ. This should be the first organ created.
     */
    void createHead();
    /**
     * @brief This method generates the complete skeleton without alterations of the robot from the matrix of voxels.
     * \todo This method might not be necessary!
     */
    void generateSkeleton(PolyVox::RawVolume<AREVoxel>& areMatrix, PolyVox::RawVolume<uint8_t>& skeletonMatrix, VoxelType _voxelType);
    /**
     * @brief This method loads model, creates force sensor sets position and orientation and assigns parent.
     */
    void createOrgan(OrganSpec& organ);
    /**
     * @brief Creates a temporal gripper. The isOrganColliding method checks this gripper is not colliding.
     */
    void createTemporalGripper(OrganSpec& organ);
    /**
     * @brief Takes the last three outputs of the network and converts them into rotations (radians) in x, y and z.
     * \todo EB: This method needs updating. Only one output required.
     */
    void setOrganOrientation(NEAT::NeuralNetwork &cppn, OrganSpec& organ);
    /**
     * @brief Creates a model to only visualize the vale connector
     */
    void createMaleConnector(OrganSpec& organ);

    ////////////////////////////////////
    /////  Manufacturability tests /////
    ////////////////////////////////////
    /**
     * @brief This method check if a specific organ (organHandle) is colliding with other components.ding
     */
    bool IsOrganColliding(OrganSpec& organ);
    /**
     * @brief This method checks if the organ is within the skeleton.
     * The main difference between IsOrganColloding and IsOrganConnected is that IsOrganColliding does not check
     * if one component is inside of a second component.
     */
    bool IsOrganInsideSkeleton(PolyVox::RawVolume<uint8_t>& skeletonMatrix, int organHandle);
    /**
     * @brief Checks if an organ has the correct orientation.
     * \todo EB: This method this updating. New method only requires one rotation.
     */
    bool isOrganGoodOrientation(OrganSpec& organ);
    /**
     * Checks if the gripper for the specified organ is colliding.
     */
    bool isGripperColliding(int gripperHandle);

    ////////////////////////////////////////
    ///// Fixing phenotype techniques //////
    ////////////////////////////////////////
    /**
     * @brief This methods makes space for the Head Organ by removing voxels.
     * @param skeletonMatrix Skeleton Matrix
     * \todo Define skeleton matrix
     */
    void emptySpaceForHead(PolyVox::RawVolume<uint8_t>& skeletonMatrix);

    void createSkeletonBase(PolyVox::RawVolume<uint8_t>& skeletonMatrix);

    //////////////////////////////////////
    ///// Manufacturability methods //////
    /////////////////////////////////////

    /**
     * @brief This method tests that robot as a whole.
     */
    void testRobot(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    /**
     * @brief Tests each component (organ) in the robot.
     */
    void testComponents(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    /**
     * @brief If a compoenent fails any manufacturability test that component is removed from the final ns.
     */
    void geneRepression();

    /// These following two methods are not being used for the PPSN experiments...
    void removeRobot();

    void manufacturabilityScore();

    //////////////////////////////////
    ///// Miscellanous functions /////
    //////////////////////////////////
    /**
     * @brief Export mesh file (stl) from a list of vertices and indices.
     * \todo EB: We might not want this method here and this should be in logging instead.
     */
    void exportMesh(int loadInd, std::vector<float> vertices, std::vector<int> indices);
    /**
     * @brief Export the robot as ttm model
     */
    void exportRobotModel(int indNum);
    /**
     * @brief Load specific genome
     */
    void loadMorphologyGenome(int indNum);
    /**
     * @brief This method renders the matrix of a specific organ. Useful for debugging.
     */
    void tempVisualizeMatrix(NEAT::NeuralNetwork &neuralNetwork, VoxelType _voxelType, float posX, float posY, float posZ);
    /**
     * @brief Returns the number of organs for a specific organ type.
     */
    int countOrgans(int organType);

    ///////////////////////////
    ///// Surface methods /////
    ///////////////////////////
    /**
     * @brief This method is used to explore the entire skeleton regions in order to find the voxels adjecent to the
     * surface.
     */
    void exploreSkeleton(PolyVox::RawVolume<uint8_t>& skeletonMatrix, PolyVox::RawVolume<bool>& visitedVoxels, int32_t posX, int32_t posY, int32_t posZ, int surfaceCounter);
    /**
     * @brief This recursive method records all the voxels adjecent to the surface of the skeleton.
     * @param skeletonMatrix
     */
    void findSkeletonSurface(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    /**
     * @brief This method generates organs only in specific regions of the surface of the skeleton according to the cppn.
     */
    void generateOrgans(NEAT::NeuralNetwork &cppn);
    /**
     * @brief This method generates the orientation of the organ accoriding to the "normal" of the surface
     * In reality, it takes the position of the last outer voxel. In other words, it generates orientations in
     * intervals of 45o.
     */
    void generateOrientations(int x, int y, int z, OrganSpec& organ);

    /////////////////////////////////////////////
    ///// Methods handling skeleton regions /////
    /////////////////////////////////////////////
    /**
     * @brief This method counts the number of regions in the skeletonMatrix
     * @param skeletonMatrix
     */
    void skeletonRegionCounter(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    /**
     * @brief This method explore the skeleton matrix
     */
    void exploreSkeletonRegion(PolyVox::RawVolume<uint8_t>& skeletonMatrix, PolyVox::RawVolume<bool>& visitedVoxels, int32_t posX, int32_t posY, int32_t posZ, int regionCounter);
    /**
     * @brief If there is more than one region in the skeleton delete the smaller regions.
     */
    void removeSkeletonRegions(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    /**
     * @brief This method explore the organs matrix
     */
    void organRegionCounter(PolyVox::RawVolume<AREVoxel>& areMatrix, VoxelType voxelType);
    /**
     * @brief This method explore the skeleton matrix
     */
    void exploreOrganRegion(PolyVox::RawVolume<AREVoxel>& areMatrix, PolyVox::RawVolume<bool>& visitedVoxels, int32_t posX, int32_t posY, int32_t posZ, int regionCounter, VoxelType voxelType);

    /**
     * @brief Get the width, depth and  height of the skeleton
     */
    std::vector<float> getSkeletonDimmensions(PolyVox::RawVolume<uint8_t>& skeletonMatrix);

    static void removeGripper(int gripperHandle);

    void createAREPuck(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    void createAREPotato(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    void createARETricyle(PolyVox::RawVolume<uint8_t>& skeletonMatrix);

    ///////////////////////////////
    ///// Setters and getters /////
    ///////////////////////////////
    std::vector<bool> getRobotManRes(){return robotManRes.getResVector();};
    NEAT::NeuralNetwork getGenome(){return nn;};
    void setGenome(NEAT::NeuralNetwork genome){nn = genome;};
    std::vector<std::vector<float>> getRawMatrix(){return rawMatrix;};
    double getManScore(){return manScore;};
    void setManScore(double ms){ manScore = ms;};

    /// Getters for descriptors.
    /// \todo EB: There must be a better way to retrieve descriptor. Perhaps as the descritor as a whole?
    Eigen::VectorXd getMorphDesc(){return indDesc.cartDesc.cartDesc;};
    std::vector<std::vector<std::vector<int>>> getGraphMatrix(){return indDesc.matDesc.graphMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolSkeletonMatrix(){return indDesc.cppnBinDesc.skeletonMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolWheelMatrix(){return indDesc.cppnBinDesc.wheelMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolSensorMatrix(){return indDesc.cppnBinDesc.sensorMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolJointMatrix(){return indDesc.cppnBinDesc.jointMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolCasterMatrix(){return indDesc.cppnBinDesc.casterMatrix;};
    std::vector<std::vector<std::vector<bool>>> getInterWheelMatrix(){return indDesc.interDesc.wheelInter;};
    std::vector<std::vector<std::vector<bool>>> getInterSensorMatrix(){return indDesc.interDesc.sensorInter;};
    std::vector<std::vector<std::vector<bool>>> getInterJointMatrix(){return indDesc.interDesc.jointInter;};
    std::vector<std::vector<std::vector<bool>>> getInterCasterMatrix(){return indDesc.interDesc.casterInter;};

protected:
    void getObjectHandles() override;

private:
    NEAT::NeuralNetwork nn;

    /////////////////////
    ///// Constants /////
    /////////////////////
    constexpr static const float VOXEL_SIZE = 0.0009; //m³ - 0.9mm³
    // WAS 4 -> 3.6mm
    // 6 -> 5.4mm
    // 11 -> 9.9mm (EB: with this value there is no stack overflow!)
    static const int VOXEL_MULTIPLIER = 22;
    constexpr static const float VOXEL_REAL_SIZE = VOXEL_SIZE * static_cast<float>(VOXEL_MULTIPLIER);
    static const int MATRIX_SIZE = (264 / VOXEL_MULTIPLIER);
    const int MATRIX_HALF_SIZE = MATRIX_SIZE / 2;
    const float SHAPE_SCALE_VALUE = VOXEL_REAL_SIZE; // results into 23.76x23.76x23.76 cm³ - in reality is 28x28x25 cm³
    static const int VOXELS_NUMBER = MATRIX_SIZE * MATRIX_SIZE *MATRIX_SIZE;
    static const int MAX_NUM_ORGANS = 10;
    constexpr static const float MATRIX_SIZE_M = MATRIX_SIZE * VOXEL_REAL_SIZE;

    const int EMPTYVOXEL = 0;
    const int FILLEDVOXEL = 255;

    // Skeleton dimmesions in voxels
    const int xHeadUpperLimit = 2;
    const int xHeadLowerLimit = -2;
    const int yHeadUpperLimit = 2;
    const int yHeadLowerLimit = -2;
    // Parameters for skeleton base
    const int skeletonBaseThickness = 1;
    const int skeletonBaseHeight = 2;

    unsigned int id;
    // Variables used to contain handles.
    std::vector<OrganSpec> _organSpec;
    int numSkeletonVoxels;

    std::vector<std::vector<std::vector<int>>> skeletonSurfaceCoord;
    std::vector<std::vector<std::vector<int>>> skeletonRegionCoord;

    std::vector<std::vector<std::vector<int>>> wheelRegionCoord;
    std::vector<std::vector<std::vector<int>>> sensorRegionCoord;
    std::vector<std::vector<std::vector<int>>> jointRegionCoord;
    std::vector<std::vector<std::vector<int>>> casterRegionCoord;

    std::vector<std::vector<float>> rawMatrix;

    double manScore;

    // Handles used by the controller
    std::vector<int> jointHandles;
    std::vector<int> proxHandles;

    Descriptors indDesc;
};

}

#endif //MORPHOLOGY_CPPNMATRIX_H
