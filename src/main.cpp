#include <iostream>
#include <opencv2/opencv.hpp>
#include "vision.hpp"

using namespace std;

int main() {
    cv::VideoCapture cap("../assets/jalan.mp4");
    cv::Mat frame, output;

    while (cap.isOpened())
    {
        cap >> frame;
        if (frame.empty()) break;

        kemiringan_jalan(frame, output);
        cv::imshow("Raw Frame", frame);
        cv::imshow("Processed Frame", output);
        if (cv::waitKey(30) == 'q') break;
    }

    return 0;
}
