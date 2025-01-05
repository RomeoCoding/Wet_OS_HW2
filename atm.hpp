#ifndef ATM_HPP
#define ATM_HPP

#include <string>

class ATM {
private:
    std::string atm_id;       // Unique ATM ID
    std::string input_file;   // Path to input file

public:
    ATM(const std::string& id, const std::string& file_path);

    // Accessors
    const std::string& get_id() const { return atm_id; }
    const std::string& get_input_file() const { return input_file; }
};

#endif // ATM_HPP
