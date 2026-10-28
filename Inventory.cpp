#include <iostream>
#include <unordered_map>
using namespace std;

// PRODUCT
class Product {
public:
    string sku, name;
    double price;

    Product() {}

    Product(string s, string n, double p) {
        sku = s;
        name = n;
        price = p;
    }
};

// INVENTORY ITEM
class InventoryItem {
public:
    int quantity, threshold;

    InventoryItem() {
        quantity = 0;
        threshold = 0;
    }

    InventoryItem(int q, int t) {
        quantity = q;
        threshold = t;
    }

    bool isLowStock() {
        return quantity <= threshold;
    }
};

// WAREHOUSE
class Warehouse {
public:
    string name;
    unordered_map<string, InventoryItem> inventory;

    Warehouse() {}

    Warehouse(string n) {
        name = n;
    }

    void addStock(string sku, int quantity, int threshold) {
        auto it = inventory.find(sku);

        if (it != inventory.end()) {
            it->second.quantity += quantity;
        } else {
            inventory[sku] = InventoryItem(quantity, threshold);
        }
    }

    bool removeStock(string sku, int quantity) {
        auto it = inventory.find(sku);

        if (it == inventory.end() ||
            quantity <= 0 ||
            it->second.quantity < quantity) {
            return false;
        }

        it->second.quantity -= quantity;
        return true;
    }
};

// USER
enum class Role {
    ADMIN,
    MANAGER,
    STAFF
};

class User {
public:
    string name;
    Role role;

    User(string n, Role r) {
        name = n;
        role = r;
    }

    bool canTransfer() {
        return role == Role::ADMIN ||
               role == Role::MANAGER;
    }
};

// NOTIFICATION SERVICE
class NotificationService {
public:
    void checkLowStock(string sku, InventoryItem &item) {
        if (item.isLowStock()) {
            cout << "LOW STOCK ALERT: " << sku << endl;
        }
    }
};

// INVENTORY SYSTEM
class InventorySystem {
public:
    unordered_map<string, Product> products;
    unordered_map<int, Warehouse> warehouses;

    NotificationService notification;

    void addProduct(string sku, string name, double price) {
        if (products.find(sku) != products.end()) {
            cout << "Product already exists\n";
            return;
        }

        products[sku] = Product(sku, name, price);
    }

    void addWarehouse(int id, string name) {
        if (warehouses.find(id) != warehouses.end()) {
            cout << "Warehouse already exists\n";
            return;
        }

        warehouses[id] = Warehouse(name);
    }

    void addStock(int warehouseId, string sku,
                  int quantity, int threshold) {
        if (warehouses.find(warehouseId) == warehouses.end() ||
            products.find(sku) == products.end() ||
            quantity <= 0 || threshold < 0) {
            cout << "Invalid details\n";
            return;
        }

        warehouses[warehouseId].addStock(
            sku, quantity, threshold
        );

        cout << "Stock added successfully\n";
    }

    void removeStock(int warehouseId, string sku, int quantity) {
        if (warehouses.find(warehouseId) == warehouses.end()) {
            cout << "Warehouse not found\n";
            return;
        }

        Warehouse &w = warehouses[warehouseId];

        if (!w.removeStock(sku, quantity)) {
            cout << "Insufficient stock or invalid quantity\n";
            return;
        }

        cout << "Stock removed successfully\n";

        auto it = w.inventory.find(sku);

        if (it != w.inventory.end()) {
            notification.checkLowStock(sku, it->second);
        }
    }

    void transferStock(int from, int to, string sku,
                       int quantity, User &user) {
        if (!user.canTransfer()) {
            cout << "Access denied\n";
            return;
        }

        if (warehouses.find(from) == warehouses.end() ||
            warehouses.find(to) == warehouses.end() ||
            products.find(sku) == products.end() ||
            from == to || quantity <= 0) {
            cout << "Invalid transfer details\n";
            return;
        }

        Warehouse &source = warehouses[from];
        Warehouse &destination = warehouses[to];

        auto it = source.inventory.find(sku);

        if (it == source.inventory.end() ||
            it->second.quantity < quantity) {
            cout << "Insufficient stock\n";
            return;
        }

        int threshold = it->second.threshold;

        if (!source.removeStock(sku, quantity)) {
            cout << "Transfer failed\n";
            return;
        }

        destination.addStock(sku, quantity, threshold);

        cout << "Stock transferred successfully\n";

        auto sourceItem = source.inventory.find(sku);

        if (sourceItem != source.inventory.end()) {
            notification.checkLowStock(sku, sourceItem->second);
        }
    }

    void searchProduct(string sku) {
        auto it = products.find(sku);

        if (it == products.end()) {
            cout << "Product not found\n";
            return;
        }

        cout << "\nProduct: " << it->second.name << endl;
        cout << "SKU: " << it->second.sku << endl;
        cout << "Price: " << it->second.price << endl;
    }

    void generateReport() {
        cout << "\n===== INVENTORY REPORT =====\n";

        for (auto &w : warehouses) {
            cout << "\nWarehouse: " << w.second.name << endl;

            for (auto &item : w.second.inventory) {
                string sku = item.first;
                InventoryItem &inv = item.second;

                cout << "Product: " << products[sku].name
                     << " | Quantity: " << inv.quantity;

                if (inv.isLowStock()) {
                    cout << " | LOW STOCK";
                }

                cout << endl;
            }
        }
    }
};

// MAIN
int main() {
    InventorySystem system;

    system.addWarehouse(1, "Chennai");
    system.addWarehouse(2, "Bangalore");

    system.addProduct("P101", "Laptop", 60000);
    system.addProduct("P102", "Mouse", 1000);

    system.addStock(1, "P101", 20, 5);
    system.addStock(1, "P102", 50, 10);
    system.addStock(2, "P101", 10, 5);

    User admin("Navin", Role::ADMIN);
    User staff("John", Role::STAFF);

    cout << "\n--- Product Search ---\n";
    system.searchProduct("P101");

    cout << "\n--- Remove Stock ---\n";
    system.removeStock(1, "P101", 16);

    cout << "\n--- Staff Transfer Attempt ---\n";
    system.transferStock(1, 2, "P101", 2, staff);

    cout << "\n--- Admin Transfer ---\n";
    system.transferStock(1, 2, "P101", 2, admin);

    cout << "\n--- Inventory Report ---\n";
    system.generateReport();

    return 0;
}
