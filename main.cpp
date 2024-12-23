#include <iostream>
#include <fstream>
#include <string>
#include "bank.hpp"
#include "procedure_handler.hpp"

//for now this works with one thread only

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_command_file>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    std::ifstream command_file(file_path);

    if (!command_file.is_open()) {
        std::cerr << "Failed to open the file: " << file_path << std::endl;
        return 1;
    }

    Bank bank;
    std::string atm_id = "ATM001"; 
    std::string command;

    while (std::getline(command_file, command)) {
        if (!command.empty()) {
            process_command(command, bank, atm_id);
        }
    }

    command_file.close();
    return 0;
}
