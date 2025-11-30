#pragma once
#include <string>
#include <iostream>
#include "../websocket/websocket_client.h"
using namespace std;

class CLI {
public:
    CLI(WebSocketClient& client);
    void run();

private:
    WebSocketClient& client;
};
