/**
	@file Camera.hpp
	@brief Header and documentation for Camera.cpp, for getting aruco tag infomation from the Head's pi camera
	@author Matt Hale
*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp> // for cv::VideoCapture
#include <list>
#include <wiringPi.h> //for timing functions

class Camera{
    public:
        Camera();

        std::vector<int> tags_to_look_for;

        void setTagsToLookFor(std::vector<int> tags_list);

        /**
         * see if a aruco tag (in the list tags_to_look_for) is visible to the camera
         */
        bool presenceDetect(std::string save_image_for_debugging="");

private:
        /**
            the dictionary that defines the settings for aruco tags,
            i.e. 4x4 grid, and only using 50 possible tags
        */
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
        
        // detection paramters (defaults):
        cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();

        cv::VideoCapture inputVideo; // videoCapture object for the default camera

};

#endif
