#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <vector>
#include <string>

// Kita perlu ini agar header "tahu" apa itu 'unsigned char' (uchar)
#include <opencv2/core.hpp> 

typedef websocketpp::client<websocketpp::config::asio_client> ws_client;
typedef websocketpp::connection_hdl ws_hdl;

/**
 * @brief Menginisialisasi dan menjalankan koneksi WebSocket di thread terpisah.
 */
bool ws_init(const std::string& uri);

/**
 * @brief Mengecek apakah koneksi WebSocket sudah siap.
 */
bool ws_is_ready();

/**
 * @brief Mengirim frame video ASLI (mentah).
 */
void ws_send_image(const std::vector<unsigned char>& img_buf, int width, int height);

/**
 * @brief Mengirim frame video YANG SUDAH DIPROSES (olahan).
 */
void ws_send_processed_image(const std::vector<unsigned char>& img_buf, int width, int height);

/**
 * @brief Mengirim data telemetri.
 */
void ws_send_telemetry(double speed, double angle, double distance, const std::string& laneStatus);

/**
 * @brief Menutup koneksi WebSocket dan membersihkan thread.
 */
void ws_close();

#endif // WEBSOCKET_HPP