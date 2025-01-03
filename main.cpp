#include "bank.hpp"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file1> <input_file2> ... <input_fileN>" << std::endl;
        return 1;
    }

    Bank bank;
    std::vector<std::string> input_files;

    for (int i = 1; i < argc; ++i) {
        input_files.push_back(argv[i]);
    }

    bank.initialize_atms(input_files);

    bank.start_atm_threads();

    return 0;
}
