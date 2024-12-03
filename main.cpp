#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>

using namespace std;

struct Account {
    int bankNumber;
    string name;
    string pin;
    double balance;
};

class BankManagementSystem {
private:
    map<int, Account> accounts;
    string fileName;

    void loadAccounts() {
        ifstream file(fileName);
        if (!file) {
            cerr << "Error: Could not open file " << fileName << ". Starting fresh.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            Account account;

            try {
                getline(ss, token, ':');
                account.bankNumber = stoi(token);

                getline(ss, token, ':');
                account.name = token;

                getline(ss, token, ':');
                account.pin = token;

                getline(ss, token, ':');
                account.balance = stod(token);

                accounts[account.bankNumber] = account;
            } catch (const exception &e) {
                cerr << "Error reading account data: " << e.what() << ". Skipping entry.\n";
            }
        }

        file.close();
    }

    void saveAccounts() {
        ofstream file(fileName, ios::trunc);
        if (!file) {
            cerr << "Error: Could not open file " << fileName << " for writing.\n";
            return;
        }

        for (const auto &entry : accounts) {
            const Account &account = entry.second;
            file << account.bankNumber << ':'
                 << account.name << ':'
                 << account.pin << ':'
                 << fixed << setprecision(2) << account.balance << '\n';
        }

        file.close();
    }

    int generateBankNumber() {
        return accounts.empty() ? 1 : accounts.rbegin()->first + 1;
    }

public:
    BankManagementSystem(const string &fileName) : fileName(fileName) {
        loadAccounts();
    }

    ~BankManagementSystem() {
        saveAccounts();
    }

    void createAccount(const string &name, const string &pin) {
        if (name.empty() || pin.empty() || pin.length() < 4) {
            cout << "Invalid name or PIN. Please try again.\n";
            return;
        }

        int newBankNumber = generateBankNumber();
        accounts[newBankNumber] = {newBankNumber, name, pin, 0.0};
        cout << "Account created successfully. Your Bank Number is " << newBankNumber << ".\n";
    }

    Account *signIn(int bankNumber, const string &pin) {
        auto it = accounts.find(bankNumber);
        if (it != accounts.end() && it->second.pin == pin) {
            return &it->second;
        }
        return nullptr;
    }

    void modifyAccount(Account &account, const string &newName, const string &newPin) {
        if (newName.empty() || newPin.empty() || newPin.length() < 4) {
            cout << "Invalid name or PIN. Please try again.\n";
            return;
        }

        account.name = newName;
        account.pin = newPin;
        cout << "Account updated successfully.\n";
    }

    void deposit(Account &account, double amount) {
        if (amount <= 0) {
            cout << "Invalid deposit amount.\n";
            return;
        }
        account.balance += amount;
        cout << "Deposit successful. New Balance: " << fixed << setprecision(2) << account.balance << "\n";
    }

    void withdraw(Account &account, double amount) {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount.\n";
            return;
        }
        if (amount > account.balance) {
            cout << "Insufficient balance.\n";
            return;
        }
        account.balance -= amount;
        cout << "Withdrawal successful. New Balance: " << fixed << setprecision(2) << account.balance << "\n";
    }

    void balanceSummary(const Account &account) {
        cout << "Bank Number: " << account.bankNumber << "\n"
             << "Name: " << account.name << "\n"
             << "Balance: " << fixed << setprecision(2) << account.balance << "\n";
    }
};

int main() {
    BankManagementSystem bms("Accounts.txt");

    while (true) {
        cout << "\n--- Bank Management System ---\n"
             << "1. Sign In\n"
             << "2. Create Account\n"
             << "3. Exit\n"
             << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            int bankNumber;
            string pin;
            cout << "Enter Bank Number: ";
            cin >> bankNumber;
            cout << "Enter PIN: ";
            cin >> pin;

            Account *account = bms.signIn(bankNumber, pin);
            if (account) {
                cout << "Signed in successfully.\n";
                while (true) {
                    cout << "\n--- Account Menu ---\n"
                         << "1. Modify Account\n"
                         << "2. Deposit\n"
                         << "3. Withdraw\n"
                         << "4. Balance Summary\n"
                         << "5. Log Out\n"
                         << "Enter your choice: ";
                    int accountChoice;
                    cin >> accountChoice;

                    if (accountChoice == 1) {
                        string newName, newPin;
                        cout << "Enter new name: ";
                        cin >> newName;
                        cout << "Enter new PIN: ";
                        cin >> newPin;
                        bms.modifyAccount(*account, newName, newPin);
                    } else if (accountChoice == 2) {
                        double amount;
                        cout << "Enter amount to deposit: ";
                        cin >> amount;
                        bms.deposit(*account, amount);
                    } else if (accountChoice == 3) {
                        double amount;
                        cout << "Enter amount to withdraw: ";
                        cin >> amount;
                        bms.withdraw(*account, amount);
                    } else if (accountChoice == 4) {
                        bms.balanceSummary(*account);
                    } else if (accountChoice == 5) {
                        cout << "Logged out successfully.\n";
                        break;
                    } else {
                        cout << "Invalid choice.\n";
                    }
                }
            } else {
                cout << "Invalid Bank Number or PIN.\n";
            }
        } else if (choice == 2) {
            string name, pin;
            cout << "Enter your name: ";
            cin >> name;
            cout << "Enter a PIN: ";
            cin >> pin;
            bms.createAccount(name, pin);
        } else if (choice == 3) {
            cout << "Exiting program.\n";
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
