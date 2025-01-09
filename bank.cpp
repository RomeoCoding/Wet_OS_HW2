#include "bank.hpp"
#include "error_handler.hpp"
#include "logger.hpp"
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
//Bank bank;
ErrorHandler error_handler;
extern pthread_mutex_t log_lock;
//Constructor
Bank::Bank() {
    //Initialize the bank's own account
    bank_account = std::make_shared<Account>("BANK", "bank123", 0.0);

    //Creating threads for periodic functions
    start_account_print_thread();
    start_withdrawal_thread();
    //start_snapshot_thread();

}

Bank::~Bank() {
    pthread_mutex_destroy(&accounts_lock);
    pthread_mutex_destroy(&accounts_reading_lock);
    pthread_mutex_destroy(&rollback_lock);
    pthread_mutex_destroy(&threads_counter_lock);
    pthread_cond_destroy(&threads_cond_var);
    pthread_cond_destroy(&snapshot_cond_var);
    
}

bool Bank::create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance, bool Persistance) {
    // from here it should be locked
    //reason to lock form here is that you dont want more than thread creating mutiple accounts with the same id
    //we also until finishing this we dont want anyone to read account with the id we are creating
  
    if (find_account(id) != nullptr) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'O', id); 
        return false; 
    }
    
    auto new_account = std::make_shared<Account>(id, password, initial_balance);
    add_account_to_list(new_account); 
    error_handler.log_open_account_success(atm_id , id , password,initial_balance);

    return true;
    //to here
}
/*
=========================
======== Deposit ========
=========================
*/

bool Bank::deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount, bool Persistance) {
   
    Account* acc = find_account(id); 
    if (acc == nullptr) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'I', id); 
        return false;
    }

    if (!acc->authenticate(password)) {
        if(!Persistance)
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

bool Bank::withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount, bool Persistance) {

    Account* acc = find_account(id); 
    if (acc == nullptr) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'I', id); 
        return false;
    }

    if (!acc->authenticate(password)) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'W', id); 
        return false;
    }

    if (acc->view_balance() < amount) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'W', id, amount); 
        return false;
    }

    if(acc->withdraw(amount) != -1)
    {
    error_handler.log_success(atm_id, 'W', id, acc->view_balance(), amount);
    return true;
    }
    if(!Persistance)
        error_handler.log_error(atm_id, 'W', id,amount); 
    return false;
}

/*
=========================
======== Balance ========
=========================
*/

bool Bank::balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password,bool Persistance) {

    Account* acc = find_account(account_id); 
    if (acc == nullptr) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'I', account_id); 
        return false;
    }
   

    if (acc->authenticate(password)) {
        double balance = acc->view_balance();
        error_handler.log_success(atm_id, 'B', account_id, balance, 0.0); 
        return true;
    } else {
        if(!Persistance)
            error_handler.log_error(atm_id, 'D', account_id);  
        return false;
    }
}

/*
=========================
========= Close =========
=========================
*/
bool Bank::close_account(const std::string& atm_id, const std::string& account_id, const std::string& password, bool Persistance) {
    Account* account = find_account(account_id); 
    if (account == nullptr) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'I', account_id); 
        return false;
    }

    if (account->get_password() != password) {
        if(!Persistance)
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

bool Bank::transfer(const std::string& atm_id, const std::string& source_account_id, const std::string& password, const std::string& target_account_id, double amount, bool Persistance) {
    Account* source_account = find_account(source_account_id); 
    Account* target_account = find_account(target_account_id);

    if (source_account == nullptr) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'I', source_account_id); 
        return false;
    }

    if(target_account == nullptr)
    {
        if(!Persistance)
            error_handler.log_error(atm_id, 'I', target_account_id); 
        return false;
    }
    if (source_account->get_password() != password) {
        if(!Persistance)
            error_handler.log_error(atm_id, 'T', source_account_id);
        return false;
    }


    
    //here was error where you could view the balance but the balance can change until you withdraw
    if (source_account->withdraw(amount) == -1) {
        // If there are insufficient funds in the source account, log the error
        if(!Persistance)
            error_handler.log_error(atm_id, 'T', source_account_id, amount);  // true indicates insufficient funds
        return false;
    }

    // Perform the transfer
    target_account->deposit(amount);   

    // Log the successful transfer
    error_handler.log_success(atm_id, 'T', source_account_id, source_account->view_balance(), amount);
    return true;
}

void Bank::rollback_add(const int num, std::string atm_id){

    pthread_mutex_lock(&rollback_lock);
    if( num > max_rollback )
    {
        max_rollback = num;
        max_rollback_atm_id = atm_id;
    }
    rollback_request = true;
    pthread_mutex_unlock(&rollback_lock);
}
//void Bank::rollback(int iterations){
  //  restore_snapshot(iterations, "GG");
//}

/*
=========================
======== Close ATM ======
=========================
*/
bool Bank::close_atm(const std::string& atm_id,const  std::string& target_atm_id, bool Persistance) { //the letter is C
    if(find_atm(target_atm_id))
    {
        if(close_target_atm(target_atm_id))
        {
            error_handler.log_atm_success(atm_id,target_atm_id);
            return true;
        }
        if(!Persistance)
            error_handler.log_atm_error(atm_id,target_atm_id,true);
        return false;
    }
    if(!Persistance)
        error_handler.log_atm_error(atm_id,target_atm_id,false);
    return false;
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
}


//for this function the caller should make sure threads are locked
bool Bank::restore_snapshot(size_t iterations, std::string atm_id) {
    if (iterations == 0 || iterations > account_list_snapshots.size()) {
        return false;
    }

    //pthread_mutex_lock(&accounts_lock);
    accounts = account_list_snapshots[iterations - 1];  
    bank_account = std::make_shared<Account>(*main_account_snapshot[iterations - 1]); 

    error_handler.log_success(atm_id,'R',"BAW",0.0,iterations);
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
void Bank::initialize_atms(const std::vector<std::string>& file_paths) {
   
    for (size_t i = 0; i < file_paths.size(); ++i) {
        std::string atm_id = "ATM" + std::to_string(i + 1); 
        atms.emplace_back(std::unique_ptr<ATM>(new ATM(atm_id, file_paths[i]))); 

    }
}


//Join the threads for each ATM
//this function joins the threads for each ATM
void Bank::join_atm_threads() {
    
    pthread_mutex_lock(&threads_counter_lock);
    while(threads_counter > 0)
    {
        pthread_cond_wait(&threads_cond_var,&threads_counter_lock);
    }
    pthread_mutex_unlock(&threads_counter_lock);
    End_All_Threads = 1;
    thread_pool.Set_End_Vip_Threads();

    for(pthread_t& thread : vip_threads){
        pthread_join(thread, nullptr);
    }
}

void* Bank::atm_thread_function(void* arg) {
    auto thread_data = static_cast<std::shared_ptr<Thread_Data>*>(arg);
 //   Thread_Data* thread_data = static_cast<Thread_Data*>(arg);
    ATM* atm=(*thread_data)->get_atm();
    Bank* bank=(*thread_data)->get_Bank();
    std::ifstream file(atm->get_input_file());
    if (!file.is_open()) {
      return nullptr;
    }
    process_atm_commands(bank, file, atm->get_id());
    file.close();
    pthread_cond_wait(&(bank->snapshot_cond_var),&(bank->threads_counter_lock));
    bank->threads_counter --;
    pthread_cond_signal(&(bank->threads_cond_var));
    pthread_mutex_unlock(&(bank->threads_counter_lock));
    
    return nullptr;
}

//Join the threads for each ATM
//this function starts the threads for each ATM and
//pushes the threads into the atm_threads vector
void Bank::start_atm_threads(const int vip_threads_number) {
    for (const auto& atm : atms) {
        pthread_t thread_id;
        auto thread_data=std::make_shared<Thread_Data>(atm.get(),this);
        if (pthread_create(&thread_id, nullptr, atm_thread_function,static_cast<void*>(new std::shared_ptr<Thread_Data>(thread_data))) != 0) {
        error_handler.handle_system_error("pthread_create");
        } else {
            atm_threads.push_back(thread_id);
            pthread_mutex_lock(&threads_counter_lock);
            threads_counter++;
            pthread_mutex_unlock(&threads_counter_lock);
        }
    }

    for(int i=0;i < vip_threads_number; i++){
        pthread_t thread_id;

        if (pthread_create(&thread_id, nullptr, Vip_Worker, (void*)this) != 0) {
        error_handler.handle_system_error("pthread_create");
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
            unLock_Bank_Account_List_For_Reading_Access();
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
    printf("\033[H\033[J");  
    printf("\033[H");  
    
    std::cout << "Current Bank Status" << std::endl;

    for (const auto& account : accounts) {
        account->print_account_details();  //Print account details
    }

    std::cout << std::endl; 
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
    pthread_mutex_lock(&accounts_lock);
    accounts.push_back(account);
    pthread_mutex_unlock(&accounts_lock);
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
        pthread_mutex_unlock(&accounts_lock);
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
        error_handler.handle_system_error("pthread_create");
    }
}

void Bank::start_withdrawal_thread() {
    pthread_t thread_id;

    // Create a new thread for periodic commission withdrawal
    if (pthread_create(&thread_id, nullptr, withdraw_from_accounts, (void*)this) != 0) {
        error_handler.handle_system_error("pthread_create");
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

void* Bank::print_accounts_periodically(void* arg) {
    Bank* bank = static_cast<Bank*>(arg);  
    while (true) {
         //Sleep for 500,000 microseconds (0.5 seconds)
        usleep(500000);
        if(bank->End_All_Threads == 1)
            break;
        pthread_mutex_lock(&(bank->rollback_lock));
        bank->Lock_Bank_For_Printing();
        bank->take_snapshot();  
        bank->print_all_accounts();
        pthread_cond_broadcast(&(bank->snapshot_cond_var));
        if(bank->rollback_request)
        {
            bank->restore_snapshot(bank->max_rollback,bank->max_rollback_atm_id);
            bank->rollback_request = false;
            bank->max_rollback = 0;
            bank->max_rollback_atm_id = "";
        }
        pthread_mutex_unlock(&(bank->rollback_lock));
        bank->unLock_Bank_For_Printing();

        if(bank->End_All_Threads == 1)
            break;

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
        if(bank->End_All_Threads == 1)
            break;

        //Generate a random percentage between 1% and 5%
        double commission_percentage = (rand() % 5 + 1); 
        double commission_fraction = commission_percentage / 100.0;

        for (const auto& account : bank->accounts) {
            double commission = account->withdraw(commission_fraction,1);
            if (commission != -1) {
             
                //Deposit the commission into the bank's main account
                bank->bank_account->deposit(commission);

                if (log_file.is_open()) {
                    pthread_mutex_lock(&log_lock); 
                    log_file << "Bank: commissions of " 
                             << commission_percentage << "% were charged, bank gained " 
                             << commission << " from account " 
                             <<account->get_id() << std::endl;
                     pthread_mutex_unlock(&log_lock);
                } else {
                    error_handler.handle_system_error("open file");
                }
            }
        }

       // pthread_mutex_unlock(&(bank->print_mutex)); 
    }

    return nullptr;
}

/*
==========================
 Snapshotting periodically 
==========================
*/
void* Bank::Vip_Worker(void* arg){
    Bank* bank = static_cast<Bank*>(arg);
    while(1){
        Vip_Function command = bank->get_Next_Vip_Command();
        if(bank->End_All_Threads == 1)
            break;
        execute_command_with_retries(bank, command.get_function_name(),command.get_atm_id(),false);  
    }
    return nullptr;
}


bool Bank::find_atm(const std::string& atm_id)
{
    for (const auto& atm : atms) {
        if(atm->get_id() == atm_id)
            return true;
    }
    return false;   
}

bool Bank::close_target_atm(const std::string& atm_id)
{
    for (const auto& atm : atms) {
       if(atm->get_id() == atm_id)
           return atm->End_ATM();
        }
        return false;   
}
 void Bank::Lock_Bank_For_Printing(){

    Lock_Bank_Account_List_For_Reading_Access();
    for (const auto& acc : accounts) {
        
        acc->Lock_Account_For_Reading_Access();
    }
    bank_account->Lock_Account_For_Reading_Access();
 }


void Bank::unLock_Bank_For_Printing(){

    unLock_Bank_Account_List_For_Reading_Access();
    for (const auto& acc : accounts) {
        
        acc->unLock_Account_For_Reading_Access();
    }
    bank_account->unLock_Account_For_Reading_Access();
}





void Bank::Lock_Bank_Account_List_For_Reading_Access(){

    pthread_mutex_lock(&accounts_reading_lock);
    access_count ++;
    if(access_count == 1)
    {
        pthread_mutex_lock(&accounts_lock);
    }
    pthread_mutex_unlock(&accounts_reading_lock);

 }

void Bank::unLock_Bank_Account_List_For_Reading_Access(){

    pthread_mutex_lock(&accounts_reading_lock);
    access_count --;
    if(access_count == 0)
    {
        pthread_mutex_unlock(&accounts_lock);
    }
    pthread_mutex_unlock(&accounts_reading_lock);

 }



///////////////////////////////////
//++++++thread_pool_commands++++++
//////////////////////////////////

 void Bank::Insert_Vip_Command(std::string command_name,int vip_number,std::string atm_id){

    thread_pool.Insert_Vip_Command(command_name,vip_number,atm_id);
 }
    
Vip_Function Bank::get_Next_Vip_Command(){

    return thread_pool.get_Next_Vip_Command();
}

