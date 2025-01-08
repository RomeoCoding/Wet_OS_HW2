#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include <pthread.h>
extern std::ofstream log_file;

class ErrorHandler {
private:
//pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
public:
    // Log error messages
    static void log_error(const std::string& atm_id, char action, const std::string& account_id, double amount = 0.0);
    static void log_atm_error(const std::string& source_atm_id, const std::string& target_atm_id, bool atm_already_closed);

    // Log success messages
    static void log_success(const std::string& atm_id, char action, const std::string& account_id, double balance, double amount = 0.0, const std::string& target_account = "");
    static void log_atm_success(const std::string& source_atm_id, const std::string& target_atm_id);

    ~ErrorHandler();
};

#endif // ERROR_HANDLER_HPP
