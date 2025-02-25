#ifndef SOLAR_PANEL_POSE_H
#define SOLAR_PANEL_POSE_H

#include <opencv2/opencv.hpp>

// 估计光伏板的位姿，如果未检测到标定板，返回0，否则返回1
int estimate_solar_panel_pose(
    Stereo &stereo_model,  // 双目重建模型
    YOLO &seg_model,          // 实例分割模型
    const cv::Mat &left_img,  // 左相机的图像，未经过畸变与立体校正
    const cv::Mat &right_img, // 右相机的图像，未经过畸变与立体校正
    int direction,            // right to left:1   left to right:2
    const cv::Mat &lcam_mat,  // 左相机的投影矩阵
    const cv::Mat &lcam_dist, // 左相机的畸变系数
    const cv::Mat &rcam_mat,  // 右相机的投影矩阵
    const cv::Mat &rcam_dist, // 右相机的畸变系数
    const cv::Mat &r_mat,     // 双目相机的旋转矩阵
    const cv::Mat &t_vect,    // 双目相机的平移向量
    cv::Mat &rotation_matrix, // 返回值，光伏板的旋转矩阵
    cv::Mat &t_vector,        // 返回值，光伏板的平移向量
    float &rms,               // 返回值，光伏板平面拟合残差
    float &angle_x_y);        // 返回值，XY轴的夹角
#endif