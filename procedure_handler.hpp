#ifndef PROCEDURE_HANDLER_HPP
#define PROCEDURE_HANDLER_HPP

#include <string>
#include "bank.hpp"

//Main command processing function
bool process_command(const std::string& command, Bank& bank, const std::string& atm_id);

//Command handlers
void execute_command_with_retries(const std::string& command,const std::string& atm_id, bool is_persistent);
void process_atm_commands(std::ifstream& file, const std::string& atm_id);


//Helper function implementations
bool handle_open_account(std::istringstream& stream, Bank& bank, const std::string& atm_id);
bool handle_deposit(std::istringstream& stream, Bank& bank, const std::string& atm_id);
bool handle_withdraw(std::istringstream& stream, Bank& bank, const std::string& atm_id);
bool handle_balance_inquiry(std::istringstream& stream, Bank& bank, const std::string& atm_id);
bool handle_transfer(std::istringstream& stream, Bank& bank, const std::string& atm_id);
bool handle_close_account(std::istringstream& stream, Bank& bank, const std::string& atm_id);
bool handle_rollback(std::istringstream& stream, Bank& bank, const std::string& atm_id);

//Utilities
bool is_command_persistent(const std::string& command);
std::string remove_persistent_keyword(const std::string& command);

//commands for thread_pool purpose
bool is_command_Vip(std::string command);
int get_Vip_number(std::string command);
std::String remove_Vip_keyword(std::string command);


#endif // PROCEDURE_HANDLER_HPP
