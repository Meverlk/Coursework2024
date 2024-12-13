//
// Created by Meverlk on 13.12.2024.
//

#include "TaskQueue.h"

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
