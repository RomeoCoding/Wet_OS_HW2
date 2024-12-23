#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>

class Logger {
private:
    static std::ofstream log_file; // Log file object

    // Private constructor to prevent direct instantiation
    Logger() {}

public:
    // Deleted copy constructor and assignment operator to prevent copies
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& get_instance(); // Method to access the single instance
    void open_log(); // Open the log file
    void close_log(); // Close the log file
    void log_message(const std::string& message); // Write a message to the log file
};

#endif
