cmake_minimum_required(VERSION 3.29)
project(Server)

set(CMAKE_CXX_STANDARD 20)

add_executable(Server main.cpp
        InvertedIndex/HashNode.h
        InvertedIndex/HashTable.cpp
        InvertedIndex/HashTable.h
        InvertedIndex/ThreadSafeHashTable.cpp
        InvertedIndex/ThreadSafeHashTable.h
        InvertedIndex/Database/Document.h
        InvertedIndex/Database/Database.cpp
        InvertedIndex/Database/Database.h
        InvertedIndex/InvertedIndex.cpp
        InvertedIndex/InvertedIndex.h
        ThreadPool/TaskQueue.cpp
        ThreadPool/TaskQueue.h
        ThreadPool/ThreadPool.cpp
        ThreadPool/ThreadPool.h
        JSONCreation.h)

target_link_libraries(Server ws2_32.lib)