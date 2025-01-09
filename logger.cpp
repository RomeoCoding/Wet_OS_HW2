#include "logger.hpp"
#include <iostream>
#include <pthread.h>
// Initialize static members
std::ofstream Logger::log_file;
extern pthread_mutex_t log_lock;
// Provide access to the single instance of Logger
Logger& Logger::get_instance() {
    static Logger instance; // This is the single instance
    return instance;
}

// Open the log file
void Logger::open_log() {
    log_file.open("log.txt", std::ios_base::app); 
    if (!log_file.is_open()) {
        perror("System error: open file failed");
        exit(1);
    }
}

// Close the log file
void Logger::close_log() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

// Write a message to the log file
void Logger::log_message(const std::string& message) {
    if (log_file.is_open()) {
        pthread_mutex_lock(&log_lock);
        log_file << message << std::endl;
        pthread_mutex_unlock(&log_lock);
    } else {
        std::cerr << "Error: Log file is not open!" << std::endl;
    }
}
