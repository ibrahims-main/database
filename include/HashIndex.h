#ifndef HASHINDEX_H
#define HASHINDEX_H

#include <iostream>
#include "Index.h"
#include <unordered_map>

using namespace std;

class HashIndex : public Index {
    public:
        void insert(const string& id, long position) override;
        long search(const string& id) const override;
        void remove(const string& id) override;
    
    private:
        unordered_map<string, long> index;
};

#endif // HASHINDEX_H