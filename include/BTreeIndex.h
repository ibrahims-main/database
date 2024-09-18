#ifndef BTREEINDEX_H
#define BTREEINDEX_H

#include <iostream>
#include "index.h"
#include <map>

class BTreeIndex : public Index {
    public:
        void insert(const string& id, long position) override;
        long search(const string& id) const override;
        void remove(const string& id) override;
    private:
        map<string, long> index;
};

#endif // BTREEINDEX_H