//
// Created by Meverlk on 12.12.2024.
//

#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <unordered_set>

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

public:
    InvertedIndex(Database& docManager) : documentManager(docManager), lastDatabaseSize(0) {}

    void buildIndex() {
        size_t size = documentManager.getSize();
        if (size > lastDatabaseSize) {
            auto documents = documentManager.getAllDocuments();
            for (size_t i = lastDatabaseSize; i < size; i++) {
                indexDocument(documents[i]);
            }
            lastDatabaseSize = size;
        }
    }

    std::unordered_set<int> search(const std::string& word) const {
        std::unordered_set<int> result;
        index.find(word, result);
        return result;
    }
};



#endif //INVERTEDINDEX_H
