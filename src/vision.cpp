#include "vision.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace std;

double kemiringan_jalan(cv::Mat& frame, cv::Mat& output)
{
    if (frame.empty()) {
        cerr << "Frame kosong!" << endl;
        return 0.0;
    }

    cv::Mat gray, blurred, edges;

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::threshold(gray, gray, 160, 255, cv::THRESH_BINARY);

    cv::GaussianBlur(gray, blurred, cv::Size(1, 1), 0);

    // cv::Canny(blurred, edges, 30, 120);

    output = blurred;

    return 1.0;
}