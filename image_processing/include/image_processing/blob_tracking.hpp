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
void blob_detection(const cv::Mat& image, const cv::Scalar& colorMin, const cv::Scalar& colorMax, const std::vector<int> &crop_rect, cv::KeyPoint& keyPt);

/**
 * @brief from an image (from the overhead camera), will return the barrel position in the world frame, in meters(?)
 * @param image
 * @param barrel_position
 */
cv::Point2f track_a_barrel(const cv::Mat& image, const are::settings::ParametersMapPtr &parameters);

/**
 * @brief from an image (from the overhead camera), will return the IDs of all the visible markers, and their positions in pixels
 * @param image
 * @param marker_ids: the visible marker IDs
 * @param marker_centre_points: the positions of the centre of the markers in pixels
 */
void aruco_detection(const cv::Mat& image, std::vector<int>& marker_ids , std::vector<cv::Point2f>& marker_centre_points);

/**
 * @brief image_proc::convert_pixel_to_world_frame
 * @param pixel_position: the position in pixels, as a openCV 2D point (cv::Point2f)
 * @return the position in world frame, as a openCV 2D point (cv::Point2f)
 */
cv::Point2f convert_pixel_to_world_frame(cv::Point2f pixel_position,  const are::settings::ParametersMapPtr& parameters);

/**
 * @brief pixel_to_world_frame
 * @param point
 * @param position
 * @param parameters
 */
void keypoint_pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters);


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
//void topdown_camera_pixel_to_world_frame(const cv::KeyPoint &point,std::vector<double> &position, const are::settings::ParametersMapPtr& parameters);

}

#endif //BLOB_TRACKING_HPP
