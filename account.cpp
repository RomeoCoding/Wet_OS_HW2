#include "error_handler.hpp"
#include <stdexcept>
#include "account.hpp"
Account::Account(const std::string& id, const std::string& password, double initial_balance)
    : id(id), password(password), balance(initial_balance) {}

std::string Account::get_id() const {
    return id;
}
std::string Account::get_password() const {
    return password;
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
void Account::deposit(double amount) {
    balance += amount;
}

void Account::withdraw(double amount) {
    
        balance -= amount;
}
