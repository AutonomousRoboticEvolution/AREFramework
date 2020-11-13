#include "image_processing/blob_tracking.hpp"

void image_proc::blob_detection(const cv::Mat& image, const cv::Scalar& colorMin, const cv::Scalar& colorMax, cv::KeyPoint& keyPt){

    cv::Mat hsv;
    cv::cvtColor(image,hsv,cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv,colorMin,colorMax,mask);

    cv::Mat masked_image;
    cv::bitwise_and(image,image,masked_image,mask);
    cv::SimpleBlobDetector::Params params = cv::SimpleBlobDetector::Params();
    params.thresholdStep = 255;
    params.minRepeatability = 1;
    params.blobColor = 255;
    params.filterByConvexity = false;
    params.filterByArea = true;
    params.maxArea = 500000;
    params.filterByInertia = false;
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
    }

// if feedback on
    cv::Mat kp_image;
    cv::drawKeypoints(mask,key_pts,kp_image,cv::Scalar(0,0,255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imshow("Frame", image);
    cv::imshow("Blob",kp_image);
    cv::waitKey(1);
    for(const auto k : key_pts)
        std::cout << k.pt.x << "," << k.pt.y << std::endl;
}


void image_proc::pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters){
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
