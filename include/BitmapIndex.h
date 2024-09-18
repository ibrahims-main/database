#ifndef BITMAPINDEX_H
#define BITMAPINDEX_H

#include <iostream>
#include "Index.h"
#include <unordered_map>
#include <bitset>

using namespace std;

const size_t MAX_ID = 1000; // Adjust based on expected size

class BitmapIndex : public Index {
    public:
        void insert(const string& id, long position) override;
        long search(const string& id) const override;
        void remove(const string& id) override;

    private:
        unordered_map<string, bitset<MAX_ID>> index;
        bitset<MAX_ID> bitset_positions;
};

#endif // BITMAPINDEX_H