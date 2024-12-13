//
// Created by Meverlk on 10.12.2024.
//

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>

#include "HashNode.h"

template<typename Key, typename Value>
class HashTable {
private:
    std::vector<HashNode<Key, Value> *> table;
    size_t capacity;
    size_t size;
    double load_factor;

    size_t hashFunction(const Key& key) const {
        return std::hash<Key>{}(key) % capacity;
    }

    void rehash() {
        capacity *= 2;
        std::vector<HashNode<Key, Value> *> oldTable = std::move(table);
        table = std::vector<HashNode<Key, Value> *>(capacity, nullptr);
        size = 0;

        for (auto& node : oldTable) {

            while (node != nullptr) {
                insert(node->key, node->value);
                HashNode<Key, Value> *nextNode = node->next;
                delete node;
                node = nextNode;
            }
        }

    }

public:
    explicit HashTable(const size_t capacity = 10, double maxLoadFactor = 5.0) :
    capacity(capacity), size(0), load_factor(maxLoadFactor) {
        table.resize(capacity,nullptr);
    }

    ~HashTable() {
        for (auto& node : table) {
            while (node) {
                HashNode<Key, Value> *temp = node;
                node = node->next;
                delete temp;
            }
        }
    }

    void insert(const Key& key, const Value& value) {
        size_t index = hashFunction(key);
        HashNode<Key, Value> *node = table[index];
        HashNode<Key, Value> *prev = nullptr;

        //finding node with the same key
        while (node && node->key != key) {
            prev = node;
            node = node->next;
        }

        if (node) {
            node->value = value;
        } else {
            HashNode<Key, Value> *newNode = new HashNode<Key, Value>(key, value);
            if (prev) {
                prev->next = newNode;
            } else {
                table[index] = newNode;
            }
            size++;
        }

        if (static_cast<double>(size) / capacity > load_factor) {
            rehash();
        }
    }

    bool find(const Key& key, Value& value) const {
        size_t index = hashFunction(key);
        HashNode<Key, Value> *node = table[index];

        while (node) {
            if (node->key == key) {
                value = node->value;
                return true;
            }
            node = node->next;
        }
        return false;
    }

    bool remove(const Key& key) {
        size_t index = hashFunction(key);
        HashNode<Key, Value> *node = table[index];
        HashNode<Key, Value> *prev = nullptr;

        while (node && node->key != key) {
            prev = node;
            node = node->next;
        }

        if (node) {
            if (prev) {
                prev->next = node->next;
            } else {
                table[index] = node->next;
            }
            delete node;
            size--;
            return true;
        }

        return false;
    }

    size_t getSize() const {
        return size;
    }

    bool isEmpty () const {
        return size == 0;
    }
};



#endif //HASHTABLE_H
