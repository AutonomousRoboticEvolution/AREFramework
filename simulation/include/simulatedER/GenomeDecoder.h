//
// Created by ebb505 on 16/03/2021.
//

#ifndef ER_GENOMEDECODER_H
#define ER_GENOMEDECODER_H

#include "PolyVox/RawVolume.h"
#include <vector>
#include "simulatedER/nn2/NN2CPPNGenome.hpp"
#include "simulatedER/nn2/sq_cppn_genome.hpp"

namespace are {

namespace sim {



typedef PolyVox::RawVolume<uint8_t> skeleton_matrix_t;

class GenomeDecoder{
private:
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
    void genomeDecoder(nn2_cppn_t &cppn, PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                       std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord, int &numSkeletonVoxels);

    void genomeDecoderGrowth(nn2_cppn_t &cppn, PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                       std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord, int &numSkeletonVoxels);

    void superquadricsDecoder(quadric_t<quadric_params> &quadric, nn2_cppn_t &cppn, PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                              std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord, int &numSkeletonVoxels);

    /**
     * @brief Reads the cppn from nn2 and creates the regions for each organ.
     * @param areMatrix - All regions for each organ.
     * @param cppn - Body plan genome
     */
    static void decodeGenome(PolyVox::RawVolume<uint8_t> &skeleton, nn2_cppn_t &cppn);

    static void assignSkeletonVoxel(int32_t x, int32_t y, int32_t z, PolyVox::RawVolume<uint8_t> &skeleton, nn2_cppn_t &cppn);

    static void growthBasedSkeletonGeneration(PolyVox::RawVolume<uint8_t> &skeleton, nn2_cppn_t &cppn);

    static void superquadricSkeletonGeneration(PolyVox::RawVolume<uint8_t> &skeleton, quadric_t<quadric_params> &quadric);

    static void countNumberSkeletonVoxels(const PolyVox::RawVolume<uint8_t> &skeleton,int &numSkeletonVoxels);

    /**
     * @brief Creates the skeleton.
     * @param areMatrix - All regions for each organ.
     * @param skeletonMatrix - Main skeleton matrix.
     * @param numSkeletonVoxels - Keeps track of the number of voxels (for descriptor purposes).
     */
    static void generateSkeleton(const PolyVox::RawVolume<uint8_t> &input_skeleton, PolyVox::RawVolume<uint8_t> &output_skeleton, int &numSkeletonVoxels);
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
    static void exploreSkeleton(const PolyVox::RawVolume<uint8_t> &skeletonMatrix,
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
    static void findSkeletonSurface(const PolyVox::RawVolume<uint8_t> &skeletonMatrix, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord);

    /**
     * @brief find any overhangs greater than one voxel and remove the overhanging voxels
     * @param skeletonMatrix- Main skeleton matrix
     */
    void removeOverhangs(PolyVox::RawVolume<uint8_t> &skeletonMatrix);
};


}//sim

}//are


#endif //ER_GENOMEDECODER_H
