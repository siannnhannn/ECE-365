#ifndef _HEAP_H
#define _HEAP_H

#include <vector>
#include <string>
#include "hash.h"

class heap {
public:
    explicit heap(int capacity);

    int insert(const std::string &id, int key, void *pv = nullptr);
    int setKey(const std::string &id, int key);
    int deleteMin(std::string *pId = nullptr, int *pKey = nullptr, void *ppData = nullptr);
    int remove(const std::string &id, int *pKey = nullptr, void *ppData = nullptr);

private:
    class node {
    public:
        std::string id;
        int key;
        void *pData;
        node() : id(), key(0), pData(nullptr) {}
    };

    std::vector<node> data;
    hashTable mapping;
    int maxSize;
    int currentSize;

    void percolateUp(int posCur);
    void percolateDown(int posCur);
    int getPos(node *pn);
};

#endif
