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
#include <fstream>
#include <random>
#include <chrono>
#include <mutex>

using namespace std;

// Forward declarations
class Car;
class Booking;
class Payment;
class CarRentalSystem;
class User;
class Customer;
class Admin;
class Logger;

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

// Helper function to get current date as string
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-" 
       << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
       << setw(2) << setfill('0') << ltm->tm_mday;
    return ss.str();
}

// Helper function to generate random IDs
int generateRandomId() {
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(1000, 9999);
    return dist(rng);
}

// Calculate days between two dates (simple implementation)
int calculateDaysBetweenDates(const string& startDate, const string& endDate) {
    // In a real system, you'd use proper date parsing and calculation
    // This is a simplified version that assumes dates are in YYYY-MM-DD format
    // and just returns a random number between 1-14 for demonstration
    return (generateRandomId() % 14) + 1;
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
    string model;
    string type;
    int year;
    string color;
    double pricePerDay;
    bool available;
    string registrationNumber;

public:
    Car(int id, const string& brand, const string& model, const string& type, 
        int year, const string& color, double pricePerDay, const string& regNum, bool available = true)
        : id(id), brand(brand), model(model), type(type), year(year), 
          color(color), pricePerDay(pricePerDay), registrationNumber(regNum), available(available) {}

    // Getters
    int getId() const { return id; }
    string getBrand() const { return brand; }
    string getModel() const { return model; }
    string getType() const { return type; }
    int getYear() const { return year; }
    string getColor() const { return color; }
    double getPricePerDay() const { return pricePerDay; }
    bool isAvailable() const { return available; }
    string getRegistrationNumber() const { return registrationNumber; }

    // Setters
    void setPricePerDay(double price) { pricePerDay = price; }
    void setAvailable(bool avail) { available = avail; }

    void display() const {
        cout << "ID: " << id << " | " << year << " " << brand << " " << model 
             << " (" << color << ")\n"
             << "Type: " << type << " | Reg: " << registrationNumber 
             << " | Price/Day: $" << fixed << setprecision(2) << pricePerDay
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
    string bookingDate;

public:
    Booking(int id, int userId, int carId, const string& startDate, 
            const string& endDate, double totalPrice, const string& status = "Pending")
        : id(id), userId(userId), carId(carId), startDate(startDate), 
          endDate(endDate), totalPrice(totalPrice), status(status), bookingDate(getCurrentDate()) {}

    // Getters
    int getId() const { return id; }
    int getUserId() const { return userId; }
    int getCarId() const { return carId; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    string getStatus() const { return status; }
    double getTotalPrice() const { return totalPrice; }
    string getBookingDate() const { return bookingDate; }

    // Setters
    void setStatus(const string& newStatus) { status = newStatus; }
    void setEndDate(const string& date) { endDate = date; }

    void display() const {
        cout << "Booking ID: " << id << " | Car ID: " << carId 
             << "\nDates: " << startDate << " to " << endDate
             << " | Booked on: " << bookingDate
             << "\nTotal: $" << fixed << setprecision(2) << totalPrice
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
    string transactionId;

public:
    Payment(int id, int bookingId, double amount, const string& method, 
            const string& status = "Completed")
        : id(id), bookingId(bookingId), amount(amount), method(method), 
          status(status), date(getCurrentDate()), transactionId(to_string(generateRandomId())) {}

    // Getters
    int getId() const { return id; }
    int getBookingId() const { return bookingId; }
    double getAmount() const { return amount; }
    string getDate() const { return date; }
    string getStatus() const { return status; }
    string getMethod() const { return method; }
    string getTransactionId() const { return transactionId; }

    void display() const {
        cout << "Payment ID: " << id << " | Booking ID: " << bookingId 
             << " | Transaction: " << transactionId
             << "\nAmount: $" << fixed << setprecision(2) << amount
             << " | Date: " << date << " | Method: " << method 
             << "\nStatus: " << status << endl;
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
    
    // Serialization
    virtual string serialize() const {
        return to_string(id) + "," + username + "," + password + "," + email + "," + role;
    }
};

// Logger class
class Logger {
private:
    static Logger* instance;
    mutex logMutex;
    const string logFile = "logs.txt";
    
    Logger() = default;

    string getCurrentTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << setw(2) << setfill('0') << ltm->tm_hour << ":"
           << setw(2) << setfill('0') << ltm->tm_min << ":"
           << setw(2) << setfill('0') << ltm->tm_sec;
        return ss.str();
    }

public:
    static Logger* getInstance() {
        if (!instance) {
            instance = new Logger();
        }
        return instance;
    }

    void logTransaction(const string& username, const string& email, 
                       const Car& car, const Booking& booking, 
                       const Payment& payment) {
        lock_guard<mutex> lock(logMutex);
        ofstream outFile(logFile, ios::app);
        if (!outFile) {
            cerr << "Error: Could not open log file!" << endl;
            return;
        }

        outFile << "\n=== TRANSACTION LOG ===\n";
        outFile << "Timestamp: " << getCurrentDate() << " " << getCurrentTime() << "\n";
        outFile << "Customer Details:\n";
        outFile << "  Username: " << username << "\n";
        outFile << "  Email: " << email << "\n";
        outFile << "Car Details:\n";
        outFile << "  ID: " << car.getId() << "\n";
        outFile << "  Brand: " << car.getBrand() << " " << car.getModel() << "\n";
        outFile << "  Type: " << car.getType() << "\n";
        outFile << "  Registration: " << car.getRegistrationNumber() << "\n";
        outFile << "Booking Details:\n";
        outFile << "  Booking ID: " << booking.getId() << "\n";
        outFile << "  Start Date: " << booking.getStartDate() << "\n";
        outFile << "  End Date: " << booking.getEndDate() << "\n";
        outFile << "  Duration: " << calculateDaysBetweenDates(booking.getStartDate(), booking.getEndDate()) << " days\n";
        outFile << "Payment Details:\n";
        outFile << "  Payment ID: " << payment.getId() << "\n";
        outFile << "  Amount: $" << fixed << setprecision(2) << payment.getAmount() << "\n";
        outFile << "  Method: " << payment.getMethod() << "\n";
        outFile << "  Status: " << payment.getStatus() << "\n";
        outFile << "  Transaction ID: " << payment.getTransactionId() << "\n";
        outFile << "Revenue Generated: $" << fixed << setprecision(2) << payment.getAmount() << "\n";
        outFile << "========================\n";
    }

    void logBookingUpdate(const string& username, const string& action, 
                         const Booking& booking, const Car& car) {
        lock_guard<mutex> lock(logMutex);
        ofstream outFile(logFile, ios::app);
        if (!outFile) {
            cerr << "Error: Could not open log file!" << endl;
            return;
        }

        outFile << "\n=== BOOKING UPDATE ===\n";
        outFile << "Timestamp: " << getCurrentDate() << " " << getCurrentTime() << "\n";
        outFile << "Action: " << action << "\n";
        outFile << "Customer: " << username << "\n";
        outFile << "Booking Details:\n";
        outFile << "  Booking ID: " << booking.getId() << "\n";
        outFile << "  Car: " << car.getBrand() << " " << car.getModel() << "\n";
        outFile << "  Status: " << booking.getStatus() << "\n";
        outFile << "========================\n";
    }

    vector<string> readLogs() {
        lock_guard<mutex> lock(logMutex);
        vector<string> logs;
        ifstream inFile(logFile);
        if (!inFile) {
            return logs;
        }

        string line;
        while (getline(inFile, line)) {
            logs.push_back(line);
        }
        return logs;
    }
};

// Initialize static member
Logger* Logger::instance = nullptr;

// Forward declaration
class CarRentalSystem;

// Customer class
class Customer : public User {
private:
    vector<Booking> bookings;
    vector<Payment> payments;

public:
    Customer(int id, const string& username, const string& password, const string& email)
        : User(id, username, password, email, "customer") {}

    void displayMenu() override;
    void searchCars(const CarRentalSystem& system) const;
    void bookCar(CarRentalSystem& system);
    void viewBookings() const;
    void cancelBooking(CarRentalSystem& system);
    void viewRentalHistory() const;
    void makePayment();
};

// Admin class
class Admin : public User {
public:
    Admin(int id, const string& username, const string& password, const string& email)
        : User(id, username, password, email, "admin") {}

    void displayMenu() override;
    void manageCars(CarRentalSystem& system);
    void addCar(CarRentalSystem& system);
    void updateCar(CarRentalSystem& system);
    void removeCar(CarRentalSystem& system);
    void viewAllCars(const CarRentalSystem& system) const;
    void manageBookings(CarRentalSystem& system);
    void viewPaymentRecords() const;
    void manageUsers(CarRentalSystem& system);
    void generateReports() const;
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
    const string userDataFile = "users.dat";

    // Private constructor for singleton
    CarRentalSystem() {
        loadUserData();
        // Initialize with some sample data if no users exist
        if (users.empty()) {
            users.push_back(make_unique<Admin>(nextUserId++, "admin", "admin123", "admin@carrental.com"));
            users.push_back(make_unique<Customer>(nextUserId++, "john", "john123", "john@example.com"));
            users.push_back(make_unique<Customer>(nextUserId++, "alice", "alice123", "alice@example.com"));
            saveUserData();
        }

        cars.emplace_back(nextCarId++, "Toyota", "Camry", "Sedan", 2022, "Blue", 50.0, "ABC123");
        cars.emplace_back(nextCarId++, "Honda", "Civic", "Sedan", 2021, "Red", 45.0, "DEF456");
        cars.emplace_back(nextCarId++, "Ford", "Explorer", "SUV", 2023, "Black", 70.0, "GHI789");
        cars.emplace_back(nextCarId++, "Chevrolet", "Silverado", "Truck", 2020, "White", 85.0, "JKL012");
    }

    void loadUserData() {
        ifstream inFile(userDataFile);
        if (!inFile) {
            // File doesn't exist yet, that's okay
            return;
        }
        string line;
        while (getline(inFile, line)) {
            istringstream iss(line);
            string token;
            vector<string> tokens;
            while (getline(iss, token, ',')) {
                tokens.push_back(token);
            }
            if (tokens.size() != 5) continue; // Skip invalid lines
            int id = stoi(tokens[0]);
            string username = tokens[1];
            string password = tokens[2];
            string email = tokens[3];
            string role = tokens[4];
            if (role == "admin") {
                users.push_back(make_unique<Admin>(id, username, password, email));
            } else {
                users.push_back(make_unique<Customer>(id, username, password, email));
            }
            // Update nextUserId to be higher than any existing ID
            if (id >= nextUserId) {
                nextUserId = id + 1;
            }
        }
    }

    void saveUserData() {
        ofstream outFile(userDataFile);
        if (!outFile) {
            cerr << "Error: Could not open user data file for writing!" << endl;
            return;
        }
        for (const auto& user : users) {
            outFile << user->serialize() << endl;
        }
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
        saveUserData(); // Save the updated user list to file
        cout << "Registration successful! You can now login.\n";
    }

    void addCar(const Car& car) {
        cars.push_back(car);
    }

    void removeCar(int carId) {
        auto it = find_if(cars.begin(), cars.end(), 
            [carId](const Car& car) { return car.getId() == carId; });
        
        if (it == cars.end()) {
            throw CarNotFoundException();
        }
        
        cars.erase(it);
    }

    Car& getCarById(int carId) {
        auto it = find_if(cars.begin(), cars.end(), 
            [carId](const Car& car) { return car.getId() == carId; });
        
        if (it == cars.end()) {
            throw CarNotFoundException();
        }
        
        return *it;
    }

    const vector<Car>& getAllCars() const {
        return cars;
    }

    vector<Car> getAllAvailableCars() const {
        vector<Car> availableCars;
        copy_if(cars.begin(), cars.end(), back_inserter(availableCars),
            [](const Car& car) { return car.isAvailable(); });
        return availableCars;
    }

    void addBooking(const Booking& booking) {
        bookings.push_back(booking);
    }

    vector<Booking>& getAllBookings() {
        return bookings;
    }

    void viewAllCars() const {
        if (cars.empty()) {
            cout << "No cars in the system.\n";
            return;
        }

        for (const auto& car : cars) {
            car.display();
            cout << "------------------------\n";
        }
    }

    void viewAllBookings() const {
        if (bookings.empty()) {
            cout << "No bookings in the system.\n";
            return;
        }

        for (const auto& booking : bookings) {
            booking.display();
            cout << "------------------------\n";
        }
    }

    const vector<unique_ptr<User>>& getUsers() const {
        return users;
    }

    void run();

private:
    void login();
    void registerNewUser();
};

// Initialize static member
CarRentalSystem* CarRentalSystem::instance = nullptr;

int main() {
    CarRentalSystem* system = CarRentalSystem::getInstance();
    system->run();
    return 0;
}

void Admin::displayMenu() {
    CarRentalSystem* system = CarRentalSystem::getInstance();
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
                case 1: manageCars(*system); break;
                case 2: manageBookings(*system); break;
                case 3: viewPaymentRecords(); break;
                case 4: manageUsers(*system); break;
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

void Admin::manageCars(CarRentalSystem& system) {
    int choice;
    do {
        clearScreen();
        cout << "\n=== Manage Cars ===\n";
        cout << "1. Add New Car\n";
        cout << "2. Update Car Details\n";
        cout << "3. Remove Car\n";
        cout << "4. View All Cars\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: addCar(system); break;
            case 2: updateCar(system); break;
            case 3: removeCar(system); break;
            case 4: viewAllCars(system); break;
            case 0: break;
            default: cout << "Invalid choice!\n"; break;
        }
        if (choice != 0) pressEnterToContinue();
    } while (choice != 0);
}

void Admin::addCar(CarRentalSystem& system) {
    cout << "\n--- Add New Car ---\n";
    string brand, model, type, color, regNum;
    int year;
    double pricePerDay;

    cout << "Enter brand: ";
    getline(cin, brand);
    cout << "Enter model: ";
    getline(cin, model);
    cout << "Enter type (Sedan/SUV/Truck): ";
    getline(cin, type);
    cout << "Enter year: ";
    cin >> year;
    cin.ignore();
    cout << "Enter color: ";
    getline(cin, color);
    cout << "Enter registration number: ";
    getline(cin, regNum);
    cout << "Enter price per day: ";
    cin >> pricePerDay;
    cin.ignore();

    int carId = generateRandomId();
    system.addCar(Car(carId, brand, model, type, year, color, pricePerDay, regNum));
    cout << "Car added successfully with ID: " << carId << endl;
}

void Admin::updateCar(CarRentalSystem& system) {
    cout << "\n--- Update Car Details ---\n";
    system.viewAllCars();
    
    cout << "Enter Car ID to update: ";
    int carId;
    cin >> carId;
    cin.ignore();

    try {
        Car& car = system.getCarById(carId);
        car.display();

        cout << "Select field to update:\n";
        cout << "1. Price Per Day\n";
        cout << "2. Availability\n";
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                cout << "Enter new price per day: ";
                double newPrice;
                cin >> newPrice;
                cin.ignore();
                car.setPricePerDay(newPrice);
                cout << "Price updated successfully.\n";
                break;
            }
            case 2: {
                cout << "Set availability (1 for Available, 0 for Rented): ";
                bool available;
                cin >> available;
                cin.ignore();
                car.setAvailable(available);
                cout << "Availability updated successfully.\n";
                break;
            }
            default:
                cout << "Invalid choice.\n";
        }
    } catch (const CarNotFoundException& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void Admin::removeCar(CarRentalSystem& system) {
    cout << "\n--- Remove Car ---\n";
    system.viewAllCars();
    
    cout << "Enter Car ID to remove: ";
    int carId;
    cin >> carId;
    cin.ignore();

    try {
        system.removeCar(carId);
        cout << "Car removed successfully.\n";
    } catch (const CarNotFoundException& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void Admin::viewAllCars(const CarRentalSystem& system) const {
    cout << "\n--- All Cars ---\n";
    system.viewAllCars();
}

void Admin::manageBookings(CarRentalSystem& system) {
    int choice;
    do {
        clearScreen();
        cout << "\n=== Manage Bookings ===\n";
        cout << "1. View All Bookings\n";
        cout << "2. Approve/Reject Booking\n";
        cout << "3. View Booking History\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                cout << "\n--- All Bookings ---\n";
                system.viewAllBookings();
                break;
            }
            case 2: {
                cout << "\n--- Approve/Reject Booking ---\n";
                system.viewAllBookings();
                
                cout << "Enter Booking ID to manage: ";
                int bookingId;
                cin >> bookingId;
                cin.ignore();

                vector<Booking>& bookings = system.getAllBookings();
                auto it = find_if(bookings.begin(), bookings.end(),
                    [bookingId](const Booking& b) { return b.getId() == bookingId; });

                if (it != bookings.end()) {
                    cout << "\nSelected Booking:\n";
                    it->display();
                    cout << "\n1. Approve\n2. Reject\nChoice: ";
                    int action;
                    cin >> action;
                    cin.ignore();

                    string status = (action == 1) ? "Approved" : "Rejected";
                    it->setStatus(status);

                    try {
                        Car& car = system.getCarById(it->getCarId());
                        car.setAvailable(status == "Rejected");
                        
                        // Find the customer username
                        string username = "Unknown";
                        for (const auto& user : system.getUsers()) {
                            if (user->getId() == it->getUserId()) {
                                username = user->getUsername();
                                break;
                            }
                        }
                        
                        Logger::getInstance()->logBookingUpdate(username, status, *it, car);
                        cout << "Booking " << status << " successfully.\n";
                    } catch (const exception& e) {
                        cout << "Error: " << e.what() << endl;
                    }
                } else {
                    cout << "Booking not found.\n";
                }
                break;
            }
            case 3: {
                cout << "\n--- Booking History ---\n";
                auto logs = Logger::getInstance()->readLogs();
                for (const auto& log : logs) {
                    cout << log << endl;
                }
                break;
            }
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
                break;
        }
        if (choice != 0) pressEnterToContinue();
    } while (choice != 0);
}

void Admin::viewPaymentRecords() const {
    cout << "\n--- Payment Records ---\n";
    cout << "1. View All Transactions\n";
    cout << "2. View Revenue Report\n";
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;
    cin.ignore();

    auto logs = Logger::getInstance()->readLogs();
    double totalRevenue = 0.0;
    map<string, double> revenueByMethod;
    
    switch (choice) {
        case 1: {
            cout << "\n=== Transaction History ===\n";
            for (const auto& log : logs) {
                cout << log << endl;
            }
            break;
        }
        case 2: {
            cout << "\n=== Revenue Report ===\n";
            for (const auto& log : logs) {
                if (log.find("Revenue Generated: $") != string::npos) {
                    size_t pos = log.find("$");
                    if (pos != string::npos) {
                        try {
                            double amount = stod(log.substr(pos + 1));
                            totalRevenue += amount;
                        } catch (...) {
                            // Skip invalid amounts
                        }
                    }
                }
                if (log.find("Method: ") != string::npos) {
                    size_t methodPos = log.find("Method: ") + 8;
                    size_t amountPos = log.find("$");
                    if (methodPos != string::npos && amountPos != string::npos) {
                        string method = log.substr(methodPos, log.find("\n", methodPos) - methodPos);
                        try {
                            double amount = stod(log.substr(amountPos + 1));
                            revenueByMethod[method] += amount;
                        } catch (...) {
                            // Skip invalid amounts
                        }
                    }
                }
            }
            
            cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << "\n\n";
            cout << "Revenue by Payment Method:\n";
            for (const auto& [method, amount] : revenueByMethod) {
                cout << method << ": $" << fixed << setprecision(2) << amount 
                     << " (" << (amount/totalRevenue * 100) << "%)\n";
            }
            break;
        }
        default:
            cout << "Invalid choice!\n";
            break;
    }
}

void Admin::manageUsers(CarRentalSystem& system) {
    int choice;
    do {
        clearScreen();
        cout << "\n=== Manage Users ===\n";
        cout << "1. View All Users\n";
        cout << "2. View User Activity\n";
        cout << "3. Block/Unblock User\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                cout << "\n--- All Users ---\n";
                for (const auto& user : system.getUsers()) {
                    cout << "ID: " << user->getId() 
                         << " | Username: " << user->getUsername()
                         << " | Email: " << user->getEmail()
                         << " | Role: " << user->getRole() << endl;
                }
                break;
            }
            case 2: {
                cout << "\n--- User Activity ---\n";
                cout << "Enter username to view activity: ";
                string username;
                getline(cin, username);
                
                cout << "\nActivity for user '" << username << "':\n";
                auto logs = Logger::getInstance()->readLogs();
                bool found = false;
                for (const auto& log : logs) {
                    if (log.find("Username: " + username) != string::npos ||
                        log.find("Customer: " + username) != string::npos) {
                        cout << log << endl;
                        found = true;
                    }
                }
                if (!found) {
                    cout << "No activity found for this user.\n";
                }
                break;
            }
            case 3:
                cout << "User blocking functionality would be implemented here.\n";
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
                break;
        }
        if (choice != 0) pressEnterToContinue();
    } while (choice != 0);
}

void Admin::generateReports() const {
    int choice;
    cout << "\n=== Generate Reports ===\n";
    cout << "1. Revenue Report\n";
    cout << "2. Booking Statistics\n";
    cout << "3. Popular Cars Report\n";
    cout << "4. Customer Activity Report\n";
    cout << "Enter choice: ";
    
    cin >> choice;
    cin.ignore();

    auto logs = Logger::getInstance()->readLogs();
    
    switch (choice) {
        case 1: {
            // Revenue Report (reuse from viewPaymentRecords)
            viewPaymentRecords();
            break;
        }
        case 2: {
            cout << "\n=== Booking Statistics ===\n";
            map<string, int> bookingsByStatus;
            map<string, int> bookingsByMonth;
            
            for (const auto& log : logs) {
                if (log.find("Status: ") != string::npos) {
                    size_t pos = log.find("Status: ") + 8;
                    string status = log.substr(pos, log.find("\n", pos) - pos);
                    bookingsByStatus[status]++;
                }
                if (log.find("Start Date: ") != string::npos) {
                    size_t pos = log.find("Start Date: ") + 12;
                    string date = log.substr(pos, 7); // YYYY-MM
                    bookingsByMonth[date]++;
                }
            }
            
            cout << "Bookings by Status:\n";
            for (const auto& [status, count] : bookingsByStatus) {
                cout << status << ": " << count << endl;
            }
            
            cout << "\nBookings by Month:\n";
            for (const auto& [month, count] : bookingsByMonth) {
                cout << month << ": " << count << endl;
            }
            break;
        }
        case 3: {
            cout << "\n=== Popular Cars Report ===\n";
            map<string, int> carBookings;
            map<string, double> carRevenue;
            
            for (const auto& log : logs) {
                if (log.find("Brand: ") != string::npos) {
                    size_t brandPos = log.find("Brand: ") + 7;
                    string car = log.substr(brandPos, log.find("\n", brandPos) - brandPos);
                    carBookings[car]++;
                    
                    size_t revenuePos = log.find("Revenue Generated: $");
                    if (revenuePos != string::npos) {
                        try {
                            double amount = stod(log.substr(revenuePos + 19));
                            carRevenue[car] += amount;
                        } catch (...) {
                            // Skip invalid amounts
                        }
                    }
                }
            }
            
            cout << "Car Booking Frequency:\n";
            for (const auto& [car, count] : carBookings) {
                cout << car << ":\n";
                cout << "  Bookings: " << count << "\n";
                cout << "  Revenue: $" << fixed << setprecision(2) << carRevenue[car] << "\n";
            }
            break;
        }
        case 4: {
            cout << "\n=== Customer Activity Report ===\n";
            map<string, int> customerBookings;
            map<string, double> customerSpending;
            
            for (const auto& log : logs) {
                if (log.find("Username: ") != string::npos) {
                    size_t userPos = log.find("Username: ") + 10;
                    string username = log.substr(userPos, log.find("\n", userPos) - userPos);
                    customerBookings[username]++;
                    
                    size_t amountPos = log.find("Amount: $");
                    if (amountPos != string::npos) {
                        try {
                            double amount = stod(log.substr(amountPos + 9));
                            customerSpending[username] += amount;
                        } catch (...) {
                            // Skip invalid amounts
                        }
                    }
                }
            }
            
            cout << "Customer Activity:\n";
            for (const auto& [username, count] : customerBookings) {
                cout << "Customer: " << username << "\n";
                cout << "  Total Bookings: " << count << "\n";
                cout << "  Total Spending: $" << fixed << setprecision(2) 
                     << customerSpending[username] << "\n";
            }
            break;
        }
        default:
            cout << "Invalid choice!\n";
            break;
    }
}

void CarRentalSystem::run() {
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

void CarRentalSystem::login() {
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

void CarRentalSystem::registerNewUser() {
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

void Customer::displayMenu() {
    CarRentalSystem* system = CarRentalSystem::getInstance();
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
                case 1: searchCars(*system); break;
                case 2: bookCar(*system); break;
                case 3: viewBookings(); break;
                case 4: cancelBooking(*system); break;
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

void Customer::searchCars(const CarRentalSystem& system) const {
    cout << "\n--- Search Cars ---\n";
    cout << "Filter options:\n";
    cout << "1. By Brand\n";
    cout << "2. By Type\n";
    cout << "3. By Price Range\n";
    cout << "4. Show All Available Cars\n";
    cout << "Enter your choice: ";
    
    int choice;
    cin >> choice;
    cin.ignore();

    vector<Car> availableCars = system.getAllAvailableCars();

    if (availableCars.empty()) {
        cout << "No available cars found.\n";
        return;
    }

    vector<Car> filteredCars;

    switch (choice) {
        case 1: {
            cout << "Enter brand name (or part of it): ";
            string brand;
            getline(cin, brand);
            transform(brand.begin(), brand.end(), brand.begin(), ::tolower);

            copy_if(availableCars.begin(), availableCars.end(), back_inserter(filteredCars),
                [&brand](const Car& car) {
                    string carBrand = car.getBrand();
                    transform(carBrand.begin(), carBrand.end(), carBrand.begin(), ::tolower);
                    return carBrand.find(brand) != string::npos;
                });
            break;
        }
        case 2: {
            cout << "Enter type (Sedan/SUV/Truck): ";
            string type;
            getline(cin, type);
            transform(type.begin(), type.end(), type.begin(), ::tolower);

            copy_if(availableCars.begin(), availableCars.end(), back_inserter(filteredCars),
                [&type](const Car& car) {
                    string carType = car.getType();
                    transform(carType.begin(), carType.end(), carType.begin(), ::tolower);
                    return carType.find(type) != string::npos;
                });
            break;
        }
        case 3: {
            cout << "Enter minimum price: ";
            double minPrice;
            cin >> minPrice;
            cout << "Enter maximum price: ";
            double maxPrice;
            cin >> maxPrice;
            cin.ignore();

            copy_if(availableCars.begin(), availableCars.end(), back_inserter(filteredCars),
                [minPrice, maxPrice](const Car& car) {
                    return car.getPricePerDay() >= minPrice && car.getPricePerDay() <= maxPrice;
                });
            break;
        }
        case 4:
            filteredCars = availableCars;
            break;
        default:
            cout << "Invalid choice. Showing all available cars.\n";
            filteredCars = availableCars;
    }

    if (filteredCars.empty()) {
        cout << "No cars match your criteria.\n";
    } else {
        cout << "\nFound " << filteredCars.size() << " car(s):\n";
        cout << "========================================\n";
        for (const auto& car : filteredCars) {
            car.display();
            cout << "----------------------------------------\n";
        }
    }
}

void Customer::bookCar(CarRentalSystem& system) {
    cout << "\n--- Book a Car ---\n";
    vector<Car> availableCars = system.getAllAvailableCars();
    
    if (availableCars.empty()) {
        cout << "No cars available for booking at the moment.\n";
        return;
    }

    cout << "Available Cars:\n";
    cout << "========================================\n";
    for (const auto& car : availableCars) {
        car.display();
        cout << "----------------------------------------\n";
    }

    cout << "Enter Car ID to book: ";
    int carId;
    cin >> carId;
    cin.ignore();

    try {
        Car& car = system.getCarById(carId);
        if (!car.isAvailable()) {
            cout << "This car is not available for booking.\n";
            return;
        }

        cout << "Selected Car:\n";
        car.display();
        cout << "----------------------------------------\n";

        string startDate, endDate;
        bool validDates = false;
        
        while (!validDates) {
            cout << "Enter start date (YYYY-MM-DD): ";
            getline(cin, startDate);
            
            cout << "Enter end date (YYYY-MM-DD): ";
            getline(cin, endDate);
            
            // Simple date validation
            if (startDate.length() == 10 && endDate.length() == 10 && 
                startDate < endDate) {
                validDates = true;
            } else {
                cout << "Invalid dates! Please make sure:\n";
                cout << "1. Dates are in YYYY-MM-DD format\n";
                cout << "2. End date is after start date\n";
            }
        }

        int rentalDays = calculateDaysBetweenDates(startDate, endDate);
        double totalPrice = car.getPricePerDay() * rentalDays;

        int bookingId = generateRandomId();
        Booking newBooking(bookingId, this->id, carId, startDate, endDate, totalPrice);
        bookings.push_back(newBooking);
        system.addBooking(newBooking);
        car.setAvailable(false);

        cout << "\nBooking successful!\n";
        cout << "========================================\n";
        cout << "Booking ID: " << bookingId << "\n";
        cout << "Car: " << car.getBrand() << " " << car.getModel() << "\n";
        cout << "Rental Period: " << startDate << " to " << endDate << "\n";
        cout << "Total Days: " << rentalDays << "\n";
        cout << "Price per Day: $" << fixed << setprecision(2) << car.getPricePerDay() << "\n";
        cout << "Total Price: $" << fixed << setprecision(2) << totalPrice << "\n";
        cout << "========================================\n";
    } catch (const CarNotFoundException& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void Customer::viewBookings() const {
    cout << "\n--- My Bookings ---\n";
    if (bookings.empty()) {
        cout << "No bookings found.\n";
        return;
    }

    CarRentalSystem* system = CarRentalSystem::getInstance();
    const auto& systemBookings = system->getAllBookings();

    for (const auto& booking : bookings) {
        // Find the corresponding system booking to get the latest status
        auto it = find_if(systemBookings.begin(), systemBookings.end(),
            [&booking](const Booking& b) { return b.getId() == booking.getId(); });
        
        if (it != systemBookings.end()) {
            // Display with the latest status from the system
            cout << "Booking ID: " << it->getId() << "\n";
            cout << "Status: " << it->getStatus() << "\n";
            cout << "Dates: " << it->getStartDate() << " to " << it->getEndDate() << "\n";
            cout << "Total Price: $" << fixed << setprecision(2) << it->getTotalPrice() << "\n";
            cout << "------------------------\n";
        } else {
            // Fallback to local booking data if not found in system
            booking.display();
            cout << "------------------------\n";
        }
    }
}

void Customer::cancelBooking(CarRentalSystem& system) {
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
        if (it->getStatus() == "Cancelled") {
            cout << "This booking is already cancelled.\n";
            return;
        }

        it->setStatus("Cancelled");
        try {
            Car& car = system.getCarById(it->getCarId());
            car.setAvailable(true);
            cout << "Booking cancelled successfully.\n";
        } catch (const CarNotFoundException& e) {
            cout << "Error: " << e.what() << endl;
        }
    } else {
        cout << "Booking not found.\n";
    }
}

void Customer::viewRentalHistory() const {
    cout << "\n--- Rental History ---\n";
    
    if (bookings.empty()) {
        cout << "No rental history found.\n";
        return;
    }

    // Sort bookings by start date (newest first)
    vector<Booking> sortedBookings = bookings;
    sort(sortedBookings.begin(), sortedBookings.end(),
        [](const Booking& a, const Booking& b) {
            return a.getStartDate() > b.getStartDate();
        });

    cout << "You have " << sortedBookings.size() << " booking(s):\n";
    cout << "========================================\n";
    
    for (const auto& booking : sortedBookings) {
        cout << "Booking ID: " << booking.getId() << "\n";
        cout << "Status: " << booking.getStatus() << "\n";
        cout << "Dates: " << booking.getStartDate() << " to " << booking.getEndDate() << "\n";
        cout << "Total Price: $" << fixed << setprecision(2) << booking.getTotalPrice() << "\n";
        
        // Find corresponding payment if exists
        auto paymentIt = find_if(payments.begin(), payments.end(),
            [&booking](const Payment& p) { return p.getBookingId() == booking.getId(); });
        
        if (paymentIt != payments.end()) {
            cout << "Payment Method: " << paymentIt->getMethod() << "\n";
            cout << "Payment Status: " << paymentIt->getStatus() << "\n";
        } else {
            cout << "Payment: Pending\n";
        }
        
        cout << "----------------------------------------\n";
    }
}

void Customer::makePayment() {
    cout << "\n--- Make Payment ---\n";
    if (bookings.empty()) {
        cout << "No bookings requiring payment.\n";
        return;
    }
    
    // Show pending bookings
    vector<Booking*> pendingBookings;
    for (auto& booking : bookings) {
        if (booking.getStatus() == "Pending") {
            pendingBookings.push_back(&booking);
        }
    }

    if (pendingBookings.empty()) {
        cout << "No pending bookings requiring payment.\n";
        return;
    }

    cout << "Pending Bookings:\n";
    for (const auto& booking : pendingBookings) {
        booking->display();
        cout << "------------------------\n";
    }

    cout << "Enter Booking ID to pay for: ";
    int bookingId;
    cin >> bookingId;
    cin.ignore();

    auto it = find_if(bookings.begin(), bookings.end(), 
        [bookingId](const Booking& b) { return b.getId() == bookingId && b.getStatus() == "Pending"; });

    if (it == bookings.end()) {
        cout << "Booking not found or already paid.\n";
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
    string method;
    switch (choice) {
        case 1: 
            strategy = make_unique<CreditCardStrategy>(); 
            method = "Credit Card";
            break;
        case 2: 
            strategy = make_unique<PayPalStrategy>(); 
            method = "PayPal";
            break;
        case 3: 
            strategy = make_unique<CashStrategy>(); 
            method = "Cash";
            break;
        default: 
            cout << "Invalid choice.\n";
            return;
    }

    cout << "Processing payment of $" << fixed << setprecision(2) << it->getTotalPrice() << "...\n";
    strategy->pay(it->getTotalPrice());
    
    // Record payment
    int paymentId = generateRandomId();
    Payment payment(paymentId, bookingId, it->getTotalPrice(), method);
    payments.push_back(payment);
    it->setStatus("Confirmed");
    
    // Log the transaction
    try {
        CarRentalSystem* system = CarRentalSystem::getInstance();
        Car& car = system->getCarById(it->getCarId());
        Logger::getInstance()->logTransaction(username, email, car, *it, payment);
    } catch (const exception& e) {
        cerr << "Error logging transaction: " << e.what() << endl;
    }
    
    cout << "Payment completed successfully via " << method << ".\n";
    cout << "Payment ID: " << paymentId << endl;
} 
