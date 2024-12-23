#ifndef BANK_HPP
#define BANK_HPP

#include "account.hpp"
#include <vector>
#include <memory>
#include <string>

class Bank {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    std::shared_ptr<Account> bank_account; // The bank's own account

public:
    // Constructor
    Bank();

    // Methods
    void create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance); // Added atm_id
    bool deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount); // Added atm_id
    bool withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount); // Added atm_id
    bool balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password); // Added atm_id
    void close_account(const std::string& atm_id, const std::string& account_id, const std::string& password); // Added atm_id
    void transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount); // Added atm_id

    // Additional methods that may be used in the implementation
    Account* find_account(const std::string& account_id); // Make sure this is implemented
    void remove_account(Account* account); // Make sure this is implemented
};

#endif // BANK_HPP
