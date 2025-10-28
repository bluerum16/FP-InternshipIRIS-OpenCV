#ifndef VISION_HPP
#define VISION_HPP

#include <opencv2/opencv.hpp>
#include <string> 

struct LaneInfo {
    double slope = 0.0;
    std::string status = "Lost"; 
};

LaneInfo proses_lajur(cv::Mat& frame, cv::Mat& output);

#endif    