//
// Created by andrej on 23.2.2024.
//

#include "Vehicle.h"
#include <utility>
#include <algorithm>
#include <climits>

Vehicle::Vehicle(int id, double capacity, double x, double y, double readyTime, double dueTime, int custSize, int isWorking) {
    this->id = id;
    this->capacity = capacity;
    editWorkingHours(readyTime, dueTime);
    xcord = x;
    ycord = y;
    usedCapacity = 0;
    this->custSize = custSize;
    route.emplace_back(0);
    route.emplace_back(custSize);
    timeSchedule.emplace_back(getReadyTimeAt(0));
    timeSchedule.emplace_back(getReadyTimeAt(0));
    if (isWorking == 1) {
        this->isWorking = true;
    }
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
    timeSchedule = vehicle.timeSchedule;
    usedCapacity = vehicle.usedCapacity;
    readyTime = vehicle.readyTime;
    dueTime = vehicle.dueTime;
    xcord = vehicle.xcord;
    ycord = vehicle.ycord;
    id = vehicle.id;
    custSize = vehicle.custSize;
    isWorking = vehicle.isWorking;
}

void Vehicle::addCustomer(int idCustomer) {
    customersServed.push_back(idCustomer);
}

double Vehicle::getCapacity() const {
    return capacity;
}

std::vector<int> Vehicle::getRoute() const {
    return route;
}

void Vehicle::editWorkingHours(double start, double end) {
    readyTime.push_back(start);
    dueTime.push_back(end);
}

int Vehicle::getId() const {
    return id;
}

double Vehicle::getUsedCapacity() const {
    return usedCapacity;
}

void Vehicle::setRoute(std::vector<int> r) {
    this->route = r;
}

void Vehicle::setTimeSchedule(std::vector<double> tS) {
    this->timeSchedule = std::vector<double>(tS);
}

void Vehicle::setUsedCapacity(double usedCap) {
    this->usedCapacity = usedCap;

}

void Vehicle::addTimeToSchedule(double time, int position) {
    timeSchedule.insert(timeSchedule.begin() + position, time);
}

double Vehicle::getReadyTimeAt(double customersTime) const {
    double lowest = INT_MAX;
    for (int i = (int)readyTime.size() - 1; i >= 0; i--) {
        if (readyTime[i] - 0.00001 <= customersTime) {
            return readyTime[i];
        }
        if (readyTime[i] < lowest) {
            lowest = readyTime[i];
        }
    }
    return lowest;
}

double Vehicle::getDueTimeAt(double customersTime) const {
    for (double i : dueTime) {
        if (i + 0.00001 >= customersTime) {
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

std::pair<double, double> Vehicle::getTimeWindow(double customersTime) const {
    auto beginingOfTheWindowTime = getReadyTimeAt(customersTime);
    return std::make_pair(beginingOfTheWindowTime, getDueTimeAt(beginingOfTheWindowTime));
}

std::vector<std::pair<double, double>> Vehicle::getAllTimeWindows() const {
    std::vector<std::pair<double, double>> timeWindows;
    for (int i = 0; i < readyTime.size(); ++i) {
        timeWindows.emplace_back(readyTime[i], dueTime[i]);
    }
    return timeWindows;
}

void Vehicle::removeCustomerFromServed(int idCustomer) {
    for (int i = 0; i < customersServed.size(); i++) {
        if (customersServed[i] == idCustomer) {
            customersServed.erase(customersServed.begin() + i);
            --i;
            break;
        }
    }
}

bool Vehicle::getIsWorking() const {
    return isWorking;
}

void Vehicle::editTimeScheduleAtPlace(double time, int place) {
    timeSchedule[place] = time;
}

void Vehicle::prepareForNextRun() {
    customersServed.clear();
    usedCapacity = 0;
    route.clear();
    route.emplace_back(0);
    route.emplace_back(custSize);
    timeSchedule.clear();
    timeSchedule.emplace_back(getReadyTimeAt(0));
    timeSchedule.emplace_back(getReadyTimeAt(0));
}
