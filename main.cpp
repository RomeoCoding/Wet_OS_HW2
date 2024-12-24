#include <iostream>
#include <fstream>
#include <string>
#include "bank.hpp"
#include "procedure_handler.hpp"
#include <pthread.h>

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

    // Process commands from the file
    std::string command;
    while (std::getline(command_file, command)) {
        if (!command.empty()) {
            process_command(command, bank, atm_id);
        }
    }

    // Close the command file after reading
    command_file.close();

    // Keep the main function alive to allow the print thread to keep running
    while (true) {
        // Optionally, you can add a small delay here or handle signals to gracefully exit
        sleep(1);  // Sleep 1 second to prevent the main function from exiting
    }

    return 0;
}
