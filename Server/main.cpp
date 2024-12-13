#include <iostream>
#include <string>

#include "InvertedIndex/HashTable.h"
#include "InvertedIndex/InvertedIndex.h"
#include "InvertedIndex/Database/Database.h"

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
    auto result = invertedIndex.search("before");
    for (auto res : result) {
        Document document;
        if (database.getDocumentById(res, document)) {
            std::cout << document.name << std::endl << document.content << std::endl;
        } else {
            std::cout << "Id not found" << std::endl;
        }
    }
}

int main()
{
    InvertedIndexTest();
    return 0;
}
