#ifndef INTRINSIC_H
#define INTRINSIC_H
#include <string>
#include <opencv2/opencv.hpp>

int load_intrinsic(
    const std::string& intrinsic_path, // intrinsic file path
    cv::Mat& lcam_mat,  // projection matrix of left camera
    cv::Mat& lcam_dist, // distortion cofficients of left camera
    cv::Mat& rcam_mat,  // projection matrix of right camera
    cv::Mat& rcam_dist, // distortion cofficients of right camera
    cv::Mat &r_mat,   // rotation matrix of right camera to left camear
    cv::Mat &t_vect); // translation vector of right camera to left camera
#endif