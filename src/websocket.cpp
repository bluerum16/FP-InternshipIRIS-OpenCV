#include "websocket.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
typedef websocketpp::client<websocketpp::config::asio_client> client;

int main() {
    client ws_client;
    
}

