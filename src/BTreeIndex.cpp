#include "BTreeIndex.h";
#include <iostream>

void BTreeIndex::insert(const string& id, long position) {
    index[id] = position;
}

long BTreeIndex::search(const string& id) const {
    auto it = index.find(id);
    if (it != index.end()) {
        return it -> second;
    }
    return -1; // not found
}

void BTreeIndex::remove(const string& id) {
    index.erase(id);
}