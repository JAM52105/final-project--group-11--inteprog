#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <map>

using namespace std;

// Utility functions
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Exception classes
class InvalidInputException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid input! Please try again.";
    }
};

class AuthenticationException : public exception {
public:
    const char* what() const noexcept override {
        return "Authentication failed! Invalid username or password.";
    }
};

class AuthorizationException : public exception {
public:
    const char* what() const noexcept override {
        return "You are not authorized to perform this action!";
    }
};

class CarNotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "Car not found!";
    }
};

class BookingNotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "Booking not found!";
    }
};

// Strategy Pattern: Payment Strategy
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;
    virtual string getType() const = 0;
};

class CreditCardStrategy : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << fixed << setprecision(2) << amount << " via Credit Card\n";
    }
    string getType() const override { return "Credit Card"; }
};

class PayPalStrategy : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << fixed << setprecision(2) << amount << " via PayPal\n";
    }
    string getType() const override { return "PayPal"; }
};

class CashStrategy : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << fixed << setprecision(2) << amount << " in Cash\n";
    }
    string getType() const override { return "Cash"; }
};

// Car class
class Car {
private:
    int id;
    string brand;
    string type;
    double pricePerDay;
    bool available;

public:
    Car(int id, const string& brand, const string& type, double pricePerDay, bool available = true)
        : id(id), brand(brand), type(type), pricePerDay(pricePerDay), available(available) {}

    // Getters
    int getId() const { return id; }
    string getBrand() const { return brand; }
    string getType() const { return type; }
    double getPricePerDay() const { return pricePerDay; }
    bool isAvailable() const { return available; }

    // Setters
    void setPricePerDay(double price) { pricePerDay = price; }
    void setAvailable(bool avail) { available = avail; }

    void display() const {
        cout << "ID: " << id << " | Brand: " << brand 
             << " | Type: " << type << " | Price/Day: $" << fixed << setprecision(2) << pricePerDay
             << " | Status: " << (available ? "Available" : "Rented") << endl;
    }
};

// Booking class
class Booking {
private:
    int id;
    int userId;
    int carId;
    string startDate;
    string endDate;
    string status;
    double totalPrice;

public:
    Booking(int id, int userId, int carId, const string& startDate, const string& endDate, double totalPrice, const string& status = "Pending")
        : id(id), userId(userId), carId(carId), startDate(startDate), endDate(endDate), totalPrice(totalPrice), status(status) {}

    // Getters
    int getId() const { return id; }
    int getUserId() const { return userId; }
    int getCarId() const { return carId; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    string getStatus() const { return status; }
    double getTotalPrice() const { return totalPrice; }

    // Setters
    void setStatus(const string& newStatus) { status = newStatus; }

    void display() const {
        cout << "Booking ID: " << id << " | Car ID: " << carId 
             << " | Dates: " << startDate << " to " << endDate
             << " | Total: $" << fixed << setprecision(2) << totalPrice
             << " | Status: " << status << endl;
    }
};

// Payment class
class Payment {
private:
    int id;
    int bookingId;
    double amount;
    string date;
    string status;
    string method;

public:
    Payment(int id, int bookingId, double amount, const string& date, const string& status, const string& method)
        : id(id), bookingId(bookingId), amount(amount), date(date), status(status), method(method) {}

    // Getters
    int getId() const { return id; }
    int getBookingId() const { return bookingId; }
    double getAmount() const { return amount; }
    string getDate() const { return date; }
    string getStatus() const { return status; }
    string getMethod() const { return method; }

    void display() const {
        cout << "Payment ID: " << id << " | Booking ID: " << bookingId 
             << " | Amount: $" << fixed << setprecision(2) << amount
             << " | Date: " << date << " | Method: " << method 
             << " | Status: " << status << endl;
    }
};

// Abstract User class
class User {
protected:
    int id;
    string username;
    string password;
    string email;
    string role;

public:
    User(int id, const string& username, const string& password, const string& email, const string& role)
        : id(id), username(username), password(password), email(email), role(role) {}

    virtual ~User() = default;

    // Getters
    int getId() const { return id; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getEmail() const { return email; }
    string getRole() const { return role; }

    // Setters
    void setPassword(const string& newPassword) { password = newPassword; }
    void setEmail(const string& newEmail) { email = newEmail; }

    // Virtual functions
    virtual void displayMenu() = 0;
    virtual void updateProfile() {
        cout << "\n--- Update Profile ---\n";
        cout << "Current email: " << email << endl;
        cout << "Enter new email (or press Enter to keep current): ";
        string newEmail;
        getline(cin, newEmail);
        if (!newEmail.empty()) {
            email = newEmail;
        }

        cout << "Enter new password (or press Enter to keep current): ";
        string newPassword;
        getline(cin, newPassword);
        if (!newPassword.empty()) {
            password = newPassword;
        }

        cout << "Profile updated successfully!\n";
    }
};

// Customer class
class Customer : public User {
private:
    vector<Booking> bookings;

public:
    Customer(int id, const string& username, const string& password, const string& email)
        : User(id, username, password, email, "customer") {}

    void displayMenu() override {
        int choice;
        do {
            clearScreen();
            cout << "\n=== CUSTOMER DASHBOARD ===\n";
            cout << "Welcome, " << username << "!\n";
            cout << "1. Search Cars\n";
            cout << "2. Book a Car\n";
            cout << "3. View My Bookings\n";
            cout << "4. Cancel Booking\n";
            cout << "5. View Rental History\n";
            cout << "6. Make Payment\n";
            cout << "7. Update Profile\n";
            cout << "0. Logout\n";
            cout << "Enter your choice: ";

            try {
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw InvalidInputException();
                }
                cin.ignore();

                switch (choice) {
                    case 1: searchCars(); break;
                    case 2: bookCar(); break;
                    case 3: viewBookings(); break;
                    case 4: cancelBooking(); break;
                    case 5: viewRentalHistory(); break;
                    case 6: makePayment(); break;
                    case 7: updateProfile(); break;
                    case 0: cout << "Logging out...\n"; break;
                    default: cout << "Invalid choice! Try again.\n"; break;
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            if (choice != 0) pressEnterToContinue();
        } while (choice != 0);
    }

    void searchCars() {
        cout << "\n--- Search Cars ---\n";
        // In a real system, this would search the database
        cout << "Car search functionality would be implemented here.\n";
    }

    void bookCar() {
        cout << "\n--- Book a Car ---\n";
        // In a real system, this would interact with the database
        cout << "Car booking functionality would be implemented here.\n";
    }

    void viewBookings() {
        cout << "\n--- My Bookings ---\n";
        if (bookings.empty()) {
            cout << "No bookings found.\n";
            return;
        }
        for (const auto& booking : bookings) {
            booking.display();
        }
    }

    void cancelBooking() {
        cout << "\n--- Cancel Booking ---\n";
        if (bookings.empty()) {
            cout << "No bookings to cancel.\n";
            return;
        }
        viewBookings();
        cout << "Enter Booking ID to cancel: ";
        int bookingId;
        cin >> bookingId;
        cin.ignore();

        auto it = find_if(bookings.begin(), bookings.end(), 
            [bookingId](const Booking& b) { return b.getId() == bookingId; });

        if (it != bookings.end()) {
            it->setStatus("Cancelled");
            cout << "Booking cancelled successfully.\n";
        } else {
            cout << "Booking not found.\n";
        }
    }

    void viewRentalHistory() {
        cout << "\n--- Rental History ---\n";
        // In a real system, this would show completed rentals
        cout << "Rental history functionality would be implemented here.\n";
    }

    void makePayment() {
        cout << "\n--- Make Payment ---\n";
        if (bookings.empty()) {
            cout << "No bookings requiring payment.\n";
            return;
        }
        
        viewBookings();
        cout << "Enter Booking ID to pay for: ";
        int bookingId;
        cin >> bookingId;
        cin.ignore();

        auto it = find_if(bookings.begin(), bookings.end(), 
            [bookingId](const Booking& b) { return b.getId() == bookingId; });

        if (it == bookings.end()) {
            cout << "Booking not found.\n";
            return;
        }

        cout << "Select payment method:\n";
        cout << "1. Credit Card\n";
        cout << "2. PayPal\n";
        cout << "3. Cash\n";
        cout << "Choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        unique_ptr<PaymentStrategy> strategy;
        switch (choice) {
            case 1: strategy = make_unique<CreditCardStrategy>(); break;
            case 2: strategy = make_unique<PayPalStrategy>(); break;
            case 3: strategy = make_unique<CashStrategy>(); break;
            default: 
                cout << "Invalid choice.\n";
                return;
        }

        cout << "Processing payment of $" << fixed << setprecision(2) << it->getTotalPrice() << "...\n";
        strategy->pay(it->getTotalPrice());
        cout << "Payment completed successfully via " << strategy->getType() << ".\n";
    }
};

// Admin class
class Admin : public User {
public:
    Admin(int id, const string& username, const string& password, const string& email)
        : User(id, username, password, email, "admin") {}

    void displayMenu() override {
        int choice;
        do {
            clearScreen();
            cout << "\n=== ADMIN DASHBOARD ===\n";
            cout << "Welcome, " << username << "!\n";
            cout << "1. Manage Cars\n";
            cout << "2. Manage Bookings\n";
            cout << "3. View Payment Records\n";
            cout << "4. Manage Users\n";
            cout << "5. Generate Reports\n";
            cout << "6. Update Profile\n";
            cout << "0. Logout\n";
            cout << "Enter your choice: ";

            try {
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw InvalidInputException();
                }
                cin.ignore();

                switch (choice) {
                    case 1: manageCars(); break;
                    case 2: manageBookings(); break;
                    case 3: viewPaymentRecords(); break;
                    case 4: manageUsers(); break;
                    case 5: generateReports(); break;
                    case 6: updateProfile(); break;
                    case 0: cout << "Logging out...\n"; break;
                    default: cout << "Invalid choice! Try again.\n"; break;
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            if (choice != 0) pressEnterToContinue();
        } while (choice != 0);
    }

    void manageCars() {
        cout << "\n--- Manage Cars ---\n";
        // In a real system, this would interact with the database
        cout << "Car management functionality would be implemented here.\n";
    }

    void manageBookings() {
        cout << "\n--- Manage Bookings ---\n";
        // In a real system, this would show all bookings
        cout << "Booking management functionality would be implemented here.\n";
    }

    void viewPaymentRecords() {
        cout << "\n--- Payment Records ---\n";
        // In a real system, this would show all payments
        cout << "Payment records functionality would be implemented here.\n";
    }

    void manageUsers() {
        cout << "\n--- Manage Users ---\n";
        // In a real system, this would allow adding/removing users
        cout << "User management functionality would be implemented here.\n";
    }

    void generateReports() {
        cout << "\n--- Generate Reports ---\n";
        // In a real system, this would generate business reports
        cout << "Report generation functionality would be implemented here.\n";
    }
};

// Singleton Pattern: CarRentalSystem
class CarRentalSystem {
private:
    static CarRentalSystem* instance;
    vector<unique_ptr<User>> users;
    vector<Car> cars;
    vector<Booking> bookings;
    vector<Payment> payments;
    int nextUserId = 1;
    int nextCarId = 1;
    int nextBookingId = 1;
    int nextPaymentId = 1;

    // Private constructor for singleton
    CarRentalSystem() {
        // Initialize with some sample data
        users.push_back(make_unique<Admin>(nextUserId++, "admin", "admin123", "admin@carrental.com"));
        users.push_back(make_unique<Customer>(nextUserId++, "john", "john123", "john@example.com"));
        users.push_back(make_unique<Customer>(nextUserId++, "alice", "alice123", "alice@example.com"));

        cars.emplace_back(nextCarId++, "Toyota", "Sedan", 50.0);
        cars.emplace_back(nextCarId++, "Honda", "SUV", 70.0);
        cars.emplace_back(nextCarId++, "Ford", "Truck", 90.0);
    }

public:
    // Delete copy constructor and assignment operator
    CarRentalSystem(const CarRentalSystem&) = delete;
    CarRentalSystem& operator=(const CarRentalSystem&) = delete;

    static CarRentalSystem* getInstance() {
        if (!instance) {
            instance = new CarRentalSystem();
        }
        return instance;
    }

    User* authenticate(const string& username, const string& password) {
        for (const auto& user : users) {
            if (user->getUsername() == username && user->getPassword() == password) {
                return user.get();
            }
        }
        throw AuthenticationException();
    }

    void registerUser(const string& username, const string& password, const string& email) {
        // Check if username already exists
        for (const auto& user : users) {
            if (user->getUsername() == username) {
                throw runtime_error("Username already exists!");
            }
        }
        users.push_back(make_unique<Customer>(nextUserId++, username, password, email));
        cout << "Registration successful! You can now login.\n";
    }

    void run() {
        int choice;
        do {
            clearScreen();
            cout << "\n=== CAR RENTAL SYSTEM ===\n";
            cout << "1. Login\n";
            cout << "2. Register New User\n";
            cout << "0. Exit\n";
            cout << "Enter your choice: ";

            try {
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw InvalidInputException();
                }
                cin.ignore();

                switch (choice) {
                    case 1: login(); break;
                    case 2: registerNewUser(); break;
                    case 0: cout << "Exiting system...\n"; break;
                    default: cout << "Invalid choice! Try again.\n"; break;
                }
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
                pressEnterToContinue();
            }
        } while (choice != 0);
    }

private:
    void login() {
        string username, password;
        cout << "\n--- Login ---\n";
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        try {
            User* user = authenticate(username, password);
            cout << "Login successful! Welcome, " << user->getUsername() << ".\n";
            pressEnterToContinue();

            if (user->getRole() == "admin") {
                dynamic_cast<Admin*>(user)->displayMenu();
            } else {
                dynamic_cast<Customer*>(user)->displayMenu();
            }
        } catch (const AuthenticationException& e) {
            cout << "Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }

    void registerNewUser() {
        string username, password, email;
        cout << "\n--- Register New User ---\n";
        
        // Username validation
        while (true) {
            cout << "Username (3-20 chars, no spaces): ";
            getline(cin, username);
            if (username.length() < 3 || username.length() > 20 || 
                username.find(' ') != string::npos) {
                cout << "Invalid username! Must be 3-20 characters with no spaces.\n";
            } else {
                break;
            }
        }

        // Password validation
        while (true) {
            cout << "Password (6+ chars): ";
            getline(cin, password);
            if (password.length() < 6) {
                cout << "Password too short! Must be at least 6 characters.\n";
            } else {
                break;
            }
        }

        // Email validation (simple check)
        while (true) {
            cout << "Email: ";
            getline(cin, email);
            if (email.find('@') == string::npos || email.find('.') == string::npos) {
                cout << "Invalid email format! Please include @ and .\n";
            } else {
                break;
            }
        }

        try {
            registerUser(username, password, email);
            pressEnterToContinue();
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            pressEnterToContinue();
        }
    }
};

// Initialize static member
CarRentalSystem* CarRentalSystem::instance = nullptr;

int main() {
    CarRentalSystem* system = CarRentalSystem::getInstance();
    system->run();
    return 0;
}