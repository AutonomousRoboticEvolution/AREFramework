//
// Created by edgar on 26/04/22.
//

#ifndef ARE_PROTOMATRIX_UTILS_H
#define ARE_PROTOMATRIX_UTILS_H

#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include "math.h"
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include "nn2/random.hpp"
#include "ARE/nn2/NN2CPPNGenome.hpp"

namespace are {

namespace protomatrix {

    std::vector<std::vector<double>> load_robot_matrix(const std::string &filepath);
    void random_matrix(std::vector<std::vector<double> > &matrix_4d);
    std::vector<std::vector<double>> mutate_matrix(const std::vector<std::vector<double>> &matrix_4d);
    std::vector<std::vector<double>> crossover_matrix(const std::vector<std::vector<double>> &first_parent_4d,const std::vector<std::vector<double>> &second_parent_4d);
    std::vector<std::vector<double>> retrieve_matrices_from_cppn(nn2_cppn_t cppn);


}
}

#endif //ARE_PROTOMATRIX_UTILS_H
