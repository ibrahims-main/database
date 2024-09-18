#include "Database.h"
#include "Index.h"
#include "Logger.h"
#include "ErrorHandler.h"
#include "Table.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <stack>

using namespace std;

Database::Database(const string& db_name) 
    : db_name(db_name), in_transaction(false), next_id(1) {
    loadData();
    loadUserCredentials();
    Logger::log("Database initialized: " + db_name);
}

void Database::beginTransaction() {
    lock_guard<mutex> guard(db_mutex);
    if (in_transaction) {
        ErrorHandler::logError("Transaction already in progress");
        return;
    }
    in_transaction = true;
    transaction_log.push({});
    Logger::log("Transaction started.");
}

void Database::commitTransaction() {
    lock_guard<mutex> guard(db_mutex);
    if (!in_transaction) {
        ErrorHandler::logError("No transaction in progress.");
        return;
    }
    if (writeTransactionLog()) {
        in_transaction = false;
        transaction_log.pop();
        Logger::log("Transaction committed.");
    } else {
        ErrorHandler::logError("Failed to commit transaction.");
    }
}

void Database::rollbackTransaction() {
    lock_guard<mutex> guard(db_mutex);
    if (!in_transaction) {
        ErrorHandler::logError("No transaction in progress.");
        return;
    }
    auto& log = transaction_log.top();
    for (const auto& entry : log) {
        removeRecord(entry.first);
    }
    in_transaction = false;
    transaction_log.pop();
    Logger::log("Transaction rolled back.");
}

void Database::insert(const vector<string>& data) {
    lock_guard<mutex> guard(db_mutex);
    ofstream db_file(db_name, ios::app);

    if (db_file.is_open()) {
        string id = to_string(next_id++);
        db_file << id << " ";

        for (const auto& item : data) {
            db_file << item << " ";
        }
        db_file << "\n";
        db_file.close();

        if (index) {
            index->insert(id, db_file.tellp());

            if (in_transaction) {
                transaction_log.top()[id] = db_file.tellp();
            }

            Logger::log("Record inserted successfully with ID: " + id);
        } else {
            ErrorHandler::logError("Index not set.");
        }
    } else {
        ErrorHandler::logError("Unable to open database file.");
    }
}

void Database::select() {
    lock_guard<mutex> guard(db_mutex);
    ifstream db_file(db_name);
    string line;
    if (db_file.is_open()) {
        while (getline(db_file, line)) {
            cout << line << endl;
        }
        db_file.close();
    } else {
        ErrorHandler::logError("Unable to open database file.");
    }
}

void Database::query(const string& id) {
    lock_guard<mutex> guard(db_mutex);
    if (index) {
        long position = index->search(id);
        if (position != -1) {
            ifstream db_file(db_name);
            if (db_file.is_open()) {
                db_file.seekg(position);
                string line;
                getline(db_file, line);
                cout << "Record with ID " << id << ": " << line << endl;
                db_file.close();
            } else {
                ErrorHandler::logError("Unable to open database file.");
            }
        } else {
            ErrorHandler::logError("ID not found in index.");
        }
    } else {
        ErrorHandler::logError("Index not set.");
    }
}

void Database::removeRecord(const string& id) {
    lock_guard<mutex> guard(db_mutex);
    ifstream db_file(db_name);
    ofstream temp_file("temp.txt");

    if (db_file.is_open() && temp_file.is_open()) {
        string line;
        while (getline(db_file, line)) {
            if (line.find(id + " ") != 0) {
                temp_file << line << "\n";
            }
        }

        db_file.close();
        temp_file.close();

        if (remove(db_name.c_str()) != 0) {
            ErrorHandler::logError("Failed to remove original database file.");
            return;
        }
        if (rename("temp.txt", db_name.c_str()) != 0) {
            ErrorHandler::logError("Failed to rename temp file.");
            return;
        }

        if (index) {
            index->remove(id);
        }

        Logger::log("Record with ID: " + id + " removed.");
    } else {
        ErrorHandler::logError("Unable to open database file or temp file.");
    }
}

void Database::executeQuery(const string& query_) {
    lock_guard<mutex> guard(db_mutex);
    if (current_user.empty()) {
        ErrorHandler::logError("User not authenticated.");
        return;
    }

    istringstream iss(query_);
    string command;
    iss >> command;

    try {
        if (command == "CREATE" || command == "DROP" || command == "ALTER") {
            handleDDL(query_);

        } else if (command == "INSERT") {
            vector<string> values;
            string value;
            while (iss >> value) {
                values.push_back(value);
            }
            insert(values);

        } else if (command == "SELECT") {
            select();

        } else if (command == "QUERY") {
            string id;
            iss >> id;
            query(id);

        } else if (command == "REMOVE") {
            string id;
            iss >> id;
            removeRecord(id);

        } else if (command == "BEGIN") {
            beginTransaction();

        } else if (command == "COMMIT") {
            commitTransaction();

        } else if (command == "ROLLBACK") {
            rollbackTransaction();

        } else if (command == "REGISTER") {
            string username, password;
            iss >> username >> password;
            registerUser(username, password);

        } else if (command == "LOGIN") {
            string username, password;
            iss >> username >> password;
            loginUser(username, password);

        } else if (command == "LOGOUT") {
            logoutUser();

        } else if (command == "BACKUP") {
            string backup_file;
            iss >> backup_file;
            backup(backup_file);

        } else if (command == "RESTORE") {
            string backup_file;
            iss >> backup_file;
            restore(backup_file);

        } else {
            ErrorHandler::logError("Unknown command.");
        }
    } catch (const exception& e) {
        ErrorHandler::logError(string("Exception occurred: ") + e.what());
    }
}

void Database::handleDDL(const string& query_) {
    istringstream iss(query_);
    string command, tableName, columnName, dataType, operation, newColumnName;
    iss >> command;

    try {
        if (command == "CREATE") {
            iss >> tableName;
            vector<Column> columns;
            while (iss >> columnName >> dataType) {
                columns.push_back({ columnName, dataType, false });
            }
            createTable(tableName, columns);

        } else if (command == "DROP") {
            iss >> tableName;
            dropTable(tableName);

        } else if (command == "ALTER") {
            iss >> tableName >> operation;

            if (operation == "ADD") {
                iss >> columnName >> dataType;
                alterTable(tableName, operation, columnName, dataType);

            } else if (operation == "DROP") {
                iss >> columnName;
                alterTable(tableName, operation, columnName);

            } else if (operation == "RENAME") {
                iss >> columnName >> newColumnName;
                alterTable(tableName, operation, columnName, "", newColumnName);

            } else {
                ErrorHandler::logError("Invalid ALTER TABLE operation");
            }

        } else {
            ErrorHandler::logError("Invalid DDL command.");
        }
    } catch (const exception& e) {
        ErrorHandler::logError(string("Exception occurred in handleDDL: ") + e.what());
    }
}

void Database::backup(const string& backup_file) {
    lock_guard<mutex> guard(db_mutex);
    ifstream db_file(db_name, ios::binary);
    ofstream backup(backup_file, ios::binary);

    if (db_file.is_open() && backup.is_open()) {
        backup << db_file.rdbuf();
        Logger::log("Backup created successfully: " + backup_file);
    } else {
        ErrorHandler::logError("Unable to open database or backup file.");
    }
}

void Database::restore(const string& backup_file) {
    lock_guard<mutex> guard(db_mutex);
    ifstream backup(backup_file, ios::binary);
    ofstream db_file(db_name, ios::binary);

    if (backup.is_open() && db_file.is_open()) {
        db_file << backup.rdbuf();
        loadData(); // reload data to update indexes
        Logger::log("Database restored from backup: " + backup_file);
    } else {
        ErrorHandler::logError("Unable to open backup or database file.");
    }
}

void Database::setIndexType(const string& indexType) {
    lock_guard<mutex> guard(db_mutex);
    if (indexType == "BTREE") {
        index = make_unique<BTreeIndex>();
        Logger::log("Index type set to BTREE.");
    } else if (indexType == "HASH") {
        index = make_unique<HashIndex>();
        Logger::log("Index type set to HASH.");
    } else if (indexType == "BITMAP") {
        index = make_unique<BitmapIndex>();
        Logger::log("Index type set to BITMAP.");
    } else if (indexType == "FULLTEXT") {
        index = make_unique<FullTextIndex>();
        Logger::log("Index type set to FULLTEXT.");
    } else {
        ErrorHandler::logError("Unknown index type.");
    }
}

bool Database::writeTransactionLog() {
    ofstream log_file("transaction_log.txt", ios::app);
    if (!log_file.is_open()) {
        ErrorHandler::logError("Unable to open transaction log file.");
        return false;
    }

    auto& log = transaction_log.top();
    for (const auto& entry : log) {
        log_file << "INSERT " << entry.first << " " << entry.second << "\n";
    }

    Logger::log("Transaction log written successfully.");
    return true;
}

void Database::loadData() {
    ifstream db_file(db_name);
    if (db_file.is_open()) {
        string line;
        while (getline(db_file, line)) {
            istringstream iss(line);
            string id;
            iss >> id;
            long position = iss.tellg();
            if (index) {
                index->insert(id, position);
            }
        }
        db_file.close();
        Logger::log("Data loaded successfully from the database.");
    } else {
        ErrorHandler::logError("Unable to open database file.");
    }
}

void Database::loadUserCredentials() {
    ifstream user_file("users.txt");
    if (user_file.is_open()) {
        string username, password;
        while (user_file >> username >> password) {
            user_credentials[username] = password;
        }
        user_file.close();
        Logger::log("User credentials loaded successfully.");
    } else {
        ErrorHandler::logError("Unable to open user credentials file.");
    }
}

void Database::saveUserCredentials() {
    ofstream user_file("users.txt");
    if (user_file.is_open()) {
        for (const auto& [username, password] : user_credentials) {
            user_file << username << " " << password << "\n";
        }
        user_file.close();
        Logger::log("User credentials saved successfully.");
    } else {
        ErrorHandler::logError("Unable to open user credentials file.");
    }
}

bool Database::registerUser(const string& username, const string& password) {
    lock_guard<mutex> guard(db_mutex);
    if (user_credentials.find(username) != user_credentials.end()) {
        ErrorHandler::logError("User already exists.");
        return false;
    }
    user_credentials[username] = password;
    saveUserCredentials();
    Logger::log("User registered successfully: " + username);
    return true;
}

bool Database::loginUser(const string& username, const string& password) {
    lock_guard<mutex> guard(db_mutex);
    if (authenticate(username, password)) {
        current_user = username;
        Logger::log("User logged in successfully: " + username);
        return true;
    } else {
        ErrorHandler::logError("Invalid username or password.");
        return false;
    }
}

void Database::logoutUser() {
    lock_guard<mutex> guard(db_mutex);
    current_user.clear();
    Logger::log("User logged out successfully.");
}

bool Database::authenticate(const string& username, const string& password) {
    auto it = user_credentials.find(username);
    return it != user_credentials.end() && it->second == password;
}

void Database::createTable(const string& tableName, const vector<Column>& columns) {
    lock_guard<mutex> guard(db_mutex);

    if (tables.find(tableName) != tables.end()) {
        ErrorHandler::logError("Table " + tableName + " already exists.");
        return;
    }

    Table newTable(tableName);
    for (const auto& col : columns) {
        newTable.addColumn(col.name, col.dataType, col.isPrimaryKey);
    }

    tables[tableName] = newTable;
    Logger::log("Table " + tableName + " created successfully.");
}

void Database::dropTable(const string& tableName) {
    lock_guard<mutex> guard(db_mutex);

    auto it = tables.find(tableName);
    if (it == tables.end()) {
        ErrorHandler::logError("Table " + tableName + " does not exist.");
        return;
    }

    tables.erase(it);
    Logger::log("Table " + tableName + " dropped successfully.");
}

void Database::alterTable(const string& tableName, const string& operation, const string& columnName, const string& dataType, const string& newColumnName) {
    lock_guard<mutex> guard(db_mutex);

    auto it = tables.find(tableName);
    if (it == tables.end()) {
        ErrorHandler::logError("Table " + tableName + " does not exist.");
        return;
    }

    Table& table = it->second;
    if (operation == "ADD") {
        table.addColumn(columnName, dataType);
        Logger::log("Column " + columnName + " added to table " + tableName + ".");
    } else if (operation == "DROP") {
        table.removeColumn(columnName);
        Logger::log("Column " + columnName + " removed from table " + tableName + ".");
    } else if (operation == "RENAME") {
        table.renameColumn(columnName, newColumnName);
        Logger::log("Column " + columnName + " renamed to " + newColumnName + " in table " + tableName + ".");
    } else {
        ErrorHandler::logError("Unknown ALTER TABLE operation.");
    }
}