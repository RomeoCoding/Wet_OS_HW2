#ifndef BANK_HPP
#define BANK_HPP

#include "account.hpp"
#include <vector>
#include <memory>
#include <string>

class Bank {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    std::shared_ptr<Account> bank_account; // he bank's own account

public:
    // Constructor
    Bank();

    //Methods
    void create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance);
    bool deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount); 
    bool withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount); 
    bool balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password); 
    void close_account(const std::string& atm_id, const std::string& account_id, const std::string& password); 
    void transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount);

    //utility functions
    Account* find_account(const std::string& account_id); 
    void remove_account(Account* account); 
};

#endif // BANK_HPP
