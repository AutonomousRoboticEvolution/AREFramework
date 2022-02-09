#include "image_processing/blob_tracking.hpp"

void image_proc::aruco_detection(const cv::Mat& image){

    // make empty variables to fill
    std::vector<int> markerIds; // the list of aruco tags in the image
    std::vector<std::vector<cv::Point2f>> markerCorners , rejectedCandidates; // details of where in the image the tags are

    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    // do the aruco detection
    cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    // draw the image with markers on
    cv::Mat outputImage = image.clone();
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    cv::imshow("out", outputImage);
}

void image_proc::blob_detection(const cv::Mat& image, const cv::Scalar& colorMin, const cv::Scalar& colorMax, const std::vector<int> &crop_rect, cv::KeyPoint& keyPt){


    cv::Mat cropped;

    if(crop_rect[0] >= 0)
    {
        cv::Rect rect(crop_rect[0],
                crop_rect[1],
                crop_rect[2],
                crop_rect[3]);
        cv::Mat croppedRef(image,rect);
        croppedRef.copyTo(cropped);
    }else cropped = image;

    cv::Mat hsv;
    cv::cvtColor(cropped,hsv,cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv,colorMin,colorMax,mask);

    cv::Mat masked_image;
    cv::bitwise_and(cropped,cropped,masked_image,mask);
    cv::SimpleBlobDetector::Params params = cv::SimpleBlobDetector::Params();
    params.thresholdStep = 255;
    params.minRepeatability = 1;
    params.blobColor = 255;
    params.filterByConvexity = false;
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 500000;
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
    std::vector<cv::KeyPoint> key_pts;
    detector->detect(mask,key_pts);
    if(!key_pts.empty()){
        float max_size = key_pts[0].size;
        keyPt = key_pts[0];
        for(const cv::KeyPoint& kp : key_pts){
            if(max_size < kp.size){
                keyPt = kp;
                max_size = kp.size;
            }
        }
//        cv::Mat kp_image;
//        cv::drawKeypoints(mask,{keyPt},kp_image,cv::Scalar(0,0,255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // just the main keypoint
//        cv::imshow("Blob",kp_image);

        std::cout<< keyPt.pt.x <<","<< keyPt.pt.y ; // main keypoint first
//        for(const cv::KeyPoint& kp : key_pts){ std::cout<<","<< kp.pt.x <<","<< kp.pt.y ;} // all the keypoints (in case the wrong one is picked up)
        std::cout<<std::endl;
    }else{
        keyPt.pt.x = -999;
        keyPt.pt.y = -999;
        std::cout<< -999 <<","<< -999 <<std::endl; // no keypoints found
    }


    cv::Mat kp_image;
    cv::drawKeypoints(mask,key_pts,kp_image,cv::Scalar(0,0,255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imshow("Blob",kp_image);
    //cv::imshow("mask",masked_image);
    cv::imshow("frame", cropped);
    cv::waitKey(1);

}

void image_proc::pixel_to_world_frame(const cv::KeyPoint &point, std::vector<double> &position, const are::settings::ParametersMapPtr& parameters){
    std::vector<int> reference_pt =  are::settings::getParameter<are::settings::Sequence<int>>(parameters,"#referencePt").value;
    int scale = are::settings::getParameter<are::settings::Integer>(parameters,"#pixelImageScale").value;
    std::vector<int> crop_rect = are::settings::getParameter<are::settings::Sequence<int>>(parameters,"#cropRectangle").value;
    if(crop_rect[0] >= 0){
        reference_pt[0] = reference_pt[0] - crop_rect[0];
        reference_pt[1] = reference_pt[1] - crop_rect[1];
    }
//    int scale = pixel_image_scale/2.;

    position.resize(2);
    position[0] = (point.pt.x - reference_pt[0])/scale;
    position[1] = -(point.pt.y - reference_pt[1])/scale;
}

void image_proc::topdown_camera_pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters){
    double focal = are::settings::getParameter<are::settings::Double>(parameters,"#focalDistance").value;
    double o_x = are::settings::getParameter<are::settings::Double>(parameters,"#principlePoint_x").value;
    double o_y = are::settings::getParameter<are::settings::Double>(parameters,"#principlePoint_y").value;
    double s_x = are::settings::getParameter<are::settings::Double>(parameters,"#aspectRatio_x").value;
    double s_y = are::settings::getParameter<are::settings::Double>(parameters,"#aspectRatio_y").value;
    double z = are::settings::getParameter<are::settings::Double>(parameters,"#cameraHeight").value;


    double x,y;
    x = (point.pt.x-o_x)*s_x*z/focal;
    y = (point.pt.y-o_y)*s_y*z/focal;

    position[0] = x;
    position[1] = y;
}
