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

template<typename task_T>
bool TaskQueue<task_T>::empty() {
    readLock lock(rwLock);
    return tasks.empty();
}

template<typename task_T>
int TaskQueue<task_T>::size() {
    readLock lock(rwLock);
    return tasks.size();
}

template<typename task_T>
void TaskQueue<task_T>::clear() {
    writeLock lock(rwLock);
    while (!tasks.empty()) {
        tasks.pop();
    }
}

template<typename task_T>
bool TaskQueue<task_T>::pop(task_T &task) {
    writeLock lock(rwLock);
    if (tasks.empty()) {
        return false;
    } else {
        task = tasks.front();
        tasks.pop();
        return true;
    }
}

template<typename task_T>
template<typename... arguments>
void TaskQueue<task_T>::emplace(arguments &&... parameters) {
    writeLock lock(rwLock);
    tasks.emplace(std::forward<arguments>(parameters)...);
}

#endif //TASKQUEUE_H
