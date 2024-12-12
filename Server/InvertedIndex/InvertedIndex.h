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

    void indexDocument(const Document& document) {
        std::istringstream stream(document.content);
        std::string word;
        while (stream >> word) {
            index.insertOrAppend(word, document.id);
        }
    }

public:
    InvertedIndex(Database& docManager) : documentManager(docManager) {}

    void buildIndex() {
        auto documents = documentManager.getAllDocuments();
        for (const auto& document : documents) {
            indexDocument(document);
        }
    }

    std::unordered_set<int> search(const std::string& word) const {
        std::unordered_set<int> result;
        index.find(word, result);
        return result;
    }
};



#endif //INVERTEDINDEX_H
