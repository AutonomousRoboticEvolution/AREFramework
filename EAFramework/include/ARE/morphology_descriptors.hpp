///////////////////////
///// Descriptors /////
///////////////////////
#ifndef MORPHOLOGY_DESCRIPTORS_HPP
#define MORPHOLOGY_DESCRIPTORS_HPP

#include <eigen3/Eigen/Core>
#include <vector>

namespace are {
/////////////////////
/////////////////////

struct morphology_constants{
    static constexpr int organ_trait_limit = 16;
    static constexpr float dimension_limit = 0.55;// Skeleton - > 0.23m; Joint - > 0.08; 1 Skeleton + 4 Joints
    static constexpr float voxel_size = 0.0009; //m³ - 0.9mm³
    static constexpr int voxel_multiplier = 22;
    static constexpr float voxel_real_size = 0.0198;//voxel_size * voxel_multiplier;
    static constexpr int matrix_size = 12; //264 / voxel_multiplier;
    static constexpr float shape_scale_value = 0.0198;//voxel_real_size; // results into 23.76x23.76x23.76 cm³ - in reality is 28x28x25 cm³
    static constexpr int voxels_number = 1728;//matrix_size*matrix_size*matrix_size;
    static constexpr float matrix_size_m = 0.2376; //matrix_size * voxel_real_size;
    static constexpr short int empty_voxel=0;
    static constexpr short int filled_voxel=255;

    // Skeleton dimmesions in voxels
    static constexpr short int xHeadUpperLimit = 2;
    static constexpr short int xHeadLowerLimit = -2;
    static constexpr short int yHeadUpperLimit = 2;
    static constexpr short int yHeadLowerLimit = -2;

    // Parameters for skeleton base
    static constexpr short int skeletonBaseThickness = 1;
    static constexpr short int skeletonBaseHeight = 2;
};

using morph_const = struct morphology_constants;

struct CartDesc
{
    float robotWidth = 0; // X
    float robotDepth = 0; // Y
    float robotHeight = 0; // Z
    int voxelNumber = 0;
    int wheelNumber = 0;
    int sensorNumber = 0;
    int casterNumber = 0;
    int jointNumber = 0;

    Eigen::VectorXd getCartDesc() const{
        Eigen::VectorXd cartDesc(8);
        cartDesc(0) = robotWidth / morph_const::dimension_limit;
        cartDesc(1) = robotDepth / morph_const::dimension_limit;
        cartDesc(2) = robotHeight / morph_const::dimension_limit;
        cartDesc(3) = (double) voxelNumber / morph_const::voxels_number;
        cartDesc(4) = (double) wheelNumber / morph_const::organ_trait_limit;
        cartDesc(5) = (double) sensorNumber / morph_const::organ_trait_limit;
        cartDesc(6) = (double) jointNumber / morph_const::organ_trait_limit;
        cartDesc(7) = (double) casterNumber / morph_const::organ_trait_limit;
        return cartDesc;
    }

    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & robotWidth;
        arch & robotDepth;
        arch & robotHeight;
        arch & voxelNumber;
        arch & wheelNumber;
        arch & sensorNumber;
        arch & casterNumber;
        arch & jointNumber;
    }
};

struct OrganPositionDesc
{
    int organ_matrix[morph_const::matrix_size-1][morph_const::matrix_size-1][morph_const::matrix_size-1] = {{{0}}};
    Eigen::VectorXd getCartDesc() const{
        Eigen::VectorXd organ_position_descriptor((morph_const::matrix_size-1)*(morph_const::matrix_size-1)*(morph_const::matrix_size-1));
        int counter = 0;
        for(int k = 0; k < morph_const::matrix_size-1; k++){
            for(int j = 0; j < morph_const::matrix_size-1; j++) {
                for(int i = 0; i < morph_const::matrix_size-1; i++) {
                    organ_position_descriptor(counter) = organ_matrix[i][j][k];
                    counter++;
                }
            }
        }
        return organ_position_descriptor;
    }
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & organ_matrix;
    }
};

}//are

#endif //MORPHOLOGY_DESCRIPTORS_HPP
