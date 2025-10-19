#ifndef BUYER_H
#define BUYER_H

#include <iostream>
#include <cstddef>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cctype>
#include <fstream>
#include "bankcustomer.h"
#include "store.h"

using namespace std;

struct Spending {
    chrono::system_clock::time_point date;
    const string description;
    double amount;

    Spending(chrono::system_clock::time_point date, const string& description, double amount)
    : date(date), description(description), amount(amount) {}

    static chrono::system_clock::time_point parseDate(const std::string& dta) {
        tm dateStruct = {};
        istringstream ss(dta);
        ss >> get_time(&dateStruct, "%Y-%m-%d");
        return chrono::system_clock::from_time_t(mktime(&dateStruct));
    }

    static string formatDate(const chrono::system_clock::time_point& timePoint) {
        time_t time = chrono::system_clock::to_time_t(timePoint);
        tm* dateStruct = localtime(&time);
        ostringstream ss;
        ss << put_time(dateStruct, "%Y-%m-%d");
        return ss.str(); 
    };
};

ostream& operator<<(ostream& os, const Spending& b) {
    time_t t = chrono::system_clock::to_time_t(b.date);
    os << "Date: " << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S") << "\n"
       << "Description: " << b.description << "\n"
       << "Amount: " << b.amount << "\n";
    return os;
}

ostream& operator<<(ostream& os, const Item& item) {
    os << "Item ID: " << to_string(item.id) << " | Name: " << item.name
       << " | Price: " << to_string(item.price) << " | Quantity: " << to_string(item.quantity)
       << " | Seller: " << item.sellerName;
    return os;
}

class Buyer {
private:
    static int nextId;
    int id;
    string name;
    bankCustomer &account; // Association with BankCustomer
    vector<string> History;
    vector<Spending> spending;

    int loginState;

public:    
    Buyer(const string& name, bankCustomer &account)
        : id(nextId++), name(name), account(account) {}

    int getId() const { return this->id; }
    string getName() const { return this->name; }

    static bankCustomer createAccount() { 
        string inputName;
        double inputBalance;

        cout << "Enter Name: ";
        cin >> inputName;
        cout << "Enter initial balance: ";
        cin >> inputBalance;

        return bankCustomer(nextId++, inputName, inputBalance); 
    }
    
    bankCustomer& getAccount() { return this->account; }

    void topupBalance(double amount) { account.addBalance(amount); }

    void withdrawBalance(double amount) { account.withdrawBalance(amount); }

    void setName(const string& newName) { name = newName; }

    void addTransaction(chrono::system_clock::time_point date, const string& description, double amount) {
        spending.push_back(Spending(date, description, amount));
    }

    void purchaseItem(Item& item, Store& store) {
        char input;
        cout << "Do you want to purchase the following item: " << item << "? (Y / any char)";
        cin >> input;
        input = toupper(input);

        if (input == 'Y') {
            string result = store.processPurchase(account, item);
            History.push_back(result);
            cout << result << endl;
        }
    }

    void listSpending(int kDays) {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto cutoff = now - hours(24 * kDays);

        for (const auto& txs : spending) {
            if (txs.date >= cutoff && txs.amount < 0) {
                std::cout << txs << std::endl;
            }
        }
    }

    int getLoginState(){return loginState;};

    // void serialize(ofstream& out) const {
    //     out << std::to_string(id) << '\n' << name << '\n';

    //     out << spending.size() << '\n';
    //     for (const auto& s : spending) {
    //         out << chrono::duration_cast<chrono::seconds>(s.date.time_since_epoch()).count() << '\n'
    //             << s.description << '\n'
    //             << s.amount << '\n';
    //     }

    //     out << History.size() << '\n';
    //     for (const auto& h : History) {
    //         out << h << '\n';
    //     }
    // }

    // static Buyer deserialize(ifstream& in, bankCustomer& acc) {
    //     int id;
    //     string name;
    //     in >> id;
    //     in.ignore();
    //     getline(in, name);

    //     Buyer buyer(name, acc);
    //     buyer.id = id;

    //     size_t spendCount;
    //     in >> spendCount;
    //     in.ignore();
    //     for (size_t i = 0; i < spendCount; ++i) {
    //         long long seconds;
    //         string description;
    //         double amount;

    //         in >> seconds;
    //         in.ignore();
    //         getline(in, description);
    //         in >> amount;
    //         in.ignore();

    //         chrono::system_clock::time_point date = chrono::system_clock::time_point(chrono::seconds(seconds));
    //         buyer.spending.emplace_back(date, description, amount);
    //     }

    //     size_t histCount;
    //     in >> histCount;
    //     in.ignore();
    //     for (size_t i = 0; i < histCount; ++i) {
    //         string line;
    //         getline(in, line);
    //         buyer.History.push_back(line);
    //     }

    //     return buyer;
    // }
};

#endif // BUYER_H
