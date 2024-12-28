#include <iostream>
#include <string>
#include <winsock2.h>

#include "InvertedIndex/HashTable.h"
#include "InvertedIndex/InvertedIndex.h"
#include "InvertedIndex/Database/Database.h"
#include "ThreadPool/ThreadPool.h"

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

// Формування JSON відповіді
std::string createJsonResponse(const std::vector<std::string>& results) {
    std::ostringstream json;
    json << "{ \"results\": [";
    for (size_t i = 0; i < results.size(); ++i) {
        json << "\"" << results[i] << "\"";
        if (i != results.size() - 1) {
            json << ", ";
        }
    }
    json << "] }";
    return json.str();
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

    // Закриття сокету
    closesocket(clientSocket);
}

int startServer() {
    Database database;
    database.addDocumentsFromDirectory("../Data/test/neg");
    InvertedIndex invertedIndex(database);
    invertedIndex.buildIndex();

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

    std::cout << "Server started. Waiting for connections..." << std::endl;

    ThreadPool pool;
    pool.initialize(6);

    std::thread input(inputThread);

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

    closesocket(serverSocket);
    pool.terminate();
    input.join();

    WSACleanup();
}

void hashTableTest() {
    HashTable<std::string, int> hashTable;

    hashTable.insert("Alice", 25);
    hashTable.insert("Bob", 30);
    hashTable.insert("Carol", 35);

    int value;
    if (hashTable.find("Alice", value)) {
        std::cout << "Alice: " << value << std::endl;
    } else {
        std::cout << "Alice not found" << std::endl;
    }

    hashTable.remove("Bob");

    if (hashTable.find("Bob", value)) {
        std::cout << "Bob: " << value << std::endl;
    } else {
        std::cout << "Bob not found" << std::endl;
    }
}

void InvertedIndexTest() {
    Database database;
    database.addDocumentsFromDirectory("../Data/test/neg");
    InvertedIndex invertedIndex(database);
    invertedIndex.buildIndex();
    auto result = invertedIndex.search("character");
    for (auto res : result) {
        Document document;
        if (database.getDocumentById(res, document)) {
            std::cout << document.name << std::endl << document.content << std::endl;
        } else {
            std::cout << "Id not found" << std::endl;
        }
    }
}


void testThreadPool() {
    ThreadPool pool;
    pool.initialize(4);

    auto task = [](int id, int delaySeconds) {
        std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        std::cout << "Task " << id << " completed after " << delaySeconds << " seconds\n";
    };

    for (int i = 0; i < 10; ++i) {
        pool.addTask(task, i, i % 5 + 1); // ID задачі та затримка від 1 до 5 секунд
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    pool.terminate();
}

int main() {
    startServer();
    return 0;
}
