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
        // Intentional inversion of locking order compared to the previous example
        unique_lock<mutex> lock2(destination.mtx);  // Lock destination first
        this_thread::sleep_for(chrono::milliseconds(100));  // Introduce delay to increase the chance of deadlock
        unique_lock<mutex> lock1(source.mtx);  // Lock source later

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

    // Simulate concurrent execution of transfers (in a way that can lead to deadlock)
    thread thread1([&] { transfer1.run(); });
    thread thread2([&] { transfer2.run(); });

    thread1.join();
    thread2.join();

    // This point will not be reached in case of a deadlock

    return 0;
}
