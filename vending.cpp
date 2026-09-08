#include <iostream>
#include <unordered_map>
using namespace std;

enum class State {
    READY,
    ITEM_SELECTED,
    PAYMENT_PENDING,
    DISPENSING,
    MAINTENANCE
};

class Product {
public:
    string id;
    string name;
    double price;
    int quantity;

    Product() {}

    Product(string id, string name, double price, int quantity) {
        this->id = id;
        this->name = name;
        this->price = price;
        this->quantity = quantity;
    }
};

class VendingMachine {
private:
    unordered_map<string, Product> inventory;
    State state;
    
    string selectedProduct;
    double insertedMoney;

public:
    VendingMachine() {
        state = State::READY;
        insertedMoney = 0;
    }

    void addProduct(Product product) {
        inventory[product.id] = product;
    }

    void showProducts() {
        cout << "\nAvailable Products:\n";

        for (auto &p : inventory) {
            if (p.second.quantity > 0) {
                cout << p.second.id << " - "
                     << p.second.name << " - Rs."
                     << p.second.price << " - Qty: "
                     << p.second.quantity << endl;
            }
        }
    }

    void selectProduct(string id) {
        if (state == State::MAINTENANCE) {
            cout << "Machine is under maintenance.\n";
            return;
        }

        if (inventory.find(id) == inventory.end()) {
            cout << "Product not found.\n";
            return;
        }

        if (inventory[id].quantity == 0) {
            cout << "Product is out of stock.\n";
            return;
        }

        selectedProduct = id;
        state = State::PAYMENT_PENDING;

        cout << "Selected: "
             << inventory[id].name << endl;

        cout << "Price: Rs."
             << inventory[id].price << endl;
    }

    void insertCoin(double coin) {
        if (state != State::PAYMENT_PENDING) {
            cout << "Please select a product first.\n";
            return;
        }

        insertedMoney += coin;

        cout << "Inserted: Rs."
             << coin << endl;

        cout << "Total inserted: Rs."
             << insertedMoney << endl;

        if (insertedMoney >= inventory[selectedProduct].price) {
            dispense();
        }
    }

    void dispense() {
        state = State::DISPENSING;

        Product &product = inventory[selectedProduct];

        cout << "\nDispensing "
             << product.name << "...\n";

        product.quantity--;

        double change = insertedMoney - product.price;

        if (change > 0) {
            cout << "Returning change: Rs."
                 << change << endl;
        }

        cout << "Purchase successful!\n";

        logPurchase(product);

        insertedMoney = 0;
        selectedProduct = "";

        state = State::READY;
    }

    void cancel() {
        if (state == State::PAYMENT_PENDING) {
            cout << "Transaction cancelled.\n";
            cout << "Returning Rs."
                 << insertedMoney << endl;

            insertedMoney = 0;
            selectedProduct = "";

            state = State::READY;
        }
    }

    void enterMaintenance() {
        state = State::MAINTENANCE;
        cout << "Machine entered maintenance mode.\n";
    }

    void exitMaintenance() {
        state = State::READY;
        cout << "Machine is ready.\n";
    }

private:
    void logPurchase(Product product) {
        cout << "[AUDIT] Purchased: "
             << product.name << endl;

        cout << "[AUDIT] Price: Rs."
             << product.price << endl;

        cout << "[AUDIT] Remaining quantity: "
             << product.quantity << endl;
    }
};

int main() {

    VendingMachine machine;

    machine.addProduct(
        Product("P1", "Coke", 40, 5)
    );

    machine.addProduct(
        Product("P2", "Pepsi", 35, 3)
    );

    machine.addProduct(
        Product("P3", "Chips", 20, 10)
    );

    machine.showProducts();

    cout << "\n--- Purchase ---\n";

    machine.selectProduct("P1");

    machine.insertCoin(10);
    machine.insertCoin(10);
    machine.insertCoin(20);

    cout << "\n--- Products After Purchase ---\n";

    machine.showProducts();

    return 0;
}

// Available Products:
// P3 - Chips - Rs.20 - Qty: 10
// P2 - Pepsi - Rs.35 - Qty: 3
// P1 - Coke - Rs.40 - Qty: 5

// --- Purchase ---
// Selected: Coke
// Price: Rs.40
// Inserted: Rs.10
// Total inserted: Rs.10
// Inserted: Rs.10
// Total inserted: Rs.20
// Inserted: Rs.20
// Total inserted: Rs.40

// Dispensing Coke...
// Purchase successful!
// [AUDIT] Purchased: Coke
// [AUDIT] Price: Rs.40
// [AUDIT] Remaining quantity: 4

// --- Products After Purchase ---

// Available Products:
// P3 - Chips - Rs.20 - Qty: 10
// P2 - Pepsi - Rs.35 - Qty: 3
// P1 - Coke - Rs.40 - Qty: 4