#include "bank.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//Define return values
#define FAIL_START 0
#define SUCCESS_END 1

//Initialise Global Mutexes
pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cerr_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Bank error: illegal arguments" << std::endl;
        return FAIL_START;
    }

    Bank bank;
    std::vector<std::string> input_files;
    int vip_threads_number = atoi(argv[1]);
    //Validate file paths through a "wrapper"
    for (int i = 2; i < argc; ++i) {
        std::ifstream file(argv[i]);
        if (!file) {
            std::cerr << "Bank error: illegal arguments" << std::endl;
            return FAIL_START;
        }
        input_files.push_back(argv[i]);
    }


    bank.initialize_atms(input_files);
    bank.start_atm_threads(vip_threads_number);

    return SUCCESS_END;
}