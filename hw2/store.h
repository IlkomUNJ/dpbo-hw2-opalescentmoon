#ifndef STORE_H
#define STORE_H

#include <iostream>
#include <cstddef>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cctype>
#include "bank.h"
#include "bankcustomer.h"
#include "seller.h"
#include "buyer.h"

class Store {
private:
    std::vector<Transactiona> transactions;
    
public:
    void addTransaction(bankCustomer& buyer, Seller& seller, Item& item, int quantity) {
        Transactiona trx;
        trx.buyerId = buyer.getId();
        trx.sellerId = seller.getId();
        trx.itemId = item.id;
        trx.itemName = item.name;
        trx.quantityBought = quantity;
        trx.totalPrice = item.price * quantity;
        trx.timestamp = chrono::system_clock::now();

        if (buyer.getBalance() >= trx.totalPrice && item.quantity >= quantity) {
            buyer.withdrawBalance(trx.totalPrice);
            item.quantity -= quantity;
            trx.status = "Completed";
        } else {
            trx.status = "Canceled";
        }

        transactions.push_back(trx);
    }
    
    // List all transactions within k days
    std::vector<Transactiona> getTransactionsWithinDays(int k) {
        using namespace std::chrono;

        std::vector<Transactiona> recentTransactions;
        auto now = system_clock::now();
        auto cutoff = now - hours(24 * k);

        for (const auto& trx : transactions) {
            if (trx.timestamp >= cutoff) {
                recentTransactions.push_back(trx);
            }
        }

        return recentTransactions;
    }

    string processPurchase(bankCustomer& buyer, Item& item) {
    if (buyer.getBalance() >= item.price) {
        buyer.withdrawBalance(item.price);
        if (item.quantity > 0) {
            item.quantity--;
            return "Completed - Item: " + item.name +
                ", Price: " + to_string(item.price) +
                ", Remaining Stock: " + to_string(item.quantity);
        } else {
            return "Paid - Item: " + item.name +
                ", Price: " + to_string(item.price) +
                ", Awaiting restock";
        }
    } else {
        if (buyer.getBalance() < item.price) {
            return "Canceled - " + item.name + " (Insufficient balance)";
        } 
    }
}
    
    // List paid but not completed transactions
    std::vector<Transactiona> getPaidNotCompletedTransactions(const std::vector<Transactiona>& allTransactions) {
        std::vector<Transactiona> result;

        for (const auto& trx : allTransactions) {
            if (trx.status == "Paid") {
                result.push_back(trx);
            }
        }
        return result;
    }

    
    // List top m frequent items
    std::vector<std::pair<int, int>> getMostFrequentItems(int k) {
        map<int, int> frequency;

        for (const auto& trx : transactions) {
            frequency[trx.itemId]++;
        }

        vector<pair<int, int>> sortedItems(frequency.begin(), frequency.end());
        
        sort(sortedItems.begin(), sortedItems.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        if (k > sortedItems.size()) k = sortedItems.size();
        vector<pair<int, int>> topItems(sortedItems.begin(), sortedItems.begin() + k);

        return topItems;

    }
    
    // Get most active buyer (based on transactions per day)
    std::vector<std::pair<int, int>> getMostActiveBuyer() {
        std::map<int, int> buyerFrequency;

        // Count how many times each buyerId appears in completed transactions
        for (const auto& trx : transactions) {
            if (trx.status == "Completed") {
                buyerFrequency[trx.buyerId]++;
            }
        }

        std::vector<std::pair<int, int>> sortedBuyers(buyerFrequency.begin(), buyerFrequency.end());

        // Sort by activity descending
        std::sort(sortedBuyers.begin(), sortedBuyers.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        return sortedBuyers;
    }

    
    // Get most active seller (based on transactions per day)
    std::vector<std::pair<int, int>> getMostActiveSeller() {
        std::map<int, int> sellerFrequency;

        for (const auto& trx : transactions) {
            if (trx.status == "Completed") {
                sellerFrequency[trx.sellerId]++;
            }
        }

        std::vector<std::pair<int, int>> sortedSellers(sellerFrequency.begin(), sellerFrequency.end());

        // Sort by activity descending
        std::sort(sortedSellers.begin(), sortedSellers.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        return sortedSellers;
    }
};

#endif // STORE_H