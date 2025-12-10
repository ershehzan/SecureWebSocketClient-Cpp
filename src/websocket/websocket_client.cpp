 #define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "websocket_client.h"
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>
#pragma comment(lib, "Ws2_32.lib")

// --- Helper Functions ---
static string base64_encode_bytes(const unsigned char* input, int len) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_push(b64, mem);
    BIO_write(b64, input, len);
    BIO_flush(b64);
    BUF_MEM* bptr = nullptr;
    BIO_get_mem_ptr(b64, &bptr);
    string s(bptr->data, bptr->length);
    BIO_free_all(b64);
    return s;
}

static string make_ws_key() {
    unsigned char bytes[16];
    random_device rd;
    for (int i = 0; i < 16; ++i) bytes[i] = (unsigned char)(rd() & 0xFF);
    return base64_encode_bytes(bytes, 16);
}

// --- Class Implementation ---

WebSocketClient::WebSocketClient(const string& host, const string& port, const string& path, TLSContext& tls)
    : host(host), port(port), path(path), tls(tls), sock(INVALID_SOCKET), ssl(nullptr), is_running(false) {}

WebSocketClient::~WebSocketClient() {
    close();
}

bool WebSocketClient::connect() {
    cout << "[DEBUG] Initializing Winsock..." << endl;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "[ERROR] WSAStartup failed" << endl;
        return false;
    }

    addrinfo hints{}, * res = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    cout << "[DEBUG] Resolving Hostname: " << host << "..." << endl;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) {
        cerr << "[ERROR] DNS Lookup failed. Check internet or hostname." << endl;
        return false;
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) { 
        cerr << "[ERROR] Socket creation failed" << endl;
        freeaddrinfo(res); return false; 
    }

    cout << "[DEBUG] Connecting TCP Socket..." << endl;
    if (::connect(sock, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        cerr << "[ERROR] TCP Connect failed. Firewall or Server Down?" << endl;
        closesocket(sock); freeaddrinfo(res); return false;
    }
    freeaddrinfo(res);

    cout << "[DEBUG] Starting SSL Handshake..." << endl;
    ssl = SSL_new(tls.get());
    if (!ssl) { cerr << "[ERROR] SSL_new failed" << endl; return false; }
    
    SSL_set_tlsext_host_name(ssl, host.c_str());
    BIO* sbio = BIO_new_socket((int)sock, BIO_NOCLOSE);
    SSL_set_bio(ssl, sbio, sbio);

    if (SSL_connect(ssl) != 1) {
        if (on_error) on_error("SSL Handshake failed (Check OpenSSL/Certificates)");
        ERR_print_errors_fp(stderr); // Print detailed OpenSSL errors
        return false;
    }

    cout << "[DEBUG] Sending WebSocket Upgrade Request..." << endl;
    string key = make_ws_key();
    ostringstream req;
    req << "GET " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Upgrade: websocket\r\n"
        << "Connection: Upgrade\r\n"
        << "Sec-WebSocket-Key: " << key << "\r\n"
        << "Sec-WebSocket-Version: 13\r\n\r\n";

    SSL_write(ssl, req.str().c_str(), (int)req.str().size());

    char buf[2048];
    int r = SSL_read(ssl, buf, sizeof(buf));
    if (r <= 0) return false;

    string response(buf, r);
    cout << "[DEBUG] Server Response Code: " << response.substr(0, response.find("\r\n")) << endl;

    if (response.find("101") == string::npos) {
        if (on_error) on_error("Server rejected WebSocket upgrade");
        return false;
    }

    return true;
}

void WebSocketClient::start_listening() {
    is_running = true;
    // Launch the background thread
    listener_thread = thread(&WebSocketClient::listen_loop, this);
}

void WebSocketClient::listen_loop() {
    while (is_running) {
        string msg = receive_text_frame();
        if (!is_running) break; // Exit if closed during read

        if (!msg.empty()) {
            if (on_message) on_message(msg);
        } else {
            // If empty, connection might be closed or error, small sleep to avoid cpu spin
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
}

void WebSocketClient::send_text(const string& msg) {
    if (!ssl) return;

    vector<unsigned char> frame;
    frame.push_back(0x81); // FIN + Text

    size_t len = msg.size();
    if (len <= 125) frame.push_back(0x80 | (unsigned char)len);
    else if (len <= 65535) {
        frame.push_back(0x80 | 126);
        frame.push_back((len >> 8) & 0xFF);
        frame.push_back(len & 0xFF);
    } else {
        frame.push_back(0x80 | 127);
        for (int i = 7; i >= 0; --i) frame.push_back((len >> (8 * i)) & 0xFF);
    }

    unsigned char mask[4];
    random_device rd;
    for (int i = 0; i < 4; ++i) mask[i] = (unsigned char)(rd() & 0xFF);
    frame.insert(frame.end(), mask, mask + 4);

    for (size_t i = 0; i < len; ++i) {
        frame.push_back(((unsigned char)msg[i]) ^ mask[i % 4]);
    }

    SSL_write(ssl, frame.data(), (int)frame.size());
}

string WebSocketClient::receive_text_frame() {
    if (!ssl) return "";

    // Read header (2 bytes)
    char head[2];
    int r = SSL_read(ssl, head, 2);
    if (r <= 0) {
        is_running = false; // Server closed connection
        if (on_error) on_error("Server disconnected");
        return "";
    }
 
    unsigned char b2 = head[1];
    uint64_t len = b2 & 0x7F;

    if (len == 126) {
        char ext[2];
        if (SSL_read(ssl, ext, 2) <= 0) return "";
        len = ((unsigned char)ext[0] << 8) | (unsigned char)ext[1];
    }
    else if (len == 127) {
        char ext[8];
        if (SSL_read(ssl, ext, 8) <= 0) return "";
        len = 0;
        for (int i = 0; i < 8; ++i) len = (len << 8) | (unsigned char)ext[i];
    }

    // Read Payload
    string msg;
    msg.resize((size_t)len);
    
    // SSL_read might not return all bytes at once, we need a loop
    size_t total_read = 0;
    while (total_read < len) {
        int r = SSL_read(ssl, &msg[total_read], (int)(len - total_read));
        if (r <= 0) return "";
        total_read += r;
    }

    return msg;
}

void WebSocketClient::close() {
    is_running = false;
    
    // Close SSL and Socket to unblock any pending reads
    if (ssl) SSL_shutdown(ssl);
    if (sock != INVALID_SOCKET) closesocket(sock);

    if (listener_thread.joinable()) {
        listener_thread.join();
    }

    if (ssl) { SSL_free(ssl); ssl = nullptr; }
    sock = INVALID_SOCKET;
}
