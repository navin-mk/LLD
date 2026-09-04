#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <stdexcept>

using namespace std;


// ======================================================
// ENUMS
// ======================================================

enum class VehicleType {
    BIKE,
    CAR,
    TRUCK
};

enum class SlotType {
    BIKE,
    CAR,
    TRUCK
};


// ======================================================
// VEHICLE
// ======================================================

class Vehicle {
private:
    string number;
    VehicleType type;

public:

    Vehicle(string number, VehicleType type)
        : number(number), type(type) {}

    string getNumber() {
        return number;
    }

    VehicleType getType() {
        return type;
    }
};


// ======================================================
// PARKING SLOT
// ======================================================

class ParkingSlot {
private:
    int id;
    SlotType type;
    bool occupied;
    Vehicle* vehicle;

public:

    ParkingSlot(int id, SlotType type)
        : id(id),
          type(type),
          occupied(false),
          vehicle(nullptr) {}

    bool isFree() {
        return !occupied;
    }

    SlotType getType() {
        return type;
    }

    int getId() {
        return id;
    }

    void parkVehicle(Vehicle* v) {

        if (occupied) {
            throw runtime_error(
                "Slot is already occupied"
            );
        }

        vehicle = v;
        occupied = true;
    }

    void removeVehicle() {

        if (!occupied) {
            throw runtime_error(
                "Slot is already empty"
            );
        }

        vehicle = nullptr;
        occupied = false;
    }
};


// ======================================================
// TICKET
// ======================================================

class Ticket {
private:
    int id;
    Vehicle* vehicle;
    ParkingSlot* slot;

    chrono::system_clock::time_point entryTime;

public:

    Ticket(
        int id,
        Vehicle* vehicle,
        ParkingSlot* slot
    )
        : id(id),
          vehicle(vehicle),
          slot(slot),
          entryTime(
              chrono::system_clock::now()
          ) {}

    int getId() {
        return id;
    }

    Vehicle* getVehicle() {
        return vehicle;
    }

    ParkingSlot* getSlot() {
        return slot;
    }

    chrono::system_clock::time_point
    getEntryTime() {

        return entryTime;
    }
};


// ======================================================
// PAYMENT
// ======================================================

class Payment {
public:

    virtual bool pay(double amount) = 0;

    virtual ~Payment() {}
};


// ======================================================
// CASH PAYMENT
// ======================================================

class CashPayment : public Payment {

public:

    bool pay(double amount) override {

        cout << "Paid ?"
             << amount
             << " using Cash\n";

        return true;
    }
};


// ======================================================
// CARD PAYMENT
// ======================================================

class CardPayment : public Payment {

public:

    bool pay(double amount) override {

        cout << "Paid ?"
             << amount
             << " using Card\n";

        return true;
    }
};


// ======================================================
// UPI PAYMENT
// ======================================================

class UPIPayment : public Payment {

public:

    bool pay(double amount) override {

        cout << "Paid ?"
             << amount
             << " using UPI\n";

        return true;
    }
};


// ======================================================
// PARKING FLOOR
// ======================================================

class ParkingFloor {
private:
    int floorNumber;

    vector<ParkingSlot> slots;

public:

    ParkingFloor(int floorNumber)
        : floorNumber(floorNumber) {}

    void addSlot(ParkingSlot slot) {

        slots.push_back(slot);
    }

    ParkingSlot* findAvailableSlot(
        VehicleType vehicleType
    ) {

        for (auto& slot : slots) {

            if (!slot.isFree())
                continue;

            if (
                (vehicleType == VehicleType::BIKE &&
                 slot.getType() == SlotType::BIKE)

                ||

                (vehicleType == VehicleType::CAR &&
                 slot.getType() == SlotType::CAR)

                ||

                (vehicleType == VehicleType::TRUCK &&
                 slot.getType() == SlotType::TRUCK)
            ) {

                return &slot;
            }
        }

        return nullptr;
    }
};


// ======================================================
// PARKING LOT
// ======================================================

class ParkingLot {
private:

    vector<ParkingFloor> floors;

    unordered_map<int, Ticket> tickets;

    int nextTicketId = 1;


    // -----------------------------------------------
    // Calculate Parking Fee
    // -----------------------------------------------

    double calculateFee(Ticket* ticket) {

        auto now =
            chrono::system_clock::now();

        auto duration =
            chrono::duration_cast<
                chrono::hours
            >(
                now - ticket->getEntryTime()
            );

        long long hours = duration.count();

        // Minimum one hour
        hours = max(1LL, hours);

        VehicleType type =
            ticket->getVehicle()->getType();

        double rate;

        if (type == VehicleType::BIKE)
            rate = 20;

        else if (type == VehicleType::CAR)
            rate = 40;

        else
            rate = 60;

        return hours * rate;
    }


public:

    // -----------------------------------------------
    // Add Floor
    // -----------------------------------------------

    void addFloor(ParkingFloor floor) {

        floors.push_back(floor);
    }


    // -----------------------------------------------
    // Park Vehicle
    // -----------------------------------------------

    int parkVehicle(Vehicle* vehicle) {

        for (auto& floor : floors) {

            ParkingSlot* slot =
                floor.findAvailableSlot(
                    vehicle->getType()
                );

            if (slot != nullptr) {

                slot->parkVehicle(vehicle);

                Ticket ticket(
                    nextTicketId++,
                    vehicle,
                    slot
                );

                int ticketId =
                    ticket.getId();

                tickets.emplace(
                    ticketId,
                    ticket
                );

                cout << "\nVehicle parked successfully";
                cout << "\nTicket ID: "
                     << ticketId;
                cout << "\nSlot ID: "
                     << slot->getId()
                     << "\n";

                return ticketId;
            }
        }

        cout << "\nNo suitable slot available.\n";

        return -1;
    }


    // -----------------------------------------------
    // Exit Vehicle
    // -----------------------------------------------

    bool exitVehicle(
        int ticketId,
        Payment* payment
    ) {

        auto it =
            tickets.find(ticketId);

        // Invalid ticket
        if (it == tickets.end()) {

            cout << "Invalid ticket.\n";

            return false;
        }

        Ticket* ticket =
            &it->second;


        // Calculate fee
        double fee =
            calculateFee(ticket);

        cout << "\nParking Fee: ?"
             << fee << "\n";


        // Payment
        bool success =
            payment->pay(fee);


        // Payment failed
        if (!success) {

            cout <<
                "Payment failed.\n"
                "Vehicle cannot exit.\n";

            return false;
        }


        // Payment successful
        // Free parking slot

        ticket->getSlot()->removeVehicle();


        // Remove ticket
        tickets.erase(it);


        cout <<
            "Payment successful.\n";

        cout <<
            "Vehicle exited successfully.\n";

        return true;
    }
};


// ======================================================
// MAIN
// ======================================================

int main() {

    ParkingLot parkingLot;


    // -----------------------------------------------
    // Create Floor 1
    // -----------------------------------------------

    ParkingFloor floor1(1);


    // Add slots

    floor1.addSlot(
        ParkingSlot(1, SlotType::BIKE)
    );

    floor1.addSlot(
        ParkingSlot(2, SlotType::BIKE)
    );

    floor1.addSlot(
        ParkingSlot(3, SlotType::CAR)
    );

    floor1.addSlot(
        ParkingSlot(4, SlotType::CAR)
    );

    floor1.addSlot(
        ParkingSlot(5, SlotType::TRUCK)
    );


    // Add floor to parking lot

    parkingLot.addFloor(floor1);


    // -----------------------------------------------
    // Create Vehicle
    // -----------------------------------------------

    Vehicle car(
        "TN01AB1234",
        VehicleType::CAR
    );


    // -----------------------------------------------
    // Park Vehicle
    // -----------------------------------------------

    int ticketId =
        parkingLot.parkVehicle(&car);


    // -----------------------------------------------
    // Exit Vehicle
    // -----------------------------------------------

    UPIPayment payment;

    parkingLot.exitVehicle(
        ticketId,
        &payment
    );

    return 0;
}