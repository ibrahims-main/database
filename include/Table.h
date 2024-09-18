#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;

struct Column {
    string name;
    string dataType;
    bool isPrimaryKey;
};

class Table {
    public:
        Table(const string& name);

        void addColumn(const string& name, const string& dataType, bool isPrimaryKey);
        void removeColumn(const string& name);
        void renameColumn(const string& oldName, const string& newName);

        const vector<Column>& getColumns() const;
        string getName() const;

    private:
        string name;
        vector<Column> columns;
};


#endif // TABLE_H