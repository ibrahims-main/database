#include "HashIndex.h"
#include <iostream>

void HashIndex::insert(const string& id, long position) {
    index[id] = position;
}

long HashIndex::search(const string& id) const {
    auto it = index.find(id);
    if (it != index.end()) {
        return it -> second;
    }
    return -1; // Not found
}

void HashIndex::remove(const string& id) {
    index.erase(id);
}