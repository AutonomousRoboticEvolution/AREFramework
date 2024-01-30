#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <list>
#include <vector>
#include <eigen3/Eigen/Core>
#include <chrono>
#include <cmath>

namespace are {

namespace misc {

void split_line(const std::string &line,const std::string &delim,
                std::list<std::string> &values);

void split_line(const std::string &line,const std::string &delim,
                std::vector<std::string> &values);

void stdvect_to_eigenvect(const std::vector<double>& std_v, Eigen::VectorXd &eigen_v);
void stdvect_to_eigenmat(const std::vector<double>& std_v, Eigen::MatrixXd &eigen_v);
void eigenvect_to_stdvect(const Eigen::VectorXd &eigen_v, std::vector<double>& std_v);
void eigenvect_to_stdvect(const Eigen::MatrixXd &eigen_v, std::vector<double>& std_v);
double sinusoidal(double amplitude, double time, double frequency, double time_offset, double position_offset);

/**
 * @brief get_next_joint_position - compute the new target angle based on the neural network and the current target value
 * @param nn_output - in range [-1,1]
 * @param time - in seconds
 * @param previous_position - in radians
 * @return the new target position, in radians
 */
double get_next_joint_position(double nn_output, double time, double previous_position);

std::string int_to_string(int n);

/**
 * @brief generate_unique_id
 * @param maximum length of the id
 * @return unique id
 */
int generate_unique_id(int n);

}//misc

}//are

#endif //UTILITIES_H
