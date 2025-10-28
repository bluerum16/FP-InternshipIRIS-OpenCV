#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <string>
#include <vector> // Diperlukan untuk std::vector

// Hanya perlu header core OpenCV, bukan seluruh <opencv2/opencv.hpp>
#include <opencv2/core.hpp> 

// Includes WebSocket Anda (sudah benar)
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

// --- DEKLARASI FUNGSI ---

/**
 * @brief Memulai koneksi WebSocket ke URI yang diberikan.
 */
void ws_connect(const std::string& uri);

/**
 * @brief Menjalankan loop pemrosesan WebSocket. (Dijalankan di thread terpisah)
 */
void ws_run();

/**
 * @brief Menghentikan koneksi WebSocket dan membersihkan.
 */
void ws_stop();

/**
 * @brief Memeriksa apakah koneksi WebSocket siap untuk mengirim data.
 * @return true jika terhubung, false jika tidak.
 */
bool ws_is_ready();

/**
 * @brief Mengirim data telemetri (data penting Anda) ke server.
 */
void ws_send_telemetry(double speed, double angle, double distance, const std::string& status);

/**
 * @brief Mengirim buffer gambar (frame) yang sudah di-encode ke server.
 * @param img_buffer Vektor uchar dari cv::imencode
 * @param width Lebar gambar
 * @param height Tinggi gambar
 */
// PERBAIKAN: Menghapus 'cv::' dari 'uchar'
void ws_send_image(const std::vector<uchar>& img_buffer, int width, int height);


#endif // WEBSOCKET_HPP