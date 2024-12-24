#include "status_snapshot.hpp"
#include <iostream>
#include <deque>
#include <memory>
#include "account.hpp"

StatusSnapshot::StatusSnapshot(const std::vector<std::shared_ptr<Account>>& accounts) {
    for (const auto& account : accounts) {
        accounts_snapshot.push_back(std::make_shared<Account>(*account));
    }
}

void StatusSnapshot::restore(std::vector<std::shared_ptr<Account>>& accounts) const {
    accounts.clear();
    for (const auto& account : accounts_snapshot) {
        accounts.push_back(std::make_shared<Account>(*account));
    }
}

void StatusManager::take_snapshot(const std::vector<std::shared_ptr<Account>>& accounts) {
    snapshots.push_front(StatusSnapshot(accounts));
    if (snapshots.size() > MAX_SNAPSHOTS) {
        snapshots.pop_back();
    }
}

void StatusManager::restore_snapshot(size_t iterations, std::vector<std::shared_ptr<Account>>& accounts) {
    if (iterations == 0 || iterations > snapshots.size()) {
        std::cerr << "Error: Invalid rollback iterations\n";
        return;
    }
    StatusSnapshot snapshot = snapshots[iterations - 1];
    snapshot.restore(accounts);
    std::cout << "Rollback to " << iterations << " iterations ago completed successfully.\n";
}

