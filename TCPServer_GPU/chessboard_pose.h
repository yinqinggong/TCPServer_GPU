#ifndef CHESSBOARD_POSE_H
#define CHESSBOARD_POSE_H

#include <opencv2/opencv.hpp>

// 估计标定板的位姿，如果未检测到标定板，返回0，否则返回1
int estimate_chessboard_pose(
    const cv::Mat &left_img,  // 左相机的图像，未经过畸变与立体校正
    const cv::Mat &right_img, // 右相机的图像，未经过畸变与立体校正
    const cv::Mat &lcam_mat,  // 左相机的投影矩阵
    const cv::Mat &lcam_dist, // 左相机的畸变系数
    const cv::Mat &rcam_mat,  // 右相机的投影矩阵
    const cv::Mat &rcam_dist, // 右相机的畸变系数
    const cv::Mat &r_mat,     // 双目相机的旋转矩阵
    const cv::Mat &t_vect,    // 双目相机的平移向量
    cv::Mat &rotation_matrix, // 返回值，标定板的旋转矩阵, 3x3
    cv::Mat &t_vector,        // 返回值，标定板的平移向量, 3x1
    float &rms,               // 返回值，标定板平面拟合残差
    float &angle_x_y);        // 返回值，XY轴的夹角
#endif