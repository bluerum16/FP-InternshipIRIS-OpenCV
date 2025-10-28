#include "vision.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <string>

using namespace std;
using namespace cv;

// Nama fungsi dan tipe return diubah
LaneInfo proses_lajur(cv::Mat& frame, cv::Mat& output)
{
    // Struct untuk menampung hasil
    LaneInfo info; // Default statusnya "Lost" dari .hpp

    if (frame.empty()) {
        cerr << "Frame input kosong!" << endl;
        return info; // Kembalikan info (status "Lost")
    }

    cv::Mat gray, blurred, thresholded;
    
    // --- 1. Grayscale ---
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // --- 2. Gaussian Blur ---
    cv::GaussianBlur(gray, blurred, cv::Size(7, 7), 0);

    // --- 3. Binary Threshold ---
    cv::threshold(blurred, thresholded, 160, 255, cv::THRESH_BINARY);

    // --- 4. Region of Interest (ROI) ---
    cv::Mat mask = cv::Mat::zeros(thresholded.size(), thresholded.type());
    cv::Point pts[4] = {
        cv::Point(frame.cols * 0.05, frame.rows),
        cv::Point(frame.cols * 0.95, frame.rows),
        cv::Point(frame.cols * 0.7, frame.rows * 0.28),
        cv::Point(frame.cols * 0.3, frame.rows * 0.28)
    };
    cv::fillConvexPoly(mask, pts, 4, cv::Scalar(255));
    cv::Mat maskedThreshold;
    cv::bitwise_and(thresholded, mask, maskedThreshold);

    // --- 5. Inisialisasi Output Visual ---
    frame.copyTo(output);
    cv::Mat lineOverlay = cv::Mat::zeros(frame.size(), frame.type());

    // --- 6. Deteksi Garis (Hough Transform) ---
    vector<Vec4i> lines;
    cv::HoughLinesP(
        maskedThreshold, lines, 1, CV_PI / 180,
        100, // Threshold
        30,  // minLineLength
        100  // maxLineGap
    );

    // --- 7. Hitung Kemiringan (Slope) ---
    vector<double> leftSlopes, rightSlopes;
    // Hapus lineCount, kita akan cek .empty()

    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i l = lines[i];
        Point p1 = Point(l[0], l[1]);   
        Point p2 = Point(l[2], l[3]);
        cv::line(lineOverlay, p1, p2, cv::Scalar(0, 0, 255), 5, cv::LINE_AA);

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

    // --- 8. Rata-ratakan Slope ---
    double avgLeftSlope = 0.0, avgRightSlope = 0.0;
    bool leftFound = !leftSlopes.empty();
    bool rightFound = !rightSlopes.empty();

    if (leftFound) {
        avgLeftSlope = std::accumulate(leftSlopes.begin(), leftSlopes.end(), 0.0) / leftSlopes.size();
    }
    if (rightFound) {
        avgRightSlope = std::accumulate(rightSlopes.begin(), rightSlopes.end(), 0.0) / rightSlopes.size();
    }

    // --- 9. Set data untuk dikembalikan (LOGIKA BARU SESUAI PERMINTAAN) ---
    if (leftFound && rightFound) {
        info.status = "Detected"; // Kiri dan kanan ditemukan
        info.slope = (avgLeftSlope + avgRightSlope) / 2.0;
    } else if (leftFound) {
        info.status = "Partial"; // Hanya kiri
        info.slope = avgLeftSlope;
    } else if (rightFound) {
        info.status = "Partial"; // Hanya kananbool leftFound = !leftSlopes.empty();
    bool rightFound = !rightSlopes.empty();
        info.slope = avgRightSlope;
    } else {
        info.status = "Lost"; // Tidak ada yang ditemukan
        info.slope = 0.0;
    }

    // --- 10. Finalisasi Output Visual ---
    cv::addWeighted(output, 1.0, lineOverlay, 0.8, 0, output);
    
    string resText = to_string(frame.cols) + "x" + to_string(frame.rows);
    cv::putText(output, resText, Point(15, frame.rows - 15), FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 0, 255), 2);
    
    // Kembalikan struct
    return info; 
}