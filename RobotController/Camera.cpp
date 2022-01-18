#include "Camera.hpp"
//#include <opencv2/imgcodecs.hpp> // imwrite/imread
#include <wiringPi.h> //for timing functions

#define debugPrinting false
#include <iostream>

Camera::Camera() {
    inputVideo.set(cv::CAP_PROP_BUFFERSIZE, 0); // internal buffer will now store only 1 frame
    inputVideo.open(0); // start the camera feed
}

void Camera::setTagsToLookFor(std::vector<int> tags_list){
    tags_to_look_for=tags_list;
}

bool Camera::presenceDetect(){
    //if(debugPrinting){std::cout<<"cam test..."<<std::endl;}
    u_int32_t start_time = millis();

    cv::Mat image; // create empty array to store the image in
    inputVideo.grab();
    inputVideo.retrieve(image); // get camera image

    std::vector<int> foundTags; // the list of aruco tags in the image
    std::vector<std::vector<cv::Point2f>> markerCorners , rejectedCandidates; // details of where in the image the tags are
    cv::aruco::detectMarkers(image, dictionary, markerCorners, foundTags, parameters, rejectedCandidates);

    bool correct_tag_seen=false;
    for(int i=0; i<foundTags.size(); i++){
        for(int j=0; j<tags_to_look_for.size(); j++){
            if(foundTags[i] == tags_to_look_for[j]) { correct_tag_seen=true; }
        }    
    }
    
    u_int32_t end_time = millis();
    // debugging printing:
    if(debugPrinting){
        std::cout<<"Time taken: "<<end_time-start_time<<" ms\t";

        //cv::aruco::drawDetectedMarkers(image, markerCorners,foundTags);
        //cv::aruco::drawDetectedMarkers(image, rejectedCandidates);
        //cv::imwrite("/home/pi/debugOutput.png", image);

        if (foundTags.size() > 0){
            std::cout<<"can see tags:\t";
            for(int i=0;i<foundTags.size();i++){
                std::cout<<foundTags[i]<<",";
            }
            std::cout<<"\t correct?: "<<correct_tag_seen;
            std::cout<<std::endl;
        }else{
            std::cout<<"no tags seen"<<std::endl;
        }
    }

    if (correct_tag_seen){
        return true;
    }
    return false;
}
