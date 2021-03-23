//
// Created by ebb505 on 16/03/2021.
//

#ifndef ER_GENOMEDECODER_H
#define ER_GENOMEDECODER_H

#include "PolyVox/RawVolume.h"
#include <multineat/Substrate.h>
#include <vector>

namespace are {

namespace sim {

struct AREVoxel{
    uint8_t bone;
    uint8_t wheel;
    uint8_t sensor;
    uint8_t joint;
    uint8_t caster;
};

class GenomeDecoder{
private:
    /// \todo EB: This shouldn't be here!
    static const short int EMPTYVOXEL = 0;
    static const short int FILLEDVOXEL = 255;
    // Skeleton dimmesions in voxels
    static const short int xHeadUpperLimit = 2;
    static const short int xHeadLowerLimit = -2;
    static const short int yHeadUpperLimit = 2;
    static const short int yHeadLowerLimit = -2;
    // Parameters for skeleton base
    static const short int skeletonBaseThickness = 1;
    static const short int skeletonBaseHeight = 2;
    static const int VOXEL_MULTIPLIER = 22;
    static const int MATRIX_SIZE = (264 / VOXEL_MULTIPLIER);
    static const int MATRIX_HALF_SIZE = MATRIX_SIZE / 2;

    std::vector<std::vector<std::vector<int>>> skeletonRegionCoord;

public:
    /**
     * @brief Runs all the steps to decode the genome.
     * @param cppn - Body plan genome
     * @param areMatrix - All regions for each organ.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param skeletonSurfaceCoord - Voxels on the surface of the skeleton.
     * @param numSkeletonVoxels - Keeps track of the number of voxels (for descriptor purposes).
     */
    void genomeDecoder(NEAT::NeuralNetwork &cppn, PolyVox::RawVolume<AREVoxel>& areMatrix, PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                      std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord, int &numSkeletonVoxels);
    /**
     * @brief Reads the genome and creates the regions for each organ.
     * @param areMatrix - All regions for each organ.
     * @param cppn - Body plan genome
     */
    static void decodeGenome(PolyVox::RawVolume<AREVoxel>& areMatrix, NEAT::NeuralNetwork &cppn);
    /**
     * @brief Creates the skeleton.
     * @param areMatrix - All regions for each organ.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param numSkeletonVoxels - Keeps track of the number of voxels (for descriptor purposes).
     */
    static void generateSkeleton(PolyVox::RawVolume<AREVoxel> &areMatrix, PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels);
    /**
     * @brief This creates the skeleton seed in the main skeleton.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param numSkeletonVoxels - Keeps track of the number of voxels (for descriptor purposes).
     */
    static void createSkeletonBase(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels);
    /**
     * @brief Creates an empt space for the gripper to connect the head organ.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param numSkeletonVoxels - Keeps track of the number of voxels (for descriptor purposes).
     */
    static void emptySpaceForHead(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels);
    /**
     * @brief Counts the number of isolated of skeleton regions.
     * @param skeletonMatrix - Main skeleton matrix.
     */
    void skeletonRegionCounter(PolyVox::RawVolume<uint8_t> &skeletonMatrix);
    /**
     * @brief Remove smaller skeleton bits.
     * @param skeletonMatrix - Main skeleton matrix.
     */
    void removeSkeletonRegions(PolyVox::RawVolume<uint8_t> &skeletonMatrix);
    /**
     * @brief Recursive method.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param visitedVoxels - Tracks the voxels that have been visited.
     * @param posX - Current position x in the matrix.
     * @param posY - Current position y in the matrix.
     * @param posZ - Current position z in the matrix.
     * @param surfaceCounter - Number of surfaces.
     * @param skeletonSurfaceCoord
     */
    void exploreSkeleton(PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                                PolyVox::RawVolume<bool> &visitedVoxels, int32_t posX, int32_t posY,
                                                int32_t posZ, int surfaceCounter, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord);
    /**
     * @brief Explore skeleton region.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param visitedVoxels - Tracks the voxels that have been visited.
     * @param posX - Current position x in the matrix.
     * @param posY - Current position y in the matrix.
     * @param posZ - Current position z in the matrix.
     * @param regionCounter - Number of regions
     */
    void exploreSkeletonRegion(PolyVox::RawVolume<uint8_t> &skeletonMatrix, PolyVox::RawVolume<bool> &visitedVoxels,
                                                        int32_t posX, int32_t posY,int32_t posZ, int regionCounter);
    /**
     * @brief Find the voxels on the surface.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param skeletonSurfaceCoord -
     */
    void findSkeletonSurface(PolyVox::RawVolume<uint8_t> &skeletonMatrix, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord);
};


}//sim

}//are


#endif //ER_GENOMEDECODER_H
