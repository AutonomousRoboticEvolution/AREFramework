#include "image_processing/blob_tracking.hpp"

void image_proc::blob_detection(const cv::Mat& image, const cv::Scalar& colorMin, const cv::Scalar& colorMax, cv::KeyPoint& keyPt){

    cv::Rect rect(240,70,346,346);
    cv::Mat croppedRef(image,rect);
    cv::Mat cropped;
    croppedRef.copyTo(cropped);
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
        cv::Mat kp_image;
        cv::drawKeypoints(mask,{keyPt},kp_image,cv::Scalar(0,0,255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // just the main keypoint
        cv::imshow("Blob",kp_image);
        cv::waitKey(1);

        std::cout<< keyPt.pt.x <<","<< keyPt.pt.y ; // main keypoint first
        for(const cv::KeyPoint& kp : key_pts){ std::cout<<","<< kp.pt.x <<","<< kp.pt.y ;} // all the keypoints (in case the wrong one is picked up)
        std::cout<<std::endl;
    }else{
        std::cout<< -999 <<","<< -999 <<std::endl; // no keypoints found
    }



    cv::Mat kp_image;
    cv::drawKeypoints(mask,key_pts,kp_image,cv::Scalar(0,0,255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::imshow("Blob",kp_image);
    cv::imshow("frame", cropped);


}

void image_proc::pixel_to_world_frame(const cv::KeyPoint &point, std::vector<double> &position, const are::settings::ParametersMapPtr& parameters){
    int reference_pt_x =  are::settings::getParameter<are::settings::Integer>(parameters,"#referencePtX").value;
    int reference_pt_y =  are::settings::getParameter<are::settings::Integer>(parameters,"#referencePtY").value;
    int pixel_image_scale = are::settings::getParameter<are::settings::Integer>(parameters,"#pixelImageScale").value;

    int scale = pixel_image_scale/2.;

    position.resize(2);
    position[0] = (point.pt.x - reference_pt_x)/scale;
    position[1] = -(point.pt.y - reference_pt_y)/scale;
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
