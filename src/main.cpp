#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture cap("../assets/jalan.mp4");
    cv::Mat frame;

    while (cap.isOpened())
    {
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("Video Frame", frame);
        if (cv::waitKey(30) == 'q') break;

        
    }
    
    return 0;
}