#ifndef REALMAZE_HPP
#define REALMAZE_HPP

#include <chrono>
#include "ARE/Environment.h"
#include "image_processing/blob_tracking.hpp"


namespace are {

class RealMaze : public Environment
{
public:
    RealMaze();
    void init() override;
    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    void update_info() override;

private:
    std::vector<double> current_position;
    std::vector<double> target_position;

    cv::VideoCapture video_capture;
    std::pair<cv::Scalar,cv::Scalar> colour_range;
    bool usingIPCamera = true;
};

} //are

#endif //REALMAZE_HPP
