#include "ARE/realMaze.hpp"

using namespace are;

void RealMaze::init(){

    std::string pipe = settings::getParameter<settings::String>(parameters,"#cameraPipe").value;

     video_capture = cv::VideoCapture(pipe);

     cv::Scalar redMin(0,100,110);
     cv::Scalar redMax(200,255,255);

     float framerate = 25.f;

}

std::vector<double> RealMaze::fitnessFunction(const Individual::Ptr &ind){

    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                (a[1] - b[1])*(a[1] - b[1]) +
                (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(current_position,target_position)/max_dist;

    return d;
}

void RealMaze::update_info(){

    cv::Mat image;
    video_capture.read(image);
    cv::KeyPoint key_pt;
    image_proc::blob_detection(image,colour_range.first,colour_range.second,key_pt);
    image_proc::pixel_to_world_frame(key_pt,current_position);
}
