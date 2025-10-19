#include <iostream>
#include <vector>

#include "bank.h"
#include "bankcustomer.h"
#include "buyer.h"
#include "seller.h"
#include "store.h"

using namespace std;

enum PRIMARY_PROMPT { MAIN, BUYER, SELLER, EXIT };
enum SUB_LOGIN_PROMPT { VERIFICATION_MENU, LOGIN_MENU };
enum BUYER_PROMPT { MAIN_PROMPT1, TOPUP, SPENDING, STORE_MENU };
enum SELLER_PROMPT { MAIN_PROMPT2, RESTOCK, ACTIVE_CUST, TOP_ITEMS, INIT_ITEMS, CHANGE_PRICE };
enum STORE_PROMPT { MAIN_SHOP, VERIFICATION };

int main() {
    PRIMARY_PROMPT prompt = MAIN;
    SUB_LOGIN_PROMPT sub_prompt = VERIFICATION_MENU;
    BUYER_PROMPT buyer_prompt = MAIN_PROMPT1;

    // --- Core data ---
    vector<bankCustomer> accounts;
    vector<Buyer> buyers;
    vector<Seller> sellers;
    vector<Item> items;

    // --- Demo setup ---
    bankCustomer dummyAcc(0, "default", 0.0);
    bankCustomer buyerAcc(1, "jondo", 10000.0);
    bankCustomer sellerAcc(2, "janedo", 10.0);

    Buyer defaultBuyer("default", dummyAcc);
    Buyer buyer("jondo", buyerAcc);
    buyers.push_back(buyer);

    Seller seller("janedo", sellerAcc);
    sellers.push_back(seller);

    Item item1(1, "janedo", "Apple", 5, 10);
    Item item2(2, "janedo", "Laptop", 800, 2);
    items.push_back(item1);
    items.push_back(item2);

    Store store;

    cout << "\n=== Welcome to the Store System ===\n";

    while (true) {
        switch (prompt) {

            case MAIN: {
                cout << "\nSelect account:\n";
                cout << "1. Buyer\n";
                cout << "2. Seller\n";
                cout << "0. Exit\n";
                int choice;
                cout << "Choice: ";
                cin >> choice;

                if (choice == 0) {
                    prompt = EXIT;
                    continue;
                }

                if (choice == 1) prompt = BUYER;
                else if (choice == 2) prompt = SELLER;
                else cout << "Invalid option.\n";

                sub_prompt = VERIFICATION_MENU;
                break;
            }


            case BUYER: {
                switch (sub_prompt) {

                    case VERIFICATION_MENU: {
                        string name;
                        cout << "\n=== Buyer Login ===\nEnter name: ";
                        cin >> name;

                        bool isFound = false;
                        int buyerIndex = -1;
                        for (int i = 0; i < buyers.size(); ++i) {
                            if (buyers[i].getName() == name) {
                                buyerIndex = i;
                                isFound = true;
                                break;
                            }
                        }

                        if (!isFound) {
                            cout << "No account found. Returning to main menu.\n";
                            prompt = MAIN;
                            sub_prompt = VERIFICATION_MENU;
                        } else {
                            cout << "Login successful. Welcome, " << buyers[buyerIndex].getName() << "!\n";
                            sub_prompt = LOGIN_MENU;
                        }
                        break;
                    }


                    case LOGIN_MENU: {
                        Buyer& currentBuyer = buyers[0];

                        switch (buyer_prompt) {
                            case MAIN_PROMPT1: {
                                cout << "\n--- Buyer Menu ---\n";
                                cout << "1. Top-up Balance\n";
                                cout << "2. View Spending (past k days)\n";
                                cout << "3. Store\n";
                                cout << "0. Back to Main Menu\n";
                                int choice;
                                cout << "Choice: ";
                                cin >> choice;

                                if (choice == 0) {
                                    prompt = MAIN;
                                    sub_prompt = VERIFICATION_MENU;
                                } else {
                                    buyer_prompt = static_cast<BUYER_PROMPT>(choice - 1);
                                }
                                break;
                            }

                            case TOPUP: {
                                double amount;
                                cout << "Enter top-up amount: ";
                                cin >> amount;
                                currentBuyer.topupBalance(amount);
                                cout << "Balance updated.\n";
                                buyer_prompt = MAIN_PROMPT1;
                                break;
                            }

                            case SPENDING: {
                                int k;
                                cout << "Enter days to view spending: ";
                                cin >> k;
                                currentBuyer.listSpending(k);
                                buyer_prompt = MAIN_PROMPT1;
                                break;
                            }

                            case STORE_MENU: {
                                cout << "\n--- Store ---\n";
                                cout << "Available Items:\n";
                                for (const auto& it : items) {
                                    cout << "ID: " << to_string(it.id)
                                         << " | " << it.name
                                         << " | Price: " << to_string(it.price)
                                         << " | Qty: " << to_string(it.quantity) << endl;
                                }

                                int id;
                                cout << "\nEnter Item ID to purchase (0 to go back): ";
                                cin >> id;

                                if (id == 0) {
                                    buyer_prompt = MAIN_PROMPT1;
                                    break;
                                }

                                auto it = find_if(items.begin(), items.end(),
                                                  [id](const Item& i) { return i.id == id; });

                                if (it != items.end()) {
                                    currentBuyer.purchaseItem(*it, store);
                                } else {
                                    cout << "Invalid Item ID.\n";
                                }

                                buyer_prompt = MAIN_PROMPT1;
                                break;
                            }

                            default:
                                buyer_prompt = MAIN_PROMPT1;
                                break;
                        }
                        break;
                    }
                }
                break;
            }


            case SELLER: {
                cout << "\n--- Seller Menu ---\n";
                cout << "1. Restock Items\n";
                cout << "2. View Active Customers\n";
                cout << "3. View Top Items\n";
                cout << "4. Initialize New Item\n";
                cout << "5. Change Item Price\n";
                cout << "0. Back to Main Menu\n";

                int sellerChoice;
                cout << "Choice: ";
                cin >> sellerChoice;

                if (sellerChoice == 0) {
                    prompt = MAIN;
                    break;
                }

                switch (sellerChoice) {
                    case RESTOCK: {
                        int id;
                        cout << "Enter item ID to restock: ";
                        cin >> id;
                        seller.restockItems(id);
                        break;
                    }

                    case ACTIVE_CUST: {
                        auto buyersRank = store.getMostActiveBuyer();
                        cout << "\nMost Active Buyers:\n";
                        for (auto& [id, freq] : buyersRank) {
                            cout << "Buyer ID: " << to_string(id) << " | Purchases: " << to_string(freq) << endl;
                        }
                        break;
                    }

                    case TOP_ITEMS: {
                        auto topItems = store.getMostFrequentItems(5);
                        cout << "\nTop Selling Items:\n";
                        for (auto& [id, freq] : topItems) {
                            cout << "Item ID: " << to_string(id) << " | Sales: " << to_string(freq) << endl;
                        }
                        break;
                    }

                    case INIT_ITEMS: {
                        seller.initNewItems();
                        break;
                    }

                    case CHANGE_PRICE: {
                        int id;
                        cout << "Enter item ID to change price: ";
                        cin >> id;
                        seller.changePrice(id);
                        break;
                    }

                    default:
                        cout << "Invalid seller option.\n";
                        break;
                }
                break;
            }


            case EXIT:
                cout << "Exiting program...\n";
                return 0;

            default:
                cout << "Invalid prompt state.\n";
                prompt = MAIN;
                break;
        }
    }

    return 0;
}
