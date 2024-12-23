#include "bank.hpp"
#include "error_handler.hpp"
#include <stdexcept>
#include <algorithm>
#include <string>
ErrorHandler error_handler;


//Constructor
Bank::Bank() {
    //Initialize the bank's own account
    bank_account = std::make_shared<Account>("BANK", "bank123", 0.0);
}

/*
=========================
======== Create  ========
=========================
*/

void Bank::create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance) {
    if (find_account(id) != nullptr) {
        error_handler.log_error(atm_id, 'O', id); 
        return; 
    }
    accounts.push_back(std::make_shared<Account>(id, password, initial_balance));
    error_handler.log_success(atm_id, 'O', id, initial_balance, 0.0);
}

/*
=========================
======== Deposit ========
=========================
*/

bool Bank::deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
    Account* acc = find_account(id); 
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'D', id); 
        return false;
    }

    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'D', id); 
        return false;
    }

    acc->deposit(amount);
    error_handler.log_success(atm_id, 'D', id, acc->view_balance(), amount); 
    return true; 
}

/*
=========================
======== Withdraw =======
=========================
*/

bool Bank::withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
    Account* acc = find_account(id); 
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'W', id); 
        return false;
    }

    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'W', id); 
        return false;
    }

    if (acc->view_balance() < amount) {
        error_handler.log_error(atm_id, 'W', id, true); 
        return false;
    }

    acc->withdraw(amount);
    error_handler.log_success(atm_id, 'W', id, acc->view_balance(), amount); 
    return true; 
}

/*
=========================
======== Balance ========
=========================
*/

bool Bank::balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    Account* acc = find_account(account_id); 
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'B', account_id); 
        return false;
    }

    if (acc->authenticate(password)) {
        double balance = acc->view_balance();
        error_handler.log_success(atm_id, 'B', account_id, balance, 0.0);  
        return true;
    } else {
        error_handler.log_error(atm_id, 'B', account_id);  
        return false;
    }
}

/*
=========================
========= Close =========
=========================
*/
void Bank::close_account(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    Account* account = find_account(account_id); 
    if (account == nullptr) {
        error_handler.log_error(atm_id, 'Q', account_id);
        return;
    }

    if (account->get_password() != password) {
        error_handler.log_error(atm_id, 'Q', account_id); 
        return;
    }

    double balance = account->view_balance();
    remove_account(account); 

    error_handler.log_success(atm_id, 'Q', account_id, balance, 0.0); 
}

/*
=========================
======== Transfer =======
=========================
*/

void Bank::transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount) {
    Account* source_account = find_account(source_account_id); 
    Account* target_account = find_account(target_account_id); 

    if (source_account == nullptr || target_account == nullptr) {
        error_handler.log_error(atm_id, 'T', source_account_id); 
        return;
    }

    if (source_account->get_password() != password) {
        error_handler.log_error(atm_id, 'T', source_account_id);
        return;
    }

    if (source_account->view_balance() < amount) {
        error_handler.log_error(atm_id, 'T', source_account_id, true);  
        return;
    }

    source_account->withdraw(amount);  
    target_account->deposit(amount);   

    error_handler.log_success(atm_id, 'T', source_account_id, source_account->view_balance(), amount);
}


/*
=========================
======== Utilities ======
=========================
*/


Account* Bank::find_account(const std::string& account_id) {
    for (const auto& acc : accounts) {
        if (acc->get_id() == account_id) {
            return acc.get(); 
        }
    }
    return nullptr; 
}

void Bank::remove_account(Account* account) {
    auto it = std::remove_if(accounts.begin(), accounts.end(), 
        [account](const std::shared_ptr<Account>& acc) {
            return acc.get() == account; 
        });

    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
    }
}
