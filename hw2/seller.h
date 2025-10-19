#ifndef SELLER_H
#define SELLER_H
#pragma once

#include "buyer.h"
#include "bankcustomer.h"

#include <algorithm>
#include <map>
#include <fstream>
#include <iostream>
#include <cstddef>
#include <string>
#include <chrono>
#include <sstream>
#include <vector>
#include <cctype>

using namespace std;

class bankCustomer;

struct Item {
    int id;
    string name;
    double price;
    int quantity;
    string sellerName;

    Item(int id, const string& sellerName, const std::string& name, double price, int quantity)
        : id(id), sellerName(sellerName), name(name), price(price), quantity(quantity) {}
};

struct Transactiona {
    int buyerId;
    int sellerId;
    int itemId;
    string itemName;
    int quantityBought;
    double totalPrice;
    chrono::system_clock::time_point timestamp;
    string status;
};

class Seller : public Buyer {
private:
    static int nextSellerId;
    int id;
    string name;
    bankCustomer& account;  // Association with BankCustomer
    vector<string> History;
    vector<Spending> spending;
    vector<Item> items;
    vector<Transactiona> transactionHistory;

public:
    Seller(const string &sellerName, bankCustomer& acc)
        : Buyer(sellerName, acc), account(acc) {}

    int getId() const { 
        return this->id; 
    }

    string getName() const { 
        return this->name; 
    }

    void initNewItems() {
        static int nextId = 1;
        string itemName;
        double itemPrice;
        int itemQty;
        string sellerName;

        cout << "Set item name: " << endl;
        cin >> itemName;
        cout << "Set item price (must be a number): " << endl;
        cin >> itemPrice;
        cout << "Set item quantity (must be a number): " << endl;
        cin >> itemQty;
        cout << "Set seller name: " << endl;
        cin >> sellerName;

        Item newItem{ nextId++, sellerName, itemName, itemPrice, itemQty };
        items.push_back(newItem);

        cout << "Item added successfully!\n";
    }

    void changePrice(int id) {
        double newPrice;

        cout << "Set new item price: " << endl;
        cin >> newPrice;

        for (auto& item : items) {
            if (item.id == id) {
                item.price = newPrice;
                cout << "Item price updated for " << item.name << endl;
                return;
            }
        }
        cout << "Item not found";
    }

    void restockItems(int id) {
        int restockQty;

        cout << "Add item stock: " << endl;
        cin >> restockQty;

        for (auto& item : items) {
            if (item.id == id) {
                item.quantity += restockQty;
                cout << "Item quantity updated for " << item.name << endl;
                return;
            }
        }
        cout << "Item not found";
    }

    void recordPurchase(const Item& item, int quantity, vector<Transactiona>& history, string status) {
        Transactiona trx;
        trx.buyerId = this->getId();
        trx.sellerId = this->getId();
        trx.itemId = item.id;
        trx.itemName = item.name;
        trx.quantityBought = quantity;
        trx.totalPrice = item.price * quantity;
        trx.timestamp = chrono::system_clock::now();
        trx.status = status;

        history.push_back(trx);
    }

    void fulfillTransaction(Transactiona& trx, Item& item) {
        if (item.quantity > 0 && trx.status == "Pending") {
            item.quantity--;
            trx.status = "Completed";
        }
    }

    void topItems(vector<Transactiona>& trxhistory) {
        map<int, int> itemSales;

        for (const auto& item : trxhistory) {
            itemSales[item.itemId] += item.quantityBought;
        }

        vector<pair<int, int>> sortedItems(itemSales.begin(), itemSales.end());

        sort(sortedItems.begin(), sortedItems.end(), [](const auto& a, const auto& b) {
            return a.second > b.second; // descending by quantity
        });

        cout << "\nTop Selling Items:\n";
        for (const auto& [itemId, qty] : sortedItems) {
            cout << "Item ID: " << to_string(itemId) << " | Quantity Sold: " << to_string(qty) << '\n';
        }
    }

    void mostActiveCusts(vector<Transactiona>& trxhistory) {
        map<int, int> sales;

        for (const auto& buyer : trxhistory) {
            sales[buyer.buyerId] += buyer.quantityBought;
        }

        vector<pair<int, int>> sortedBuyers(sales.begin(), sales.end());

        sort(sortedBuyers.begin(), sortedBuyers.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        cout << "\nMost Active Customers:\n";
        for (const auto& [buyerId, qty] : sortedBuyers) {
            cout << "Buyer ID: " << to_string(buyerId) << " | Items Bought: " << to_string(qty) << '\n';
        }
    }

    // void serialize(ofstream& out) const {
    //     out << id << '\n' << name << '\n';

    //     out << items.size() << '\n';
    //     for (const auto& item : items) {
    //         out << item.id << '\n' << item.sellerName << '\n' << item.name << '\n'
    //             << item.price << '\n' << item.quantity << '\n';
    //     }

    //     out << transactionHistory.size() << '\n';
    //     for (const auto& trx : transactionHistory) {
    //         out << trx.buyerId << '\n' << trx.sellerId << '\n' << trx.itemId << '\n'
    //             << trx.itemName << '\n' << trx.quantityBought << '\n'
    //             << trx.totalPrice << '\n'
    //             << chrono::duration_cast<chrono::seconds>(trx.timestamp.time_since_epoch()).count() << '\n'
    //             << trx.status << '\n';
    //     }
    // }

    // static Seller deserialize(ifstream& in, bankCustomer& acc) {
    //     int id;
    //     string name;
    //     in >> id;
    //     in.ignore();
    //     getline(in, name);

    //     Seller seller(name, acc);
    //     seller.id = id;

    //     size_t itemCount;
    //     in >> itemCount;
    //     in.ignore();
    //     for (size_t i = 0; i < itemCount; ++i) {
    //         int itemId, quantity;
    //         string sellerName, itemName;
    //         double price;

    //         in >> itemId;
    //         in.ignore();
    //         getline(in, sellerName);
    //         getline(in, itemName);
    //         in >> price >> quantity;
    //         in.ignore();

    //         seller.items.emplace_back(itemId, sellerName, itemName, price, quantity);
    //     }

    //     size_t trxCount;
    //     in >> trxCount;
    //     in.ignore();
    //     for (size_t i = 0; i < trxCount; ++i) {
    //         Transactiona trx;
    //         long long timestampSeconds;

    //         in >> trx.buyerId >> trx.sellerId >> trx.itemId;
    //         in.ignore();
    //         getline(in, trx.itemName);
    //         in >> trx.quantityBought >> trx.totalPrice >> timestampSeconds;
    //         in.ignore();
    //         getline(in, trx.status);

    //         trx.timestamp = chrono::system_clock::time_point(chrono::seconds(timestampSeconds));
    //         seller.transactionHistory.push_back(trx);
    //     }

    //     return seller;
    // }

};

#endif //SELLER_H
