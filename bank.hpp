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
#include "vip_thread_pool.hpp"
#include "thread_data.hpp"


class Bank {
private:

    std::vector<std::shared_ptr<Account>> accounts;
    std::shared_ptr<Account> bank_account; // he bank's own account
    std::vector<std::unique_ptr<ATM>> atms; 
    std::vector<pthread_t> atm_threads; 
    std::vector<pthread_t> vip_threads;

    //Mutexes for thread synchronization
    //mutable pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_t snapshot_mutex = PTHREAD_MUTEX_INITIALIZER;

   // pthread_rwlock_t accounts_rwlock = PTHREAD_RWLOCK_INITIALIZER;

    pthread_mutex_t accounts_lock = PTHREAD_MUTEX_INITIALIZER;  //locking the accout list to make or prevent change (add/delete Account)
    pthread_mutex_t accounts_reading_lock = PTHREAD_MUTEX_INITIALIZER; //this is to tell that bank when to lock accounts

    int access_count = 0;

    //rollback lock

    pthread_mutex_t rollback_lock = PTHREAD_MUTEX_INITIALIZER;
    std::string max_rollback_atm_id = "";
    int max_rollback = 0;
    bool rollback_request = false;

    //Managing snapshots for rollback

    std::deque<std::vector<std::shared_ptr<Account>>> account_list_snapshots;
    std::deque<std::shared_ptr<Account>> main_account_snapshot;  // Snapshots of the bank's account
    Vip_Thread_Pool thread_pool;

    //Thread-tracking threads and ending them

    pthread_mutex_t threads_counter_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  threads_cond_var = PTHREAD_COND_INITIALIZER;
    pthread_cond_t snapshot_cond_var = PTHREAD_COND_INITIALIZER;
    int threads_counter = 0;


    int End_All_Threads = 0;    //if end_vip_threads = 1 it ends all vip_threads this is done at the end
   
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
    void initialize_vip_threads(const int vip_threads_number);

    void start_atm_threads(const int vip_threads_number);
    void join_atm_threads();
    static void* atm_thread_function(void* arg);
    static void* Vip_Worker(void* arg);

    //Methods
    bool create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance ,bool Persistance);
    bool deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount, bool Persistance); 
    bool withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount, bool Persistance); 
    bool balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password, bool Persistance); 
    bool close_account(const std::string& atm_id, const std::string& account_id, const std::string& password, bool Persistance); 
    bool transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount, bool Persistance);
    void rollback_add(const int num,std::string atm_id);
    void rollback(int iterations); 
    

     //Snapshot Methods
    void take_snapshot();  //Take a snapshot of the current state
    bool restore_snapshot(size_t iterations,std::string atm_id);  //Restore to a specific snapshot


    //utility functions
    Account* find_account(const std::string& account_id); 
    void remove_account(Account* account); 
    void print_all_accounts() const;
    std::vector<std::shared_ptr<Account>>& get_accounts();
    std::shared_ptr<Account>& get_bank_account();
    void add_account_to_list(std::shared_ptr<Account> account);
    bool remove_account_from_list(Account* account);


    //thread functions
    static void* print_accounts_periodically(void* arg);  
    static void* withdraw_from_accounts(void* arg);
    static void* snapshot_thread(void* arg);    

    bool find_atm(const std::string& atm_id);
    bool close_target_atm(const std::string& atm_id);
    bool close_atm(const std::string& atm_id,const  std::string& target_atm_id,bool Persistance);
    //locking functions
    void Lock_Bank_For_Printing();
    void unLock_Bank_For_Printing();

    void Lock_Bank_For_Writing();
    void unLock_Bank_For_Writing();

    void Lock_Bank_Account_List_For_Reading_Access();
    void unLock_Bank_Account_List_For_Reading_Access();

    
    
    //thread pool functions

    void Insert_Vip_Command(std::string command_name,int vip_number,std::string atm_id);
    Vip_Function get_Next_Vip_Command();
    


    
};

#endif // BANK_HPP
