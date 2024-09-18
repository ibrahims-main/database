#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

class Logger {
    public:
        enum LogLevel {
            INFO,
            WARNING,
            ERROR
        };

        Logger(const string& logFileName) : logFile(logFileName, ios::app) {
            if (!logFile.is_open()) {
                cerr << "Unable to open log file: " << logFileName << endl;
            }
        }

        ~Logger() {
            if (logFile.is_open()) {
                logFile.close();
            }
        }

        void log(LogLevel level, const string& message) {
            if (logFile.is_open()) {
                time_t now = time(nullptr);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

                logFile << "[" << timestamp << "]" << getLogLevelString(level) << ": " << message << endl;
            }
        }
    
    private:
        ofstream logFile;

        string getLogLevelString(LogLevel level) {
            switch (level)
            {
            case INFO:
                return "INFO";
            case WARNING:
                return "WARNING";
            case ERROR:
                return "ERROR";
            
            default:
                return "UNKNOWN";
            }
        }
};

#endif // LOGGER_H