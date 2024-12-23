#include <iostream>
#include "bank.hpp"
#include <limits> // For clearing input buffer

void clear_input_buffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void display_menu() {
    std::cout << "\n--- Bank Menu ---\n";
    std::cout << "1. Create Account\n";
    std::cout << "2. Deposit\n";
    std::cout << "3. Withdraw\n";
    std::cout << "4. Balance Inquiry\n";
    std::cout << "5. Transfer\n";
    std::cout << "6. Close Account\n";
    std::cout << "7. View Account Details\n";
    std::cout << "8. Exit\n";
    std::cout << "Choose an option (1-8): ";
}

int main() {
    Bank bank;
    std::string atm_id, account_id, password, target_account_id;
    double amount;
    int choice;

    while (true) {
        display_menu();
        std::cin >> choice;

        switch (choice) {
            case 1: // Create Account
                std::cout << "Enter ATM ID: ";
                clear_input_buffer();
                std::getline(std::cin, atm_id);

                std::cout << "Enter Account ID: ";
                std::getline(std::cin, account_id);

                std::cout << "Enter Password: ";
                std::getline(std::cin, password);

                std::cout << "Enter Initial Balance: ";
                std::cin >> amount;

                bank.create_account(atm_id, account_id, password, amount);
                std::cout << "Account created successfully!\n";
                break;

            case 2: // Deposit
                std::cout << "Enter ATM ID: ";
                clear_input_buffer();
                std::getline(std::cin, atm_id);

                std::cout << "Enter Account ID: ";
                std::getline(std::cin, account_id);

                std::cout << "Enter Password: ";
                std::getline(std::cin, password);

                std::cout << "Enter Deposit Amount: ";
                std::cin >> amount;

                if (bank.deposit(atm_id, account_id, password, amount)) {
                    std::cout << "Deposit successful!\n";
                } else {
                    std::cout << "Deposit failed!\n";
                }
                break;

            case 3: // Withdraw
                std::cout << "Enter ATM ID: ";
                clear_input_buffer();
                std::getline(std::cin, atm_id);

                std::cout << "Enter Account ID: ";
                std::getline(std::cin, account_id);

                std::cout << "Enter Password: ";
                std::getline(std::cin, password);

                std::cout << "Enter Withdrawal Amount: ";
                std::cin >> amount;

                if (bank.withdraw(atm_id, account_id, password, amount)) {
                    std::cout << "Withdrawal successful!\n";
                } else {
                    std::cout << "Withdrawal failed!\n";
                }
                break;

            case 4: // Balance Inquiry
                std::cout << "Enter ATM ID: ";
                clear_input_buffer();
                std::getline(std::cin, atm_id);

                std::cout << "Enter Account ID: ";
                std::getline(std::cin, account_id);

                std::cout << "Enter Password: ";
                std::getline(std::cin, password);

                if (bank.balance_inquiry(atm_id, account_id, password)) {
                    std::cout << "Balance inquiry successful!\n";
                } else {
                    std::cout << "Balance inquiry failed!\n";
                }
                break;

            case 5: // Transfer
                std::cout << "Enter ATM ID: ";
                clear_input_buffer();
                std::getline(std::cin, atm_id);

                std::cout << "Enter Source Account ID: ";
                std::getline(std::cin, account_id);

                std::cout << "Enter Password: ";
                std::getline(std::cin, password);

                std::cout << "Enter Target Account ID: ";
                std::getline(std::cin, target_account_id);

                std::cout << "Enter Transfer Amount: ";
                std::cin >> amount;

                bank.transfer(atm_id, account_id, password, target_account_id, amount);
                break;

            case 6: // Close Account
                std::cout << "Enter ATM ID: ";
                clear_input_buffer();
                std::getline(std::cin, atm_id);

                std::cout << "Enter Account ID to Close: ";
                std::getline(std::cin, account_id);

                std::cout << "Enter Password: ";
                std::getline(std::cin, password);

                bank.close_account(atm_id, account_id, password);
                break;


            case 8: // Exit
                std::cout << "Exiting the system...\n";
                return 0;

            default:
                std::cout << "Invalid choice. Please select a valid option.\n";
        }
    }
}
