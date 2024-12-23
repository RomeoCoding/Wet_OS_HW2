#include "error_handler.hpp"
#include <iostream>
#include <fstream>

std::ofstream log_file("log.txt", std::ios_base::app);

void ErrorHandler::log_error(const std::string& atm_id, char action, int account_id, bool insufficient_funds) {
    std::string message;
    switch (action) {
        case 'O': // Open Account
            message = "Error " + atm_id + ": Your transaction failed – account with the same id exists";
            break;
        case 'D': // Deposit
            message = "Error " + atm_id + ": Your transaction failed – password for account id " + std::to_string(account_id) + " is incorrect";
            break;
        case 'W': // Withdraw
            if (insufficient_funds) {
                message = "Error " + atm_id + ": Your transaction failed – account id " + std::to_string(account_id) + " balance is lower than the amount";
            } else {
                message = "Error " + atm_id + ": Your transaction failed – password for account id " + std::to_string(account_id) + " is incorrect";
            }
            break;
        case 'B': // Balance Inquiry
            message = "Error " + atm_id + ": Your transaction failed – password for account id " + std::to_string(account_id) + " is incorrect";
            break;
        case 'Q': // Close Account
            message = "Error " + atm_id + ": Your transaction failed – password for account id " + std::to_string(account_id) + " is incorrect";
            break;
        case 'T': // Transfer
            if (insufficient_funds) {
                message = "Error " + atm_id + ": Your transaction failed – account id " + std::to_string(account_id) + " balance is lower than the amount";
            } else {
                message = "Error " + atm_id + ": Your transaction failed – password for account id " + std::to_string(account_id) + " is incorrect";
            }
            break;
        case 'C': // ATM Shutdown
            message = "Error " + atm_id + ": Your transaction failed – ATM ID does not exist";
            break;
        default:
            message = "Unknown error for ATM " + atm_id;
    }
    log_file << message << std::endl;
}

void ErrorHandler::log_success(const std::string& atm_id, char action, const std::string& account_id, double balance, double amount, const std::string& target_account) {
    std::string message;
    switch (action) {
        case 'O': // Open Account
            message = atm_id + ": New account id is " + account_id + " with password " + account_id + " and initial balance " + std::to_string(balance);
            break;
        case 'D': // Deposit
            message = atm_id + ": Account " + account_id + " new balance is " + std::to_string(balance) + " after " + std::to_string(amount) + " $ was deposited";
            break;
        case 'W': // Withdraw
            message = atm_id + ": Account " + account_id + " new balance is " + std::to_string(balance) + " after " + std::to_string(amount) + " $ was withdrawn";
            break;
        case 'B': // Balance Inquiry
            message = atm_id + ": Account " + account_id + " balance is " + std::to_string(balance);
            break;
        case 'Q': // Close Account
            message = atm_id + ": Account " + account_id + " is now closed. Balance was " + std::to_string(balance);
            break;
        case 'T': // Transfer
            message = atm_id + ": Transfer of " + std::to_string(amount) + " $ from account " + account_id + " to account " + target_account + " completed. New balance for source account: " + std::to_string(balance);
            break;
        default:
            message = "Unknown success for ATM " + atm_id;
    }
    log_file << message << std::endl;
}
