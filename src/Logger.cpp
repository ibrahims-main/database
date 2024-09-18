#include "Logger.h"
#include <fstream>
#include <iostream>
#include <mutex>

using namespace std;

// Initialize the static mutex
mutex Logger::log_mutex;

// Method to log messages to a file and the console
void Logger::log(const string& message) {
    lock_guard<mutex> guard(log_mutex);  // Ensure thread-safety

    // Open log file in append mode
    ofstream log_file("database_log.txt", ios::app);
    if (log_file.is_open()) {
        log_file << message << endl;
        log_file.close();
    } else {
        cerr << "Unable to open log file." << endl;
    }

    // Output message to console
    cout << message << endl;
}
