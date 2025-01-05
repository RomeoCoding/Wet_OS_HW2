#include "account.hpp"
#include <iostream>

Account::Account(const std::string& id, const std::string& password, double initial_balance)
    : id(id), password(password), balance(initial_balance) {
  //  pthread_rwlock_init(&balance_rwlock, nullptr);  // Initialize the read-write lock
  readers_count = 0;
  read_mutex = PTHREAD_MUTEX_INITIALIZER;
  write_mutex = PTHREAD_MUTEX_INITIALIZER;
}


Account::~Account() {
    pthread_mutex_destroy(&read_mutex);
    pthread_mutex_destroy(&read_mutex);
}

std::string Account::get_id() const {
    return id;
}

std::string Account::get_password() const {
    return password;
}

double Account::view_balance() {
   // pthread_rwlock_rdlock(&balance_rwlock);  // Acquire read lock
  
    Lock_Account_For_Reading_Access();
   
    double current_balance = balance;
    current_balance =current_balance +1 -1;
    unLock_Account_For_Reading_Access();
    
    //  pthread_rwlock_unlock(&balance_rwlock);  // Release lock
    return current_balance;
}

bool Account::authenticate(const std::string& input_password) const {
    return password == input_password;
}

void Account::deposit(double amount) {
  //  pthread_rwlock_wrlock(&balance_rwlock);  // Acquire write lock
    pthread_mutex_lock(&write_mutex);  //lock for writing 
    balance += amount;
    pthread_mutex_unlock(&write_mutex); //unlock fow writing
    //note there is no need to lock reading becuase the next reader will stop at writing lock
  //  pthread_rwlock_unlock(&balance_rwlock);  // Release lock
}

bool Account::withdraw(double amount) {
    pthread_mutex_lock(&write_mutex);  //lock for writing 
    if( balance < amount ){
        return false;
    }
    balance -= amount;
    pthread_mutex_unlock(&write_mutex); //unlock fow writing
    return true;
    //note there is no need to lock reading becuase the next reader will stop at writing lock
}

void Account::print_account_details() {
   
    Lock_Account_For_Reading_Access();
   // pthread_rwlock_rdlock(&balance_rwlock);  // Acquire read lock
    std::cout << "Account " << id << ": Balance - " << balance 
              << " $, Account Password - " << password << std::endl;

    //pthread_rwlock_unlock(&balance_rwlock);  // Release lock
    unLock_Account_For_Reading_Access();
}



//the idea for read lock is that we lock to add reader count (so that we count correctly
//afterwards we lock the write to make sure no one write while reading is happening
// at the ned we lock read to decrease the reader_count and unlock write if no one is reading



void Account::Lock_Account_For_Reading_Access() {
    
    pthread_mutex_lock(&read_mutex);
    readers_count++;
    if( readers_count == 1)
    {
        pthread_mutex_lock(&write_mutex);
    }
    pthread_mutex_unlock(&read_mutex);

}

void Account::unLock_Account_For_Reading_Access(){

    pthread_mutex_lock(&read_mutex);
    readers_count--;
    if( readers_count == 0)
    {
        pthread_mutex_unlock(&write_mutex);
    }
    pthread_mutex_unlock(&read_mutex);

}