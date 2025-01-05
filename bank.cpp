#include "bank.hpp"
#include "error_handler.hpp"
#include <stdexcept>
#include <algorithm>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>  
#include <fstream>
#include "procedure_handler.hpp"
Bank bank;
ErrorHandler error_handler;


//Constructor
Bank::Bank() {
    //Initialize the bank's own account
    bank_account = std::make_shared<Account>("BANK", "bank123", 0.0);

    //Creating threads for periodic functions
    start_account_print_thread();
    start_withdrawal_thread();
    start_snapshot_thread();

}

bool Bank::create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance) {
    // from here it should be locked
    //reason to lock form here is that you dont want more than thread creating mutiple accounts with the same id
    //we also until finishing this we dont want anyone to read account with the id we are creating
    pthread_mutex_lock(accounts_lock);
    if (find_account(id) != nullptr) {
        error_handler.log_error(atm_id, 'O', id); 
        return false; 
    }
    
    auto new_account = std::make_shared<Account>(id, password, initial_balance);
    add_account_to_list(new_account); 
    error_handler.log_success(atm_id, 'O', id, initial_balance, 0.0);
    pthread_mutex_unlock(accounts_lock);
    return true;
    //to here
}
/*
=========================
======== Deposit ========
=========================
*/

bool Bank::deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
   
    Account* acc = find_account(id); 
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'D', id); 
        return false;
    }

    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'D', id); 
        return false;
    }
    
    acc->deposit(amount);
    error_handler.log_success(atm_id, 'D', id, acc->view_balance(), amount); 
    return true; 
}

/*
=========================
======== Withdraw =======
=========================
*/

bool Bank::withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {

    Account* acc = find_account(id); 
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'W', id); 
        return false;
    }

    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'W', id); 
        return false;
    }

    if (acc->view_balance() < amount) {
        error_handler.log_error(atm_id, 'W', id, true); 
        return false;
    }

    acc->withdraw(amount);
    error_handler.log_success(atm_id, 'W', id, acc->view_balance(), amount);

   
    return true; 
}

/*
=========================
======== Balance ========
=========================
*/

bool Bank::balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password) {

    Account* acc = find_account(account_id); 
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'B', account_id); 
        return false;
    }
   

    if (acc->authenticate(password)) {
        double balance = acc->view_balance();
        error_handler.log_success(atm_id, 'B', account_id, balance, 0.0); 
        return true;
    } else {
        error_handler.log_error(atm_id, 'B', account_id);  
        return false;
    }
}

/*
=========================
========= Close =========
=========================
*/
bool Bank::close_account(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    //addtional lock for deleting should be added not sure how to do it
    Account* account = find_account(account_id); 
    if (account == nullptr) {
        error_handler.log_error(atm_id, 'Q', account_id);
        return false;
    }

    if (account->get_password() != password) {
        error_handler.log_error(atm_id, 'Q', account_id); 
        return false;
    }

    double balance = account->view_balance();
    if(remove_account_from_list(account)) 
    {
        error_handler.log_success(atm_id, 'Q', account_id, balance, 0.0);
        return true;
    } 

    return false;
}
/*
=========================
======== Transfer =======
=========================
*/

void Bank::transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount) {
    Account* source_account = find_account(source_account_id); // Use find_account here
    Account* target_account = find_account(target_account_id); // Use find_account here

    if (source_account == nullptr || target_account == nullptr) {
        // If either account does not exist, log an error
        error_handler.log_error(atm_id, 'T', source_account_id); 
        return;
    }

    if (source_account->get_password() != password) {
        // If the password is incorrect, log the error
        error_handler.log_error(atm_id, 'T', source_account_id);
        return;
    }
    
    //here was error where you could view the balance but the balance can change until you withdraw
    if (!source_account->withdraw(amount)) {
        // If there are insufficient funds in the source account, log the error
        error_handler.log_error(atm_id, 'T', source_account_id, true);  // true indicates insufficient funds
        return;
    }

    // Perform the transfer
    target_account->deposit(amount);   

    // Log the successful transfer
    error_handler.log_success(atm_id, 'T', source_account_id, source_account->view_balance(), amount);
}
void Bank::rollback(int iterations){
    restore_snapshot(iterations);
}
/*
=========================
=====Snapshot Methods====
=========================
*/

void Bank::take_snapshot() {
    account_list_snapshots.push_front(accounts);
    main_account_snapshot.push_front(std::make_shared<Account>(*bank_account));

    if (account_list_snapshots.size() > MAX_SNAPSHOTS) {
        account_list_snapshots.pop_back();
    }
    if (main_account_snapshot.size() > MAX_SNAPSHOTS) {
        main_account_snapshot.pop_back();
    }
    if (log_file.is_open()) {
        time_t now = time(0);
        std::string timestamp = ctime(&now);
        timestamp.pop_back();  // Remove the newline character from timestamp
        log_file << "Snapshot taken at: " << timestamp << std::endl;
    }
}


//for this function the caller should make sure threads are locked
bool Bank::restore_snapshot(size_t iterations) {
    if (iterations == 0 || iterations > account_list_snapshots.size()) {
        std::cerr << "Error: Invalid rollback iterations\n";
        return false;
    }

    //pthread_mutex_lock(&accounts_lock);
    accounts = account_list_snapshots[iterations - 1];  
    bank_account = std::make_shared<Account>(*main_account_snapshot[iterations - 1]); 

    std::cout << "Rollback to " << iterations << " iterations ago completed successfully.\n";
    //pthread_mutex_unlock(&accounts_lock);
    return true;
}

//WEE


/*
=========================
====Process Commands=====
=========================
*/


/*
=========================
=======ATM Methods=======
=========================
*/

//Initialise the threads for each ATM
//By calling the constructor of the ATM class, we can pass the ATM ID, the file path, and the Bank pointer
//then we push the atm into the atms vector
void Bank::initialize_atms(const std::vector<std::string>& file_paths,const int vip_threads_number) {
   
    for (size_t i = 0; i < file_paths.size(); ++i) {
        std::string atm_id = "ATM" + std::to_string(i + 1); // Generate ATM IDs
        atms.emplace_back(std::unique_ptr<ATM>(new ATM(atm_id, file_paths[i])));
    }
}


//Join the threads for each ATM
//this function joins the threads for each ATM
void Bank::join_atm_threads() {
    for (pthread_t& thread : atm_threads) {
        pthread_join(thread, nullptr);
    }

    End_Vip_Thread_pool = 1;

    for(pthread_t& thread : vip_threads){
        pthread_join(thread, nullptr);
    }
}

void* Bank::atm_thread_function(void* arg) {
    ATM* atm = static_cast<ATM*>(arg);
    std::ifstream file(atm->get_input_file());

    if (!file.is_open()) {
        log_file_error(atm->get_input_file(), atm->get_id());
        return nullptr;
    }

    process_atm_commands(file, atm->get_id());
    file.close();
    return nullptr;
}

//Join the threads for each ATM
//this function starts the threads for each ATM and
//pushes the threads into the atm_threads vector
void Bank::start_atm_threads(const int vip_threads_number) {
    for (const auto& atm : atms) {
        pthread_t thread_id;

        if (pthread_create(&thread_id, nullptr, atm_thread_function, atm.get()) != 0) {
            std::cerr << "Error creating thread for ATM " << atm->get_id() << std::endl;
        } else {
            atm_threads.push_back(thread_id);
        }
    }

    for(int i=0;i < vip_threads_number; i++){
        pthread_t thread_id;

        if (pthread_create(&thread_id, nullptr, Vip_Worker, nullptr) != 0) {
            std::cerr << "Error creating thread for Vip tasks" << atm->get_id() << std::endl;
        } else {
            vip_threads.push_back(thread_id);
        }
    }

    
    join_atm_threads();
}



/*
=========================
======== Utilities ======
=========================
*/


// Utility function to find an account by its ID
Account* Bank::find_account(const std::string& account_id) {
    //first should check if the account list is locked or not
   Lock_Bank_Account_List_For_Reading_Access();
    for (const auto& acc : accounts) {
        if (acc->get_id() == account_id) {
            return acc.get(); // Return raw pointer to Account
        }
    }
    unLock_Bank_Account_List_For_Reading_Access();
    return nullptr; // Return nullptr if account not found
}

// Remove an account by its pointer
void Bank::remove_account(Account* account) {
    auto it = std::remove_if(accounts.begin(), accounts.end(), 
        [account](const std::shared_ptr<Account>& acc) {
            return acc.get() == account; 
        });

    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
    }
}
void Bank::print_all_accounts() const {
    // Clear the screen
    printf("\033[H\033[J");  // Clears the screen (reset cursor position)
    
    // Move cursor to top-left corner
    printf("\033[H");  // Move cursor to top-left corner of the terminal screen

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now = *std::localtime(&time_t_now);

    // Format the current time
    std::ostringstream timestamp;
    timestamp << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");

    // Print the timestamp and the header
    std::cout << "Current Bank Status" << std::endl;
    std::cout << "Timestamp: " << timestamp.str() << std::endl;

    // Print the account details for each account
    for (const auto& account : accounts) {
        account->print_account_details();  // Print account details
    }

    std::cout << std::endl;  // Add a newline after printing all accounts
}

std::vector<std::shared_ptr<Account>>& Bank::get_accounts() {
    return accounts;
}


std::shared_ptr<Account>& Bank::get_bank_account() {
    return bank_account;
}

//We need add and remove because standard push_back does not
//account for race conditions in multithreaded environments
//so we need to lock the mutex before adding/removing accounts

void Bank::add_account_to_list(std::shared_ptr<Account> account) {
//    pthread_rwlock_wrlock(&accounts_rwlock);
    accounts.push_back(account);
  //  pthread_rwlock_unlock(&accounts_rwlock); 
}

bool Bank::remove_account_from_list(Account* account) {
   // pthread_rwlock_wrlock(&accounts_rwlock); 
    pthread_mutex_lock(&accounts_lock);
    auto it = std::remove_if(accounts.begin(), accounts.end(), 
        [account](const std::shared_ptr<Account>& acc) {
            return acc.get() == account; 
        });

    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
    }else{
        return false;
    }
    pthread_mutex_unlock(&accounts_lock);
    return true;
    //pthread_rwlock_unlock(&accounts_rwlock); 
}
/*
========================
=== Starting Threads ===
========================
*/

//Start a new thread to print account details periodically
void Bank::start_account_print_thread() {
    pthread_t thread_id;

    // Create a new thread that will run the print_accounts_periodically method
    if (pthread_create(&thread_id, nullptr, print_accounts_periodically, (void*)this) != 0) {
        std::cerr << "Error creating thread!" << std::endl;
    }
}

void Bank::start_withdrawal_thread() {
    pthread_t thread_id;

    // Create a new thread for periodic commission withdrawal
    if (pthread_create(&thread_id, nullptr, withdraw_from_accounts, (void*)this) != 0) {
        std::cerr << "Error creating withdrawal thread!" << std::endl;
    }
}



void Bank::start_snapshot_thread() {
    pthread_t thread_id;

    // Create a new thread for periodically taking snapshots
    if (pthread_create(&thread_id, nullptr, snapshot_thread, (void*)this) != 0) {
        std::cerr << "Error creating snapshot thread!" << std::endl;
    }
}


/*
==========================
=== Periodic Functions ===
==========================
*/

/*
==========================
 Printing periodically 
==========================
*/

//Static thread function to print all accounts' details every 5 seconds
void* Bank::print_accounts_periodically(void* arg) {
    Bank* bank = static_cast<Bank*>(arg);  // Cast the void* argument to Bank*

    while (true) {
        sleep(5);
        pthread_mutex_lock(&rollback_lock);
        Lock_Bank_For_Printing();
        bank->print_all_accounts();
        if(rollback_request)
        {
            restore_snapshot(max_rollback);
            rollback_request = false;
            max_rollback = 0;
        }
        pthread_mutex_unlock(&rollback_lock);
        unLock_Bank_For_Printing();
    }

    return nullptr;
}

/*
==========================
 Withdrawing periodically 
==========================
*/

//Static thread function to withdraw commissions from accounts every 3 seconds
void* Bank::withdraw_from_accounts(void* arg) {
    Bank* bank = static_cast<Bank*>(arg); 

    while (true) {
        sleep(3);

        pthread_mutex_lock(&(bank->print_mutex)); 
        
        //Generate a random percentage between 1% and 5%
        double commission_percentage = (rand() % 5 + 1); 
        double commission_fraction = commission_percentage / 100.0;

        for (const auto& account : bank->accounts) {
            double balance = account->view_balance();

            if (balance > 0) {
             
                double commission = balance * commission_fraction;

                account->withdraw(commission);

                // Deposit the commission into the bank's main account
                bank->bank_account->deposit(commission);

                if (log_file.is_open()) {
                    log_file << "Bank: commissions of " 
                             << commission_percentage << "% were charged, bank gained " 
                             << commission << " from account " 
                             << account->get_id() << std::endl;
                } else {
                    std::cerr << "Error: Log file is not open!" << std::endl;
                }
            }
        }

        pthread_mutex_unlock(&(bank->print_mutex)); 
    }

    return nullptr;
}

/*
==========================
 Snapshotting periodically 
==========================
*/
void* Bank::snapshot_thread(void* arg) {
    Bank* bank = static_cast<Bank*>(arg); 

    while (true) {
        sleep(5);  // Periodically take a snapshot (5 seconds interval, for example)

        Lock_Bank_For_Printing();
        // Directly call the Bank's take_snapshot method
        bank->take_snapshot();  // Take snapshot without needing accounts explicitly

        unLock_Bank_For_Printing();
    }

    return nullptr;
}


void Bank::Vip_Worker(){

    while(1){
        Vip_Function command = get_Next_Vip_Command();
        if(End_Vip_Threads == 1)
            break;
        execute_command_with_retries(command.get_function_name(),command.get_atm_id(),false);        
    }
}


 void Bank::Lock_Bank_For_Printing(){

    Lock_Bank_Account_List_For_Reading_Access();
    for (const auto& acc : accounts) {
        
        acc->Lock_Account_For_Reading_Access();
    }
 }


void Bank::unLock_Bank_For_Printing(){

    unLock_Bank_Account_List_For_Reading_Access();
    for (const auto& acc : accounts) {
        
        acc->unLock_Account_For_Reading_Access();
    }
}





void Bank::Lock_Bank_Account_List_For_Reading_Access(){

    pthread_mutex_lock(&accounts_reading_lock);
    access_count ++:
    if(access_count == 1)
    {
        pthread_mutex_lock(&accounts_lock);
    }
    pthread_mutex_unlock(&accounts_reading_lock);

 }

void Bank::unLock_Bank_Account_List_For_Reading_Access(){

    pthread_mutex_lock(&accounts_reading_lock);
    access_count --:
    if(access_count == 0)
    {
        pthread_mutex_unlock(&accounts_lock);
    }
    pthread_mutex_unlock(&accounts_reading_lock);

 }



///////////////////////////////////
//++++++thread_pool_commands++++++
//////////////////////////////////

 void Bank::Insert_Vip_Command(std::string command_name,int vip_number){

    thread_pool.Insert_Vip_Command(command_name,vip_number);
 }
    
Vip_Function Bank::get_Next_Vip_Command(){

    return thread_pool.get_Next_Vip_Command();
}

