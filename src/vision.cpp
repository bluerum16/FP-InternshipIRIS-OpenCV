#include "vision.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <string>

using namespace std;
using namespace cv;

LaneInfo proses_lajur(cv::Mat& frame, cv::Mat& output)
{
    LaneInfo info; 

    if (frame.empty()) {
        cerr << "Frame input kosong!" << endl;
        return info;
    }

    cv::Mat gray, blurred, edges;

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(gray, blurred, cv::Size(7, 7), 0);

    cv::Canny(blurred, edges, 170, 200);

    cv::Mat mask = cv::Mat::zeros(edges.size(), edges.type());
    cv::Point pts[4] = {
        cv::Point(frame.cols * 0.05, frame.rows),
        cv::Point(frame.cols * 0.95, frame.rows),
        cv::Point(frame.cols * 0.50, frame.rows * 0.35),
        cv::Point(frame.cols * 0.10, frame.rows * 0.35)
    };
    cv::fillConvexPoly(mask, pts, 4, cv::Scalar(255));
    
    cv::Mat maskedEdges; 
    cv::bitwise_and(edges, mask, maskedEdges);

    frame.copyTo(output);
    cv::Mat lineOverlay = cv::Mat::zeros(frame.size(), frame.type());

    
    vector<Vec4i> lines;
    cv::HoughLinesP(
        maskedEdges, lines, 1, CV_PI / 180,
        30,
        15,  
        150
    );

    vector<double> leftSlopes, rightSlopes;
    
    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i l = lines[i];
        Point p1 = Point(l[0], l[1]);   
        Point p2 = Point(l[2], l[3]);
        cv::line(lineOverlay, p1, p2, cv::Scalar(0, 0, 255), 10, cv::LINE_AA);

        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;

        if (abs(dx) > 0.001)
        {
            double slope = dy / dx;
            double x_mid = (p1.x + p2.x) / 2.0;

            if (slope < -0.4 && x_mid < frame.cols / 2.0) {
                leftSlopes.push_back(slope);
            }
            else if (slope > 0.4 && x_mid > frame.cols / 2.0) {
                rightSlopes.push_back(slope);
            }
        }
    }

    double avgLeftSlope = 0.0, avgRightSlope = 0.0;
    bool leftFound = !leftSlopes.empty();
    bool rightFound = !rightSlopes.empty();

    if (leftFound) {
        avgLeftSlope = std::accumulate(leftSlopes.begin(), leftSlopes.end(), 0.0) / leftSlopes.size();
    }
    if (rightFound) {
        avgRightSlope = std::accumulate(rightSlopes.begin(), rightSlopes.end(), 0.0) / rightSlopes.size();
    }

    if (leftFound && rightFound) {
        info.status = "Detected"; 
        info.slope = (avgLeftSlope + avgRightSlope) / 2.0;
    } else if (leftFound) {
        info.status = "Partial"; 
        info.slope = avgLeftSlope;
    } else if (rightFound) {
        info.status = "Partial"; 
        info.slope = avgRightSlope;
    } else {
        info.status = "Lost"; 
        info.slope = 0.0;
    }

    cv::addWeighted(output, 1.0, lineOverlay, 0.8, 0, output);
    
    string resText = to_string(frame.cols) + "x" + to_string(frame.rows);
    cv::putText(output, resText, Point(15, frame.rows - 15), FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 255, 0), 2); // Ganti ke hijau

    return info; 
}