#include <iostream>
#include <string>

#include "InvertedIndex/HashTable.h"
#include "InvertedIndex/InvertedIndex.h"
#include "InvertedIndex/Database/Database.h"
#include "ThreadPool/ThreadPool.h"

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

int main()
{
    testThreadPool();
    return 0;
}
