#include <iostream>
#include <unordered_map>
using namespace std;

// ================= PRODUCT =================

class Product {
public:
    string sku;
    string name;
    double price;
    string category;

    Product(string sku, string name,
            double price, string category)
        : sku(sku),
          name(name),
          price(price),
          category(category) {}
};

// ================= INVENTORY ITEM =================

class InventoryItem {
public:
    Product* product;
    int quantity;
    int threshold;

    InventoryItem(Product* product,
                  int quantity,
                  int threshold)
        : product(product),
          quantity(quantity),
          threshold(threshold) {}

    bool isLowStock() const {
        return quantity <= threshold;
    }
};

// ================= WAREHOUSE =================

class Warehouse {
public:
    int id;
    string name;

    unordered_map<string, InventoryItem> inventory;

    Warehouse(int id, string name)
        : id(id), name(name) {}

    void addStock(Product* product,
                  int quantity,
                  int threshold) {

        auto it = inventory.find(product->sku);

        if (it != inventory.end()) {
            it->second.quantity += quantity;
        }
        else {
            inventory.emplace(
                product->sku,
                InventoryItem(
                    product,
                    quantity,
                    threshold
                )
            );
        }
    }

    bool removeStock(const string& sku,
                     int quantity) {

        auto it = inventory.find(sku);

        if (it == inventory.end())
            return false;

        if (it->second.quantity < quantity)
            return false;

        it->second.quantity -= quantity;

        return true;
    }

    int getStock(const string& sku) const {

        auto it = inventory.find(sku);

        if (it == inventory.end())
            return 0;

        return it->second.quantity;
    }
};

// ================= USER =================

enum class Role {
    ADMIN,
    MANAGER,
    STAFF
};

class User {
public:
    string name;
    Role role;

    User(string name, Role role)
        : name(name), role(role) {}

    bool canTransfer() const {
        return role == Role::ADMIN ||
               role == Role::MANAGER;
    }
};

// ================= INVENTORY SYSTEM =================

class InventorySystem {

private:
    unordered_map<string, Product> products;
    unordered_map<int, Warehouse> warehouses;

public:

    // Add Product
    void addProduct(string sku,
                    string name,
                    double price,
                    string category) {

        if (products.find(sku) != products.end()) {
            cout << "Product already exists\n";
            return;
        }

        products.emplace(
            sku,
            Product(
                sku,
                name,
                price,
                category
            )
        );
    }

    // Add Warehouse
    void addWarehouse(int id,
                      string name) {

        if (warehouses.find(id) != warehouses.end()) {
            cout << "Warehouse already exists\n";
            return;
        }

        warehouses.emplace(
            id,
            Warehouse(id, name)
        );
    }

    // Add / Receive Stock
    bool addStock(int warehouseId,
                  const string& sku,
                  int quantity,
                  int threshold) {

        auto warehouseIt =
            warehouses.find(warehouseId);

        auto productIt =
            products.find(sku);

        if (warehouseIt == warehouses.end() ||
            productIt == products.end()) {

            cout << "Invalid warehouse or product\n";
            return false;
        }

        warehouseIt->second.addStock(
            &productIt->second,
            quantity,
            threshold
        );

        return true;
    }

    // Remove / Ship Stock
    bool removeStock(int warehouseId,
                     const string& sku,
                     int quantity) {

        auto warehouseIt =
            warehouses.find(warehouseId);

        if (warehouseIt == warehouses.end())
            return false;

        Warehouse& warehouse =
            warehouseIt->second;

        if (!warehouse.removeStock(
                sku,
                quantity)) {

            cout << "Insufficient stock\n";
            return false;
        }

        // Low-stock alert
        auto itemIt =
            warehouse.inventory.find(sku);

        if (itemIt != warehouse.inventory.end() &&
            itemIt->second.isLowStock()) {

            cout << "LOW STOCK ALERT: "
                 << sku << endl;
        }

        return true;
    }

    // Transfer Stock
    bool transferStock(int fromWarehouse,
                       int toWarehouse,
                       const string& sku,
                       int quantity,
                       const User& user) {

        // Access control
        if (!user.canTransfer()) {
            cout << "Access denied\n";
            return false;
        }

        auto sourceIt =
            warehouses.find(fromWarehouse);

        auto destinationIt =
            warehouses.find(toWarehouse);

        if (sourceIt == warehouses.end() ||
            destinationIt == warehouses.end()) {

            cout << "Invalid warehouse\n";
            return false;
        }

        Warehouse& source =
            sourceIt->second;

        Warehouse& destination =
            destinationIt->second;

        auto itemIt =
            source.inventory.find(sku);

        if (itemIt == source.inventory.end()) {
            cout << "Product not found in source warehouse\n";
            return false;
        }

        if (itemIt->second.quantity < quantity) {
            cout << "Insufficient stock\n";
            return false;
        }

        Product* product =
            itemIt->second.product;

        int threshold =
            itemIt->second.threshold;

        // Remove from source
        source.removeStock(
            sku,
            quantity
        );

        // Add to destination
        destination.addStock(
            product,
            quantity,
            threshold
        );

        return true;
    }

    // Search Product
    void searchProduct(const string& sku) {

        auto it = products.find(sku);

        if (it == products.end()) {
            cout << "Product not found\n";
            return;
        }

        Product& product = it->second;

        cout << "\nProduct Details\n";
        cout << "SKU      : " << product.sku << endl;
        cout << "Name     : " << product.name << endl;
        cout << "Price    : " << product.price << endl;
        cout << "Category : " << product.category << endl;
    }

    // Inventory Report
    void generateReport() {

        cout << "\n===== INVENTORY REPORT =====\n";

        for (auto& warehousePair : warehouses) {

            Warehouse& warehouse =
                warehousePair.second;

            cout << "\nWarehouse: "
                 << warehouse.name
                 << " (ID: "
                 << warehouse.id
                 << ")\n";

            for (auto& itemPair :
                 warehouse.inventory) {

                InventoryItem& item =
                    itemPair.second;

                cout << "SKU: "
                     << item.product->sku
                     << " | Product: "
                     << item.product->name
                     << " | Quantity: "
                     << item.quantity;

                if (item.isLowStock())
                    cout << " | LOW STOCK";

                cout << endl;
            }
        }

        cout << "============================\n";
    }
};

// ================= MAIN =================

int main() {

    InventorySystem system;

    // Warehouses
    system.addWarehouse(
        1,
        "Chennai Warehouse"
    );

    system.addWarehouse(
        2,
        "Bangalore Warehouse"
    );

    // Products
    system.addProduct(
        "P101",
        "Laptop",
        60000,
        "Electronics"
    );

    system.addProduct(
        "P102",
        "Mouse",
        1000,
        "Accessories"
    );

    // Add stock
    system.addStock(
        1,
        "P101",
        20,
        5
    );

    system.addStock(
        1,
        "P102",
        50,
        10
    );

    system.addStock(
        2,
        "P101",
        10,
        5
    );

    // Search
    system.searchProduct("P101");

    // Remove stock
    cout << "\nRemoving 16 laptops...\n";

    system.removeStock(
        1,
        "P101",
        16
    );

    // Transfer
    User manager(
        "John",
        Role::MANAGER
    );

    cout << "\nTransferring 2 laptops...\n";

    system.transferStock(
        1,
        2,
        "P101",
        2,
        manager
    );

    // Report
    system.generateReport();

    return 0;
}