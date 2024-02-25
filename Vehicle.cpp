//
// Created by andrej on 23.2.2024.
//

#include "Vehicle.h"

#include <utility>
#include <algorithm>
#include <climits>

Vehicle::Vehicle(unsigned int id, double capacity, double x, double y, double readyTime, double dueTime, int custSize) {
    this->id = id;
    this->capacity = capacity;
    editWorkingHours(readyTime, dueTime);
    xcord = x;
    ycord = y;
    routeLength = 0;
    routeTime = 0;
    routeWaitingTime = 0;
    usedCapacity = 0;
    
    route.emplace_back(0);
    route.emplace_back(custSize);
    timeSchedule.emplace_back(getReadyTimeAt(0));
    timeSchedule.emplace_back(getReadyTimeAt(0));
}

Vehicle::~Vehicle() {
    customersServed.clear();
    route.clear();
    timeSchedule.clear();
    readyTime.clear();
    dueTime.clear();
}

Vehicle::Vehicle(const Vehicle &vehicle) {
    customersServed = vehicle.customersServed;
    capacity = vehicle.capacity;
    route = vehicle.route;
    routeLength = vehicle.routeLength;
    routeTime = vehicle.routeTime;
    routeWaitingTime = vehicle.routeWaitingTime;
    timeSchedule = vehicle.timeSchedule;
    usedCapacity = vehicle.usedCapacity;
    readyTime = vehicle.readyTime;
    dueTime = vehicle.dueTime;
    xcord = vehicle.xcord;
    ycord = vehicle.ycord;
    id = vehicle.id;
}

void Vehicle::addCustomer(int idCustomer) {
    customersServed.push_back(idCustomer);
}

void Vehicle::removeCustomerFromRoute(int idCustomer) {
    for (int i = 0; i < route.size(); i++) {
        if (route[i] == idCustomer) {
            route.erase(route.begin() + i);
            timeSchedule.erase(timeSchedule.begin() + i);
            break;
        }
    }
}

std::vector<int> Vehicle::getCustomers() {
    return customersServed;
}

double Vehicle::getCapacity() const {
    return capacity;
}

std::vector<int> Vehicle::getRoute() const {
    return route;
}

double Vehicle::getRouteLength() const {
    return routeLength;
}

double Vehicle::getRouteTime() const {
    return routeTime;
}

double Vehicle::whenIsCustomerServed(int idCustomer) {
    for (int i = 0; i < route.size(); i++) {
        if (route[i] == idCustomer) {
            return timeSchedule[i];
        }
    }
    return -1;
}

bool Vehicle::isCustomerInRoute(int idCustomer) {
    return std::find(route.begin(), route.end(), idCustomer) != route.end();
}

void Vehicle::editWorkingHours(double start, double end) {
    readyTime.push_back(start);
    dueTime.push_back(end);
}

int Vehicle::getId() const {
    return id;
}

double Vehicle::getXcord() const {
    return xcord;
}

double Vehicle::getYcord() const {
    return ycord;
}

double Vehicle::getUsedCapacity() const {
    return usedCapacity;
}

void Vehicle::setRoute(std::vector<int> r) {
    this->route = r;
}

void Vehicle::setTimeSchedule(std::vector<double> tS) {
    this->timeSchedule = tS;
}

void Vehicle::setUsedCapacity(double usedCap) {
    this->usedCapacity = usedCap;

}

void Vehicle::addTimeToSchedule(double time, int position) {
    timeSchedule.insert(timeSchedule.begin() + position, time);
}

void Vehicle::removeTimeFromSchedule(int idCustomer) {
    for (int i = 0; i < route.size(); i++) {
        if (route[i] == idCustomer) {
            timeSchedule.erase(timeSchedule.begin() + i);
            break;
        }
    }
}

int Vehicle::getNumberOfCustomers() const {
    return route.size() - 2;
}

double Vehicle::getReadyTimeAt(double customersTime) const {
    int lowest = INT_MAX;
    for (double i : readyTime) {
        if (i <= customersTime) {
            return i;
        }
        if (i < lowest) {
            lowest = i;
        }
    }
    return lowest;
}

double Vehicle::getDueTimeAt(int customersTime) const {
    for (double i : dueTime) {
        if (i >= customersTime) {
            return i;
        }
    }
    return INT_MAX;
}

const std::vector<double> &Vehicle::getTimeSchedule() const {
    return timeSchedule;
}

void Vehicle::addCustomerToRoute(int idCustomer, int position) {
    route.insert(route.begin() + position, idCustomer);
}
