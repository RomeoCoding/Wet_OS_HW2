#include "account.hpp"
#include <iostream>

Account::Account(const std::string& id, const std::string& password, double initial_balance)
    : id(id), password(password), balance(initial_balance) {
    pthread_rwlock_init(&balance_rwlock, nullptr);  // Initialize the read-write lock
}

Account::~Account() {
    pthread_rwlock_destroy(&balance_rwlock);  // Destroy the read-write lock
}

std::string Account::get_id() const {
    return id;
}

std::string Account::get_password() const {
    return password;
}

double Account::view_balance() const {
    pthread_rwlock_rdlock(&balance_rwlock);  // Acquire read lock
    double current_balance = balance;
    pthread_rwlock_unlock(&balance_rwlock);  // Release lock
    return current_balance;
}

bool Account::authenticate(const std::string& input_password) const {
    return password == input_password;
}

void Account::deposit(double amount) {
    pthread_rwlock_wrlock(&balance_rwlock);  // Acquire write lock
    balance += amount;
    pthread_rwlock_unlock(&balance_rwlock);  // Release lock
}

void Account::withdraw(double amount) {
    pthread_rwlock_wrlock(&balance_rwlock);  // Acquire write lock
    balance -= amount;
    pthread_rwlock_unlock(&balance_rwlock);  // Release lock
}

void Account::print_account_details() const {
    pthread_rwlock_rdlock(&balance_rwlock);  // Acquire read lock
    std::cout << "Account " << id << ": Balance - " << balance 
              << " $, Account Password - " << password << std::endl;
    pthread_rwlock_unlock(&balance_rwlock);  // Release lock
}
