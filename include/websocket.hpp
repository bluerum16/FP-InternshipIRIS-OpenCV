#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <vector>
#include <string>

typedef websocketpp::client<websocketpp::config::asio> ws_client;
typedef websocketpp::connection_hdl ws_hdl;

bool ws_init(const std::string& uri);

// Mengecek apakah koneksi WebSocket sudah siap
bool ws_is_ready();

// Mengirim frame video sebagai buffer (vector<unsigned char>)
void ws_send_image(const std::vector<unsigned char>& img_buf, int width, int height);

// Mengirim telemetry: kecepatan, sudut, jarak, status lane
void ws_send_telemetry(double speed, double angle, double distance, const std::string& laneStatus);

// Menutup koneksi WebSocket
void ws_close();

#endif

