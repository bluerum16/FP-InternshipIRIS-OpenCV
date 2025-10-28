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

    // --- Variabel Kecepatan dan Jarak ---
    double totalDistance = 0.0;       // meter
    const double FAKE_SPEED_CMS = 1666.67; // = 60 km/j dalam cm/s

    // --- Loop utama ---
    while (cap.isOpened()) {
        cap >> frame;
        if (frame.empty()) break;

        cv::resize(frame, frame, resize_gambar, 0, 0, cv::INTER_AREA);

        double t = (double)cv::getTickCount();
        LaneInfo laneInfo = proses_lajur(frame, output);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

        // Hitung FPS dan jarak tempuh
        double fps = 1.0 / t;
        totalDistance += FAKE_SPEED_CMS * t / 100.0; // jarak dalam meter

        double angle = std::atan(laneInfo.slope) * 180.0 / CV_PI;

        // --- Buat HUD di atas hasil output ---
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
        speedStream << "Speed: " << std::fixed << std::setprecision(0) << FAKE_SPEED_CMS << " cm/s";
        std::string speedText = speedStream.str();

        // Gauge Kecepatan
        cv::rectangle(output, cv::Rect(cv::Point(20, 170), cv::Point(270, 190)), cv::Scalar(50, 50, 50), -1);
        int gaugeWidth = (int)((FAKE_SPEED_CMS / 3000.0) * 250.0);
        if (gaugeWidth > 250) gaugeWidth = 250;
        cv::rectangle(output, cv::Rect(cv::Point(20, 170), cv::Point(20 + gaugeWidth, 190)), cv::Scalar(0, 255, 0), -1);

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
