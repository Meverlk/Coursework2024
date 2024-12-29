//
// Created by Meverlk on 12.12.2024.
//

#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <unordered_set>
#include <thread>

#include "ThreadSafeHashTable.h"
#include "Database/Database.h"


class InvertedIndex {
private:
    ThreadSafeHashTable<std::string, std::unordered_set<int>> index;
    Database& documentManager;
    size_t lastDatabaseSize;

    void indexDocument(const Document& document) {
        std::istringstream stream(document.content);
        std::string word;
        while (stream >> word) {
            index.insertOrAppend(word, document.id);
        }
    }

    void indexer(std::vector<Document>& documents, size_t startPos, size_t finishPos) {
        for (size_t i = startPos; i < finishPos; i++) {
            indexDocument(documents[i]);
        }
    }

public:
    InvertedIndex(Database& docManager) : documentManager(docManager), lastDatabaseSize(0) {}

    void buildIndex(size_t threadsNum = 6) {
        size_t size = documentManager.getSize();
        if (size > lastDatabaseSize) {
            auto documents = documentManager.getAllDocuments();
            std::thread threads[threadsNum];
            for (size_t i = 0; i < threadsNum; i++) {
                size_t startPos = lastDatabaseSize + i * ((size - lastDatabaseSize) / threadsNum);
                size_t finishPos = lastDatabaseSize + (i + 1) * ((size - lastDatabaseSize) / threadsNum);
                threads[i] = std::thread(&InvertedIndex::indexer, this, std::ref(documents), startPos, finishPos);
            }
            for (size_t i = 0; i < threadsNum; i++) {
                threads[i].join();
            }
            lastDatabaseSize = size;
        }
    }

    std::unordered_set<int> search(const std::string& word) {
        std::unordered_set<int> result;
        index.find(word, result);
        return result;
    }
};



#endif //INVERTEDINDEX_H
