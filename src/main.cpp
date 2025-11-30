#include <iostream>
#include <string>
#include "websocket/websocket_client.h"
#include "websocket/tls_context.h"

using namespace std;

int main() {
    cout << "==========================================\n";
    cout << "   NATIVE C++ WEBSOCKET CLIENT (SECURE)   \n";
    cout << "==========================================\n";
    
    TLSContext tls;
    // Using a reliable echo server
   WebSocketClient client("ws.postman-echo.com", "443", "/raw", tls);

    // 1. Setup Callbacks
    client.on_message = [](const string& msg) {
        // \r clears the current line so the prompt > stays clean
        cout << "\r[SERVER] " << msg << "\n> " << flush;
    };

    client.on_error = [](const string& err) {
        cerr << "\n[ERROR] " << err << endl;
    };

    // 2. Connect
    cout << "Connecting to wss://echo.websocket.events/ ...\n";
    if (!client.connect()) {
        cerr << "Connection Failed.\n";
        return 1;
    }

    cout << "Connected! Start typing (type /exit to quit).\n\n";

    // 3. Start Background Listener (Full Duplex)
    client.start_listening();

    // 4. Main Thread Handles User Input
    cout << "> ";
    string input;
    while (true) {
        if (!getline(cin, input)) break;
        if (input == "/exit") break;

        client.send_text(input);
        
        // Re-print prompt after sending
        cout << "> " << flush;
    }

    cout << "Closing connection...\n";
    client.close();
    return 0;
}