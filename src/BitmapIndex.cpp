#include "BitmapIndex.h"
#include <iostream>

void BitmapIndex::insert(const string& id, long position) {
    index[id].set(position);
}

long BitmapIndex::search(const string& id) const {
    auto it = index.find(id);
    if (it != index.end()) {
        for (size_t i = 0; i < MAX_ID; ++i) {
            if (it -> second.test(i)) {
                return i;
            }
        }
    }
    return -1; // Not found
}

void BitmapIndex::remove(const string& id) {
    index.erase(id);
}