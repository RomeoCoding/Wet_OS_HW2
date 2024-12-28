#ifndef BANK_HPP
#define BANK_HPP

#include "account.hpp"
#include <vector>
#include <memory>
#include <string>
#include <pthread.h>
#include <unistd.h> 
#include <deque>
#include "atm.hpp"

class Bank {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    std::shared_ptr<Account> bank_account; // he bank's own account
    std::vector<std::unique_ptr<ATM>> atms; 
    std::vector<pthread_t> atm_threads; 

    //Mutex to synchronize print access if needed (optional)
    mutable pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t snapshot_mutex = PTHREAD_MUTEX_INITIALIZER;
    
    //Managing snapshots for rollback
    std::deque<std::vector<std::shared_ptr<Account>>> account_list_snapshots;
    std::deque<std::shared_ptr<Account>> main_account_snapshot;  // Snapshots of the bank's account

    static const int MAX_SNAPSHOTS = 120;


public:
    //Constructor
    Bank();
    
    //Destructor
    ~Bank();

    //Threads for periodic functions
    void start_account_print_thread();
    void start_withdrawal_thread();
    void start_snapshot_thread();

    //Methods for ATMs
    void initialize_atms(const std::vector<std::string>& file_paths);
    void start_atm_threads();
    void join_atm_threads();

    //Methods
    void create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance);
    bool deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount); 
    bool withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount); 
    bool balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password); 
    void close_account(const std::string& atm_id, const std::string& account_id, const std::string& password); 
    void transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount);
    void rollback(int iterations); //havent implemented this yet

     //Snapshot Methods
    void take_snapshot();  //Take a snapshot of the current state
    void restore_snapshot(size_t iterations);  //Restore to a specific snapshot


    //utility functions
    Account* find_account(const std::string& account_id); 
    void remove_account(Account* account); 
    void print_all_accounts() const;
    std::vector<std::shared_ptr<Account>>& get_accounts();
    std::shared_ptr<Account>& get_bank_account();
    void add_account_to_list(std::shared_ptr<Account> account);
    void remove_account_from_list(Account* account);


    //thread functions
    static void* print_accounts_periodically(void* arg);  
    static void* withdraw_from_accounts(void* arg);
    static void* snapshot_thread(void* arg);

};

#endif // BANK_HPP
