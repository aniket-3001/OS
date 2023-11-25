#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

class Account {
public:
    int id;
    double balance;
    mutex mtx;  // Mutex for synchronization

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
        Account& a1 = (source.id < destination.id) ? source : destination; // a1 mei account1
        Account& a2 = (source.id < destination.id) ? destination : source; // a2 mei account2

        unique_lock<mutex> lock1(a1.mtx);  // Lock a1
        this_thread::sleep_for(chrono::milliseconds(100));  // Introduce delay to increase the chance of deadlock
        unique_lock<mutex> lock2(a2.mtx);  // Lock a2

        source.debit(amount);
        destination.credit(amount);

        lock1.unlock();
        lock2.unlock();
    }
};

int main() {
    Account account1(1, 100.0);
    Account account2(2, 50.0);

    Transfer transfer1(account1, account2, 20.0);
    Transfer transfer2(account2, account1, 10.0);

    // Simulate concurrent execution of transfers
    thread thread1([&] { transfer1.run(); });
    thread thread2([&] { transfer2.run(); });

    thread1.join();
    thread2.join();

    // Print final balances
    cout << "Final balance of account 1: " << account1.balance << endl;
    cout << "Final balance of account 2: " << account2.balance << endl;

    return 0;
}
