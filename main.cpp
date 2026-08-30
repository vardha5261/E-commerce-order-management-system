#include <iostream>
#include <cstring>
using namespace std;

// ---------------- ENUMS ----------------
enum Category { ELECTRONICS, CLOTHING, GROCERY };
enum OrderStatus { PENDING, CONFIRMED, CANCELLED, DELIVERED };
enum PaymentStatus { NOT_PAID, PAID, FAILED };
enum PaymentMethod { CREDIT_CARD, UPI, CASH };

// ---------------- UNION ----------------
union PaymentInfo {
    int last4Digits;   // used when method = CREDIT_CARD
    char upiId[30];    // used when method = UPI
};

// ---------------- PRODUCT CLASS ----------------
class Product {
private:
    int id;
    string name;
    double price;
    int stock;
    Category category;

public:
    Product() {}
    Product(int i, string n, double p, int s, Category c) {
        id = i; name = n; price = p; stock = s; category = c;
    }

    void displayProduct() {
        cout << "  [" << id << "] " << name << " | Rs." << price
             << " | Stock: " << stock << " | Category: " << category << endl;
    }

    void updateStock(int qty) { stock += qty; }
    bool checkAvailability(int qty) { return stock >= qty; }

    int getID() { return id; }
    string getName() { return name; }
    double getPrice() { return price; }
};

// ---------------- CUSTOMER CLASS ----------------
class Customer {
private:
    int id;
    string name, email;

public:
    Customer() {}
    Customer(int i, string n, string e) {
        id = i; name = n; email = e;
    }

    void displayCustomer() {
        cout << "  [" << id << "] " << name << " | " << email << endl;
    }

    void updateDetails(string e) { email = e; }
    int getID() { return id; }
    string getName() { return name; }
};

// ---------------- ORDER STRUCT ----------------
struct Order {
    int orderID;
    int customerID;
    double totalAmount;
    OrderStatus orderStatus;
    PaymentStatus paymentStatus;
    PaymentMethod paymentMethod;
    PaymentInfo paymentInfo;

    Order() {}
    Order(int oid, int cid, double amount) {
        orderID = oid;
        customerID = cid;
        totalAmount = amount;
        orderStatus = PENDING;
        paymentStatus = NOT_PAID;
    }

    void displayOrder() {
        cout << "  Order " << orderID << " | Cust " << customerID
             << " | Rs." << totalAmount
             << " | Status: " << orderStatus
             << " | Payment: " << paymentStatus << endl;
    }
};

// ---------------- GLOBAL ARRAYS ----------------
Product products[10];
int productCount = 0;

Customer customers[10];
int customerCount = 0;

Order orders[10];
int orderCount = 0;

// ---------------- HELPER FUNCTIONS ----------------
int findProductIndex(int id) {
    for (int i = 0; i < productCount; i++)
        if (products[i].getID() == id) return i;
    return -1;
}

int findCustomerIndex(int id) {
    for (int i = 0; i < customerCount; i++)
        if (customers[i].getID() == id) return i;
    return -1;
}

int findOrderIndex(int id) {
    for (int i = 0; i < orderCount; i++)
        if (orders[i].orderID == id) return i;
    return -1;
}

// ---------------- INPUT-DRIVEN ACTIONS ----------------
void addProduct() {
    int id, stock, cat;
    string name;
    double price;

    cout << "Product ID: "; cin >> id;
    cout << "Name: "; cin >> name;
    cout << "Price: "; cin >> price;
    cout << "Stock: "; cin >> stock;
    cout << "Category (0=ELECTRONICS, 1=CLOTHING, 2=GROCERY): "; cin >> cat;

    products[productCount++] = Product(id, name, price, stock, (Category)cat);
    cout << "Product added.\n";
}

void addCustomer() {
    int id;
    string name, email;

    cout << "Customer ID: "; cin >> id;
    cout << "Name: "; cin >> name;
    cout << "Email: "; cin >> email;

    customers[customerCount++] = Customer(id, name, email);
    cout << "Customer added.\n";
}

void createOrder() {
    int orderID, custID, prodID, qty;
    cout << "Order ID: "; cin >> orderID;
    cout << "Customer ID: "; cin >> custID;
    cout << "Product ID: "; cin >> prodID;
    cout << "Quantity: "; cin >> qty;

    int ci = findCustomerIndex(custID);
    if (ci == -1) { cout << "ERROR: Invalid Customer ID.\n"; return; }

    int pi = findProductIndex(prodID);
    if (pi == -1) { cout << "ERROR: Invalid Product ID.\n"; return; }

    if (!products[pi].checkAvailability(qty)) {
        cout << "ERROR: Product out of stock.\n"; return;
    }

    double amount = products[pi].getPrice() * qty;
    orders[orderCount] = Order(orderID, custID, amount);
    orderCount++;
    products[pi].updateStock(-qty);

    cout << "Order " << orderID << " created. Amount: Rs." << amount << endl;
}

void makePayment() {
    int orderID, methodInt, success;
    cout << "Order ID: "; cin >> orderID;

    int oi = findOrderIndex(orderID);
    if (oi == -1) { cout << "ERROR: Invalid Order ID.\n"; return; }

    if (orders[oi].orderStatus == CANCELLED) {
        cout << "ERROR: Cannot pay for a cancelled order.\n"; return;
    }

    cout << "Payment Method (0=CREDIT_CARD, 1=UPI, 2=CASH): "; cin >> methodInt;
    PaymentMethod method = (PaymentMethod)methodInt;
    orders[oi].paymentMethod = method;

    cout << "Did payment succeed? (1=yes, 0=no): "; cin >> success;
    if (!success) {
        orders[oi].paymentStatus = FAILED;
        cout << "Payment FAILED for order " << orderID << endl;
        return;
    }

    if (method == CREDIT_CARD) {
        int last4;
        cout << "Last 4 digits of card: "; cin >> last4;
        orders[oi].paymentInfo.last4Digits = last4;
    } else if (method == UPI) {
        string upi;
        cout << "UPI ID: "; cin >> upi;
        strcpy(orders[oi].paymentInfo.upiId, upi.c_str());
    }
    // CASH needs no extra info

    orders[oi].paymentStatus = PAID;
    cout << "Payment SUCCESSFUL for order " << orderID << endl;
}

void confirmOrder() {
    int orderID;
    cout << "Order ID: "; cin >> orderID;

    int oi = findOrderIndex(orderID);
    if (oi == -1) { cout << "ERROR: Invalid Order ID.\n"; return; }

    if (orders[oi].paymentStatus != PAID) {
        cout << "ERROR: Cannot confirm, payment not done.\n"; return;
    }

    orders[oi].orderStatus = CONFIRMED;
    cout << "Order " << orderID << " confirmed.\n";
}

void cancelOrder() {
    int orderID;
    cout << "Order ID: "; cin >> orderID;

    int oi = findOrderIndex(orderID);
    if (oi == -1) { cout << "ERROR: Invalid Order ID.\n"; return; }

    if (orders[oi].orderStatus == DELIVERED) {
        cout << "ERROR: Cannot cancel a delivered order.\n"; return;
    }

    orders[oi].orderStatus = CANCELLED;
    cout << "Order " << orderID << " cancelled.\n";
}

void deliverOrder() {
    int orderID;
    cout << "Order ID: "; cin >> orderID;

    int oi = findOrderIndex(orderID);
    if (oi == -1) { cout << "ERROR: Invalid Order ID.\n"; return; }

    if (orders[oi].orderStatus != CONFIRMED) {
        cout << "ERROR: Order must be confirmed before delivery.\n"; return;
    }

    orders[oi].orderStatus = DELIVERED;
    cout << "Order " << orderID << " delivered.\n";
}

// ---------------- MENU ----------------
int main() {
    int choice;
    do {
        cout << "\n===== E-COMMERCE MENU =====\n";
        cout << "1. Add Product\n";
        cout << "2. Add Customer\n";
        cout << "3. Create Order\n";
        cout << "4. Make Payment\n";
        cout << "5. Confirm Order\n";
        cout << "6. Deliver Order\n";
        cout << "7. Cancel Order\n";
        cout << "8. Display Products\n";
        cout << "9. Display Orders\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addProduct(); break;
            case 2: addCustomer(); break;
            case 3: createOrder(); break;
            case 4: makePayment(); break;
            case 5: confirmOrder(); break;
            case 6: deliverOrder(); break;
            case 7: cancelOrder(); break;
            case 8:
                cout << "-- Products --\n";
                for (int i = 0; i < productCount; i++) products[i].displayProduct();
                break;
            case 9:
                cout << "-- Orders --\n";
                for (int i = 0; i < orderCount; i++) orders[i].displayOrder();
                break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
