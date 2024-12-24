#ifndef STATUS_SNAPSHOT_HPP
#define STATUS_SNAPSHOT_HPP

#include <vector>
#include <memory>
#include "bank.hpp"  

class StatusSnapshot {
public:
    StatusSnapshot(const std::vector<std::shared_ptr<Account>>& accounts);

    //Method to restore the bank's state from this snapshot
    void restore(std::vector<std::shared_ptr<Account>>& accounts) const;

private:
    std::vector<std::shared_ptr<Account>> accounts_snapshot;
};

class StatusManager {
public:
    void take_snapshot(const std::vector<std::shared_ptr<Account>>& accounts);

    //Restore the bank's state from a snapshot
    void restore_snapshot(int iterations, std::vector<std::shared_ptr<Account>>& accounts);

private:
    //A list of snapshots (maximum size 120)
    std::deque<StatusSnapshot> snapshots;

    static const int MAX_SNAPSHOTS = 120;
};

#endif  // STATUS_SNAPSHOT_HPP
