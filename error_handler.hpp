#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>

class ErrorHandler {
public:
    // Log error messages
    static void log_error(const std::string& atm_id, char action, const std::string& account_id, double amount = 0.0);

    // Log success messages
    static void log_success(const std::string& atm_id, char action, const std::string& account_id, double balance, double amount = 0.0, const std::string& target_account = "");

};

#endif // ERROR_HANDLER_HPP
