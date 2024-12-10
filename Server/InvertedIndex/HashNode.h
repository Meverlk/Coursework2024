//
// Created by Meverlk on 10.12.2024.
//

#ifndef HASHNODE_H
#define HASHNODE_H

template <typename Key, typename Value>
struct HashNode {
    Key key;
    Value value;
    HashNode* next;
    HashNode(const Key& key, const Value& value) : key(key), value(value), next(nullptr) {}
};

#endif //HASHNODE_H
