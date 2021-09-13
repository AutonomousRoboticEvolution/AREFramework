#include "realMaze.hpp"
#include <chrono>

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
     usingIPCamera = pipe.substr(0,4).compare("http") == 0; // if the pipe starts "http", we're using an IP camera. This is important later because the buffer needs flushing on each update
     if(verbose) std::cout<<"usingIPCamera: "<<usingIPCamera<<std::endl;
     video_capture = cv::VideoCapture(pipe);

     colour_range.first = cv::Scalar(0,104,96);
     colour_range.second = cv::Scalar(51,255,181);


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

void RealMaze::update_info(double time){

    cv::Mat image;
    if (usingIPCamera==true){
        // this is a rather hacky method to flush the buffer created when using an IP camera.
        // We time how long it takes to get a frame - if it was very quick, it probably came from the buffer, so we discard it and get another frame.
        // This only works because we are requesting frames at a slower rate than they are created.
        // inspired by answers.opencv.org/question/29957/highguivideocapture-buffer-introducing-lag/?answer=38217#post-id-38217/
        int framesWithDelayCount = 0;
        while (framesWithDelayCount < 1)
        {
            auto time_before = std::chrono::system_clock::now();
            video_capture.grab();
            auto time_after = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = time_after-time_before;
            if(elapsedTime > std::chrono::milliseconds(3)) framesWithDelayCount++;
        }
        video_capture.retrieve(image);
    }else{
        video_capture.read(image);
    }

    if (image.empty()) {
        std::cerr << "ERROR! the frame is empty\n";
    }

    cv::KeyPoint key_pt(0,0,0);
    image_proc::blob_detection(image,colour_range.first,colour_range.second,key_pt);
    image_proc::pixel_to_world_frame(key_pt,current_position,parameters);
//    for(const auto &val : current_position)
//        std::cout << val << ",";
//    std::cout << key_pt.pt.x <<","<<key_pt.pt.y << std::endl;
}

