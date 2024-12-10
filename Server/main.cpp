#include <iostream>
#include <string>

#include "InvertedIndex/HashTable.h"

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

int main()
{
    hashTableTest();
    return 0;
}
