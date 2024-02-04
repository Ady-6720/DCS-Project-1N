#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>

#define PORT 8080
#define BUFFER_SIZE 2048

using namespace std;

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error: Socket creation failed" << endl;
        return -1;
    }

    // Allow user to specify IP address and port number
    char ip[20];
    cout << "Enter server IP address: ";
    cin.getline(ip, sizeof(ip));

    int port;
    cout << "Enter server port number: ";
    cin >> port;
    cin.ignore(); // Ignore newline character left in the input buffer

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Error: Connection failed" << endl;
        return -1;
    }

    cout << "Connected to server" << endl;

    // Send commands to server
    while (true) {
        string command;
        cout << "Enter command (or 'quit' to exit): ";
        getline(cin, command);

        // Check if the command is a PUT command
        if (command.substr(0, 4) == "put ") {
            // Extract filename from the command
            string filename = command.substr(4);

            // Read file content
            ifstream file(filename, ios::binary);
            if (!file.is_open()) {
                cerr << "Error: Failed to open file for reading" << endl;
                continue;
            }

            // Send the PUT command to the server
            ssize_t bytesSent = send(clientSocket, command.c_str(), command.size(), 0);
            if (bytesSent == -1) {
                perror("send");
                cout << "Error code: " << errno << endl;
                // Handle error condition
                continue;
            }

            // Read and send file content
            stringstream fileContent;
            fileContent << file.rdbuf();
            string fileContentStr = fileContent.str();
            ssize_t contentSent = send(clientSocket, fileContentStr.c_str(), fileContentStr.size(), 0);
            if (contentSent == -1) {
                perror("send");
                cout << "Error code: " << errno << endl;
                // Handle error condition
                continue;
            }

            cout << "File sent successfully" << endl;

            file.close();
            continue;
        }

        // Convert string to C-style string for sending
        strncpy(buffer, command.c_str(), BUFFER_SIZE);

        // Send command to server
        ssize_t bytesSent = send(clientSocket, buffer, strlen(buffer), 0);
        if (bytesSent == -1) {
            perror("send");
            cout << "Error code: " << errno << endl;
            // Handle error condition
        }

        // Receive response from server
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        cout << "Server response: " << buffer << endl;

        // Check if user wants to quit
        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        memset(buffer, 0, BUFFER_SIZE); // Clear buffer for next command
    }

    close(clientSocket);

    return 0;
}

