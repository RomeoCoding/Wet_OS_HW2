#include "error_handler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::ofstream log_file("log.txt", std::ios_base::app);
extern pthread_mutex_t log_lock;

void ErrorHandler::log_error(const std::string& atm_id, char action, const std::string& account_id, double amount) {
    std::string message;
    switch (action) {
        case 'O':
            message = "Error " + atm_id + ": Your transaction failed – account with the same id exists";
            break;
        case 'D':
            message = "Error " + atm_id + ": Your transaction failed – password for account id " + account_id + " is incorrect";
            break;
        case 'W':
            if (amount > 0) {
                message = "Error " + atm_id + ": Your transaction failed – account id " + account_id + " balance is lower than " + std::to_string(amount);
            } else {
                message = "Error " + atm_id + ": Your transaction failed – password for account id " + account_id + " is incorrect";
            }
            break;
        case 'Q':
            message = "Error " + atm_id + ": Your transaction failed – password for account id " + account_id + " is incorrect";
            break;
        case 'T':
            if (amount > 0) {
                message = "Error " + atm_id + ": Your transaction failed – account id " + account_id + " balance is lower than " + std::to_string(amount);
            } else {
                message = "Error " + atm_id + ": Your transaction failed – password for account id " + account_id + " is incorrect";
            }
            break;
        case 'C':
            if(atm_already_closed){
                message = "Error " + atm_id + ": Your close operation failed - ATM ID " + target_atm_id + " is already in a closed state";
            }else{
                message = "Error " + atm_id + ": Your transaction failed – ATM_ID " + target_atm_id + " does not exist";
            }
            break;
        case 'I': //Invalid account id
            message = "Error " + atm_id + ": Your transaction failed – account id " + account_id + " does not exist";
            break;
    }

    pthread_mutex_lock(&log_lock);
    log_file << message << std::endl;
    pthread_mutex_unlock(&log_lock);
}

void ErrorHandler::log_success(const std::string& atm_id, char action, const std::string& account_id, double balance, double amount, const std::string& target_account) {
    std::string message;
    switch (action) {
        case 'O':
            message = atm_id + ": New account id is " + account_id + " with password " + account_id + " and initial balance " + std::to_string(balance);
            break;
        case 'D':
            message = atm_id + ": Account " + account_id + " new balance is " + std::to_string(balance) + " after " + std::to_string(amount) + " $ was deposited";
            break;
        case 'W':
            message = atm_id + ": Account " + account_id + " new balance is " + std::to_string(balance) + " after " + std::to_string(amount) + " $ was withdrawn";
            break;
        case 'B':
            message = atm_id + ": Account " + account_id + " balance is " + std::to_string(balance);
            break;
        case 'Q':
            message = atm_id + ": Account " + account_id + " is now closed. Balance was " + std::to_string(balance);
            break;
        case 'T':
            message = atm_id + ": Transfer of " + std::to_string(amount) + " $ from account " + account_id + " to account " + target_account + " completed. New balance for source account: " + std::to_string(balance);
            break;
        case 'R':
            message = atm_id + ": Rollback to " + std::to_string(static_cast<int>(amount)) + " bank iterations ago was completed successfully";
            break;
        default:
            message = "Unknown success action for ATM " + atm_id;
    }

    pthread_mutex_lock(&log_lock);
    log_file << message << std::endl;
    pthread_mutex_unlock(&log_lock);
}

void error_handler::log_atm_error(const std::string& source_atm_id, const std::string& target_atm_id, bool atm_already_closed) {
    std::string message;
    if(atm_already_closed){
        message = "Error " + source_atm_id + ": Your close operation failed - ATM ID " + target_atm_id + " is already in a closed state";
    }else{
        message = "Error " + source_atm_id + ": Your transaction failed – ATM_ID " + target_atm_id + " does not exist";
    }

    pthread_mutex_lock(&log_lock);
    log_file << message << std::endl;
    pthread_mutex_unlock(&log_lock);
}
void error_handler::log_atm_success(const std::string& source_atm_id, const std::string& target_atm_id) {
    std::string message = "Bank: " +source_atm_id + " closed " + target_atm_id + " successfully";
    pthread_mutex_lock(&log_lock);
    log_file << message << std::endl;
    pthread_mutex_unlock(&log_lock);
}

ErrorHandler::~ErrorHandler() {
    pthread_mutex_destroy(&log_lock);
}
