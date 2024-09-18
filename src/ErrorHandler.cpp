#include "ErrorHandler.h"

// Initialize the mutex
mutex ErrorHandler::error_mutex;

// Log the error message to both the console and the error log file
void ErrorHandler::logError(const string& errorMessage) {
    lock_guard<mutex> guard(error_mutex);
    cerr << "Error: " << errorMessage << endl; // Output error to the console
    writeToErrorLog(errorMessage); // Log the error to a file
}

// Write the error message to an error log file
void ErrorHandler::writeToErrorLog(const string& errorMessage) {
    ofstream error_log("error_log.txt", ios::app); // Append mode
    if (error_log.is_open()) {
        error_log << "Error: " << errorMessage << endl;
        error_log.close();
    } else {
        cerr << "Unable to open error log file." << endl;
    }
}