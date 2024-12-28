//
// Created by Meverlk on 13.12.2024.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <atomic>
#include "TaskQueue.h"

class ThreadPool {
private:
    std::vector<std::thread> workers;
    TaskQueue<std::function<void()>> tasks;
    std::atomic<bool> initialized = false;
    std::atomic<bool> terminated = false;
    std::atomic<bool> paused = false;
    std::condition_variable_any pauseCond;
    std::condition_variable_any taskAvailable;
    mutable std::mutex poolMutex;

    void workerRoutine() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(poolMutex);
                taskAvailable.wait(lock, [this]() {
                    return terminated || (!paused && !tasks.empty());
                });

                if (terminated && tasks.empty()) {
                    return; // Завершуємо потік, якщо завершення ініційовано
                }

                if (!tasks.pop(task)) {
                    continue; // Якщо черга порожня, чекаємо далі
                }
            }

            // Виконуємо задачу
            task();
        }
    }

public:
    ThreadPool() = default;
    ~ThreadPool() { terminate(); }

    void initialize(size_t workerCount) {
        if (initialized || terminated) {
            return;
        }
        terminated = false;
        paused = false;

        for (size_t i = 0; i < workerCount; ++i) {
            workers.emplace_back(&ThreadPool::workerRoutine, this);
        }
        initialized = true;
    }

    void terminate() {
        {
            std::unique_lock lock(poolMutex);
            terminated = true;
        }
        taskAvailable.notify_all();

        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers.clear();
        initialized = false;
    }

    void pause() {
        std::unique_lock lock(poolMutex);
        paused = true;
    }

    void resume() {
        {
            std::unique_lock lock(poolMutex);
            paused = false;
        }
        pauseCond.notify_all();
        taskAvailable.notify_all();
    }

    template<typename Task, typename... Args>
    void addTask(Task&& task, Args&&... args) {
        if (!initialized || terminated) {
            return;
        }

        auto boundTask = std::bind(std::forward<Task>(task), std::forward<Args>(args)...);
        tasks.emplace(boundTask);
        taskAvailable.notify_one();
    }

    bool isPaused() const {
        return paused;
    }

    bool isInitialized() const {
        return initialized;
    }

    bool isTerminated() const {
        return terminated;
    }
};




#endif //THREADPOOL_H
