#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

using namespace std;

class ErrorHandler {
public:
    // Log an error message to the console and a log file
    static void logError(const string& errorMessage);

private:
    static mutex error_mutex;
    static void writeToErrorLog(const string& errorMessage);
};

#endif // ERROR_HANDLER_H