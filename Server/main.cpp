#include <iostream>
#include <string>
#include <winsock2.h>
#include <chrono>

#include "InvertedIndex/InvertedIndex.h"
#include "InvertedIndex/Database/Database.h"
#include "ThreadPool/ThreadPool.h"
#include "JSONCreation.h"

#define PORT 8080

std::atomic<bool> running(true);

void inputThread() {
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "exit") {
            std::cout << "Exiting..." << std::endl;
            running.store(false);
            break;
        }
    }
}

void updateIndex(InvertedIndex& index) {
    while (running) {
        std::cout << "Updating index..." << std::endl;
        index.buildIndex();
        std::cout << "Index updated" << std::endl;
        if (!running) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

// Обробка клієнтського запиту
void handleClient(SOCKET clientSocket, InvertedIndex& invertedIndex, Database& database) {
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);

    if (bytesReceived > 0) {
        std::string request(buffer);
        // Перевірка методу та запиту
        if (request.find("GET") == 0) {
            size_t queryPos = request.find("query=");
            if (queryPos != std::string::npos) {
                size_t endPos = request.find(" ", queryPos);
                std::string query = request.substr(queryPos + 6, endPos - queryPos - 6);

                // Звернення до інвертованого індексу
                auto resultsId = invertedIndex.search(query);

                std::vector<std::string> results;
                for (const int& docId : resultsId) {
                    Document document;
                    auto flag = database.getDocumentById(docId, document);
                    if (flag) {
                        results.push_back(document.content);
                    }
                }

                // Формування JSON відповіді
                std::string jsonResponse = createJsonResponse(results);
                std::string response = "HTTP/1.1 200 OK\r\n";
                response += "Content-Type: application/json\r\n";
                response += "Content-Length: " + std::to_string(jsonResponse.size()) + "\r\n";
                response += "\r\n";
                response += jsonResponse;

                // Надсилання відповіді клієнту
                send(clientSocket, response.c_str(), response.size(), 0);
            } else {
                std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            }
        } else {
            std::string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
    }

    closesocket(clientSocket);
}

int startServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. " << WSAGetLastError() << std::endl;
        return -1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr *)(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed. " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listening failed. " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    ThreadPool pool;
    pool.initialize(6);

    Database database;
    pool.addTask(std::bind(&Database::addDocumentsFromDirectory, &database, "../Data/test/neg"));
    pool.addTask(std::bind(&Database::addDocumentsFromDirectory, &database, "../Data/test/pos"));
    pool.addTask(std::bind(&Database::addDocumentsFromDirectory, &database, "../Data/train/neg"));
    pool.addTask(std::bind(&Database::addDocumentsFromDirectory, &database, "../Data/train/pos"));
    pool.addTask(std::bind(&Database::addDocumentsFromDirectory, &database, "../Data/train/unsup"));

    InvertedIndex invertedIndex(database);
    std::thread update(updateIndex, std::ref(invertedIndex));

    std::thread input(inputThread);

    std::cout << "Server started. Waiting for connections..." << std::endl;

    while(running.load()) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accepting connection failed. " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }
        pool.addTask(std::bind(&handleClient, clientSocket, std::ref(invertedIndex), std::ref(database)));
    }
    pool.terminate();
    if (input.joinable()) input.join();
    if (update.joinable()) update.join();

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

void invertedIndexTest() {
    Database database;
    database.addDocumentsFromDirectory("../Data/test/neg");
    database.addDocumentsFromDirectory("../Data/test/pos");
    database.addDocumentsFromDirectory("../Data/train/neg");
    database.addDocumentsFromDirectory("../Data/train/pos");
    database.addDocumentsFromDirectory("../Data/train/unsup");

    int threadsNum[] = {1, 2, 4, 6, 8, 12, 20, 40, 60, 80};

    for (auto threadNum : threadsNum) {
        InvertedIndex invertedIndex(database);

        auto building_begin = high_resolution_clock::now();
        invertedIndex.buildIndex(threadNum);
        auto building_end = high_resolution_clock::now();

        auto elapsed = duration_cast<nanoseconds>(building_end - building_begin);
        std::printf("Threads: %d, Time: %.3f seconds.\n", threadNum, elapsed.count() * 1e-9);
    }
}

int main() {
    startServer();
    //invertedIndexTest();
    return 0;
}
