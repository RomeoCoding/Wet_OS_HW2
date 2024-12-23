#include "logger.hpp"
#include <iostream>

// Initialize static members
std::ofstream Logger::log_file;

// Provide access to the single instance of Logger
Logger& Logger::get_instance() {
    static Logger instance; // This is the single instance
    return instance;
}

// Open the log file
void Logger::open_log() {
    log_file.open("log.txt", std::ios_base::app); // Open in append mode
    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to open log file!" << std::endl;
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
        log_file << message << std::endl;
    } else {
        std::cerr << "Error: Log file is not open!" << std::endl;
    }
}
