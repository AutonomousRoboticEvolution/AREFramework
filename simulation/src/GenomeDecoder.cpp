//
// Created by ebb505 on 16/03/2021.
//

//
// Created by ebb505 on 12/03/2021.
//
#include "simulatedER/GenomeDecoder.h"

/// \todo EB: Do i need this?
//using namespace are;
//namespace cop = coppelia;

using namespace are::sim;

void GenomeDecoder::decodeGenome(PolyVox::RawVolume<AREVoxel>& areMatrix, NEAT::NeuralNetwork &cppn)
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
                areVoxel.bone = morph_const::empty_voxel;

                if(cppn.Output()[1] > 0) {
                    areVoxel.bone = morph_const::filled_voxel;
                }
                areMatrix.setVoxel(x, y, z, areVoxel);
            }
        }
    }
}

void GenomeDecoder::decodeGenome(PolyVox::RawVolume<AREVoxel>& areMatrix, nn2_cppn_t &cppn)
{
    std::vector<double> input{0,0,0,0}; // Vector used as input of the Neural Network (NN).
    std::vector<double> outputs;
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

                // Activate NN
                cppn.step(input);
                outputs = cppn.outf();
                // Take output from NN and store it.
                areVoxel.bone = morph_const::empty_voxel;
                if(outputs[1] > 0) {
                    areVoxel.bone = morph_const::filled_voxel;
                }
                areMatrix.setVoxel(x, y, z, areVoxel);
            }
        }
    }
}

void GenomeDecoder::decodeGenome(PolyVox::RawVolume<AREVoxel>& areMatrix, std::vector<std::vector<double>> &matrix_4d)
{
    AREVoxel areVoxel;
    int cell_counter = 0;
    // Generate voxel matrix
    auto region = areMatrix.getEnclosingRegion();
    for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
                // Take output from NN and store it.
                areVoxel.bone = morph_const::empty_voxel;
                if(matrix_4d.at(1).at(cell_counter) > 0.) {
                    areVoxel.bone = morph_const::filled_voxel;
                }
                areMatrix.setVoxel(x, y, z, areVoxel);
                cell_counter++;
            }
        }
    }
}

void GenomeDecoder::generateSkeleton(PolyVox::RawVolume<AREVoxel> &areMatrix, PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels)
{
    AREVoxel areVoxel;
    uint8_t uVoxelValue;
    uint8_t voxel;
    auto region = skeletonMatrix.getEnclosingRegion();
    bool isSkeletonConnected = false;

    for(int32_t z = region.getLowerZ()+1; z < region.getLowerZ() + morph_const::skeletonBaseHeight; z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness && x >= morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness &&
                   y <= morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness && y >= morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness){
                    if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit &&
                       y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit){
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
                        uVoxelValue = morph_const::filled_voxel;
                        numSkeletonVoxels++;
                    }
                    else
                        uVoxelValue = morph_const::empty_voxel;

                    skeletonMatrix.setVoxel(x, y, z, uVoxelValue);
                }
            }
        }
    }
}

void GenomeDecoder::createSkeletonBase(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels)
{
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getLowerZ() + morph_const::skeletonBaseHeight; z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness && x >= morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness &&
                   y <= morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness && y >= morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness){ // Additive condition
                    if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit &&
                       y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit){ // Substractive condition
                        if(skeletonMatrix.getVoxel(x, y, z) != morph_const::empty_voxel){
                            skeletonMatrix.setVoxel(x, y, z, morph_const::empty_voxel);
                            numSkeletonVoxels--;
                        }
                    }
                    else{
                        if(skeletonMatrix.getVoxel(x, y, z) != morph_const::filled_voxel){
                            skeletonMatrix.setVoxel(x, y, z, morph_const::filled_voxel);
                            numSkeletonVoxels++;
                        }
                    }
                }
            }
        }
    }
}

void GenomeDecoder::emptySpaceForHead(PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels)
{
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit && y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit){
                    if(skeletonMatrix.getVoxel(x,y,z) == morph_const::filled_voxel){
                        skeletonMatrix.setVoxel(x, y, z, morph_const::empty_voxel);
                        numSkeletonVoxels--;
                    }
                }
            }
        }
    }
}

void GenomeDecoder::skeletonRegionCounter(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    // This matrix stores the visited elements.
    PolyVox::RawVolume<bool > visitedVoxels(PolyVox::Region(PolyVox::Vector3DInt32(-morph_const::matrix_size/2, -morph_const::matrix_size/2, -morph_const::matrix_size/2), PolyVox::Vector3DInt32(morph_const::matrix_size/2, morph_const::matrix_size/2, morph_const::matrix_size/2)));
    uint8_t voxel;
    int regionCounter = 0;
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                voxel = skeletonMatrix.getVoxel(x, y, z);
                if(voxel == morph_const::filled_voxel && !visitedVoxels.getVoxel(x, y, z)){
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

void GenomeDecoder::removeSkeletonRegions(PolyVox::RawVolume<uint8_t> &skeletonMatrix)
{
    int regionConnected = -1;
    if(skeletonRegionCoord.size() > 1){
        for(int i = 0; i < skeletonRegionCoord.size(); i++){
            for(int j = 0; j < skeletonRegionCoord[i].size(); j++){
                if(skeletonRegionCoord[i][j][0] <= morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness &&
                   skeletonRegionCoord[i][j][0] >= morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness &&
                   skeletonRegionCoord[i][j][1] <= morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness &&
                   skeletonRegionCoord[i][j][1] >= morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness &&
                   skeletonRegionCoord[i][j][2] <= -6 + morph_const::skeletonBaseHeight){ /// \todo :EB make this a constant!
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

void GenomeDecoder::exploreSkeleton(PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                    PolyVox::RawVolume<bool> &visitedVoxels, int32_t posX, int32_t posY,
                                    int32_t posZ, int surfaceCounter, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord)
{
    visitedVoxels.setVoxel(posX, posY, posZ, true); // Cell visited
    uint8_t voxel;
    // Explore neighbourhood.
    for (int dz = -1; dz <= 1; dz+=1) {
        if (posZ + dz > -morph_const::matrix_size/2 && posZ + dz < morph_const::matrix_size/2) {
            voxel = skeletonMatrix.getVoxel(posX, posY, posZ + dz);
            if (!visitedVoxels.getVoxel(posX, posY, posZ + dz) && voxel == morph_const::filled_voxel) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX, posY, posZ + dz, surfaceCounter, skeletonSurfaceCoord);
            }
            /// EB: Organ pointing upwards or downwards (z-axis) are not allowed by the RoboFab.
        }
    }
    for (int dy = -1; dy <= 1; dy+=1) {
        if (posY + dy > -morph_const::matrix_size/2 && posY + dy < morph_const::matrix_size/2) {
            voxel = skeletonMatrix.getVoxel(posX, posY + dy, posZ);
            if (!visitedVoxels.getVoxel(posX, posY + dy, posZ) && voxel == morph_const::filled_voxel) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX, posY + dy, posZ, surfaceCounter, skeletonSurfaceCoord);
            }
            else if(!visitedVoxels.getVoxel(posX, posY + dy, posZ) && voxel == morph_const::empty_voxel) {
                std::vector<int> newCoord{posX, posY, posZ, 0, dy, 0};
                skeletonSurfaceCoord[surfaceCounter-1].push_back(newCoord);
            }
        }
    }
    for (int dx = -1; dx <= 1; dx+=1) {
        if (posX + dx > -morph_const::matrix_size/2 && posX + dx < morph_const::matrix_size/2) {
            voxel = skeletonMatrix.getVoxel(posX + dx, posY, posZ);
            if (!visitedVoxels.getVoxel(posX + dx, posY, posZ) && voxel == morph_const::filled_voxel) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX + dx, posY, posZ, surfaceCounter, skeletonSurfaceCoord);
            }
            else if(!visitedVoxels.getVoxel(posX + dx, posY, posZ) && voxel == morph_const::empty_voxel) {
                std::vector<int> newCoord{posX, posY, posZ, dx, 0, 0};
                skeletonSurfaceCoord[surfaceCounter-1].push_back(newCoord);
            }
        }
    }
}

void GenomeDecoder::exploreSkeletonRegion(PolyVox::RawVolume<uint8_t> &skeletonMatrix, PolyVox::RawVolume<bool> &visitedVoxels,
                                          int32_t posX, int32_t posY,int32_t posZ, int regionCounter)
{
    visitedVoxels.setVoxel(posX, posY, posZ, true); // Cell visited
    uint8_t voxel;
    // Explore neighbourhood.
    for (int dz = -1; dz <= 1; dz+=1) {
        for (int dy = -1; dy <= 1; dy+=1) {
            for (int dx = -1; dx <= 1; dx+=1) {
                if (posX + dx > -morph_const::matrix_size/2 && posX + dx < morph_const::matrix_size/2 &&
                    posY + dy > -morph_const::matrix_size/2 && posY + dy < morph_const::matrix_size/2 &&
                    posZ + dz > -morph_const::matrix_size/2 && posZ + dz < morph_const::matrix_size/2) {
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

void GenomeDecoder::findSkeletonSurface(PolyVox::RawVolume<uint8_t> &skeletonMatrix, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord)
{
    // This matrix stores the visited elements.
    PolyVox::RawVolume<bool > visitedVoxels(PolyVox::Region(PolyVox::Vector3DInt32(-morph_const::matrix_size/2, -morph_const::matrix_size/2, -morph_const::matrix_size/2), PolyVox::Vector3DInt32(morph_const::matrix_size/2, morph_const::matrix_size/2, morph_const::matrix_size/2)));
    uint8_t voxel;
    int surfaceCounter = 0;
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 2) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 2) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 2) {
                voxel = skeletonMatrix.getVoxel(x, y, z);
                if(voxel == morph_const::filled_voxel && !visitedVoxels.getVoxel(x, y, z)){
                    surfaceCounter++;
                    skeletonSurfaceCoord.resize(surfaceCounter);
                    exploreSkeleton(skeletonMatrix, visitedVoxels, x, y, z, surfaceCounter, skeletonSurfaceCoord);
                }
            }
        }
    }
}

void GenomeDecoder::genomeDecoder(NEAT::NeuralNetwork &cppn, PolyVox::RawVolume<AREVoxel> &areMatrix,
                                 PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                 std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord,
                                 int &numSkeletonVoxels)
{
    decodeGenome(areMatrix, cppn);
    generateSkeleton(areMatrix, skeletonMatrix, numSkeletonVoxels);
    createSkeletonBase(skeletonMatrix, numSkeletonVoxels);
    emptySpaceForHead(skeletonMatrix, numSkeletonVoxels);
    skeletonRegionCounter(skeletonMatrix);
    removeSkeletonRegions(skeletonMatrix);
    findSkeletonSurface(skeletonMatrix, skeletonSurfaceCoord);
}

void GenomeDecoder::genomeDecoder(nn2_cppn_t &cppn, PolyVox::RawVolume<AREVoxel> &areMatrix,
                                 PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                 std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord,
                                 int &numSkeletonVoxels)
{
    decodeGenome(areMatrix, cppn);
    generateSkeleton(areMatrix, skeletonMatrix, numSkeletonVoxels);
    createSkeletonBase(skeletonMatrix, numSkeletonVoxels);
    emptySpaceForHead(skeletonMatrix, numSkeletonVoxels);
    skeletonRegionCounter(skeletonMatrix);
    removeSkeletonRegions(skeletonMatrix);
    findSkeletonSurface(skeletonMatrix, skeletonSurfaceCoord);
}

void GenomeDecoder::genomeDecoder(std::vector<std::vector<double>> &matrix_4d, PolyVox::RawVolume<AREVoxel> &areMatrix,
                                  PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                  std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord,
                                  int &numSkeletonVoxels)
{
    decodeGenome(areMatrix, matrix_4d);
    generateSkeleton(areMatrix, skeletonMatrix, numSkeletonVoxels);
    createSkeletonBase(skeletonMatrix, numSkeletonVoxels);
    emptySpaceForHead(skeletonMatrix, numSkeletonVoxels);
    skeletonRegionCounter(skeletonMatrix);
    removeSkeletonRegions(skeletonMatrix);
    findSkeletonSurface(skeletonMatrix, skeletonSurfaceCoord);
}
