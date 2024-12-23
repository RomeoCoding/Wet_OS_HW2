#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>

class Account {
private:
    std::string id;
    std::string password;
    double balance;

public:
    // Constructor
    Account(const std::string& id, const std::string& password, double initial_balance);

    // Getters
    std::string get_id() const;
    
    // Methods
    double view_balance() const;
    bool authenticate(const std::string& input_password) const;
};

#endif // ACCOUNT_HPP
