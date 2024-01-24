#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace fs = std::filesystem;
using namespace std;

void handleClient(int clientSocket);
bool sendFile(int clientSocket, const string& filename);
bool receiveFile(int clientSocket, const string& filename);
bool deleteFile(const string& filename);
void listFiles(int clientSocket);
void changeDirectory(const string& path);
bool createDirectory(const string& dirname);
string getCurrentDirectory();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    fs::current_path(fs::path(argv[0]).parent_path());

    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Cannot create socket" << endl;
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed" << endl;
        return 1;
    }

    if (listen(serverSocket, 50) == -1) {
        cerr << "Listen failed" << endl;
        return 1;
    }
    cout << "Server started on port " << port << ". Waiting for connections..." << endl;

    while (true) {
        addrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
        if (clientSocket < 0) {
            cerr << "Accept failed" << endl;
            continue;
        }
        handleClient(clientSocket);
    }

    close(serverSocket);
    return 0;
}

void handleClient(int clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, 1024, 0)) > 0) {
        string command(buffer, bytesReceived);
        istringstream iss(command);
        string cmd;
        iss >> cmd;

        if (cmd == "get") {
            string filename;
            iss >> filename;
            sendFile(clientSocket, filename);
        } else if (cmd == "put") {
            string filename;
            iss >> filename;
            receiveFile(clientSocket, filename);
        } else if (cmd == "delete") {
            string filename;
            iss >> filename;
            deleteFile(filename);
        } else if (cmd == "ls") {
            listFiles(clientSocket);
        } else if (cmd == "cd") {
            string path;
            iss >> path;
            changeDirectory(path);
        } else if (cmd == "mkdir") {
            string dirname;
            iss >> dirname;
            createDirectory(dirname);
        } else if (cmd == "pwd") {
            string currentPath = getCurrentDirectory();
            send(clientSocket, currentPath.c_str(), currentPath.size(), 0);
        } else if (cmd == "quit") {
            break;
        } else {
            string errorMsg = "Unknown command";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
        }
    }

    if (bytesReceived < 0) {
        cerr << "Error in recv()" << endl;
    }

    close(clientSocket);
}

bool sendFile(int clientSocket, const string& filename) {
    ifstream file(filename, ios::binary);
    if (file.is_open()) {
        stringstream buffer;
        buffer << file.rdbuf();
        string fileContent = buffer.str();
        send(clientSocket, fileContent.c_str(), fileContent.size(), 0);
        return true;
    } else {
        string errorMsg = "Error: File not found";
        send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
        return false;
    }
}

bool receiveFile(int clientSocket, const string& filename) {
    char fileBuffer[1024];
    int bytesReceived = recv(clientSocket, fileBuffer, 1024, 0);
    if (bytesReceived < 0) {
        cerr << "Error in receiving file" << endl;
        return false;
    }

    ofstream file(filename, ios::binary);
    file.write(fileBuffer, bytesReceived);
    return true;
}

bool deleteFile(const string& filename) {
    if (remove(filename.c_str()) != 0) {
        string errorMsg = "Error: Unable to delete file";
        return false;
    }
    return true;
}

void listFiles(int clientSocket) {
    string directoryContents;
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        directoryContents += entry.path().filename().string() + "\n";
    }
    send(clientSocket, directoryContents.c_str(), directoryContents.length(), 0);
}

void changeDirectory(const string& path) {
    if (path == "..") {
        fs::current_path(fs::current_path().parent_path());
    } else if (fs::exists(path) && fs::is_directory(path)) {
        fs::current_path(path);
    }
}

bool createDirectory(const string& dirname) {
    if (!fs::create_directory(dirname)) {
        return false;
    }
    return true;
}

string getCurrentDirectory() {
    return fs::current_path().string();
}

