#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
}

SOCKET createSocket(const std::string& serverAddress, int port) {
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    sockaddr_in serverInfo{};
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(port);
    serverInfo.sin_addr.s_addr = inet_addr(serverAddress.c_str());

    if (connect(clientSocket, (sockaddr*)&serverInfo, sizeof(serverInfo)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    return clientSocket;
}

void sendRequest(SOCKET clientSocket, const std::string& request) {
    int bytesSent = send(clientSocket, request.c_str(), request.size(), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }
}

std::string receiveResponse(SOCKET clientSocket) {
    const int bufferSize = 1024;
    char buffer[bufferSize];
    std::string response;

    int bytesReceived;
    do {
        bytesReceived = recv(clientSocket, buffer, bufferSize - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            response += buffer;
        } else if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }
    } while (bytesReceived > 0);

    return response;
}

void cleanup(SOCKET clientSocket) {
    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    const std::string serverAddress = "127.0.0.1"; // Замініть на адресу вашого сервера
    const int port = 8080;                         // Замініть на порт вашого сервера

    initializeWinsock();
    SOCKET clientSocket = createSocket(serverAddress, port);

    std::cout << "Enter text to search: ";
    std::string userInput;
    std::getline(std::cin, userInput);

    std::string httpRequest = "GET /search?query=" + userInput + " HTTP/1.1\r\n"
                              "Host: " + serverAddress + "\r\n"
                              "Connection: close\r\n\r\n";

    sendRequest(clientSocket, httpRequest);
    std::string response = receiveResponse(clientSocket);

    std::cout << "Server response:\n" << response << std::endl;

    cleanup(clientSocket);
    return 0;
}
