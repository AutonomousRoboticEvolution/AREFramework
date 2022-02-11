#include "image_processing/blob_tracking.hpp"

bool debug_plotting_tracking = true; //TODO make parameter input
int aruco_tag_id_of_barrel = 42;

cv::Point2f image_proc::track_a_barrel(const cv::Mat& image, const are::settings::ParametersMapPtr& parameters){

    // make some empty variables to fill:
    std::vector<int> found_tag_ids;
    std::vector<cv::Point2f> marker_centre_points;

    // get the positions of all visible aruco tags:
    image_proc::aruco_detection(image, found_tag_ids, marker_centre_points);

    // loop through each of the visible tags, and if is the tag we are looking for, set it as barrel position
    std::cout<<"found_tag_ids.size() "<<found_tag_ids.size()<<std::endl;
    for(int i=0; i<found_tag_ids.size(); i++){
        std::cout<<"checking tag"<<found_tag_ids[i];
            if(found_tag_ids[i] == aruco_tag_id_of_barrel) {
                std::cout<<" yes"<<std::endl;
                return convert_pixel_to_world_frame ( marker_centre_points[i], parameters); // return the position, converted to world frame
            }
            std::cout<<" no"<<std::endl;
    }

    // if we get this far, the correct tag was never seen...
    return cv::Point2f (-999,-999);
}


void image_proc::aruco_detection(const cv::Mat& image, std::vector<int>& marker_ids , std::vector<cv::Point2f>& marker_centre_points){

    // make empty variables to fill
    marker_ids.clear();
    marker_centre_points.clear();
    std::vector<std::vector<cv::Point2f>> markerCorners; // details of where in the image the tags are

    cv::Ptr<cv::aruco::DetectorParameters> aruco_parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    // do the aruco detection
    cv::aruco::detectMarkers(image, dictionary, markerCorners, marker_ids, aruco_parameters);

    // compute marker centres from marker corners
    for(const auto& corners : markerCorners){ // for each set of 4 corners
        cv::Point2f centre_of_this_marker(0.f,0.f);
        for(const auto& corner : corners) { // for each corner within the set of 4
                centre_of_this_marker += corner; // add on the corner value to make a running total
            }
        centre_of_this_marker = centre_of_this_marker / 4.; // divide by four (the number of corners) to get the average position
        marker_centre_points.push_back( centre_of_this_marker ); // add to the list of marker centre points
    }


    if (debug_plotting_tracking){
    // draw the image with markers on
        cv::Mat outputImage = image.clone();
        cv::aruco::drawDetectedMarkers(outputImage, markerCorners, marker_ids);
        cv::imshow("out", outputImage);

        // debug printing:
        std::cout<<"Seen aruco markers:"<<std::endl;
        for (int i=0; i<marker_centre_points.size(); i++){
            int id = marker_ids[i];
            std::cout<<"Marker ID: "<<marker_ids[i]<<"\tPosition:";
            std::cout<<marker_centre_points[i]<<std::endl;
        }
        std::cout<<"---"<<std::endl;
    }
    std::cout<<"marker_ids.size() "<<marker_ids.size()<<std::endl;
}

void image_proc::blob_detection(const cv::Mat& image, const cv::Scalar& colorMin, const cv::Scalar& colorMax, const std::vector<int> &crop_rect, cv::KeyPoint& keyPt){

    // create a cropped version of the tracking camera image
    cv::Mat cropped;
    if(crop_rect[0] >= 0)
    {
        cv::Rect rect(crop_rect[0],
                crop_rect[1],
                crop_rect[2],
                crop_rect[3]);
        cv::Mat croppedRef(image,rect);
        croppedRef.copyTo(cropped);
    } else { // if the first value in crop_rect is set to a negative number (e.g. -1), then we do no cropping
        cropped = image;
    }

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

    // plotting the result on the screen
    if(debug_plotting_tracking){
        cv::Mat kp_image;
        cv::drawKeypoints(mask,key_pts,kp_image,cv::Scalar(0,0,255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imshow("Blob",kp_image);
        //cv::imshow("mask",masked_image);
        cv::imshow("frame", cropped);
        cv::waitKey(1);
    }
}

void image_proc::keypoint_pixel_to_world_frame(const cv::KeyPoint &point, std::vector<double> &position, const are::settings::ParametersMapPtr& parameters){
    cv::Point2f pixel_position (point.pt.x,point.pt.y);
    cv::Point2f world_position = image_proc::convert_pixel_to_world_frame(pixel_position, parameters);


    position[0] = world_position.x;
    position[1] = world_position.y;
}

//void image_proc::topdown_camera_pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters){

//    cv::Point2f pixel_position (point.pt.x,point.pt.y);
//    cv::Point2f world_position = image_proc::convert_pixel_to_world_frame(pixel_position, parameters);

//    position[0] = world_position.x;
//    position[1] = world_position.y;
//}

cv::Point2f image_proc::convert_pixel_to_world_frame(cv::Point2f pixel_position,  const are::settings::ParametersMapPtr& parameters){
    std::cout<<"in convert_pixel_to_world_frame, pixel_position:"<<pixel_position.x<<" , "<<pixel_position.y<<std::endl;

    int pixelImageScale = are::settings::getParameter<are::settings::Integer>(parameters,"#pixelImageScale").value;
    std::vector<int> reference_pt =  are::settings::getParameter<are::settings::Sequence<int>>(parameters,"#referencePt").value;
    std::vector<int> crop_rect = are::settings::getParameter<are::settings::Sequence<int>>(parameters,"#cropRectangle").value;

    if(crop_rect[0] >= 0){
        reference_pt[0] = reference_pt[0] - crop_rect[0];
        reference_pt[1] = reference_pt[1] - crop_rect[1];
    }

    double x,y;
    x = (pixel_position.x - reference_pt[0]) / pixelImageScale;
    y = -(pixel_position.y - reference_pt[1]) / pixelImageScale;

    std::cout<<cv::Point2f (x,y)<<std::endl;
    return cv::Point2f (x,y);
}
