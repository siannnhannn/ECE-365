// hash.cpp
#include "hash.h"
#include <utility>
#include <cstdint>

static const int primeNum[] = { 19, 41, 89, 197, 409, 857, 3607, 7219, 14449, 28901, 57803, 115607, 231221, 462463, 924941 };
static const int primeCount = (int)(sizeof(primeNum) / sizeof(primeNum[0]));

hashTable::hashTable(int size)
    : capacity(0), filled(0) {
    if (size <= 0) size = primeNum[0];
    capacity = (int)getPrime(size);
    data.assign((size_t)capacity, hashItem{});
}

int hashTable::insert(const std::string &key, void *pv)
{
    if (filled * 10 >= capacity * 7) {
        if (!rehash()) return 2;
    }

    int index = hash(key);
    int firstDeleted = -1;

    for (int probes = 0; probes < capacity; ++probes) {
        auto &slot = data[index];

        if (!slot.isOccupied) {
            // place into first tombstone if we saw one; else here
            int target = (firstDeleted != -1) ? firstDeleted : index;
            auto &t = data[target];
            t.key = key;
            t.isOccupied = true;
            t.isDeleted = false;
            t.pv = pv;
            ++filled;
            return 0;
        }

        if (!slot.isDeleted) {
            // check for duplicate
            if (slot.key == key) return 1;
        } else {
            // remember earliest reusable tombstone
            if (firstDeleted == -1) firstDeleted = index;
        }

        index = (index + 1) % capacity;
    }

    return 2;
}

bool hashTable::contains(const std::string &key)
{
    return findPos(key) != -1;
}

// FNV-1a (32-bit) hash
int hashTable::hash(const std::string& key)
{
    uint32_t h = 2166136261u;
    for (unsigned char c : key) {
        h ^= c;
        h *= 16777619u;
    }
    return capacity ? static_cast<int>(h % capacity) : 0;
}

// consistent position finding using linear probing
int hashTable::findPos(const std::string &key)
{
    int index = hash(key);

    for (int probes = 0; probes < capacity; ++probes) {
        auto &slot = data[index];

        if (!slot.isOccupied) {
            return -1;
        }
        if (!slot.isDeleted && slot.key == key) {
            return index;
        }
        index = (index + 1) % capacity;
    }
    return -1;
}

void *hashTable::getPointer(const std::string &key, bool *b)
{
    int pos = findPos(key);

    if (pos == -1) {
        if (b) *b = false;
        return nullptr;
    }

    if (b) *b = true;
    return data[pos].pv;
}

int hashTable::setPointer(const std::string &key, void *pv) {
    int pos = findPos(key);
    if (pos == -1) return 1;
    data[pos].pv = pv;
    return 0;
}

bool hashTable::rehash()
{
    unsigned int newSize = getPrime(capacity * 2);
    std::vector<hashItem> oldData = std::move(data);

    capacity = (int)newSize;
    data.assign((size_t)capacity, hashItem{});
    filled = 0;

    for (const auto &itemToInsert : oldData) {
        if (itemToInsert.isOccupied && !itemToInsert.isDeleted) {
            insert(itemToInsert.key, itemToInsert.pv);
        }
    }
    return true;
}

unsigned int hashTable::getPrime(int size)
{
    for (int i = 0; i < primeCount; ++i) {
        if (primeNum[i] >= size) return (unsigned int)primeNum[i];
    }
    return (unsigned int)primeNum[primeCount - 1];
}

bool hashTable::remove(const std::string &key) {
    int pos = findPos(key);
    if (pos == -1) return false;
    data[pos].isDeleted = true;
    data[pos].pv = nullptr;
    return true;
}

