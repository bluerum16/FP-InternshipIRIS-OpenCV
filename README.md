# 🚀 Lane Detection with WebSocket and UDP Communication

Project ini bertujuan untuk mendeteksi **jalur jalan (lane detection)** secara real-time menggunakan **OpenCV (C++)**, serta mengirimkan data hasil deteksi dan kemiringan jalan ke **Base Station (BS)** melalui **WebSocket** dan **UDP**.

---

## 📋 Deskripsi Project

### 🎯 Tujuan
- Mengakses kamera melalui jaringan WiFi (IP Camera).
- Mendeteksi **garis jalan** menggunakan **OpenCV C++**.
- Menampilkan **frame raw dan hasil pemrosesan** ke Base Station melalui **WebSocket**.
- Mengirimkan **data kemiringan jalan** secara **UDP** ke Base Station.
- (Opsional) Mengirimkan data tambahan seperti posisi obstacle, jarak, dll.

---

## 🧩 Teknologi yang Digunakan

| Komponen | Bahasa | Library / Tool |
|-----------|---------|----------------|
| Pemrosesan Citra | C++ | OpenCV |
| Komunikasi WebSocket | C++ | WebSocket++ |
| Komunikasi UDP | C++ | Socket API (Boost Asio / BSD) |
| Server / Base Station | Python | `websockets` |
| Build System | - | CMake |
| Environment | Linux / macOS | Terminal / bash |

---

## ⚙️ Instalasi dan Setup

### 🧱 1️⃣ Persiapan Umum

Pastikan sudah menginstal:
```bash
git
cmake
g++
python3
pip 
```
## Install Websocket++ (C++)

- Linux / Ubuntu
  sudo apt install libboost-all-dev cmake g++ git
- MacOS
  brew install boost cmake git

## Install Websocket Python
- Linux / Ubuntu
  python3 -m venv venv
  source venv/bin/activate
  pip install websockets
