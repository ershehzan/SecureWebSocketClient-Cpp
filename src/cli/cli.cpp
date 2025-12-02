#include "cli.h"

CLI::CLI(WebSocketClient& client) : client(client) {}

void CLI::run() 
{
    cout << "Type your messages. Use /exit to quit.\n";

    string input;
    
    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "/exit") break;

        client.send_text(input);
        string reply = client.receive_text();

        cout << "[server] " << reply << endl;
    }
}
