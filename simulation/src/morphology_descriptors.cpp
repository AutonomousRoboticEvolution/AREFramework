#include "simulatedER/morphology_descriptors.hpp"


using namespace are::sim;

void FeaturesDesc::create(const PolyVox::RawVolume<uint8_t> &skeleton,const std::vector<sim::Organ> &organ_list){
    //Get the number of components
    for(const Organ &organ: organ_list){
        if(!organ.isOrganRemoved() && organ.isOrganChecked()) {
            if (organ.getOrganType() == 1)
                wheel_number++;
            else if (organ.getOrganType() == 2)
                sensor_number++;
            else if (organ.getOrganType() == 3)
                joint_number+=2;
            else if (organ.getOrganType() == 4)
                caster_number++;
        }
    }

    PolyVox::Region region = skeleton.getEnclosingRegion();
    float min_x = region.getUpperX(),
        min_y = region.getUpperY(),
        min_z = region.getUpperZ(),
        max_x = region.getLowerX() + 1,
        max_y = region.getLowerY() + 1,
        max_z = region.getLowerZ() + 1;
    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                if(skeleton.getVoxel(x,y,z) == morph_const::empty_voxel)
                    continue;
                if(x < min_x)
                    min_x = x;
                if(y < min_y)
                    min_y = y;
                if(z < min_z)
                    min_y = y;
                if(x > max_x)
                    max_x = x;
                if(y > max_y)
                    max_y = y;
                if(z > max_z)
                    max_y = y;
                voxel_number++;
            }
        }
    }
    // Get dimensions
    robot_width = fabs(max_x - min_x);
    robot_depth = fabs(max_y - min_y);
    robot_height = fabs(max_z - min_z);
}

Eigen::VectorXd FeaturesDesc::to_eigen_vector() const{
    Eigen::VectorXd desc(8);
    desc(0) = robot_width / morph_const::dimension_limit;
    desc(1) = robot_depth / morph_const::dimension_limit;
    desc(2) = robot_height / morph_const::dimension_limit;
    desc(3) = (double) voxel_number / morph_const::voxels_number;
    desc(4) = (double) wheel_number / morph_const::organ_trait_limit;
    desc(5) = (double) sensor_number / morph_const::organ_trait_limit;
    desc(6) = (double) joint_number / morph_const::organ_trait_limit;
    desc(7) = (double) caster_number / morph_const::organ_trait_limit;
    return desc;
}

std::string FeaturesDesc::to_string() const{
    std::stringstream sstr;
    sstr << "robotWidth," << robot_width << std::endl;
    sstr << "robotHeight," << robot_height << std::endl;
    sstr << "robotDepth," << robot_depth << std::endl;
    sstr << "voxelNumber," << voxel_number << std::endl;
    sstr << "wheelNumber," << wheel_number << std::endl;
    sstr << "sensorNumber," << sensor_number << std::endl;
    sstr << "casterNumber," << caster_number << std::endl;
    sstr << "jointNumber," << joint_number << std::endl;
    return sstr.str();
}

void FeaturesDesc::from_string(const std::string &str){
    std::stringstream sstr;
    sstr << str;
    for(std::string line; std::getline(sstr,line);){
        std::vector<std::string> split;
        misc::split_line(line,",",split);
        if(split[0] == "robotWidth")
            robot_width = std::stof(split[1]);
        else if(split[0] == "robotHeight")
            robot_height = std::stof(split[1]);
        else if(split[0] == "robotDepth")
            robot_depth = std::stof(split[1]);
        else if(split[0] == "voxelNumber")
            voxel_number = std::stof(split[1]);
        else if(split[0] == "wheelNumber")
            wheel_number = std::stof(split[1]);
        else if(split[0] == "sensorNumber")
            sensor_number = std::stof(split[1]);
        else if(split[0] == "casterNumber")
            caster_number = std::stof(split[1]);
        else if(split[0] == "jointNumber")
            joint_number = std::stof(split[1]);
    }
}

MatrixDesc::MatrixDesc(const MatrixDesc& opd){
    for(int k = 0; k < morph_const::real_matrix_size; k++)
        for(int j = 0; j < morph_const::real_matrix_size; j++)
            for(int i = 0; i < morph_const::real_matrix_size; i++)
                matrix[i][j][k] = opd.matrix[i][j][k];
}

void MatrixDesc::create(const PolyVox::RawVolume<uint8_t> &skeleton,
                        const std::vector<sim::Organ> &organ_list){
    PolyVox::Region region = skeleton.getEnclosingRegion();

    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                int matPosX, matPosY, matPosZ;
                matPosX = x + 5;
                matPosY = y + 5;
                matPosZ = z + 5; // The connector is slightly up and when rounding it increases one voxel.
                if (matPosX > morph_const::real_matrix_size)
                    matPosX = morph_const::real_matrix_size;
                if (matPosX < 0)
                    matPosX = 0;
                if (matPosY > morph_const::real_matrix_size)
                    matPosY = morph_const::real_matrix_size;
                if (matPosY < 0)
                    matPosY = 0;
                if (matPosZ > morph_const::real_matrix_size)
                    matPosZ = morph_const::real_matrix_size;
                if (matPosZ < 0)
                    matPosZ = 0;
                uint8_t voxel = skeleton.getVoxel(x,y,z);
                if(voxel == morph_const::filled_voxel)
                    matrix[matPosX][matPosY][matPosZ] = 1;
                else
                    matrix[matPosX][matPosY][matPosZ] = 0;
            }
        }
    }
    for(const Organ& organ: organ_list){
        if(!organ.isOrganRemoved() && organ.isOrganChecked()) {
            int voxelPosX = static_cast<int>(std::round(organ.connectorPos[0] / morph_const::voxel_real_size));
            int voxelPosY = static_cast<int>(std::round(organ.connectorPos[1] / morph_const::voxel_real_size));
            int voxelPosZ = static_cast<int>(std::round(organ.connectorPos[2] / morph_const::voxel_real_size));
            int matPosX, matPosY, matPosZ;
            matPosX = voxelPosX + 5;
            matPosY = voxelPosY + 5;
            matPosZ = voxelPosZ - 1; // The connector is slightly up and when rounding it increases one voxel.
            if (matPosX > morph_const::real_matrix_size)
                matPosX = morph_const::real_matrix_size;
            if (matPosX < 0)
                matPosX = 0;
            if (matPosY > morph_const::real_matrix_size)
                matPosY = morph_const::real_matrix_size;
            if (matPosY < 0)
                matPosY = 0;
            if (matPosZ > morph_const::real_matrix_size)
                matPosZ = morph_const::real_matrix_size;
            if (matPosZ < 0)
                matPosZ = 0;
            if(organ.getOrganType() != 0)
                matrix[matPosX][matPosY][matPosZ] = organ.getOrganType() + 1;
        }
    }
}

Eigen::VectorXd MatrixDesc::to_eigen_vector() const{
    Eigen::VectorXd vect_descriptor((morph_const::real_matrix_size)*(morph_const::real_matrix_size)*(morph_const::real_matrix_size));
    int counter = 0;
    for(int k = 0; k < morph_const::real_matrix_size; k++){
        for(int j = 0; j < morph_const::real_matrix_size; j++) {
            for(int i = 0; i < morph_const::real_matrix_size; i++) {
                vect_descriptor(counter) = matrix[i][j][k];
                counter++;
            }
        }
    }
    return vect_descriptor;
}

std::string MatrixDesc::to_string() const{
    std::stringstream sstr;
    sstr << matrix[0][0][0];
    for(int k = 0; k < morph_const::real_matrix_size; k++){
        for(int j = 0; j < morph_const::real_matrix_size; j++) {
            for(int i = 1; i < morph_const::real_matrix_size; i++) {
                sstr << "," << matrix[i][j][k];
            }
        }
    }
    return sstr.str();
}

void MatrixDesc::from_string(const std::string& str){
    std::stringstream sstr;
    sstr << str;
    int i = 0, j = 0, k = 0;
    for(std::string line; std::getline(sstr,line,',');){
        matrix[i][j][k] = std::stoi(line);
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

void OrganMatrixDesc::create(const PolyVox::RawVolume<uint8_t> &skeleton,
                             const std::vector<sim::Organ> &organ_list){
    for(const Organ& organ: organ_list){
        if(!organ.isOrganRemoved() && organ.isOrganChecked()) {
            int voxelPosX = static_cast<int>(std::round(organ.connectorPos[0] / morph_const::voxel_real_size));
            int voxelPosY = static_cast<int>(std::round(organ.connectorPos[1] / morph_const::voxel_real_size));
            int voxelPosZ = static_cast<int>(std::round(organ.connectorPos[2] / morph_const::voxel_real_size));
            int matPosX, matPosY, matPosZ;
            matPosX = voxelPosX + 5;
            matPosY = voxelPosY + 5;
            matPosZ = voxelPosZ - 1; // The connector is slightly up and when rounding it increases one voxel.
            if (matPosX > morph_const::real_matrix_size)
                matPosX = morph_const::real_matrix_size;
            if (matPosX < 0)
                matPosX = 0;
            if (matPosY > morph_const::real_matrix_size)
                matPosY = morph_const::real_matrix_size;
            if (matPosY < 0)
                matPosY = 0;
            if (matPosZ > morph_const::real_matrix_size)
                matPosZ = morph_const::real_matrix_size;
            if (matPosZ < 0)
                matPosZ = 0;
            if(organ.getOrganType() != 0)
                matrix[matPosX][matPosY][matPosZ] = organ.getOrganType() + 1;
        }
    }
}
