//
// Created by Meverlk on 10.12.2024.
//

#ifndef THREADSAFEHASHTABLE_H
#define THREADSAFEHASHTABLE_H

#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "HashTable.h"

using readWriteLock = std::shared_mutex;
using readLock = std::shared_lock<readWriteLock>;
using writeLock = std::unique_lock<readWriteLock>;


template <typename Key, typename Value>
class ThreadSafeHashTable {
private:
    HashTable<Key, Value> table;
    size_t segmentCount;
    std::vector<readWriteLock> segmentLocks;
    std::atomic<bool> rehashInProgress{false};

    size_t getSegmentIndex(const Key& key) const {
        size_t index = table.hashFunction(key);
        return index % segmentCount;
    }

    void rehash() {
        if (!rehashInProgress.exchange(true)) {
            for (auto& lock : segmentLocks) {
                lock.lock();
            }
            table.rehash();
            for (auto& lock : segmentLocks) {
                lock.unlock();
            }
            rehashInProgress = false;
        }
    }
public:
    explicit ThreadSafeHashTable(size_t segments = 10)
            : segmentCount(segments), segmentLocks(segments) {}

    void insert(const Key& key, const Value& value) {
        size_t segmentIndex = getSegmentIndex(key);
        {
            writeLock lock(segmentLocks[segmentIndex]);
            table.insert(key, value);
        }
        if (table.rehashNeeded()) {
            rehash();
        }
    }

    void insertOrAppend(const Key& key, const typename Value::value_type& item) {
        size_t segmentIndex = getSegmentIndex(key);
        {
            writeLock lock(segmentLocks[segmentIndex]);

            Value currentValue;
            if (table.find(key, currentValue)) {
                currentValue.insert(item);
                table.insert(key, currentValue);
            } else {
                Value newValue = {item};
                table.insert(key, newValue);
            }
        }
        if (table.rehashNeeded()) {
            rehash();
        }
    }

    bool find(const Key& key, Value& value) {
        size_t segmentIndex = getSegmentIndex(key);
        readLock lock(segmentLocks[segmentIndex]);
        return table.find(key, value);
    }
};



#endif //THREADSAFEHASHTABLE_H
