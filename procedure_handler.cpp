#include "procedure_handler.hpp"
#include <sstream>
#include <iostream>

extern std::ofstream log_file;  // For logging
extern pthread_mutex_t log_lock;
extern pthread_mutex_t cerr_lock;
//Utilities
bool is_command_persistent(const std::string& command) {
    return command.find("PERSISTENT") != std::string::npos;
}

std::string remove_persistent_keyword(const std::string& command) {
    return command.substr(0, command.find("PERSISTENT") - 1);
}


// Main command processing function
bool process_command(const std::string& command, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::istringstream stream(command);
    char action;
    bool success;
    stream >> action;

    if (stream.fail()) {
        pthread_mutex_lock(&cerr_lock);
        return false;
    }

    switch (action) {
        case 'O': success = handle_open_account(stream, bank, atm_id, Persistance); break;
        case 'D': success = handle_deposit(stream, bank, atm_id, Persistance); break;
        case 'W': success = handle_withdraw(stream, bank, atm_id, Persistance); break;
        case 'B': success = handle_balance_inquiry(stream, bank, atm_id, Persistance); break;
        case 'T': success = handle_transfer(stream, bank, atm_id, Persistance); break;
        case 'Q': success = handle_close_account(stream, bank, atm_id, Persistance); break;
        case 'R': success = handle_rollback(stream, bank, atm_id, Persistance); break;  
        case 'C': success = handle_close_atm(stream, bank, atm_id, Persistance); break;
        default:
            success = false;
    }
    return success;
}

void execute_command_with_retries(Bank* bank, const std::string& command,const std::string& atm_id, bool is_persistent) {
    bool first_attempt = true;
    do {
        bool success = process_command(command, *bank, atm_id, is_persistent);
        if (success) {
            break;
        }

        if (is_persistent) {
            if (first_attempt) {
                first_attempt = false; 
                is_persistent = false;
            } else {
                break; 
            }
            sleep(1); //sleep for 1 sec/
            usleep(100000); 
        } else {
            break;
        }
    } while (true);
}


void process_atm_commands(Bank* bank, std::ifstream& file, const std::string& atm_id) {
    std::string command;
    usleep(100000);// sleep 100 milisec
    while (std::getline(file, command)) {
        if (command.empty()) continue;

         if(is_command_Vip(command))
        {
            int vip_number = get_Vip_number(command);
            command = remove_Vip_keyword(command);
            bank->Insert_Vip_Command(command,vip_number, atm_id);
        }else{

            bool is_persistent = is_command_persistent(command);
            if (is_persistent) {
                command = remove_persistent_keyword(command);
            }
            execute_command_with_retries(bank, command, atm_id, is_persistent);
            sleep(1); //sleep for 1 sec
            usleep(100000);// sleep 100 milisec      
        }
        //incase the command is vip
    }
}


bool handle_rollback(std::istringstream& stream, Bank& bank, const std::string& atm_id ,bool Persistance) {
    int iterations;
    stream >> iterations;
    if (stream.fail() && !Persistance) {
                return false;
    }
    bank.rollback_add(iterations , atm_id);
    return true; 
}

// Helper function implementations
bool handle_open_account(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string account_id, password;
    double initial_balance;
    stream >> account_id >> password >> initial_balance;

    if (stream.fail() && !Persistance) {
        return false;
    }

    return (bank.create_account(atm_id, account_id, password, initial_balance, Persistance));
}

bool handle_deposit(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string account_id, password;
    double amount;
    stream >> account_id >> password >> amount;

    if (stream.fail() && !Persistance) {
        return false;
    }

    return (bank.deposit(atm_id, account_id, password, amount,Persistance));
}

bool handle_withdraw(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string account_id, password;
    double amount;
    stream >> account_id >> password >> amount;

    if (stream.fail() && !Persistance) {
        return false;
    }

    return(bank.withdraw(atm_id, account_id, password, amount,Persistance));
}

bool handle_balance_inquiry(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string account_id, password;
    stream >> account_id >> password;

    if (stream.fail() && !Persistance) {
        return false;
    }

    return(bank.balance_inquiry(atm_id, account_id, password,Persistance));
}

bool handle_transfer(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string source_account_id, password, target_account_id;
    double amount;
    stream >> source_account_id >> password >> target_account_id >> amount;

    if (stream.fail() && !Persistance) {
        return false;
    }

    return(bank.transfer(atm_id, source_account_id, password, target_account_id, amount, Persistance));
}

bool handle_close_account(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string account_id, password;
    stream >> account_id >> password;

    if (stream.fail() && !Persistance) {
        return false;
    }

    return(bank.close_account(atm_id, account_id, password,Persistance));
}

bool handle_close_atm(std::istringstream& stream, Bank& bank, const std::string& atm_id, bool Persistance) {
    std::string target_atm_id;
    if (stream.fail() && !Persistance) {
        return false;
    }
    stream >> target_atm_id;
    target_atm_id = "ATM" + target_atm_id;

    return bank.close_atm(atm_id, target_atm_id, Persistance);
}



//////////////////////////////////////////
//+++++++thread_pool_commands+++++++++++++
//////////////////////////////////////////

bool is_command_Vip(std::string command){

return command.find("VIP") != std::string::npos;
}


int get_Vip_number(std::string command){
    return std::stoi(command.substr(command.find("VIP=") + 4));
}

std::string remove_Vip_keyword(std::string command){
    return command.substr(0, command.find("VIP") - 1);
}
