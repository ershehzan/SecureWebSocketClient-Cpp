# 🚀 **SecureWebSocketClient-Cpp**

### **A Native C++17 Secure WebSocket Client (Winsock + OpenSSL)**

A lightweight, high-performance WebSocket client built **from scratch**, implementing the **full WebSocket RFC 6455 handshake, framing logic, masking, and TLS encryption** — without using any external WebSocket libraries.

This client connects to **secure WSS servers**, performs the HTTP Upgrade handshake, encrypts packets using OpenSSL, and handles full-duplex messaging.

---

## ⭐ **Why This Project Stands Out**

### 🔹 Fully Native WebSocket Stack

Implements the protocol manually:

* HTTP Upgrade → `101 Switching Protocols`
* Base64 nonce generation
* Frame masking/unmasking
* Bit-level frame construction
* Opcode handling

### 🔹 Secure by Design

Uses **OpenSSL** for TLS handshaking and encrypted communication.

### 🔹 Windows-Native

Built using:

* **Winsock2**
* **CMake**
* **MSVC (Visual Studio)**
* **vcpkg** for dependency management

### 🔹 Clean, Modular Codebase

Separated into:

* `websocket_client.*`
* `tls_context.*`
* `cli.*`
* `tests/`

---

## 🧩 **Features**

| Feature                      | Description                                 |
| ---------------------------- | ------------------------------------------- |
| ✔️ Secure WSS support        | Encrypted WebSocket over TLS                |
| ✔️ Manual frame construction | FIN, Opcode, Payload Length, Masking        |
| ✔️ True full-duplex          | Separate listening + sending threads        |
| ✔️ Native sockets            | No Boost or external WebSocket libs         |
| ✔️ Unit tests                | Simple connection and echo tests            |
| ✔️ Clean CLI                 | `/exit`, message input, and error reporting |

---

## 🛠️ **Tech Stack**

* **C++17**
* **Windows API (Winsock2)**
* **OpenSSL (via vcpkg)**
* **CMake Build System**
* **MSVC Toolchain**

---

## 📦 **Installation & Build Guide**

### 1️⃣ Install Dependencies

```powershell
vcpkg install openssl:x64-windows
```

---

### 2️⃣ Configure the Project

```powershell
mkdir build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/Users/ershe/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
```

---

### 3️⃣ Build

```powershell
cmake --build build --config Release
```

---

### 4️⃣ Run

```powershell
./build/Release/websocket_client.exe
```

---

## 🧠 **Core Concepts Learned**

### 🔸 WebSocket Protocol (RFC 6455)

* HTTP Upgrade handshake
* `Sec-WebSocket-Key` + Base64
* Accept hash using SHA-1
* Opcode handling

### 🔸 Secure Networking

* TLS handshake
* Certificate validation
* OpenSSL BIO socket integration

### 🔸 Windows Systems Programming

* Winsock socket creation
* Address resolution
* TCP stream communication

### 🔸 Low-Level Bit Manipulation

* ASN.1 compliant masking
* Payload segmentation
* Frame packing/unpacking

---

## 🧪 **Testing**

Inside `tests/`:

* Automated connect / send / receive test
* Asserts handshake success
* Tests echo server consistency

Run manually:

```powershell
./build/Release/websocket_tests.exe
```

---

## 🎥 **Demo Video (Loom)**

📌 *Include your Loom demo link here.*

---

## 📁 **Project Structure**

```
SecureWebSocketClient-Cpp
│   CMakeLists.txt
│   LICENSE
│
├── src
│   ├── main.cpp
│   ├── cli/
│   │     cli.h
│   │     cli.cpp
│   └── websocket/
│         websocket_client.h
│         websocket_client.cpp
│         tls_context.h
│         tls_context.cpp
│
└── tests/
      websocket_client_test.cpp
```

---

## 📜 **License**

This project is licensed under the **MIT License**.

---

## ✨ Created By

**Shehzan Khan**


