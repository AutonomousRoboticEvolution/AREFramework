#include "ARE/misc/utilities.h"

using namespace are;

void misc::split_line(const std::string& line, const std::string& delim,
                      std::list<std::string>& values){
    values.clear();
    size_t pos = 0;
    std::string l = line;
    while ((pos = l.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = l.substr(0, pos);
        values.push_back(p);
        l = l.substr(pos + 1);
        pos = 0;
    }

    if (!l.empty()) {
        //        split_line(l,delim,values);
        values.push_back(l);
    }
}

void misc::split_line(const std::string& line, const std::string& delim,
                      std::vector<std::string>& values){
    values.clear();
    size_t pos = 0;
    std::string l = line;
    while ((pos = l.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = l.substr(0, pos);
        values.push_back(p);
        l = l.substr(pos + 1);
        pos = 0;
    }

    if (!l.empty()) {
        //        split_line(l,delim,values);
        values.push_back(l);
    }
}

void misc::stdvect_to_eigenvect(const std::vector<double>& std_v, Eigen::VectorXd &eigen_v){
    eigen_v = Eigen::VectorXd::Zero(std_v.size());
    for(size_t i = 0; i < std_v.size(); i++)
        eigen_v(i) = std_v[i];
}

void misc::eigenvect_to_stdvect(const Eigen::VectorXd &eigen_v, std::vector<double>& std_v){
    std_v.resize(eigen_v.rows());
    for(long i = 0; i < eigen_v.rows(); i++)
        std_v[i] = eigen_v[i];
}

double misc::sinusoidal(double amplitude, double time, double frequency, double time_offset, double position_offset){
    return amplitude * sin(time * frequency + time_offset) + position_offset;
}

double misc::get_next_joint_position(double nn_output, double time, double previous_position) {
    double amplitude = M_PI_4 * 8./9., time_offset = 0.0, position_offset = 0.0; // Constant
    double max_frequency = 3.0;
    double frequency = nn_output * (max_frequency / 2.) + max_frequency / 2.; // From [-1,1] to [0,max_frequency];
    double pos_diff = sinusoidal(amplitude, time, frequency, time_offset, position_offset);
    double next_pos = pos_diff + previous_position;
    // Limitation by the joints of -80 to 80 degrees
    if(next_pos > (4./9.) * M_PI)
        next_pos = (4./9.) * M_PI;
    if(next_pos < -(4./9.) * M_PI)
        next_pos = -(4./9.) * M_PI;
    return next_pos;
}
