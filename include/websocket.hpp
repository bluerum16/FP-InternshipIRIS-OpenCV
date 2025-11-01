#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <vector>
#include <string>
#include <opencv2/core.hpp> 

typedef websocketpp::client<websocketpp::config::asio_client> ws_client;
typedef websocketpp::connection_hdl ws_hdl;

bool ws_init(const std::string& uri);


bool ws_is_ready();

void ws_send_image(const std::vector<unsigned char>& img_buf, int width, int height);

void ws_send_processed_image(const std::vector<unsigned char>& img_buf, int width, int height);

void ws_send_telemetry(double speed, double angle, double distance, const std::string& laneStatus);

void ws_close();

#endif 