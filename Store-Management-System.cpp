#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <ctime>

using namespace std;

enum ProductType { H, T, N, B };

class Product {
private:
    int id;
    string name;
    int quantity;
    double price;
    double cost;
    ProductType type;
    string addedDate;

public:
Product(int inputId, string inputName, int inputQty, double inputPrice, 
    double inputCost, ProductType inputType, string inputDate)
: id(inputId), 
  name(inputName),
  quantity(inputQty),
  price(inputPrice),
  cost(inputCost),
  type(inputType),
  addedDate(inputDate) {}

    int getId() const { return id; }
    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    double getCost() const { return cost; }
    ProductType getType() const { return type; }
    string getDate() const { return addedDate; }

    void setQuantity(int q) { quantity = q; }
};

class Inventory {
private:
    vector<Product> products;

public:
    void addProduct(const Product& product) {
        if (findProductById(product.getId()) != nullptr) {
            cerr << "Error: Duplicate product ID!\n";
            return;
        }
        products.push_back(product);
    }

    void displayProducts(const vector<Product*>& filteredProducts = {}) const {
        vector<Product*> tempList;
        if (filteredProducts.empty()) {
            for (const auto& p : products) {
                tempList.push_back(const_cast<Product*>(&p));
            }
        }
        
        const vector<Product*>& displayList = filteredProducts.empty() ? tempList : filteredProducts;

        cout << "\n=== Product List ===\n";
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Qty"
             << setw(10) << "Price" << setw(15) << "Added Date\n";
        for (const auto& p : displayList) {
            cout << setw(5) << p->getId()
                 << setw(20) << p->getName()
                 << setw(10) << p->getQuantity()
                 << setw(10) << p->getPrice()
                 << setw(15) << p->getDate()
                 << endl;
        }
    }

    Product* findProductById(int id) {
        auto it = find_if(products.begin(), products.end(),
            [id](const Product& p) { return p.getId() == id; });
        return (it != products.end()) ? &(*it) : nullptr;
    }

    vector<Product*> searchByName(const string& keyword) {
        vector<Product*> results;
        for (auto& p : products) {
            if (p.getName().find(keyword) != string::npos) {
                results.push_back(&p);
            }
        }
        return results;
    }

    vector<Product*> filterByPrice(bool ascending = true) {
        vector<Product*> sorted;
        for (auto& p : products) {
            sorted.push_back(&p);
        }

        sort(sorted.begin(), sorted.end(),
            [ascending](Product* a, Product* b) {
                return ascending ? 
                    (a->getPrice() < b->getPrice()) : 
                    (a->getPrice() > b->getPrice());
            });

        return sorted;
    }
};

class SalesRecord {
private:
    struct Sale {
        Product* product; // <-- إزالة const
    int quantity;
    string date;
    };
    vector<Sale> sales;

public:
    void addSale(Product& product, int quantity, string date) {
        sales.push_back({&product, quantity, date});
    }

    void generateInvoice(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error creating invoice file!\n";
            return;
        }
        file << "=== INVOICE ===\n";
        double total = 0;
        for (const auto& sale : sales) {
            double subtotal = sale.product->getPrice() * sale.quantity;
            file << sale.product->getName() << "\t"
                 << sale.quantity << "\t$"
                 << fixed << setprecision(2) << subtotal << endl;
            total += subtotal;
        }
        file << "----------------------------\n";
        file << "Total: $" << fixed << setprecision(2) << total << endl;
        file.close();
        cout << "Invoice saved as: " << filename << endl;
    }
};

class PurchaseRecord {
private:
    struct Purchase {
        Product* product;
        int quantity;
        string date;
    };
    vector<Purchase> purchases;

public:
    void addPurchase(Product& product, int quantity, string date) {
        purchases.push_back({&product, quantity, date});
    }
};

template<typename T>
T getInput(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid input. Try again.\n";
        } else {
            cin.ignore();
            return value;
        }
    }
}

template<>
string getInput<string>(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

void displayMenu() {
    cout << "\n=== Store Management System ===\n"
         << "1. Add Product\n2. Display All\n3. Search by Name\n"
         << "4. Filter by Price (Low-High)\n5. Filter by Price (High-Low)\n"
         << "6. Record Sale\n7. Record Purchase\n8. Generate Invoice\n9. Exit\nChoice: ";
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}

int main() {
    Inventory inventory;
    SalesRecord sales;
    PurchaseRecord purchases;

    int choice;
    do {
        displayMenu();
        choice = getInput<int>("");
        switch (choice) {
        case 1: {
            int id = getInput<int>("ID: ");
            string name = getInput<string>("Name: ");
            int qty = getInput<int>("Quantity: ");
            double price = getInput<double>("Price: $");
            double cost = getInput<double>("Cost: $");
            int type = getInput<int>("Type (0-H,1-T,2-N,3-B): ");
            string date = getCurrentDate();
            
            inventory.addProduct(Product(
                id, name, qty, price, cost, 
                static_cast<ProductType>(type), date
            ));
            break;
        }
        case 2:
            inventory.displayProducts();
            break;
        case 3: {
            string keyword = getInput<string>("Search name: ");
            auto results = inventory.searchByName(keyword);
            inventory.displayProducts(results);
            break;
        }
        case 4: {
            auto sorted = inventory.filterByPrice(true);
            inventory.displayProducts(sorted);
            break;
        }
        case 5: {
            auto sorted = inventory.filterByPrice(false);
            inventory.displayProducts(sorted);
            break;
        }
        case 6: {
            int id = getInput<int>("Enter product ID: ");
            Product* product = inventory.findProductById(id);
            if (!product) {
                cerr << "Product not found!\n";
                break;
            }
            int qty = getInput<int>("Enter sold quantity: ");
            if (qty > product->getQuantity()) {
                cerr << "Insufficient stock!\n";
                break;
            }
            product->setQuantity(product->getQuantity() - qty);
            string date = getInput<string>("Enter date (YYYY-MM-DD): ");
            sales.addSale(*product, qty, date);
            cout << "Sale recorded!\n";
            break;
        }
        case 7: {
            int id = getInput<int>("Enter product ID: ");
            Product* product = inventory.findProductById(id);
            if (!product) {
                cerr << "Product not found!\n";
                break;
            }
            int qty = getInput<int>("Enter purchased quantity: ");
            product->setQuantity(product->getQuantity() + qty);
            string date = getInput<string>("Enter date (YYYY-MM-DD): ");
            purchases.addPurchase(*product, qty, date);
            cout << "Purchase recorded!\n";
            break;
        }
        case 8: {
            string filename = getInput<string>("Enter invoice filename: ");
            string filename1 = filename + ".txt";
            sales.generateInvoice(filename1);
            break;
        }
        case 9:
            cout << "Exiting...\n";
            break;
        default:
            cerr << "Invalid choice!\n";
        }
    } while (choice != 9);
    return 0;
}