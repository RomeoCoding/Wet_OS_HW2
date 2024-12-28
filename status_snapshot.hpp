/*#ifndef STATUS_SNAPSHOT_HPP
#define STATUS_SNAPSHOT_HPP

#include <deque>
#include <vector>
#include <memory>
#include "account.hpp"  
#include "bank.hpp"
extern Bank bank;

class StatusSnapshot {
public:
    StatusSnapshot(Bank& bank);

    void restore(Bank& bank);

private:
    std::vector<std::shared_ptr<Account>> accounts_snapshot;
    std::shared_ptr<Account> main_account_snapshot;  
};
class StatusManager {
public:
    void take_snapshot();
    void restore_snapshot(size_t iterations);

private:
    //A list of snapshots (maximum size 120)
    std::deque<StatusSnapshot> snapshots;

    static const int MAX_SNAPSHOTS = 120;
};

#endif  // STATUS_SNAPSHOT_HPP
