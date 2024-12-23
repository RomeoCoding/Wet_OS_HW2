#include "error_handler.hpp"
#include <stdexcept>

Account::Account(const std::string& id, const std::string& password, double initial_balance)
    : id(id), password(password), balance(initial_balance) {}

std::string Account::get_id() const {
    return id;
}

double Account::view_balance() const {
    return balance;
}

bool Account::authenticate(const std::string& input_password) const {
    if (password != input_password) { 
        return false;
    }
    return true;
}
