#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>
#include <pthread.h>

class Account {
private:
    std::string id;
    std::string password;
    double balance;
    pthread_rwlock_t balance_rwlock; //Read-write lock for protecting the balance
    pthread_mutex_t read_mutex;
    pthread_mutex_t write_mutex;
    int readers_count;

public:
    Account(const std::string& id, const std::string& password, double initial_balance);
    ~Account(); 

    std::string get_id() const;
    std::string get_password() const;

    double view_balance() const;
    bool authenticate(const std::string& input_password) const;
    bool withdraw(double amount);
    void deposit(double amount);
    void print_account_details() const;

    void Lock_Account_For_Reading_Access();
    void unLock_Account_For_Reading_Access();
};

#endif // ACCOUNT_HPP
