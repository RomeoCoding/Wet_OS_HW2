#include "bank.hpp"
#include <iostream>
#include <vector>
#include <string>

pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cerr_lock = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <NumberOfVipThreads> <input_file1> <input_file2> ... <input_fileN>" << std::endl;
        return 1;
    }

    Bank bank;
    std::vector<std::string> input_files;
    int vip_threads_number = atoi(argv[1]);
    for (int i = 2; i < argc; ++i) {
        input_files.push_back(argv[i]);
    }

    bank.initialize_atms(input_files);

    bank.start_atm_threads(vip_threads_number);

    return 0;
}
