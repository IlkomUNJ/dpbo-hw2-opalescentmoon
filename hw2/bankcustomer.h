#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <iostream>
#include <cstddef>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include "bank.h"

using namespace std;

struct CashFlow {
    string category; 
    double totalAmount;
    vector<Transactions> transactions;

    CashFlow(string ctg) 
    : category(ctg), totalAmount(0) {}

    void addTransaction(const Transactions& t) {
        transactions.push_back(t);
        totalAmount += t.amount;
    }

    void sumCashFlow(const vector<Transactions> trxs, int days) {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto cutoff = now - hours(24 * days);

        double inflow = 0.0;
        double outflow = 0.0;

        for (const auto& trx : trxs) {
            if (trx.date >= cutoff) {
                if (trx.amount > 0) inflow += trx.amount;
                else outflow += trx.amount;
            }
        }

    }

};

class bankCustomer {
private:
    int id;
    string name;
    double balance;
    chrono::system_clock::time_point lastActivity;

public:
    bankCustomer(int id, const string& name, double balance) 
    : id(id), name(name), balance(balance), lastActivity(chrono::system_clock::now()) {
        this->id = id;
        this->name = name;
        this->balance = balance;
        this->lastActivity = lastActivity;
    }

    bankCustomer(int id, const std::string& name, double balance,
                std::chrono::system_clock::time_point activityTime)
    : id(id), name(name), balance(balance), lastActivity(activityTime) {}


    void serialize(std::ofstream& out) const {
        out << to_string(id) << '\n' << name << '\n' << to_string(balance) << '\n'
            << std::chrono::duration_cast<std::chrono::seconds>(lastActivity.time_since_epoch()).count() << '\n';
    }

    static bankCustomer deserialize(std::ifstream& in) {
        int id;
        std::string name;
        double balance;
        long long activitySeconds;
        in >> id;
        in.ignore();
        std::getline(in, name);
        in >> balance >> activitySeconds;
        auto activityTime = std::chrono::system_clock::time_point(std::chrono::seconds(activitySeconds));
        return bankCustomer(id, name, balance, activityTime);
    }


    int getId() const {
        return id;
    }
    string getName() const {
        return name;
    }
    double getBalance() const {
        return balance;
    }
    chrono::system_clock::time_point getLastActivity() const {
        return lastActivity;
    }

    void printInfo() const {
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Balance: " << balance << endl;
    }
    void addBalance(double amount) {
        cout << "Add balance amount: ";
        cin >> amount;
        balance += amount;
        updateActivity();
    }
    bool withdrawBalance(double amount) {
        cout << "Withdraw balance amount: ";
        cin >> amount;

        if (amount > balance) {
            cout << "Insufficient funds.\n";
            return false;
        }

        balance -= amount;
        updateActivity();
        return true;
    }
    void updateActivity() {
        lastActivity = chrono::system_clock::now();
    }
};

#endif // BANK_CUSTOMER_H