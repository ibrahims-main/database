#include "FullTextIndex.h"
#include <iostream>

void FullTextIndex::insert(const string& id, long position) {
    index[id].insert(position);
}

long FullTextIndex::search(const string& id) const {
    auto it = index.find(id);
    if (it != index.end() && !it->second.empty()) {
        return *it -> second.begin(); // Return the first position
    }
    return -1; // Not found
}

void FullTextIndex::remove(const string& id) {
    index.erase(id);
}