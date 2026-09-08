#include <bits/stdc++.h>
using namespace std;

enum class VehicleType {
    ECONOMY,
    SUV,
    LUXURY
};

enum class VehicleStatus {
    AVAILABLE,
    RESERVED,
    RENTED,
    MAINTENANCE
};

enum class ReservationStatus {
    CONFIRMED,
    CANCELLED
};

enum class PaymentStatus {
    SUCCESS,
    FAILED
};

class User {
public:
    int id;
    string name;
    string email;

    User(int id, string name, string email)
        : id(id), name(name), email(email) {}
};

class Vehicle {
public:
    int id;
    string registrationNo;
    string make;
    string model;
    int year;
    VehicleType type;
    double pricePerDay;
    VehicleStatus status;

    Vehicle(int id, string reg, string make, string model,
            int year, VehicleType type, double price)
        : id(id), registrationNo(reg), make(make),
          model(model), year(year), type(type),
          pricePerDay(price),
          status(VehicleStatus::AVAILABLE) {}

    bool isAvailable() {
        return status == VehicleStatus::AVAILABLE;
    }
};

class Store {
public:
    int id;
    string location;
    vector<Vehicle*> vehicles;

    Store(int id, string location)
        : id(id), location(location) {}

    void addVehicle(Vehicle* vehicle) {
        vehicles.push_back(vehicle);
    }

    vector<Vehicle*> searchVehicles(VehicleType type) {
        vector<Vehicle*> result;

        for (auto vehicle : vehicles) {
            if (vehicle->isAvailable() &&
                vehicle->type == type) {
                result.push_back(vehicle);
            }
        }

        return result;
    }
};

class Payment {
public:
    PaymentStatus pay(double amount) {
        cout << "Payment of Rs." << amount << " successful\n";
        return PaymentStatus::SUCCESS;
    }
};

class Reservation {
public:
    int id;
    User* user;
    Vehicle* vehicle;
    string startDate;
    string endDate;
    double amount;
    ReservationStatus status;

    Reservation(int id, User* user, Vehicle* vehicle,
                string startDate, string endDate, double amount)
        : id(id), user(user), vehicle(vehicle),
          startDate(startDate), endDate(endDate),
          amount(amount),
          status(ReservationStatus::CONFIRMED) {}
};

class RentalSystem {
private:
    vector<Store*> stores;
    vector<Reservation*> reservations;
    Payment payment;
    int reservationId = 1;

public:
    void addStore(Store* store) {
        stores.push_back(store);
    }

    vector<Vehicle*> search(
        string location,
        VehicleType type
    ) {
        for (auto store : stores) {
            if (store->location == location) {
                return store->searchVehicles(type);
            }
        }

        return {};
    }

    Reservation* reserveVehicle(
        User* user,
        Vehicle* vehicle,
        string startDate,
        string endDate,
        int days
    ) {
        if (!vehicle->isAvailable()) {
            cout << "Vehicle is not available\n";
            return nullptr;
        }

        double amount = days * vehicle->pricePerDay;

        if (payment.pay(amount) == PaymentStatus::FAILED) {
            return nullptr;
        }

        vehicle->status = VehicleStatus::RESERVED;

        Reservation* reservation =
            new Reservation(
                reservationId++,
                user,
                vehicle,
                startDate,
                endDate,
                amount
            );

        reservations.push_back(reservation);

        cout << "Reservation successful\n";

        return reservation;
    }

    void cancelReservation(Reservation* reservation) {
        if (reservation == nullptr ||
            reservation->status == ReservationStatus::CANCELLED) {
            return;
        }

        reservation->status = ReservationStatus::CANCELLED;
        reservation->vehicle->status = VehicleStatus::AVAILABLE;

        cout << "Reservation cancelled\n";
    }

    void startRental(Reservation* reservation) {
        if (reservation == nullptr ||
            reservation->status == ReservationStatus::CANCELLED) {
            return;
        }

        reservation->vehicle->status = VehicleStatus::RENTED;

        cout << "Rental started\n";
    }

    void returnVehicle(Reservation* reservation) {
        if (reservation == nullptr) {
            return;
        }

        reservation->vehicle->status = VehicleStatus::AVAILABLE;

        cout << "Vehicle returned\n";
    }
};

int main() {

    RentalSystem system;

    Store* chennai = new Store(1, "Chennai");

    Vehicle* car1 = new Vehicle(
        1,
        "TN01AB1234",
        "Toyota",
        "Camry",
        2024,
        VehicleType::LUXURY,
        3000
    );

    Vehicle* car2 = new Vehicle(
        2,
        "TN01CD5678",
        "Hyundai",
        "Creta",
        2023,
        VehicleType::SUV,
        2000
    );

    chennai->addVehicle(car1);
    chennai->addVehicle(car2);

    system.addStore(chennai);

    User user(
        1,
        "Navin",
        "navin@gmail.com"
    );

    // Search
    vector<Vehicle*> cars =
        system.search(
            "Chennai",
            VehicleType::SUV
        );

    for (auto car : cars) {
        cout << car->make << " "
             << car->model << " "
             << car->pricePerDay << endl;
    }

    // Reserve
    Reservation* reservation =
        system.reserveVehicle(
            &user,
            car2,
            "2026-09-10",
            "2026-09-12",
            2
        );

    // Start rental
    system.startRental(reservation);

    // Return
    system.returnVehicle(reservation);

    return 0;
}

// Hyundai Creta 2000
// Payment of Rs.4000 successful
// Reservation successful
// Rental started
// Vehicle returned