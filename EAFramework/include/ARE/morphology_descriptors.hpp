///////////////////////
///// Descriptors /////
///////////////////////
#ifndef MORPHOLOGY_DESCRIPTORS_HPP
#define MORPHOLOGY_DESCRIPTORS_HPP

#include <eigen3/Eigen/Core>
#include <vector>
#include <ARE/misc/utilities.h>

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
    static constexpr int real_matrix_size = 11; // Polivox generates a matrix with one voxel lower.
    static constexpr float shape_scale_value = 0.0198;//voxel_real_size; // results into 23.76x23.76x23.76 cm³ - in reality is 28x28x25 cm³
    static constexpr int voxels_number = 1331;//real_matrix_size*real_matrix_size*real_matrix_size;
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

typedef enum desc_type_t{
    CART_DESC = 0,
    ORGAN_POSITION = 1
} desc_type_t;

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

    /**
     * @brief get the descriptor with normalized values. Mainly intended for novelty search
     * @return
     */
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

    /**
     * @brief Is the descriptor defined? i.e. Is the descriptor describing an actual body-plan?
     * @return
     */
    bool defined() const{return voxelNumber >= 24;}

    std::string to_string(){
        std::stringstream sstr;
        sstr << "robotWidth," << robotWidth << std::endl;
        sstr << "robotHeight," << robotHeight << std::endl;
        sstr << "robotDepth," << robotDepth << std::endl;
        sstr << "voxelNumber," << voxelNumber << std::endl;
        sstr << "wheelNumber," << wheelNumber << std::endl;
        sstr << "sensorNumber," << sensorNumber << std::endl;
        sstr << "casterNumber," << casterNumber << std::endl;
        sstr << "jointNumber," << jointNumber << std::endl;
        return sstr.str();
    }

    void from_string(const std::string& str){
        std::stringstream sstr;
        sstr << str;
        for(std::string line; std::getline(sstr,line);){
            std::vector<std::string> split;
            misc::split_line(line,",",split);
            if(split[0] == "robotWidth")
                robotWidth = std::stof(split[1]);
            else if(split[0] == "robotHeight")
                robotHeight = std::stof(split[1]);
            else if(split[0] == "robotDepth")
                robotDepth = std::stof(split[1]);
            else if(split[0] == "voxelNumber")
                voxelNumber = std::stof(split[1]);
            else if(split[0] == "wheelNumber")
                wheelNumber = std::stof(split[1]);
            else if(split[0] == "sensorNumber")
                sensorNumber = std::stof(split[1]);
            else if(split[0] == "casterNumber")
                casterNumber = std::stof(split[1]);
            else if(split[0] == "jointNumber")
                jointNumber = std::stof(split[1]);
        }
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

inline bool operator ==(const CartDesc& cd1, const CartDesc& cd2){
    return cd1.casterNumber == cd2.casterNumber &&
            cd1.wheelNumber == cd2.wheelNumber &&
            cd1.jointNumber == cd2.jointNumber &&
            cd1.sensorNumber == cd2.sensorNumber &&
            cd1.voxelNumber == cd2.voxelNumber &&
            fabs(cd1.robotDepth - cd2.robotDepth) <= 1e-3 &&
            fabs(cd1.robotHeight - cd2.robotHeight) <= 1e-3 &&
            fabs(cd1.robotWidth - cd2.robotWidth) <= 1e-3;
}


struct OrganPositionDesc
{
    int organ_matrix[morph_const::real_matrix_size][morph_const::real_matrix_size][morph_const::real_matrix_size] = {{{0}}};
    Eigen::VectorXd getCartDesc() const{
        Eigen::VectorXd organ_position_descriptor((morph_const::real_matrix_size)*(morph_const::real_matrix_size)*(morph_const::real_matrix_size));
        int counter = 0;
        for(int k = 0; k < morph_const::real_matrix_size; k++){
            for(int j = 0; j < morph_const::real_matrix_size; j++) {
                for(int i = 0; i < morph_const::real_matrix_size; i++) {
                    organ_position_descriptor(counter) = organ_matrix[i][j][k];
                    counter++;
                }
            }
        }
        return organ_position_descriptor;
    }

    std::string to_string(){
        std::stringstream sstr;
        sstr << organ_matrix[0][0][0];
        for(int k = 0; k < morph_const::real_matrix_size; k++){
            for(int j = 0; j < morph_const::real_matrix_size; j++) {
                for(int i = 1; i < morph_const::real_matrix_size; i++) {
                    sstr << "," << organ_matrix[i][j][k];
                }
            }
        }
    }

    void from_string(const std::string& str){
        std::stringstream sstr;
        sstr << str;
        int i = 0, j = 0, k = 0;
        for(std::string line; std::getline(sstr,line,',');){
            organ_matrix[i][j][k] = std::stoi(line);
            i++;
            if(i >= morph_const::real_matrix_size){
                i = 0;
                j++;
                if(j >= morph_const::real_matrix_size){
                    j=0;
                    k++;
                }
            }
        }
    }

    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & organ_matrix;
    }
};

}//are

#endif //MORPHOLOGY_DESCRIPTORS_HPP
