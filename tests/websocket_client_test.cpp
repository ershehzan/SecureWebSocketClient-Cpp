#include <iostream>
#include "../src/websocket/websocket_client.h"
#include "../src/websocket/tls_context.h"
using namespace std;
int main() {
    TLSContext tls;
    WebSocketClient client("echo.websocket.events", "443", "/", tls);

    if (!client.connect()) 
    {
        cout << "[TEST FAILED] Cannot connect\n";
        return 1;
    }

    client.send_text("test");
    string reply = client.receive_text();

    if (reply.empty()) 
    {
        cout << "[TEST FAILED] No reply\n";
        return 1;
    }
    cout << "[TEST PASSED]\n";
    client.close();
    return 0;
}
