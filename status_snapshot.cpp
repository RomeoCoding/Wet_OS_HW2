/*#include "status_snapshot.hpp"
#include <iostream>
#include <deque>
#include <memory>
#include "account.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>

extern std::ofstream log_file;

StatusSnapshot::StatusSnapshot(Bank& bank) {
    const auto& accounts = bank.get_accounts();
    const auto& main_account = bank.get_bank_account();

    for (const auto& account : accounts) {
        accounts_snapshot.push_back(std::make_shared<Account>(*account));
    }

    main_account_snapshot = std::make_shared<Account>(*main_account);
}

void StatusSnapshot::restore(Bank& bank) {
    auto& accounts = bank.get_accounts();
    auto& main_account = bank.get_bank_account();

    accounts.clear();
    for (const auto& account : accounts_snapshot) {
        accounts.push_back(std::make_shared<Account>(*account));
    }

    main_account = std::make_shared<Account>(*main_account_snapshot);
}

void StatusManager::take_snapshot() {
    snapshots.push_front(StatusSnapshot(bank));
    if (snapshots.size() > MAX_SNAPSHOTS) {
        snapshots.pop_back();
    }

    if (log_file.is_open()) {
        time_t now = time(0);
        std::string timestamp = ctime(&now);
        timestamp.pop_back();  // Removes the newline character
        log_file << "Snapshot taken at: " << timestamp << std::endl;
    }
}

void StatusManager::restore_snapshot(size_t iterations) {
    if (iterations == 0 || iterations > snapshots.size()) {
        std::cerr << "Error: Invalid rollback iterations\n";
        return;
    }

    StatusSnapshot& snapshot = snapshots[iterations - 1];
    snapshot.restore(bank);

    std::cout << "Rollback to " << iterations << " iterations ago completed successfully.\n";
}
//WEE