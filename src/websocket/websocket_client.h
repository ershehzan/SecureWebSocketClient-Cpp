#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include "tls_context.h"
#include <winsock2.h>
#include <openssl/ssl.h>
using namespace std;

class WebSocketClient {
public:
    WebSocketClient(const string& host,const string& port,const string& path,TLSContext& tls);
    ~WebSocketClient();

    bool connect();
    void start_listening(); // Starts the background thread
    void send_text(const string& msg);
    void close();

    // Callbacks
    function<void(const string&)> on_error;
    function<void(const string&)> on_message;

private:
    string receive_text_frame(); // Internal helper
    void listen_loop();          // The function running on the thread
    string host;
    string port;
    string path;

    TLSContext& tls;
    SOCKET sock;
    SSL* ssl;

    // Threading members
    atomic<bool> is_running;
    thread listener_thread;
};
