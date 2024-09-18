#include "Table.h"
#include <iostream>

using namespace std;

Table::Table(const string& name) : name(name) {}

void Table::addColumn(const string& name, const string& dataType, bool isPrimaryKey) {
    columns.push_back({name, dataType, isPrimaryKey});
}

void Table::removeColumn(const string& name) {
    columns.erase(remove_if(columns.begin(), columns.end(),
        [&name](const Column& col) { return col.name == name; }), columns.end());
}

void Table::renameColumn(const string& oldName, const string& newName) {
    for (auto& col : columns) {
        if (col.name == oldName) {
            col.name = newName;
            return;
        }
    }
}

const vector<Column>& Table::getColumns() const {
    return columns;
}

string Table::getName() const {
    return name;
}
