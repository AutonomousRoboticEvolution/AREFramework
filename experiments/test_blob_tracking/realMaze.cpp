#include "realMaze.hpp"

using namespace are;

RealMaze::RealMaze(): Environment(){
    current_position.resize(2);
    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target_x",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_y",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_z",new settings::Double(0.05));
}

void RealMaze::init(){

     std::string pipe = settings::getParameter<settings::String>(parameters,"#cameraPipe").value;
     bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
     if(verbose)
         std::cout << "camera pipe: " << pipe << std::endl;

     video_capture = cv::VideoCapture(pipe);

     colour_range.first = cv::Scalar(0,100,110);
     colour_range.second = cv::Scalar(200,255,255);


     target_position = {settings::getParameter<settings::Double>(parameters,"#target_x").value,
                        settings::getParameter<settings::Double>(parameters,"#target_y").value};


}

std::vector<double> RealMaze::fitnessFunction(const Individual::Ptr &ind){

    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                (a[1] - b[1])*(a[1] - b[1]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(current_position,target_position)/max_dist;

    return d;
}

void RealMaze::update_info(){

    cv::Mat image;
    video_capture.read(image);
    cv::KeyPoint key_pt(0,0,0);
    image_proc::blob_detection(image,colour_range.first,colour_range.second,key_pt);
    image_proc::pixel_to_world_frame(key_pt,current_position,parameters);
    for(const auto &val : current_position)
        std::cout << val << ",";
    std::cout << std::endl;
}
