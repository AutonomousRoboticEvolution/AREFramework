#include <iostream>
#include <chrono>
#include "image_processing/blob_tracking.hpp"

int main(int argc, char** argv){

    if(argc != 5){
        std::cerr << "usage : " << std::endl;
        std::cerr << "\targ 1 : video pipe " << std::endl;
        std::cerr << "\targ 2 : reference point x " << std::endl;
        std::cerr << "\targ 3 : reference point y " << std::endl;
        std::cerr << "\targ 4 : image scale " << std::endl;
        return 1;
    }

    cv::VideoCapture video_capture;
    std::pair<cv::Scalar,cv::Scalar> colour_range;
    std::vector<double> current_position(2);

    std::string pipe = argv[1];

    std::cout << "camera pipe: " << pipe << std::endl;
    bool usingIPCamera = pipe.substr(0,4).compare("http") == 0; // if the pipe starts "http", we're using an IP camera. This is important later because the buffer needs flushing on each update
    std::cout<<"usingIPCamera: "<<usingIPCamera<<std::endl;
    video_capture = cv::VideoCapture(pipe);


    colour_range.first = cv::Scalar(0,100,100);
    colour_range.second = cv::Scalar(10,255,255);

    are::settings::ParametersMapPtr parameters(new are::settings::ParametersMap);
    parameters->emplace("#referencePt",new are::settings::Sequence<int>({std::stoi(argv[2]),std::stoi(argv[3])}));
    parameters->emplace("#cropRectangle",new are::settings::Sequence<int>({240,70,346,346}));
    parameters->emplace("#pixelImageScale",new are::settings::Integer(std::stoi(argv[4])));



    cv::Mat image;
    while(1){
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

        // blob detection:
        std::vector<int> crop_rect = {240,70,346,346};
        cv::KeyPoint key_pt(0,0,0);
        image_proc::blob_detection(image,colour_range.first,colour_range.second,crop_rect,key_pt);
        image_proc::keypoint_pixel_to_world_frame(key_pt,current_position,parameters);
        for(const auto &val : current_position){
            std::cout << val << ",";
        }
        std::cout << std::endl;

        // aruco testing:
//        std::cout<<"doing aruco test"<<std::endl;
//        std::vector<int> marker_ids;
//        std::vector<cv::Point2f> marker_centre_points;
//        image_proc::aruco_detection(image, marker_ids, marker_centre_points);
        cv::Point2f barrel_position = image_proc::track_a_barrel(image, parameters);
        std::cout<<"barrel position: "<<barrel_position<<std::endl;
    }
    return 0;
}
