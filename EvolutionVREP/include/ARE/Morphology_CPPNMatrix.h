#ifndef Morphology_CPPNMatrix_H
#define Morphology_CPPNMatrix_H

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
            setCartDesc();
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

    class SymDesc{
    public:
        unsigned short int voxelQ1, voxelQ2, voxelQ3, voxelQ4;
        unsigned short int wheelQ1, wheelQ2, wheelQ3, wheelQ4;
        unsigned short int sensorQ1, sensorQ2, sensorQ3, sensorQ4;
        unsigned short int jointQ1, jointQ2, jointQ3, jointQ4;
        unsigned short int casterQ1, casterQ2, casterQ3, casterQ4;
        Eigen::VectorXd symDesc;
        SymDesc(){
            symDesc.resize(20);
            voxelQ1 = 0; voxelQ2 = 0; voxelQ3 = 0; voxelQ4 = 0;
            wheelQ1 = 0; wheelQ2 = 0; wheelQ3 = 0; wheelQ4 = 0;
            sensorQ1 = 0; sensorQ2 = 0; sensorQ3 = 0; sensorQ4 = 0;
            jointQ1 = 0; jointQ2 = 0; jointQ3 = 0; jointQ4 = 0;
            casterQ1 = 0; casterQ2 = 0; casterQ3 = 0; casterQ4 = 0;
            setSymDesc();
        }
        void setSymDesc(){
            symDesc(0) = voxelQ1; symDesc(1) = voxelQ2; symDesc(2) = voxelQ3; symDesc(3) = voxelQ4;
            symDesc(4) = wheelQ1; symDesc(5) = wheelQ2; symDesc(6) = wheelQ3; symDesc(7) = wheelQ4;
            symDesc(8) = sensorQ1; symDesc(9) = sensorQ2; symDesc(10) = sensorQ3; symDesc(11) = sensorQ4;
            symDesc(12) = jointQ1; symDesc(13) = jointQ2; symDesc(14) = jointQ3; symDesc(15) = jointQ4;
            symDesc(16) = casterQ1; symDesc(17) = casterQ2; symDesc(18) = casterQ3; symDesc(19) = casterQ4;
        }
    };

    class Descriptors{
    public:
        CartDesc cartDesc;
        MatDesc matDesc;
        SymDesc symDesc;
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

    void getFinalSkeletonVoxels(PolyVox::RawVolume<uint8_t>& skeletonMatrix);
    void setVoxelQuadrant(short signed int x, short signed int y, short unsigned int componentType);

    ///////////////////////////////
    ///// Setters and getters /////
    ///////////////////////////////
    std::vector<bool> getRobotManRes(){return robotManRes.getResVector();};
    NEAT::NeuralNetwork getGenome(){return nn;};
    void setGenome(NEAT::NeuralNetwork genome){nn = genome;};
    double getManScore(){return manScore;};
    void setManScore(double ms){ manScore = ms;};

    /// Getters for descriptors.
    /// \todo EB: There must be a better way to retrieve descriptor. Perhaps as the descritor as a whole?
    Eigen::VectorXd getMorphDesc(){return indDesc.cartDesc.cartDesc;};
    std::vector<std::vector<std::vector<int>>> getGraphMatrix(){return indDesc.matDesc.graphMatrix;};
    Eigen::VectorXd getSymDesc(){return indDesc.symDesc.symDesc;};

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

    double manScore;

    // Handles used by the controller
    std::vector<int> jointHandles;
    std::vector<int> proxHandles;

    Descriptors indDesc;
};

}

#endif //Morphology_CPPNMatrix_H
