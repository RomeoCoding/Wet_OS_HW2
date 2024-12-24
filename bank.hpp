#ifndef BANK_HPP
#define BANK_HPP

#include "account.hpp"
#include <vector>
#include <memory>
#include <string>
#include <pthread.h>
#include <unistd.h>  
#include "status_snapshot.hpp"

class Bank {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    std::shared_ptr<Account> bank_account; // he bank's own account

    // Mutex to synchronize print access if needed (optional)
    mutable pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t snapshot_mutex = PTHREAD_MUTEX_INITIALIZER;
    //Object to manage status snapshots
    StatusManager status_manager; 


public:
    //Constructor
    Bank();
    
    //Threads for printing account details
    void start_account_print_thread();
    void start_withdrawal_thread();
    void start_snapshot_thread();

    //Methods
    void create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance);
    bool deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount); 
    bool withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount); 
    bool balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password); 
    void close_account(const std::string& atm_id, const std::string& account_id, const std::string& password); 
    void transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount);
    void rollback(int iterations); //havent implemented this yet

    //utility functions
    Account* find_account(const std::string& account_id); 
    void remove_account(Account* account); 
    void print_all_accounts() const;

    //thread functions
    static void* print_accounts_periodically(void* arg);  
    static void* withdraw_from_accounts(void* arg);
    static void* snapshot_thread(void* arg);

};

#endif // BANK_HPP
