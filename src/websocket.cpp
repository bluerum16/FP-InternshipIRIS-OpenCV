#include "websocket.hpp" 
#include <nlohmann/json.hpp>  
#include <iostream>
#include <atomic>           
#include <thread>           
#include <vector>

using json = nlohmann::json;
typedef websocketpp::frame::opcode::value opcode;

// --- Variabel Global (Internal) ---
namespace {
    ws_client g_client;
    ws_hdl g_hdl;
    std::atomic<bool> g_connection_ready(false);
    std::thread g_ws_thread; 

    static const std::string base64_chars =
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";

    std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
        std::string ret;
        int i = 0, j = 0;
        unsigned char char_array_3[3], char_array_4[4];
        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }
        if (i) {
            for (j = i; j < 3; j++) char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (j = 0; j < i + 1; j++) ret += base64_chars[char_array_4[j]];
            while (i++ < 3) ret += '=';
        }
        return ret;
    }

    void on_open(ws_hdl hdl) {
        std::cout << "Koneksi WebSocket Terbuka!" << std::endl;
        g_hdl = hdl;
        g_connection_ready = true;
    }
    void on_fail(ws_hdl hdl) {
        std::cerr << "Koneksi WebSocket Gagal!" << std::endl;
        g_connection_ready = false;
    }
    void on_close(ws_hdl hdl) {
        std::cout << "Koneksi WebSocket Ditutup." << std::endl;
        g_connection_ready = false;
    }
} 


bool ws_init(const std::string& uri) {
    try {
        g_client.clear_access_channels(websocketpp::log::alevel::all);
        g_client.init_asio();
        g_client.set_open_handler(&on_open);
        g_client.set_fail_handler(&on_fail);
        g_client.set_close_handler(&on_close);

        websocketpp::lib::error_code ec;
        ws_client::connection_ptr con = g_client.get_connection(uri, ec);
        if (ec) {
            std::cerr << "Gagal membuat koneksi: " << ec.message() << std::endl;
            return false;
        }
        
        g_client.connect(con);

        g_ws_thread = std::thread([]() {
            try {
                g_client.run();
            } catch (const std::exception& e) {
                std::cerr << "Pengecualian di thread ws_run: " << e.what() << std::endl;
            }
        });
        
        return true; 
    } catch (const std::exception& e) {
        std::cerr << "Pengecualian ws_init: " << e.what() << std::endl;
        return false;
    }
}

bool ws_is_ready() {
    return g_connection_ready;
}

void ws_send_telemetry(double speed, double angle, double distance, const std::string& laneStatus) {
    if (!ws_is_ready()) return;
    json msg_telemetry = {
        {"type", "telemetry"},
        {"data", {
            {"speed", speed},
            {"steering_angle", angle}, 
            {"jarakTempuh", distance},
            {"laneStatus", laneStatus}
        }}
    };
    try {
        websocketpp::lib::error_code ec_send;
        g_client.send(g_hdl, msg_telemetry.dump(), opcode::text, ec_send);
        if(ec_send) std::cerr << "Error kirim telemetry: " << ec_send.message() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Pengecualian ws_send_telemetry: " << e.what() << std::endl;
    }
}

void ws_send_image(const std::vector<unsigned char>& img_buffer, int width, int height) {
    if (!ws_is_ready()) return;
    std::string img_base64 = base64_encode(img_buffer.data(), img_buffer.size());
    json msg_raw = {
        {"type", "image_raw"}, 
        {"data", img_base64},
        {"width", width},
        {"height", height}
    };
    try {
        websocketpp::lib::error_code ec_send;
        g_client.send(g_hdl, msg_raw.dump(), opcode::text, ec_send);
        if(ec_send) std::cerr << "Error kirim raw image: " << ec_send.message() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Pengecualian ws_send_image: " << e.what() << std::endl;
    }
}

void ws_send_processed_image(const std::vector<unsigned char>& img_buffer, int width, int height) {
    if (!ws_is_ready()) return;
    std::string img_base64 = base64_encode(img_buffer.data(), img_buffer.size());
    json msg_processed = {
        {"type", "image_processed"},
        {"data", img_base64},
        {"width", width},
        {"height", height}
    };
    try {
        websocketpp::lib::error_code ec_send;
        g_client.send(g_hdl, msg_processed.dump(), opcode::text, ec_send);
    } catch (const std::exception& e) {
        std::cerr << "Pengecualian ws_send_processed_image: " << e.what() << std::endl;
    }
}

void ws_close() {
    bool was_ready = ws_is_ready();
    if (was_ready) {
        try {
            websocketpp::lib::error_code ec;
            g_client.close(g_hdl, websocketpp::close::status::normal, "Client shutting down", ec);
        } catch (const std::exception& e) {
            std::cerr << "Pengecualian saat ws_close: " << e.what() << std::endl;
        }
    }
    if (!g_client.stopped()) {
        g_client.stop();
    }
    if (g_ws_thread.joinable()) {
        g_ws_thread.join();
        std::cout << "Thread WebSocket di-join." << std::endl;
    }
}