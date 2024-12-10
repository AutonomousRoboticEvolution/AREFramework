#ifndef MORPHOLOGY_CONSTANTS_HPP
#define MORPHOLOGY_CONSTANTS_HPP

namespace are{

struct morphology_constants{
    static constexpr int organ_trait_limit = 16;
    static constexpr float dimension_limit = 0.55;// Skeleton - > 0.23m; Joint - > 0.08; 1 Skeleton + 4 Joints
    static constexpr float voxel_size = 0.0009; //m³ - 0.9mm³
    static constexpr int voxel_multiplier = 22;
    static constexpr float voxel_real_size = 0.0198;//voxel_size * voxel_multiplier;
    static constexpr int matrix_size = 12; //264 / voxel_multiplier;
    static constexpr int real_matrix_size = 11; // Polivox generates a matrix with one voxel lower.
    static constexpr float shape_scale_value = 0.0198;//voxel_real_size; // results into 23.76x23.76x23.76 cm³ - in reality is 28x28x25 cm³
    static constexpr int voxels_number = 1331;//real_matrix_size*real_matrix_size*real_matrix_size;
    static constexpr short int empty_voxel=0;
    static constexpr short int filled_voxel=255;
    static constexpr float organ_voxel_size= 0.05;

    // Skeleton dimmesions in voxels
    static constexpr short int xHeadUpperLimit = 2;
    static constexpr short int xHeadLowerLimit = -2;
    static constexpr short int yHeadUpperLimit = 2;
    static constexpr short int yHeadLowerLimit = -2;
    static constexpr short int zHeadUpperLimit = 0;
    static constexpr short int zHeadLowerLimit = -5;

    // Parameters for skeleton base
    static constexpr short int skeletonBaseThickness = 1;
    static constexpr short int skeletonBaseHeight = 2;
};

using morph_const = struct morphology_constants;
}//are

#endif //MORPHOLOGY_CONSTANTS_HPP
