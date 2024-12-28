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

void Bank::create_account(const std::string& atm_id, const std::string& id, const std::string& password, double initial_balance) {
    if (find_account(id) != nullptr) {
        error_handler.log_error(atm_id, 'O', id); 
        return; 
    }
    
    auto new_account = std::make_shared<Account>(id, password, initial_balance);
    add_account_to_list(new_account); 
    error_handler.log_success(atm_id, 'O', id, initial_balance, 0.0);
}
/*
=========================
======== Deposit ========
=========================
*/

bool Bank::deposit(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
    Account* acc = find_account(id); // Use find_account here
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'D', id); // Use id as std::string here
        return false;
    }

    // Check if the password is correct
    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'D', id); // Use id as std::string here
        return false;
    }

    // Perform the deposit and log success
    acc->deposit(amount);
    error_handler.log_success(atm_id, 'D', id, acc->view_balance(), amount); // No target account for deposit
    return true; // Successful deposit
}

/*
=========================
======== Withdraw =======
=========================
*/

bool Bank::withdraw(const std::string& atm_id, const std::string& id, const std::string& password, double amount) {
    Account* acc = find_account(id); // Use find_account here
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'W', id); // Use id as std::string here
        return false;
    }

    // Check if the password is correct
    if (!acc->authenticate(password)) {
        error_handler.log_error(atm_id, 'W', id); 
        return false;
    }

    // Check if balance is sufficient for withdrawal
    if (acc->view_balance() < amount) {
        error_handler.log_error(atm_id, 'W', id, true); // true indicates insufficient funds
        return false;
    }

    // Perform the withdrawal and log success
    acc->withdraw(amount);
    error_handler.log_success(atm_id, 'W', id, acc->view_balance(), amount); // No target account for withdrawal
    return true; // Successful withdrawal
}

/*
=========================
======== Balance ========
=========================
*/

bool Bank::balance_inquiry(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    Account* acc = find_account(account_id); // Use find_account here
    if (acc == nullptr) {
        error_handler.log_error(atm_id, 'B', account_id); // Use account_id as std::string here
        return false;
    }

    if (acc->authenticate(password)) {
        double balance = acc->view_balance();
        error_handler.log_success(atm_id, 'B', account_id, balance, 0.0);  // Log success for balance inquiry
        return true;
    } else {
        error_handler.log_error(atm_id, 'B', account_id);  // Incorrect password error
        return false;
    }
}

/*
=========================
========= Close =========
=========================
*/
void Bank::close_account(const std::string& atm_id, const std::string& account_id, const std::string& password) {
    Account* account = find_account(account_id); // Use find_account here
    if (account == nullptr) {
        error_handler.log_error(atm_id, 'Q', account_id);
        return;
    }

    if (account->get_password() != password) {
        error_handler.log_error(atm_id, 'Q', account_id); 
        return;
    }

    double balance = account->view_balance();
    remove_account_from_list(account); // Use remove_account_from_list here

    error_handler.log_success(atm_id, 'Q', account_id, balance, 0.0); // No target account for close
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

    if (source_account->view_balance() < amount) {
        // If there are insufficient funds in the source account, log the error
        error_handler.log_error(atm_id, 'T', source_account_id, true);  // true indicates insufficient funds
        return;
    }

    // Perform the transfer
    source_account->withdraw(amount);  
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

void Bank::restore_snapshot(size_t iterations) {
    if (iterations == 0 || iterations > account_list_snapshots.size()) {
        std::cerr << "Error: Invalid rollback iterations\n";
        return;
    }
    accounts = account_list_snapshots[iterations - 1];  
    bank_account = std::make_shared<Account>(*main_account_snapshot[iterations - 1]); 

    std::cout << "Rollback to " << iterations << " iterations ago completed successfully.\n";
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
}


//Join the threads for each ATM
//this function starts the threads for each ATM and
//pushes the threads into the atm_threads vector
void Bank::start_atm_threads() {
    for (const auto& atm : atms) {
        pthread_t thread_id;

        if (pthread_create(&thread_id, nullptr, [](void* arg) -> void* {
                ATM* atm = static_cast<ATM*>(arg);
                std::ifstream file(atm->get_input_file());
                if (!file.is_open()) {
                    std::cerr << "Error: Unable to open file " << atm->get_input_file() 
                              << " for ATM " << atm->get_id() << std::endl;
                    return nullptr; 
                }
                std::string command;
                while (std::getline(file, command)) {
                    process_command(command, *static_cast<Bank*>(arg), atm->get_id());
                }
                file.close();
                return nullptr;  
            }, this) != 0) {  
            std::cerr << "Error creating thread for ATM " << atm->get_id() << std::endl;
        }

        atm_threads.push_back(thread_id);
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
    for (const auto& acc : accounts) {
        if (acc->get_id() == account_id) {
            return acc.get(); // Return raw pointer to Account
        }
    }
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
    accounts.push_back(account);
}

void Bank::remove_account_from_list(Account* account) {
    auto it = std::remove_if(accounts.begin(), accounts.end(), 
        [account](const std::shared_ptr<Account>& acc) {
            return acc.get() == account; 
        });

    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
    }
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
        pthread_mutex_lock(&(bank->print_mutex));
        bank->print_all_accounts();
        pthread_mutex_unlock(&(bank->print_mutex));
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

        for (const auto& account : bank->accounts) {
            double balance = account->view_balance();

            if (balance > 0) {
                //Generate a random percentage between 1% and 5%
                double commission_percentage = (rand() % 5 + 1); 
                double commission_fraction = commission_percentage / 100.0;

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

        pthread_mutex_lock(&(bank->snapshot_mutex)); 

        // Directly call the Bank's take_snapshot method
        bank->take_snapshot();  // Take snapshot without needing accounts explicitly

        pthread_mutex_unlock(&(bank->snapshot_mutex)); 
    }

    return nullptr;
}



