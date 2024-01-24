#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_address> <port>" << endl;
        return 1;
    }

    string serverAddress = argv[1];
    int port = atoi(argv[2]);
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Could not create socket" << endl;
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(serverAddress.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        cerr << "Connect failed. Error" << endl;
        return 1;
    }

    cout << "Connected to server. Type commands or 'quit' to exit." << endl;
    string input;

    while (true) {
        cout << "mytftp> ";
        getline(cin, input);

        if (input == "quit") {
            break;
        }

        if (send(sock, input.c_str(), input.size(), 0) < 0) {
            cerr << "Send failed" << endl;
            break;
        }

        char server_reply[2000];
        if (recv(sock, server_reply, 2000, 0) < 0) {
            cerr << "recv failed" << endl;
            break;
        }
        cout << "Server reply: " << server_reply << endl;
    }

    close(sock);
    return 0;
}

