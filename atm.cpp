#include "atm.hpp"

ATM::ATM(const std::string& id, const std::string& file_path)
    : atm_id(id), input_file(file_path) {}
