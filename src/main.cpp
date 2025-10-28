#include <iostream>
#include <string>
#include <iomanip> 
#include <cmath>   
#include <chrono>    
#include <thread>    

#include <opencv2/opencv.hpp>
#include "vision.hpp"       
#include "websocket.hpp"    

int main() {
    std::string uri = "ws://localhost:2007"; 
    
    if (!ws_init(uri)) {
        std::cerr << "Inisialisasi WebSocket gagal." << std::endl;
        return -1;
    }
    
    std::cout << "Menghubungkan ke " << uri << "..." << std::endl;
    int retries = 5; 
    while (!ws_is_ready() && retries > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        retries--;
    }

    if (!ws_is_ready()) {
        std::cerr << "Gagal terhubung ke WebSocket setelah 5 detik." << std::endl;
        ws_close();
        return -1;
    }
    std::cout << "Koneksi sukses. Memulai loop kamera..." << std::endl;

    
    
    cv::VideoCapture cap(0); 
    cv::Mat frame, output;
    cv::Size targetSize(640, 480); 
    double totalDistance = 0.0; 
    const double FAKE_SPEED_CMS = 1666.67; 
    // --------------------------------------------------

    if (!cap.isOpened()) {
        std::cerr << "Error: Tidak bisa membuka kamera 0" << std::endl;
        ws_close();
        return -1;
    }

    while (cap.isOpened() && ws_is_ready())
    {
        cap >> frame; 
        if (frame.empty()) break;

        cv::resize(frame, frame, targetSize, 0, 0, cv::INTER_AREA);

        double t = (double)cv::getTickCount();
        LaneInfo laneInfo = proses_lajur(frame, output); 
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        
        double fps = 1.0 / t;
        totalDistance += FAKE_SPEED_CMS * t / 100.0;
        double angle = std::atan(laneInfo.slope) * 180.0 / CV_PI;
        std::string laneStatus = laneInfo.status;

        // --- 5. Kirim Data ---
        
        std::vector<unsigned char> buf_jpg_raw; 
        cv::imencode(".jpg", frame, buf_jpg_raw); 
        ws_send_image(buf_jpg_raw, frame.cols, frame.rows);
        
        ws_send_telemetry(FAKE_SPEED_CMS, angle, totalDistance, laneStatus);
        
        cv::Mat hudPanel;
        output.copyTo(hudPanel);
        cv::rectangle(hudPanel, cv::Rect(10, 10, 350, 210), cv::Scalar(0, 0, 0), -1);
        cv::addWeighted(output, 1.0, hudPanel, 0.5, 0, output);
        
        std::string fpsText = "FPS: " + std::to_string((int)fps);
        std::string statusText;
        cv::Scalar statusColor;
        if (laneInfo.status == "Detected") { statusText = "TERDETEKSI"; statusColor = cv::Scalar(0, 255, 0); }
        else if (laneInfo.status == "Partial") { statusText = "SEBAGIAN"; statusColor = cv::Scalar(0, 255, 255); }
        else { statusText = "HILANG"; statusColor = cv::Scalar(0, 0, 255); }
        
        std::stringstream angleStream, distStream, speedStream;
        angleStream << "Sudut: " << std::fixed << std::setprecision(1) << angle << " deg";
        distStream << "Jarak: " << std::fixed << std::setprecision(2) << totalDistance << " m";
        speedStream << "Speed: " << std::fixed << std::setprecision(0) << FAKE_SPEED_CMS << " cm/s";
        std::string angleText = angleStream.str();
        std::string distText = distStream.str();
        std::string speedText = speedStream.str();
        
        cv::rectangle(output, cv::Rect(cv::Point(20, 170), cv::Point(270, 190)), cv::Scalar(50, 50, 50), -1);
        int gaugeWidth = (int)((FAKE_SPEED_CMS / 3000.0) * 250.0); 
        if (gaugeWidth > 250) gaugeWidth = 250;
        cv::rectangle(output, cv::Rect(cv::Point(20, 170), cv::Point(20 + gaugeWidth, 190)), cv::Scalar(0, 255, 0), -1);
        
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

        
        std::vector<unsigned char> buf_jpg_processed;
        cv::imencode(".jpg", output, buf_jpg_processed); 
        ws_send_processed_image(buf_jpg_processed, output.cols, output.rows);

        cv::imshow("Processed Frame", output);
        
        int key = cv::waitKey(10); 
        if (key == 'q' || key == 113 || key == 27) {
            break;
        }
    }

    std::cout << "Menutup program..." << std::endl;
    cap.release(); 
    cv::destroyAllWindows();
    ws_close(); 
    
    return 0;
}