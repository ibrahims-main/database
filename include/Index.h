#ifndef INDEX_H
#define INDEX_H

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class Index {
    public:
        virtual ~Index() = default;
        virtual void insert(const string& id, long position) = 0;
        virtual long search(const string& id) const = 0;
        virtual void remove(const string& id) = 0; 
};

#endif // INDEX_H