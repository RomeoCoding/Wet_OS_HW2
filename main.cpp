#include <iostream>
#include "bank.hpp"

int main() {
    Bank bank;

    // Create some accounts
    bank.create_account("ATM001", "123", "password123", 1000.0);
    bank.create_account("ATM001", "456", "password456", 500.0);

    // Perform some transactions
    if (bank.deposit("ATM001", "123", "password123", 200.0)) {
        std::cout << "Deposit successful!" << std::endl;
    }

    if (bank.withdraw("ATM001", "456", "password456", 100.0)) {
        std::cout << "Withdrawal successful!" << std::endl;
    }

    // Balance inquiry
    if (bank.balance_inquiry("ATM001", "123", "password123")) {
        std::cout << "Balance inquiry successful!" << std::endl;
    }

    // Transfer money between accounts
    bank.transfer("ATM001", "123", "password123", "456", 50.0);

    // Close an account
    bank.close_account("ATM001", "456", "password456");

    return 0;
}
