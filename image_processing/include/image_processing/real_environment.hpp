#ifndef REAL_ENVIRONMENT_HPP
#define REAL_ENVIRONMENT_HPP

#include "ARE/Environment.h"
#include "image_processing/blob_tracking.hpp"
#include <chrono>


namespace are {

class RealEnvironment : public Environment
{
public:
    RealEnvironment();
    void init() override;
    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    void update_info(double time) override;
    const Eigen::MatrixXi &get_visited_zone_matrix(){return grid_zone;}

    std::pair<int,int> real_coordinate_to_matrix_index(const std::vector<double> &pos);

    std::vector<double> fit_targeted_locomotion();
    std::vector<double> fit_exploration();
    std::vector<double> fit_foraging();


private:
    std::vector<double> current_position;
    std::vector<double> target_position;
    std::vector<double> beacon_position;
    Eigen::MatrixXi grid_zone;

    cv::VideoCapture video_capture;
    std::pair<cv::Scalar,cv::Scalar> colour_range;

    bool usingIPCamera;
};

} //are

#endif //REAL_ENVIRONMENT_HPP
