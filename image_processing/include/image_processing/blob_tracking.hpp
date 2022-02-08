#ifndef BLOB_TRACKING_HPP
#define BLOB_TRACKING_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "ARE/Settings.h"

namespace image_proc {

/**
 * @brief Unicolour blob detector. Take an image and a range of colours, return the position of the biggest blob detected.
 * @param image
 * @param colorMin
 * @param colorMax
 * @param keyPts
 */
void blob_detection(const cv::Mat& image, const cv::Scalar& colorMin, const cv::Scalar& colorMax, cv::KeyPoint& keyPt);

void aruco_detection(const cv::Mat& image);

/**
 * @brief pixel_to_world_frame
 * @param point
 * @param position
 * @param parameters
 */
void pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters);


/**
 * @brief conversion from pixel coordinate to world 2D coordinate
 *        \f$x = \frac{(u-o_x)*s_x*z}{f}\f$
 *        \f$y = \frac{(u-o_y)*s_y*z}{f}\f$
 * Where :
 *      - (x,y) are the coordinates in world frame
 *      - (u,v) are the coordinates in pixel frame
 *      - (o_x,o_y) the principle point of the camera
 *      - f the focal distance of the camera
 *      - (s_x,s_y) the aspect ratio of the image
 *      - z the height of the camera from the ground.
 * This formula works only for top down cameras.
 * @param point
 * @param position
 */
void topdown_camera_pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters);


}

#endif //BLOB_TRACKING_HPP
