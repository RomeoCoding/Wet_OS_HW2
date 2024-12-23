#include "bank.hpp"
#include "error_handler.hpp"
#include <stdexcept>
#include <algorithm>
#include <string>
ErrorHandler error_handler;
// Constructor
Bank::Bank() {
    // Initialize the bank's own account
    bank_account = std::make_shared<Account>("BANK", "bank123", 0.0);
}

// Create a new account
void Bank::create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance) {
    // Use find_account to check if the account already exists
    if (find_account(id) != nullptr) {
        // Log the error if account with the same ID already exists
        error_handler.log_error(atm_id, 'O', id); // Use id as std::string here
        return; // Stop further execution
    }

    // Account creation
    accounts.push_back(std::make_shared<Account>(id, password, initial_balance));
    // Log success
    error_handler.log_success(atm_id, 'O', id, initial_balance, 0.0);
}


// Deposit money
// Deposit money
bool Bank::deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
    Account* acc = find_account(id); // Use find_account here
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'D', id); // Use id as std::string here
        return false;
    }

    // Check if the password is correct
    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'D', id); // Use id as std::string here
        return false;
    }

    // Perform the deposit and log success
    acc->deposit(amount);
    error_handler.log_success(atm_id, 'D', id, acc->view_balance(), amount); // No target account for deposit
    return true; // Successful deposit
}

// Withdraw money
bool Bank::withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
    Account* acc = find_account(id); // Use find_account here
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'W', id); // Use id as std::string here
        return false;
    }

    // Check if the password is correct
    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'W', id); // Use id as std::string here
        return false;
    }

    // Check if balance is sufficient for withdrawal
    if (acc->view_balance() < amount) {
        error_handler.log_error(atm_id, 'W', id, true); // true indicates insufficient funds
        return false;
    }

    // Perform the withdrawal and log success
    acc->withdraw(amount);
    error_handler.log_success(atm_id, 'W', id, acc->view_balance(), amount); // No target account for withdrawal
    return true; // Successful withdrawal
}

// Balance Inquiry
bool Bank::balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    Account* acc = find_account(account_id); // Use find_account here
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'B', account_id); // Use account_id as std::string here
        return false;
    }

    if (acc->authenticate(password)) {
        double balance = acc->view_balance();
        error_handler.log_success(atm_id, 'B', account_id, balance, 0.0);  // Log success for balance inquiry
        return true;
    } else {
        error_handler.log_error(atm_id, 'B', account_id);  // Incorrect password error
        return false;
    }
}

// Close an account
void Bank::close_account(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    Account* account = find_account(account_id); // Use find_account here
    if (account == nullptr) {
        // If account does not exist, log the error
        error_handler.log_error(atm_id, 'Q', account_id); // Use account_id as std::string here
        return;
    }

    if (account->get_password() != password) {
        // If the password is incorrect, log the error
        error_handler.log_error(atm_id, 'Q', account_id); // Use account_id as std::string here
        return;
    }

    // If password is correct, close the account
    double balance = account->view_balance();
    remove_account(account); // Use remove_account here

    // Log the successful closure of the account
    error_handler.log_success(atm_id, 'Q', account_id, balance, 0.0); // No target account for close
}

// Transfer money between accounts
void Bank::transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount) {
    Account* source_account = find_account(source_account_id); // Use find_account here
    Account* target_account = find_account(target_account_id); // Use find_account here

    if (source_account == nullptr || target_account == nullptr) {
        // If either account does not exist, log an error
        error_handler.log_error(atm_id, 'T', source_account_id); // Use source_account_id as std::string here
        return;
    }

    if (source_account->get_password() != password) {
        // If the password is incorrect, log the error
        error_handler.log_error(atm_id, 'T', source_account_id); // Use source_account_id as std::string here
        return;
    }

    if (source_account->view_balance() < amount) {
        // If there are insufficient funds in the source account, log the error
        error_handler.log_error(atm_id, 'T', source_account_id, true);  // true indicates insufficient funds
        return;
    }

    // Perform the transfer
    source_account->withdraw(amount);  // Assuming withdraw reduces the balance by amount
    target_account->deposit(amount);   // Assuming deposit increases the balance by amount

    // Log the successful transfer
    error_handler.log_success(atm_id, 'T', source_account_id, source_account->view_balance(), amount);
}

// Utility function to find an account by its ID
Account* Bank::find_account(const std::string& account_id) {
    for (const auto& acc : accounts) {
        if (acc->get_id() == account_id) {
            return acc.get(); // Return raw pointer to Account
        }
    }
    return nullptr; // Return nullptr if account not found
}

// Remove an account by its pointer
void Bank::remove_account(Account* account) {
    // Find and remove the account from the vector
    auto it = std::remove_if(accounts.begin(), accounts.end(), 
        [account](const std::shared_ptr<Account>& acc) {
            return acc.get() == account; // Compare raw pointers
        });

    // If account was found, erase it from the vector
    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
    }
}
