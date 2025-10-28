// #include "websocket.hpp" // Header yang Anda definisikan

// // --- Library Pengganti JSON ---
// #include <sstream>      // Untuk std::stringstream (pengganti json)
// #include <iomanip>      // Untuk std::fixed, std::setprecision

// // --- Includes Lainnya ---
// #include <iostream>     // Untuk std::cout, std::cerr
// #include <atomic>       // Untuk status koneksi thread-safe
// #include <vector>

// // --- Definisi Tipe ---
// // Hapus 'using json = nlohmann::json;'
// typedef websocketpp::client<websocketpp::config::asio_client> client;
// typedef websocketpp::frame::opcode::value opcode;

// // --- Variabel Global (Hanya untuk file .cpp ini) ---
// namespace {
//     client g_client;
//     websocketpp::connection_hdl g_hdl;
//     std::atomic<bool> g_connection_ready(false);

//     // --- Fungsi Helper Base64 (Tetap sama) ---
//     static const std::string base64_chars =
//                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//                  "abcdefghijklmnopqrstuvwxyz"
//                  "0123456789+/";

//     std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
//         std::string ret;
//         int i = 0, j = 0;
//         unsigned char char_array_3[3], char_array_4[4];

//         while (in_len--) {
//             char_array_3[i++] = *(bytes_to_encode++);
//             if (i == 3) {
//                 char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
//                 char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
//                 char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
//                 char_array_4[3] = char_array_3[2] & 0x3f;
//                 for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
//                 i = 0;
//             }
//         }
//         if (i) {
//             for (j = i; j < 3; j++) char_array_3[j] = '\0';
//             char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
//             char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
//             char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
//             char_array_4[3] = char_array_3[2] & 0x3f;
//             for (j = 0; j < i + 1; j++) ret += base64_chars[char_array_4[j]];
//             while (i++ < 3) ret += '=';
//         }
//         return ret;
//     }

//     // --- WebSocket Handlers (Tetap sama) ---
//     void on_open(websocketpp::connection_hdl hdl) {
//         std::cout << "Koneksi WebSocket Terbuka!" << std::endl;
//         g_hdl = hdl;
//         g_connection_ready = true;
//     }

//     void on_fail(websocketpp::connection_hdl hdl) {
//         std::cerr << "Koneksi WebSocket Gagal!" << std::endl;
//         g_connection_ready = false;
//     }

//     void on_close(websocketpp::connection_hdl hdl) {
//         std::cout << "Koneksi WebSocket Ditutup." << std::endl;
//         g_connection_ready = false;
//     }

// } // akhir dari namespace anonim

// // --- Implementasi Fungsi Publik ---

// // ws_connect, ws_run, ws_stop, ws_is_ready tetap SAMA
// // (Salin dari jawaban saya sebelumnya jika Anda belum punya)

// void ws_connect(const std::string& uri) {
//     try {
//         g_client.init_asio();
//         g_client.set_open_handler(&on_open);
//         g_client.set_fail_handler(&on_fail);
//         g_client.set_close_handler(&on_close);
//         websocketpp::lib::error_code ec;
//         client::connection_ptr con = g_client.get_connection(uri, ec);
//         if (ec) {
//             std::cerr << "Gagal membuat koneksi: " << ec.message() << std::endl;
//             return;
//         }
//         g_client.connect(con);
//     } catch (const std::exception& e) {
//         std::cerr << "Pengecualian ws_connect: " << e.what() << std::endl;
//     }
// }

// void ws_run() {
//     try {
//         g_client.run();
//     } catch (const std::exception& e) {
//         std::cerr << "Pengecualian ws_run: " << e.what() << std::endl;
//     }
// }

// void ws_stop() {
//     if (!g_connection_ready) return;
//     try {
//         websocketpp::lib::error_code ec;
//         g_client.close(g_hdl, websocketpp::close::status::normal, "Client shutting down", ec);
//         g_client.stop(); 
//     } catch (const std::exception& e) {
//         std::cerr << "Pengecualian ws_stop: " << e.what() << std::endl;
//     }
// }

// bool ws_is_ready() {
//     return g_connection_ready;
// }


// // --- FUNGSI YANG DIUBAH (Tanpa nlohmann/json) ---

// void ws_send_telemetry(double speed, double angle, double distance, const std::string& status) {
//     if (!ws_is_ready()) return; // Jangan kirim apapun jika tidak terhubung

//     // Membuat JSON string secara manual
//     std::stringstream ss;
//     ss << "{\"type\":\"telemetry\",\"data\":{"
//        << "\"speed\":" << std::fixed << std::setprecision(2) << speed << ","
//        << "\"angle\":" << std::fixed << std::setprecision(3) << angle << ","
//        << "\"jarakTempuh\":" << std::fixed << std::setprecision(2) << distance << ","
//        << "\"laneStatus\":\"" << status << "\"" // Jangan lupa tanda kutip untuk string
//        << "}}";

//     try {
//         websocketpp::lib::error_code ec_send;
//         // Mengirim JSON sebagai string
//         g_client.send(g_hdl, ss.str(), opcode::text, ec_send);
//         if(ec_send) std::cerr << "Error kirim telemetry: " << ec_send.message() << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Pengecualian ws_send_telemetry: " << e.what() << std::endl;
//     }
// }

// void ws_send_image(const std::vector<uchar>& img_buffer, int width, int height) {
//     if (!ws_is_ready()) return;

//     // Lakukan Base64 encoding (tetap sama)
//     std::string img_base64 = base64_encode(img_buffer.data(), img_buffer.size());

//     // Membuat JSON string secara manual
//     std::stringstream ss;
//     ss << "{\"type\":\"image_raw\",\"data\":\"" 
//        << img_base64 // Data base64 sudah aman (tidak mengandung " atau \)
//        << "\",\"width\":" << width << ","
//        << "\"height\":" << height
//        << "}";

//     try {
//         websocketpp::lib::error_code ec_send;
//         // Mengirim JSON sebagai string
//         g_client.send(g_hdl, ss.str(), opcode::text, ec_send);
//         if(ec_send) std::cerr << "Error kirim raw image: " << ec_send.message() << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Pengecualian ws_send_image: " << e.what() << std::endl;
//     }
// }