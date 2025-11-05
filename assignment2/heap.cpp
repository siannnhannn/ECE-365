#include "heap.h"
#include <cassert>

// Constructor
heap::heap(int capacity)
    : mapping(capacity * 2), maxSize(capacity), currentSize(0)
{
    if (maxSize < 1)
        maxSize = 1;

    data.resize(maxSize + 1);
}

int heap::insert(const std::string &id, int key, void *pv) {
    if (currentSize >= maxSize) return 1;

    bool b = false;
    mapping.getPointer(id, &b);
    if (b) return 2;

    // Heap insert
    ++currentSize;
    data[currentSize].id = id;
    data[currentSize].key = key;
    data[currentSize].pData = pv;

    // Hash table insert
    mapping.insert(id, &data[currentSize]);
    percolateUp(currentSize);

    return 0;
}

// Restore heap order (percolateUp + percolateDown)
int heap::setKey(const std::string &id, int key) {
    bool b = false;
    node *pn = static_cast<node*>(mapping.getPointer(id, &b));
    if (!b || pn == nullptr)
        return 1;

    int pos = getPos(pn);
    int oldKey = data[pos].key;
    data[pos].key = key;

    if (key < oldKey)
        percolateUp(pos);
    else if (key > oldKey)
        percolateDown(pos);

    return 0;
}

int heap::deleteMin(std::string *pId, int *pKey, void *ppData) {
    if (currentSize == 0) return 1;

    // Copy info to output slots
    if (pId) *pId = data[1].id;
    if (pKey) *pKey = data[1].key;
    if (ppData) *(static_cast<void **>(ppData)) = data[1].pData;

    // Remove from hash
    mapping.remove(data[1].id);

    if (currentSize > 1) {
        data[1] = data[currentSize];
        mapping.setPointer(data[1].id, &data[1]);
    }

    --currentSize;
    if (currentSize >= 1)
        percolateDown(1);

    return 0;
}

int heap::remove(const std::string &id, int *pKey, void *ppData) {
    bool b = false;
    node *pn = static_cast<node*>(mapping.getPointer(id, &b));
    if (!b || pn == nullptr)
        return 1;

    int pos = getPos(pn);
    if (pKey)
        *pKey = data[pos].key;
    if (ppData)
        *(static_cast<void **>(ppData)) = data[pos].pData;

    mapping.remove(id);

    if (pos == currentSize) {
        --currentSize;
        return 0;
    }

    data[pos] = data[currentSize];
    mapping.setPointer(data[pos].id, &data[pos]);
    --currentSize;

    percolateUp(pos);
    percolateDown(pos);

    return 0;
}

// Helpers
void heap::percolateUp(int posCur) {
    node tmp = data[posCur];
    while (posCur > 1) {
        int parent = posCur / 2;
        if (tmp.key >= data[parent].key)
            break;

        data[posCur] = data[parent];
        mapping.setPointer(data[posCur].id, &data[posCur]);
        posCur = parent;
    }

    data[posCur] = tmp;
    mapping.setPointer(data[posCur].id, &data[posCur]);
}

void heap::percolateDown(int posCur) {
    node tmp = data[posCur];

    while (posCur * 2 <= currentSize) {
        int child = posCur * 2;

        if (child + 1 <= currentSize && data[child + 1].key < data[child].key)
            child++;

        if (data[child].key >= tmp.key)
            break;

        data[posCur] = data[child];
        mapping.setPointer(data[posCur].id, &data[posCur]);
        posCur = child;
    }

    data[posCur] = tmp;
    mapping.setPointer(data[posCur].id, &data[posCur]);
}

int heap::getPos(node *pn) {
    int pos = static_cast<int>(pn - &data[0]);
    return pos;
}

