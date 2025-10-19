#include "bankcustomer.h"

#include <iostream>
#include <cstddef>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

struct Transactions {
    chrono::system_clock::time_point date;
    string description;
    double amount;

    Transactions(chrono::system_clock::time_point date, const string& description, double amount)
    : date(date), description(description), amount(amount) {}

    static chrono::system_clock::time_point parseDate(const string& dta) {
        tm dateStruct = {};
        istringstream ss(dta);
        if (!(ss >> get_time(&dateStruct, "%Y-%m-%d"))) {
            throw runtime_error("Invalid date format");
        }
        return chrono::system_clock::from_time_t(mktime(&dateStruct));
    }

    static string formatDate(const chrono::system_clock::time_point& timePoint) {
        time_t time = chrono::system_clock::to_time_t(timePoint);
        tm* dateStruct = localtime(&time);
        ostringstream ss;
        ss << put_time(dateStruct, "%Y-%m-%d");
        return ss.str(); 
    }
};

ostream& operator<<(ostream& os, const Transactions& b) {
    time_t t = chrono::system_clock::to_time_t(b.date);
    os << "Date: " << ctime(&t)
       << "Description: " << b.description << "\n"
       << "Amount: " << fixed << setprecision(2) << b.amount << "\n"; 
    return os;
}

ostream& operator<<(ostream& os, const Bank& b) {
    os << "Bank Name: " << b.getName() << "\n"
       << "Total Customers: " << to_string(b.getCustomerCount()); 
    return os;
}

class Bank {
private:
    string name;
    vector<bankCustomer> Accounts;
    int customerCount;
    vector<Transactions> transactions;

public:
    Bank(const string& name) 
    : name(name), customerCount(0) {}

    string getName() const {
        return this->name;
    }

    int getCustomerCount() const {
        return this->customerCount;
    }

    void listDormantAcc(int dormantD) {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto cutoff = now - hours(24 * dormantD);

        for (const auto& cstmr : Accounts) {
            if (cstmr.getLastActivity() < cutoff) {
                std::cout << "Dormant: " << cstmr.getName() << "(Balance: " << cstmr.getBalance() << ")\n";
            }
        }
    }

    void addCustomers(const bankCustomer& cst) {
        Accounts.push_back(cst);
        customerCount++;
    }

    void addTransaction(const chrono::system_clock::time_point date, string description, double amount) {
        transactions.push_back(Transactions(date, description, amount));
    }

    void listAllCust() const {
        for (const auto& cust : Accounts) {
            std::cout << cust << std::endl;
        }
    }

    void listTransactions(int kDays) {
        using namespace std::chrono;

        auto now = system_clock::now();
        auto cutoff = now - hours(24 * kDays);

        for (const auto& txs : transactions) {
            if (txs.date >= cutoff && txs.amount < 0) {
                std::cout << txs << std::endl;
            }
        }
    }

    void rankTransactions() {
        sort(transactions.begin(), transactions.end(), [](const Transactions& a, const Transactions& b) {
            return a.amount > b.amount;
        });
        for (const auto& trx : transactions) {
            cout << trx << endl;
        }
    }
};
