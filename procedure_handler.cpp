#include "procedure_handler.hpp"
#include <sstream>
#include <iostream>

extern std::ofstream log_file;  // For logging

//Utilities
bool is_command_persistent(const std::string& command) {
    return command.find("PERSISTENT") != std::string::npos;
}

std::string remove_persistent_keyword(const std::string& command) {
    return command.substr(0, command.find("PERSISTENT") - 1);
}


// Main command processing function
void process_command(const std::string& command, Bank& bank, const std::string& atm_id) {
    std::istringstream stream(command);
    char action;
    stream >> action;

    if (stream.fail()) {
        std::cerr << "Invalid command format: " << command << std::endl;
        return;
    }

    switch (action) {
        case 'O': handle_open_account(stream, bank, atm_id); break;
        case 'D': handle_deposit(stream, bank, atm_id); break;
        case 'W': handle_withdraw(stream, bank, atm_id); break;
        case 'B': handle_balance_inquiry(stream, bank, atm_id); break;
        case 'T': handle_transfer(stream, bank, atm_id); break;
        case 'Q': handle_close_account(stream, bank, atm_id); break;
        case 'R': handle_rollback(stream, bank, atm_id); break;  
        default:
            std::cerr << "Unknown action: " << action << " in command: " << command << std::endl;
    }
}

void execute_command_with_retries(const std::string& command,const std::string& atm_id, bool is_persistent) {
    bool first_attempt = true;

    do {
        bool success = process_command(command, *this, atm->get_id());
        if (success) {
            break;
        }

        if (is_persistent) {
            if (first_attempt) {
                first_attempt = false; 
            } else {
                log_command_failure(command, atm->get_id());
                break; 
            }
            usleep(100000); 
        } else {
            log_command_failure(command, atm->get_id());
            break;
        }
    } while (true);
}


void process_atm_commands(std::ifstream& file, const std::string& atm_id) {
    std::string command;
    while (std::getline(file, command)) {
        if (command.empty()) continue;

        bool is_persistent = is_command_persistent(command);
        if (is_persistent) {
            command = remove_persistent_keyword(command);
        }

        execute_command_with_retries(command, atm_id, is_persistent);
    }
}


void handle_rollback(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    int iterations;
    stream >> iterations;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for rollback" << std::endl;
        return;
    }

    // Call restore_snapshot to perform the rollback using the correct parameters
    bank.restore_snapshot(iterations);  // No need to pass accounts or main_account

    /*// Log the rollback action
    if (log_file.is_open()) {
        log_file << atm_id << ": Rollback to " << iterations << " bank iterations ago was completed successfully\n";
    }
    */
}

// Helper function implementations
void handle_open_account(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    std::string account_id, password;
    double initial_balance;
    stream >> account_id >> password >> initial_balance;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for account creation" << std::endl;
        return;
    }

    bank.create_account(atm_id, account_id, password, initial_balance);
}

void handle_deposit(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    std::string account_id, password;
    double amount;
    stream >> account_id >> password >> amount;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for deposit" << std::endl;
        return;
    }

    bank.deposit(atm_id, account_id, password, amount);
}

void handle_withdraw(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    std::string account_id, password;
    double amount;
    stream >> account_id >> password >> amount;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for withdrawal" << std::endl;
        return;
    }

    bank.withdraw(atm_id, account_id, password, amount);
}

void handle_balance_inquiry(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    std::string account_id, password;
    stream >> account_id >> password;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for balance inquiry" << std::endl;
        return;
    }

    bank.balance_inquiry(atm_id, account_id, password);
}

void handle_transfer(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    std::string source_account_id, password, target_account_id;
    double amount;
    stream >> source_account_id >> password >> target_account_id >> amount;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for transfer" << std::endl;
        return;
    }

    bank.transfer(atm_id, source_account_id, password, target_account_id, amount);
}

void handle_close_account(std::istringstream& stream, Bank& bank, const std::string& atm_id) {
    std::string account_id, password;
    stream >> account_id >> password;

    if (stream.fail()) {
        std::cerr << "Invalid parameters for account closure" << std::endl;
        return;
    }

    bank.close_account(atm_id, account_id, password);
}
