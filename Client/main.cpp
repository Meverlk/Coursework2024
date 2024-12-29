#include <iostream>
#include <vector>
#include <sstream>
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


// Метод для розпакування JSON
std::vector<std::string> parseJsonToVector(const std::string& json) {
    std::vector<std::string> results;
    size_t start = json.find("\"results\": [");
    if (start == std::string::npos) {
        std::cerr << "Invalid JSON format: no 'results' field found." << std::endl;
        return results;
    }

    start += 11; // Зсув на початок масиву
    size_t end = json.find("]", start);
    if (end == std::string::npos) {
        std::cerr << "Invalid JSON format: no closing ']' found for 'results'." << std::endl;
        return results;
    }

    std::string arrayContent = json.substr(start, end - start);
    bool inString = false;
    bool quoted = false;
    std::string currentString;

    for (char ch : arrayContent) {
        if (ch == '\"' && !quoted) {
            inString = !inString;

            if (!inString) { // Кінець рядка
                results.push_back(currentString);
                currentString.clear();
            }
        } else if (inString) {
            quoted = false;
            if (ch == '\\') { // Екранування
                quoted = true;
                continue;
            }
            currentString += ch;
        }
    }

    return results;
}

void printVector(const std::vector<std::string>& vec) {
    for (const auto& str : vec) {
        std::cout << str << std::endl << std::endl;
    }
}

int main() {
    const std::string serverAddress = "127.0.0.1";
    const int port = 8080;

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

    auto parsedResponse = parseJsonToVector(response);
    printVector(parsedResponse);

    cleanup(clientSocket);
    return 0;
}
