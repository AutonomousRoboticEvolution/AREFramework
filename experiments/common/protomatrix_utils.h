//
// Created by edgar on 26/04/22.
//

#ifndef ARE_PROTOMATRIX_UTILS_H
#define ARE_PROTOMATRIX_UTILS_H

#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

namespace are {

namespace protomatrix {

    std::vector<std::vector<double>> load_robot_matrix(std::string filepath);
    std::vector<std::vector<double>> mutate_matrix(std::vector<std::vector<double>> matrix_4d);
    std::vector<std::vector<double>> crossover_matrix(std::vector<std::vector<double>> first_parent_4d,std::vector<std::vector<double>> second_parent_4d);

}
}

}

#endif //ARE_PROTOMATRIX_UTILS_H
