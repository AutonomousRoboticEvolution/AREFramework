#include "simulatedER/skeleton_generation.hpp"
#include <functional>
#include "simulatedER/morphology_constants.hpp"

using namespace are;


void skeleton::count_number_voxels(const skeleton::type& skeleton, int &number_voxels){
    PolyVox::Region region = skeleton.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(skeleton.getVoxel(x,y,z) == morph_const::filled_voxel)
                    number_voxels++;
            }
        }
    }
}

void skeleton::create_base(skeleton::type &skeleton){
    PolyVox::Region region = skeleton.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getLowerZ() + morph_const::skeletonBaseHeight; z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness && x >= morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness &&
                    y <= morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness && y >= morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness){ // Additive condition
                    if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit &&
                        y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit){ // Substractive condition
                        if(skeleton.getVoxel(x, y, z) != morph_const::empty_voxel)
                            skeleton.setVoxel(x, y, z, morph_const::empty_voxel);
                    }
                    else{
                        if(skeleton.getVoxel(x, y, z) != morph_const::filled_voxel)
                            skeleton.setVoxel(x, y, z, morph_const::filled_voxel);
                    }
                }
            }
        }
    }
}

void skeleton::empty_space_for_head(skeleton::type &skeleton){
    PolyVox::Region region = skeleton.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(x <= morph_const::xHeadUpperLimit && x >= morph_const::xHeadLowerLimit && y <= morph_const::yHeadUpperLimit && y >= morph_const::yHeadLowerLimit){
                    if(skeleton.getVoxel(x,y,z) == morph_const::filled_voxel){
                        skeleton.setVoxel(x, y, z, morph_const::empty_voxel);
                    }
                }
            }
        }
    }
}

void skeleton::remove_skeleton_regions(skeleton::type &skeleton){
    skeleton::coord_t regions;

    //Recursive function to find regions with adjacent voxels
    std::function<void(const skeleton::type &, skeleton::coord_t &,PolyVox::RawVolume<bool> &,int,int,int,int)> find_regions =
        [&](const skeleton::type & skeleton, skeleton::coord_t & regions,PolyVox::RawVolume<bool> &visited,int x,int y,int z,int region_counter){
            visited.setVoxel(x, y, z, true); // Cell visited
            uint8_t voxel;
            // Explore neighbourhood.
            for (int dz = -1; dz <= 1; dz+=1) {
                for (int dy = -1; dy <= 1; dy+=1) {
                    for (int dx = -1; dx <= 1; dx+=1) {
                        if (x + dx > -morph_const::matrix_size/2 && x + dx < morph_const::matrix_size/2 &&
                            y + dy > -morph_const::matrix_size/2 && y + dy < morph_const::matrix_size/2 &&
                            z + dz > -morph_const::matrix_size/2 && z + dz < morph_const::matrix_size/2) {
                            voxel = skeleton.getVoxel(x + dx, y + dy, z + dz);
                            if (!visited.getVoxel(x + dx, y + dy, z + dz) && voxel > 120) {
                                std::vector<int> newCoord{x + dx, y + dy, z + dz};
                                regions[region_counter-1].push_back(newCoord);
                                find_regions(skeleton, regions, visited, x + dx, y + dy, z + dz, region_counter);
                            }
                        }
                    }
                }
            }
        };

    //Find the regions
    PolyVox::RawVolume<bool> visited(PolyVox::Region(PolyVox::Vector3DInt32(-morph_const::matrix_size/2, -morph_const::matrix_size/2, -morph_const::matrix_size/2),
                                                     PolyVox::Vector3DInt32(morph_const::matrix_size/2, morph_const::matrix_size/2, morph_const::matrix_size/2)));
    uint8_t voxel;
    int regionCounter = 0;
    PolyVox::Region region = skeleton.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                voxel = skeleton.getVoxel(x, y, z);
                if(voxel == morph_const::filled_voxel && !visited.getVoxel(x, y, z)){
                    regionCounter++;
                    regions.resize(regionCounter);
                    std::vector<int> newCoord{x, y, z};
                    regions[regionCounter-1].push_back(newCoord);
                    find_regions(skeleton, regions, visited, x, y, z, regionCounter);
                }
            }
        }
    }

    //remove the regions not attached to the base
    int region_connected = -1;
    if(regions.size() > 1){
        for(int i = 0; i < regions.size(); i++){
            for(int j = 0; j < regions[i].size(); j++){
                if(regions[i][j][0] <= morph_const::xHeadUpperLimit + morph_const::skeletonBaseThickness &&
                    regions[i][j][0] >= morph_const::xHeadLowerLimit - morph_const::skeletonBaseThickness &&
                    regions[i][j][1] <= morph_const::yHeadUpperLimit + morph_const::skeletonBaseThickness &&
                    regions[i][j][1] >= morph_const::yHeadLowerLimit - morph_const::skeletonBaseThickness &&
                    regions[i][j][2] <= -6 + morph_const::skeletonBaseHeight){ /// \todo :EB make this a constant!
                    region_connected = i;
                    break;
                }
            }
        }
    }

    if(regions.size() > 1){
        for(int i = 0; i < regions.size(); i++){
            if(region_connected != i){
                for(int j = 0; j < regions[i].size(); j++){
                    skeleton.setVoxel(regions[i][j][0], regions[i][j][1], regions[i][j][2], 0);
                }
            }
        }
    }
}


void skeleton::remove_hoverhangs(skeleton::type &skeleton){
    // loop through every voxel, starting at the second z layer (since the whole first layer is supported by the bed so none need be removed)
    PolyVox::Region region = skeleton.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+2; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(skeleton.getVoxel(x, y, z) == morph_const::filled_voxel ){
                    // if we are in here, then this voxel is currently filled, so we need to test if it is supported
                    bool isSupported=false;

                    // check the potentially supporting voxels, i.e. the 9 voxels below this one:
                    for (int dx=-1; dx<=1; dx+=1){ // dx is the offset in x direction, so we want to test -1, 0 and +1
                        for (int dy=-1; dy<=1; dy+=1){ // dy is the offset in y direction, so we want to test -1, 0 and +1
                            // check that the potentially supporting voxel is inside the volume (may bot be the case for voxels on the edge of the volume):
                            if( x+dx >= region.getLowerX() && x+dx <= region.getUpperX() && y+dy >= region.getLowerY() && y+dy <= region.getUpperY() ) {
                                // check if the potenitally supporting voxel is filled, in which case it is supported
                                if (skeleton.getVoxel(x+dx, y+dy, z-1) == morph_const::filled_voxel)
                                    isSupported=true;
                            }
                        }
                    }
                    // if we did not find any supporting voxel, this voxel should not be filled:
                    if (!isSupported){
                        skeleton.setVoxel(x, y, z, morph_const::empty_voxel);
                    }
                }
            }
        }
    }
}

void skeleton::find_skeleton_surface(const skeleton::type &skeleton, skeleton::coord_t &skeleton_surface){
    std::function<void(const skeleton::type &, skeleton::coord_t &,PolyVox::RawVolume<bool> &,int,int,int,int)> generate_coordinates =
        [&](const skeleton::type &skeleton, skeleton::coord_t &coords,PolyVox::RawVolume<bool> &visited,int x,int y,int z,int counter){
            visited.setVoxel(x, y, z, true); // Cell visited
            uint8_t voxel;
            // Explore neighbourhood.
            // finds surface on the top of robot but doesnt actually use them as organ can't be on the top
            for (int dz = -1; dz <= 1; dz+=1) {
                if (z + dz > -morph_const::matrix_size/2 && z + dz < morph_const::matrix_size/2) {
                    voxel = skeleton.getVoxel(x, y, z + dz);
                    if (!visited.getVoxel(x, y, z + dz) && voxel == morph_const::filled_voxel) {
                        generate_coordinates(skeleton, coords, visited, x, y, z + dz, counter);
                    }
                    /// EB: Organ pointing upwards or downwards (z-axis) are not allowed by the RoboFab.
                }
            }
            // finds surfaces in the y direction
            // saves it to be facing in the y direction +/-
            for (int dy = -1; dy <= 1; dy+=1) {
                if (y + dy > -morph_const::matrix_size/2 && y + dy < morph_const::matrix_size/2) {
                    voxel = skeleton.getVoxel(x, y + dy, z);
                    if (!visited.getVoxel(x, y + dy, z) && voxel == morph_const::filled_voxel) {
                        generate_coordinates(skeleton, coords, visited, x, y + dy, z, counter);
                    }
                    else if(!visited.getVoxel(x, y + dy, z) && voxel == morph_const::empty_voxel) {
                        std::vector<int> newCoord{x, y, z, 0, dy, 0};
                        coords[counter-1].push_back(newCoord);
                    }
                }
            }
            // finds surfaces in the x direction
            for (int dx = -1; dx <= 1; dx+=1) {
                if (x + dx > -morph_const::matrix_size/2 && x + dx < morph_const::matrix_size/2) {
                    voxel = skeleton.getVoxel(x + dx, y, z);
                    if (!visited.getVoxel(x + dx, y, z) && voxel == morph_const::filled_voxel) {
                        generate_coordinates(skeleton, coords, visited, x + dx, y, z, counter);
                    }
                    else if(!visited.getVoxel(x + dx, y, z) && voxel == morph_const::empty_voxel) {
                        std::vector<int> newCoord{x, y, z, dx, 0, 0};
                        coords[counter-1].push_back(newCoord);
                    }
                }
            }
        };


    // This matrix stores the visited elements.
    PolyVox::RawVolume<bool> visited(PolyVox::Region(PolyVox::Vector3DInt32(-morph_const::matrix_size/2, -morph_const::matrix_size/2, -morph_const::matrix_size/2),
                                                           PolyVox::Vector3DInt32(morph_const::matrix_size/2, morph_const::matrix_size/2, morph_const::matrix_size/2)));
    skeleton::coord_t coords;
    uint8_t voxel;
    int counter = 0;
    auto region = skeleton.getEnclosingRegion();
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 2) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 2) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 2) {
                voxel = skeleton.getVoxel(x, y, z);
                if(voxel == morph_const::filled_voxel && !visited.getVoxel(x, y, z)){
                    counter++;
                    coords.resize(counter);
                    generate_coordinates(skeleton, coords, visited, x, y, z, counter);
                }
            }
        }
    }
}
