#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <stack>
#include <mutex>
#include <thread>
#include <atomic>
#include "BTreeIndex.h"
#include "HashIndex.h"
#include "BitmapIndex.h"
#include "FullTextIndex.h"
#include <memory>

using namespace std;

class Database {
public:
    Database(const string& db_name);

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    void insert(const vector<string>& data);
    void select();
    void query(const string& id);
    void removeRecord(const string& id);
    void executeQuery(const string& query_);
    void handleDDL(const string& query_)

    void backup(const string& backup_file);
    void restore(const string& backup_file);

    bool registerUser(const string& username, const string& password);
    bool loginUser(const string& username, const string& password);
    void logoutUser();

    void setIndexType(const string& indexType);

private:
    string db_name;
    unique_ptr<Index> index; // Index: ID -> File position
    int next_id; // Simple ID generator
    bool in_transaction;
    stack<unordered_map<string, long>> transaction_log;
    mutex db_mutex;
    unordered_map<string, string> user_credentials;
    string current_user;

    bool writeTransactionLog();
    void loadData();

    bool authenticate(const string& username, const string& password);
    void loadUserCredentials();
    void saveUserCredentials();
};

#endif // DATABASE_H