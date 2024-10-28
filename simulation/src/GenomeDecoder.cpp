//
// Created by ebb505 on 16/03/2021.
//

#include "simulatedER/GenomeDecoder.h"

/// \todo EB: Do i need this?
//using namespace are;
//namespace cop = coppelia;

using namespace are::sim;



void GenomeDecoder::assignSkeletonVoxel(int32_t x, int32_t y, int32_t z, PolyVox::RawVolume<AREVoxel> &areMatrix, nn2_cppn_t &cppn){

    //- Test if the coordinate are out of bound, if yes exit
    PolyVox::Region region = areMatrix.getEnclosingRegion();
    if(x < region.getLowerX() || x > region.getUpperX() ||
       y < region.getLowerY() || y > region.getUpperX() ||
       z < region.getLowerZ() || z > region.getUpperX())
        return;
    //- check if this voxel is aready assigned
    if(areMatrix.getVoxel(x,y,z).bone != 128)
        return;


    std::vector<double> input{ static_cast<double>(x),
                                   static_cast<double>(y),
                                   static_cast<double>(z),
                                   static_cast<double>(sqrt(pow(x,2)+pow(y,2)+pow(z,2)))
    }; // Vector used as input of the Neural Network (NN)
    cppn.step(input);
    AREVoxel voxel;
    if(cppn.outf()[1] > 0.000001){ //test if value is positive
        voxel.bone = morph_const::filled_voxel;
        areMatrix.setVoxel(x,y,z,voxel);
        assignSkeletonVoxel(x+1,y,z,areMatrix,cppn);
        assignSkeletonVoxel(x-1,y,z,areMatrix,cppn);
        assignSkeletonVoxel(x,y+1,z,areMatrix,cppn);
        assignSkeletonVoxel(x,y-1,z,areMatrix,cppn);
        assignSkeletonVoxel(x,y,z+1,areMatrix,cppn);
        assignSkeletonVoxel(x,y,z-1,areMatrix,cppn);
    }else{
        voxel.bone = morph_const::empty_voxel;
        areMatrix.setVoxel(x,y,z,voxel);
    }
}



void GenomeDecoder::growthBasedSkeletonGeneration(PolyVox::RawVolume<AREVoxel>& areMatrix, nn2_cppn_t &cppn){
    std::cout << "Growth of skeleton" << std::endl;
    PolyVox::Region region = areMatrix.getEnclosingRegion();
    int32_t centre_x = region.getCentreX();
    int32_t centre_y = region.getCentreY();
    int32_t centre_z = region.getCentreZ();
    //Starts from the voxels in the center:
    assignSkeletonVoxel(centre_x,centre_y,centre_z,areMatrix,cppn);
    assignSkeletonVoxel(centre_x+1,centre_y,centre_z,areMatrix,cppn);
    assignSkeletonVoxel(centre_x-1,centre_y,centre_z,areMatrix,cppn);
    assignSkeletonVoxel(centre_x,centre_y+1,centre_z,areMatrix,cppn);
    assignSkeletonVoxel(centre_x,centre_y-1,centre_z,areMatrix,cppn);
    assignSkeletonVoxel(centre_x,centre_y,centre_z+1,areMatrix,cppn);
    assignSkeletonVoxel(centre_x,centre_y,centre_z-1,areMatrix,cppn);

    std::vector<std::tuple<int32_t,int32_t,int32_t>> voxels_to_be_filled;

    //- Fill up holes in the skeleton and assigne empty voxel value to non assigned voxels
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                AREVoxel voxel = areMatrix.getVoxel(x,y,z);
                if(voxel.bone == 128){
                    voxel.bone = morph_const::empty_voxel;
                    areMatrix.setVoxel(x,y,z,voxel);}
//                }else if(voxel.bone == morph_const::empty_voxel){
//                    int counter = 0;
//                    if(x+1 < region.getUpperX() && areMatrix.getVoxel(x+1,y,z).bone == morph_const::filled_voxel)
//                        counter++;
//                    if(x-1 > region.getLowerX() && areMatrix.getVoxel(x-1,y,z).bone == morph_const::filled_voxel)
//                        counter++;
//                    if(y+1 < region.getUpperY() && areMatrix.getVoxel(x,y+1,z).bone == morph_const::filled_voxel)
//                        counter++;
//                    if(y-1 > region.getLowerY() && areMatrix.getVoxel(x,y-1,z).bone == morph_const::filled_voxel)
//                        counter++;
//                    if(z+1 < region.getUpperZ() && areMatrix.getVoxel(x,y,z+1).bone == morph_const::filled_voxel)
//                        counter++;
//                    if(z-1 > region.getLowerZ() && areMatrix.getVoxel(x,y,z-1).bone == morph_const::filled_voxel)
//                        counter++;
//                    if(counter >= 3)
//                        voxels_to_be_filled.push_back({x,y,z});

//                }
            }
        }
    }
    for(const std::tuple<int32_t,int32_t,int32_t> &coord: voxels_to_be_filled){
        AREVoxel voxel;
        voxel.bone = morph_const::filled_voxel;
        areMatrix.setVoxel(get<0>(coord),get<1>(coord),get<2>(coord),voxel);
    }

}

void GenomeDecoder::getSkeletonFromAREMatrix(const PolyVox::RawVolume<AREVoxel> &areMatrix, PolyVox::RawVolume<uint8_t> &skeletonMatrix, int &numSkeletonVoxels){
    PolyVox::Region region = areMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                uint8_t voxel = static_cast<uint8_t>(areMatrix.getVoxel(x,y,z).bone);
                skeletonMatrix.setVoxel(x,y,z,voxel);
            }
        }
    }
}

// makes just the skeleton not the organs
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
                for(auto& o: outputs)
                    if(std::isnan(o))
                        o = 0;

                // Take output from NN and store it.
                areVoxel.bone = morph_const::empty_voxel;
                if(outputs[1] > 0.00001) { // Sometimes values are very close to zero and these causes problems.
                    areVoxel.bone = morph_const::filled_voxel;
                }
                areMatrix.setVoxel(x, y, z, areVoxel);
            }
        }
    }
}



//checks skeleton is connected to start and not in the head
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

//add voxels to create head attachment square
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

//makes empty space for head
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

//no idea what this function does
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

// remove skeleton for head
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

//generates a list surface coordinates
// recures through each direction
void GenomeDecoder::exploreSkeleton(const PolyVox::RawVolume<uint8_t> &skeletonMatrix,
                                    PolyVox::RawVolume<bool> &visitedVoxels, int32_t posX, int32_t posY,
                                    int32_t posZ, int surfaceCounter, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord)
{
    visitedVoxels.setVoxel(posX, posY, posZ, true); // Cell visited
    uint8_t voxel;
    // Explore neighbourhood.
    // finds surface on the top of robot but doesnt actually use them as organ can't be on the top
    for (int dz = -1; dz <= 1; dz+=1) {
        if (posZ + dz > -morph_const::matrix_size/2 && posZ + dz < morph_const::matrix_size/2) {
            voxel = skeletonMatrix.getVoxel(posX, posY, posZ + dz);
            if (!visitedVoxels.getVoxel(posX, posY, posZ + dz) && voxel == morph_const::filled_voxel) {
                exploreSkeleton(skeletonMatrix, visitedVoxels, posX, posY, posZ + dz, surfaceCounter, skeletonSurfaceCoord);
            }
            /// EB: Organ pointing upwards or downwards (z-axis) are not allowed by the RoboFab.
        }
    }
    // finds surfaces in the y direction
    // saves it to be facing in the y direction +/-
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
    // finds surfaces in the x direction
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

//generates a list of coords in a region
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

void GenomeDecoder::removeOverhangs(PolyVox::RawVolume<uint8_t> &skeletonMatrix){
    // loop through every voxel, starting at the second z layer (since the whole first layer is supported by the bed so none need be removed)
    auto region = skeletonMatrix.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+2; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {

                if( skeletonMatrix.getVoxel(x, y, z) == morph_const::filled_voxel ){
                    // if we are in here, then this voxel is currently filled, so we need to test if it is supported
                    bool isSupported=false;

                    // check the potentially supporting voxels, i.e. the 9 voxels below this one:
                    for (int dx=-1; dx<=1; dx+=1){ // dx is the offset in x direction, so we want to test -1, 0 and +1
                        for (int dy=-1; dy<=1; dy+=1){ // dy is the offset in y direction, so we want to test -1, 0 and +1

                            // check that the potentially supporting voxel is inside the volume (may bot be the case for voxels on the edge of the volume):
                            if( x+dx >= region.getLowerX() && x+dx <= region.getUpperX() && y+dy >= region.getLowerY() && y+dy <= region.getUpperY() ) {
                                // check if the potenitally supporting voxel is filled, in which case it is supported
                                if (skeletonMatrix.getVoxel(x+dx, y+dy, z-1) == morph_const::filled_voxel) {isSupported=true;}
                            }
                        }}

                    // if we did not find any supporting voxel, this voxel should not be filled:
                    if (!isSupported){
                        skeletonMatrix.setVoxel(x, y, z, morph_const::empty_voxel);
                    }
                }

            }}} // end of loop through every voxel
}

void GenomeDecoder::findSkeletonSurface(const PolyVox::RawVolume<uint8_t> &skeletonMatrix, std::vector<std::vector<std::vector<int>>> &skeletonSurfaceCoord)
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



void GenomeDecoder::genomeDecoderGrowth(nn2_cppn_t &cppn, PolyVox::RawVolume<AREVoxel> &areMatrix, PolyVox::RawVolume<uint8_t> &skeletonMatrix, std::vector<std::vector<std::vector<int> > > &skeletonSurfaceCoord, int &numSkeletonVoxels){
    growthBasedSkeletonGeneration(areMatrix,cppn);
    getSkeletonFromAREMatrix(areMatrix,skeletonMatrix,numSkeletonVoxels);
    createSkeletonBase(skeletonMatrix, numSkeletonVoxels);
    emptySpaceForHead(skeletonMatrix, numSkeletonVoxels);
    skeletonRegionCounter(skeletonMatrix);
    removeSkeletonRegions(skeletonMatrix);
    removeOverhangs(skeletonMatrix);
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
    removeOverhangs(skeletonMatrix);
    findSkeletonSurface(skeletonMatrix, skeletonSurfaceCoord);
}


