#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "vision.hpp"

int main() {
    cv::VideoCapture cap("../assets/jalan.mp4");
    cv::Mat frame, output;
    cv::Size resize_gambar(640, 480);

    if (!cap.isOpened()) {
        std::cerr << "❌ Error: Tidak bisa membuka video" << std::endl;
        return -1;
    }

    double totalDistance = 0.0;      
    const double Kecepatan = 1666.67; // dalam cm/s

    // --- Loop utama ---
    while (cap.isOpened()) {
        cap >> frame;
        if (frame.empty()) break;

        cv::resize(frame, frame, resize_gambar, 0, 0, cv::INTER_AREA);

        double t = (double)cv::getTickCount();
        LaneInfo laneInfo = proses_lajur(frame, output);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

        double fps = 1.0 / t;
        totalDistance += Kecepatan * t / 100.0; 

        double angle = std::atan(laneInfo.slope) * 180.0 / CV_PI;

        cv::Mat hudPanel;
        output.copyTo(hudPanel);
        cv::rectangle(hudPanel, cv::Rect(10, 10, 350, 210), cv::Scalar(0, 0, 0), -1);
        cv::addWeighted(output, 1.0, hudPanel, 0.5, 0, output);

        std::string fpsText = "FPS: " + std::to_string((int)fps);

        // Status Jalur
        std::string statusText;
        cv::Scalar statusColor;
        if (laneInfo.status == "Detected") {
            statusText = "TERDETEKSI";
            statusColor = cv::Scalar(0, 255, 0);
        } else if (laneInfo.status == "Partial") {
            statusText = "SEBAGIAN";
            statusColor = cv::Scalar(0, 255, 255);
        } else {
            statusText = "HILANG";
            statusColor = cv::Scalar(0, 0, 255);
        }

        std::stringstream angleStream;
        angleStream << "Sudut: " << std::fixed << std::setprecision(1) << angle << " deg";
        std::string angleText = angleStream.str();

        std::stringstream distStream;
        distStream << "Jarak: " << std::fixed << std::setprecision(2) << totalDistance << " m";
        std::string distText = distStream.str();

        std::stringstream speedStream;
        speedStream << "Speed: " << std::fixed << std::setprecision(0) << Kecepatan << " cm/s";
        std::string speedText = speedStream.str();


        // Tampilkan teks ke layar
        int y_pos = 40;
        cv::putText(output, fpsText, cv::Point(20, y_pos), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        y_pos += 30;
        cv::putText(output, "Status: " + statusText, cv::Point(20, y_pos), cv::FONT_HERSHEY_SIMPLEX, 0.7, statusColor, 2);
        y_pos += 30;
        cv::putText(output, angleText, cv::Point(20, y_pos), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 0), 2);
        y_pos += 30;
        cv::putText(output, distText, cv::Point(20, y_pos), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 0), 2);
        y_pos += 60;
        cv::putText(output, speedText, cv::Point(20, y_pos), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

        // --- Tampilkan hasil ---
        cv::imshow("Frame", frame);
        cv::imshow("Processed Frame", output);

        int key = cv::waitKey(10);
        if (key == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
