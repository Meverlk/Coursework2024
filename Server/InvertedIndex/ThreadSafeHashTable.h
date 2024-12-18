//
// Created by Meverlk on 10.12.2024.
//

#ifndef THREADSAFEHASHTABLE_H
#define THREADSAFEHASHTABLE_H

#include <mutex>
#include <shared_mutex>
#include "HashTable.h"

using readWriteLock = std::shared_mutex;
using readLock = std::shared_lock<readWriteLock>;
using writeLock = std::unique_lock<readWriteLock>;


template <typename Key, typename Value>
class ThreadSafeHashTable {
private:
    HashTable<Key, Value> table;
    mutable readWriteLock rwLock;
public:
    void insert(const Key& key, const Value& value) {
        writeLock lock(rwLock);
        table.insert(key, value);
    }

    void insertOrAppend(const Key& key, const typename Value::value_type& item) {
        writeLock lock(rwLock);

        Value currentValue;
        if (table.find(key, currentValue)) {
            currentValue.insert(item);
            table.insert(key, currentValue);
        } else {
            Value newValue = {item};
            table.insert(key, newValue);
        }
    }

    bool find(const Key& key, Value& value) const {
        readLock lock(rwLock);
        return table.find(key, value);
    }

};



#endif //THREADSAFEHASHTABLE_H
