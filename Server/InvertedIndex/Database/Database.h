//
// Created by Meverlk on 12.12.2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <fstream>
#include <iostream>
#include <vector>
#include <mutex>
#include <sstream>
#include <filesystem>

#include "Document.h"

class Database {
private:
    std::vector<Document> documents;
    mutable std::mutex mutex;
    int nextId = 0;

public:
    int addDocument(const std::string& filename, const std::string& content) {
        std::lock_guard lock(mutex);
        documents.push_back({nextId, filename, content});
        return nextId++;
    }

    void addDocumentsFromDirectory(const std::string& directory) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    std::cerr << "Error opening file " << entry.path() << std::endl;
                    continue;
                }
                std::ostringstream contentStream;
                contentStream << file.rdbuf();
                addDocument(entry.path().filename().string(), contentStream.str());
                file.close();
            }
        }
    }

    bool getDocumentById(int id, Document& doc) const {
        std::lock_guard lock(mutex);
        for (const auto& document : documents) {
            if (document.id == id) {
                doc = document;
                return true;
            }
        }
        return false;
    }

    std::vector<Document> getAllDocuments() const {
        std::lock_guard lock(mutex);
        return documents;
    }
};



#endif //DATABASE_H
