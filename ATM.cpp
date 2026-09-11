#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class Card {
private:
    string cardNumber;
    int pin;

public:
    Card(string cardNumber, int pin) {
        this->cardNumber = cardNumber;
        this->pin = pin;
    }

    bool validatePin(int enteredPin) {
        return pin == enteredPin;
    }

    string getCardNumber() {
        return cardNumber;
    }
};

class Account {
private:
    string accountNumber;
    double balance;

public:
    Account(string accountNumber, double balance) {
        this->accountNumber = accountNumber;
        this->balance = balance;
    }

    double getBalance() {
        return balance;
    }

    bool withdraw(double amount) {
        if (amount > balance)
            return false;

        balance -= amount;
        return true;
    }
};

class CashDispenser {
private:
    unordered_map<int, int> cash;

public:
    CashDispenser() {
        cash[500] = 10;
        cash[200] = 10;
        cash[100] = 10;
    }

    bool canDispense(int amount) {
        int total = 0;

        for (auto &x : cash)
            total += x.first * x.second;

        return total >= amount;
    }

    void dispense(int amount) {
        for (auto &x : cash) {
            int denomination = x.first;
            int count = min(amount / denomination, x.second);

            amount -= count * denomination;
            x.second -= count;

            if (amount == 0)
                break;
        }

        cout << "Cash dispensed successfully\n";
    }
};

class ATM {
private:
    Card* card;
    Account* account;
    CashDispenser dispenser;

public:
    ATM() {
        card = nullptr;
        account = nullptr;
    }

    void insertCard(Card* c) {
        if (card != nullptr) {
            cout << "A card is already inserted\n";
            return;
        }

        card = c;
        cout << "Card inserted\n";
    }

    bool authenticate(int pin) {
        if (card == nullptr) {
            cout << "Please insert card\n";
            return false;
        }

        if (!card->validatePin(pin)) {
            cout << "Incorrect PIN\n";
            return false;
        }

        cout << "Authentication successful\n";
        return true;
    }

    void setAccount(Account* acc) {
        account = acc;
    }

    void checkBalance() {
        if (account == nullptr) {
            cout << "Account not found\n";
            return;
        }

        cout << "Balance: " << account->getBalance() << endl;
    }

    void withdraw(int amount) {
        if (account == nullptr) {
            cout << "Account not found\n";
            return;
        }

        if (!dispenser.canDispense(amount)) {
            cout << "ATM does not have enough cash\n";
            return;
        }

        if (!account->withdraw(amount)) {
            cout << "Insufficient account balance\n";
            return;
        }

        dispenser.dispense(amount);
        cout << "Remaining balance: "
             << account->getBalance() << endl;
    }

    void ejectCard() {
        if (card == nullptr) {
            cout << "No card inserted\n";
            return;
        }

        cout << "Card returned\n";

        card = nullptr;
        account = nullptr;
    }
};

int main() {

    Card card("123456789", 1234);

    Account account("ACC001", 10000);

    ATM atm;

    atm.insertCard(&card);

    if (atm.authenticate(1234)) {

        atm.setAccount(&account);

        cout << "\n1. Check Balance\n";
        cout << "2. Withdraw Cash\n\n";

        atm.checkBalance();

        cout << endl;

        atm.withdraw(1500);
    }

    atm.ejectCard();

    return 0;
}

// Output:
// Card inserted
// Authentication successful

// 1. Check Balance
// 2. Withdraw Cash

// Balance: 10000

// Cash dispensed successfully
// Remaining balance: 8500
// Card returned