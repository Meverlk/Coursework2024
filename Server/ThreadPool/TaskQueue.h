//
// Created by Meverlk on 13.12.2024.
//

#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <queue>
#include <shared_mutex>
#include <mutex>

using readWriteLock = std::shared_mutex;
using readLock = std::shared_lock<readWriteLock>;
using writeLock = std::unique_lock<readWriteLock>;

template <typename task_T>
class TaskQueue {
private:
    std::queue<task_T> tasks;
    mutable readWriteLock rwLock;
public:
    TaskQueue() = default;
    ~TaskQueue() {
        clear();
    }
    bool empty();
    int size();
    void clear();
    bool pop(task_T& task);

    template<typename... arguments>
    void emplace(arguments&&... parameters);
};

#endif //TASKQUEUE_H
