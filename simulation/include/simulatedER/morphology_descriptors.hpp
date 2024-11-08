///////////////////////
///// Descriptors /////
///////////////////////
#ifndef MORPHOLOGY_DESCRIPTORS_HPP
#define MORPHOLOGY_DESCRIPTORS_HPP

#include <eigen3/Eigen/Core>
#include <vector>
#include <ARE/misc/utilities.h>
#include <PolyVox/RawVolume.h>
#include "simulatedER/morphology_constants.hpp"
#include "simulatedER/Organ.h"

namespace are {
namespace sim{
/////////////////////
/////////////////////


typedef enum desc_type_t{
    FEATURES = 0,
    ORGAN_POSITION = 1,
    MATRIX = 2
} desc_type_t;

class MorphDescriptor{
public:
    MorphDescriptor(){}
    virtual void create(const PolyVox::RawVolume<uint8_t> &skeleton,
                        const std::vector<sim::Organ> &organ_list) = 0;
    virtual Eigen::VectorXd to_eigen_vector() const = 0;
    virtual std::string to_string() const = 0;
    virtual void from_string(const std::string&) = 0;
};

class FeaturesDesc: public MorphDescriptor
{
public:
    FeaturesDesc(){}
    FeaturesDesc(const FeaturesDesc& cd):
        robot_width(cd.robot_width),
        robot_depth(cd.robot_depth),
        robot_height(cd.robot_height),
        voxel_number(cd.voxel_number),
        wheel_number(cd.wheel_number),
        sensor_number(cd.sensor_number),
        caster_number(cd.caster_number),
        joint_number(cd.joint_number)
    {}

    void create(const PolyVox::RawVolume<uint8_t> &skeleton,const std::vector<Organ> &organ_list) override;

    /**
     * @brief get the descriptor as a vector with normalized values. Mainly intended for novelty search
     * @return
     */
    Eigen::VectorXd to_eigen_vector() const override;


    std::string to_string() const override;
    void from_string(const std::string&) override;
    /**
     * @brief Is the descriptor defined? i.e. Is the descriptor describing an actual body-plan?
     * @return
     */
    bool defined() const{return voxel_number >= 24;}

    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & robot_width;
        arch & robot_depth;
        arch & robot_height;
        arch & voxel_number;
        arch & wheel_number;
        arch & sensor_number;
        arch & caster_number;
        arch & joint_number;
    }

    float robot_width = 0; // X
    float robot_depth = 0; // Y
    float robot_height = 0; // Z
    int voxel_number = 0;
    int wheel_number = 0;
    int sensor_number = 0;
    int caster_number = 0;
    int joint_number = 0;
};

inline bool operator ==(const FeaturesDesc& cd1, const FeaturesDesc& cd2){
    return cd1.caster_number == cd2.caster_number &&
            cd1.wheel_number == cd2.wheel_number &&
            cd1.joint_number == cd2.joint_number &&
            cd1.sensor_number == cd2.sensor_number &&
            cd1.voxel_number == cd2.voxel_number &&
            fabs(cd1.robot_depth - cd2.robot_depth) <= 1e-3 &&
            fabs(cd1.robot_height - cd2.robot_height) <= 1e-3 &&
            fabs(cd1.robot_width - cd2.robot_width) <= 1e-3;
}



class MatrixDesc: public MorphDescriptor
{
public:
    MatrixDesc(){}
    MatrixDesc(const MatrixDesc& opd);

    virtual void create(const PolyVox::RawVolume<uint8_t> &skeleton,
                        const std::vector<sim::Organ> &organ_list) override;

    Eigen::VectorXd to_eigen_vect() const;

    std::string to_string() const override;

    void from_string(const std::string& str) override;

    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & matrix;
    }
    int matrix[morph_const::real_matrix_size][morph_const::real_matrix_size][morph_const::real_matrix_size] = {{{0}}};

};

class OrganMatrixDesc: public MatrixDesc{
public:
    OrganMatrixDesc(){}
    OrganMatrixDesc(const OrganMatrixDesc& opd):MatrixDesc(opd){}
    virtual void create(const PolyVox::RawVolume<uint8_t> &skeleton,
                        const std::vector<sim::Organ> &organ_list) override;
};

}//sim
}//are

#endif //MORPHOLOGY_DESCRIPTORS_HPP
