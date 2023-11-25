#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

mutex mtx = mutex();  // Mutex for synchronization

class Account {
public:
    int id;
    double balance;

    Account(int _id, double _balance) : id(_id), balance(_balance) {}

    void debit(double amount) {
        balance -= amount;
        cout << "Debit from account " << id << ": " << amount << endl;
    }

    void credit(double amount) {
        balance += amount;
        cout << "Credit to account " << id << ": " << amount << endl;
    }
};

class Transfer {
public:
    Account& source;
    Account& destination;
    double amount;

    Transfer(Account& _source, Account& _destination, double _amount)
        : source(_source), destination(_destination), amount(_amount) {}

    void run() {
        mtx.lock();
        cout << "Thread " << this_thread::get_id() << " locked" << endl;
        source.debit(amount);
        destination.credit(amount);

        std::this_thread::sleep_for(std::chrono::seconds(5));
        // this sleep statement demonstrates how we destroy the parallelism of this code by enclosing the whole body in the mutex lock
        // this is a sequential code now
        // the only way to solve this is to place individual mutex locks for every account, so that both, the removal of parallelism, and a race condition are avoided
        // however, if not done properly, this can lead to a deadlock
        // proper ordering of the mutex locks is important to avoid a deadlock

        mtx.unlock();
    }
};

int main() {
    const int numAccounts = 5;
    const int numTransfers = 5;

    // Create accounts
    vector<Account> accounts;
    for (int i = 0; i < numAccounts; ++i) {
        accounts.emplace_back(i + 1, 100.0 * (i + 1));
    }

    // Create transfers
    vector<Transfer> transfers;
    for (int i = 0; i < numTransfers; ++i) {
        transfers.emplace_back(accounts[i % numAccounts], accounts[(i + 1) % numAccounts], 20.0 * (i + 1));
    }

    // Create and launch threads
    vector<thread> threads;
    for (int i = 0; i < numTransfers; ++i) {
        threads.emplace_back([&transfers, i] { transfers[i].run(); });
    }

    // Wait for all threads to finish
    for (int i = 0; i < numTransfers; ++i) {
        threads[i].join();
    }

    return 0;
}
