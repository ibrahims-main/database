#ifndef FULLTEXTINDEX_H
#define FULLTEXTINDEX_H

#include <iostream>
#include "Index.h"
#include <unordered_map>
#include <set>
#include <sstream>

using namespace std;

class FullTextIndex : public Index {
public:
    void insert(const string& id, long position) override;
    long search(const string& id) const override;
    void remove(const string& id) override;

private:
    unordered_map<string, set<long>> index;
};

#endif // FULLTEXTINDEX_H