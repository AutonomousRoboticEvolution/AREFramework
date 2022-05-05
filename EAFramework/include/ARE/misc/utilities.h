#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <list>
#include <vector>
#include <eigen3/Eigen/Core>

namespace are {

namespace misc {

void split_line(const std::string &line,const std::string &delim,
                std::list<std::string> &values);

void split_line(const std::string &line,const std::string &delim,
                std::vector<std::string> &values);

void stdvect_to_eigenvect(const std::vector<double>& std_v, Eigen::VectorXd &eigen_v);
void eigenvect_to_stdvect(const Eigen::VectorXd &eigen_v, std::vector<double>& std_v);
double sinusoidal(double amplitude, double frequency, double time_offset, double position_offset);

}//misc

}//are

#endif //UTILITIES_H
