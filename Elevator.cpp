#include <bits/stdc++.h>
using namespace std;

enum class Direction {
    UP,
    DOWN,
    IDLE
};

enum class State {
    IDLE,
    MOVING,
    MAINTENANCE,
    EMERGENCY
};

// ============================================================
// REQUEST
// ============================================================

class Request {
public:
    int floor;
    Direction direction;

    Request(int floor, Direction direction)
        : floor(floor), direction(direction) {}
};

// ============================================================
// ELEVATOR
// ============================================================

class Elevator {
private:
    int id;
    int currentFloor;
    int maxCapacity;
    int passengers;

    double maxWeight;
    double currentWeight;

    Direction direction;
    State state;

    set<int> destinations;

public:
    Elevator(int id, int capacity, double maxWeight)
        : id(id),
          currentFloor(0),
          maxCapacity(capacity),
          passengers(0),
          maxWeight(maxWeight),
          currentWeight(0),
          direction(Direction::IDLE),
          state(State::IDLE) {}

    int getId() const {
        return id;
    }

    int getFloor() const {
        return currentFloor;
    }

    Direction getDirection() const {
        return direction;
    }

    State getState() const {
        return state;
    }

    bool isAvailable() const {
        return state != State::MAINTENANCE &&
               state != State::EMERGENCY;
    }

    // Add destination selected by passenger
    void addDestination(int floor) {

        if (!isAvailable())
            return;

        destinations.insert(floor);

        cout << "Elevator " << id
             << " -> destination " << floor << endl;
    }

    // Move one floor
    void move() {

        if (!isAvailable())
            return;

        if (passengers > maxCapacity ||
            currentWeight > maxWeight) {

            cout << "Elevator " << id
                 << " overloaded!\n";

            return;
        }

        if (destinations.empty()) {
            direction = Direction::IDLE;
            state = State::IDLE;
            return;
        }

        int target = *destinations.begin();

        if (currentFloor < target) {
            currentFloor++;
            direction = Direction::UP;
        }
        else if (currentFloor > target) {
            currentFloor--;
            direction = Direction::DOWN;
        }

        state = State::MOVING;

        cout << "Elevator " << id
             << " at floor " << currentFloor << endl;

        if (currentFloor == target) {

            cout << "Elevator " << id
                 << " reached floor " << target << endl;

            destinations.erase(target);

            direction = Direction::IDLE;
            state = State::IDLE;
        }
    }

    // Passenger enters
    bool enter(double weight) {

        if (passengers + 1 > maxCapacity)
            return false;

        if (currentWeight + weight > maxWeight)
            return false;

        passengers++;
        currentWeight += weight;

        return true;
    }

    // Passenger exits
    void exit(double weight) {

        if (passengers > 0)
            passengers--;

        currentWeight -= weight;

        if (currentWeight < 0)
            currentWeight = 0;
    }

    // Emergency
    void emergencyStop() {

        state = State::EMERGENCY;
        direction = Direction::IDLE;

        cout << "Elevator "
             << id
             << " emergency stopped!\n";
    }

    void resetEmergency() {

        if (state == State::EMERGENCY)
            state = State::IDLE;
    }

    // Maintenance
    void setMaintenance(bool value) {

        if (value) {
            state = State::MAINTENANCE;
            direction = Direction::IDLE;
        }
        else {
            state = State::IDLE;
        }
    }
};

// ============================================================
// SCHEDULER
// ============================================================

class Scheduler {
public:

    virtual Elevator* selectElevator(
        vector<Elevator*>& elevators,
        Request& request
    ) = 0;

    virtual ~Scheduler() = default;
};

// ============================================================
// NEAREST ELEVATOR SCHEDULER
// ============================================================

class NearestScheduler : public Scheduler {

public:

    Elevator* selectElevator(
        vector<Elevator*>& elevators,
        Request& request
    ) override {

        Elevator* best = nullptr;
        int minDistance = INT_MAX;

        for (auto elevator : elevators) {

            if (!elevator->isAvailable())
                continue;

            int distance =
                abs(elevator->getFloor()
                    - request.floor);

            if (distance < minDistance) {

                minDistance = distance;
                best = elevator;
            }
        }

        return best;
    }
};

// ============================================================
// CONTROLLER
// ============================================================

class ElevatorController {

private:
    vector<Elevator*> elevators;
    unique_ptr<Scheduler> scheduler;

public:

    ElevatorController(
        vector<Elevator*>& elevators
    ) : elevators(elevators) {

        scheduler =
            make_unique<NearestScheduler>();
    }

    // External request
    Elevator* requestElevator(
        int floor,
        Direction direction
    ) {

        Request request(floor, direction);

        Elevator* elevator =
            scheduler->selectElevator(
                elevators,
                request
            );

        if (!elevator) {

            cout << "No elevator available\n";

            return nullptr;
        }

        cout << "Elevator "
             << elevator->getId()
             << " assigned\n";

        elevator->addDestination(floor);

        return elevator;
    }

    // Internal request
    void selectFloor(
        Elevator* elevator,
        int floor
    ) {

        if (elevator)
            elevator->addDestination(floor);
    }
};

// ============================================================
// ELEVATOR SYSTEM
// ============================================================

class ElevatorSystem {

private:
    vector<unique_ptr<Elevator>> elevators;
    unique_ptr<ElevatorController> controller;

public:

    ElevatorSystem() = default;

    Elevator* addElevator(
        int id,
        int capacity,
        double maxWeight
    ) {

        elevators.push_back(
            make_unique<Elevator>(
                id,
                capacity,
                maxWeight
            )
        );

        return elevators.back().get();
    }

    void initialize() {

        vector<Elevator*> refs;

        for (auto& elevator : elevators)
            refs.push_back(elevator.get());

        controller =
            make_unique<ElevatorController>(refs);
    }

    Elevator* requestElevator(
        int floor,
        Direction direction
    ) {

        return controller->requestElevator(
            floor,
            direction
        );
    }

    void selectFloor(
        Elevator* elevator,
        int floor
    ) {

        controller->selectFloor(
            elevator,
            floor
        );
    }
};

// ============================================================
// MAIN
// ============================================================

int main() {

    ElevatorSystem system;

    Elevator* e1 =
        system.addElevator(1, 8, 600);

    Elevator* e2 =
        system.addElevator(2, 10, 800);

    Elevator* e3 =
        system.addElevator(3, 12, 1000);

    system.initialize();

    // Person at floor 5 presses UP
    Elevator* elevator =
        system.requestElevator(
            5,
            Direction::UP
        );

    // Move to floor 5
    while (elevator->getFloor() != 5)
        elevator->move();

    // Passenger enters
    elevator->enter(70);

    // Passenger selects floor 10
    system.selectFloor(elevator, 10);

    // Move to floor 10
    while (elevator->getFloor() != 10)
        elevator->move();

    // Emergency example
    e2->emergencyStop();

    // Maintenance example
    e3->setMaintenance(true);

    return 0;
}