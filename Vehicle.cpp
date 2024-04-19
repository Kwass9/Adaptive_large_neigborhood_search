//
// Created by andrej on 23.2.2024.
//

#include "Vehicle.h"

#include <utility>
#include <algorithm>
#include <climits>
//#include <tuple>

Vehicle::Vehicle(int id, double capacity, double x, double y, double readyTime, double dueTime, int custSize, int isWorking) {
    this->id = id;
    this->capacity = capacity;
    editWorkingHours(readyTime, dueTime);
    xcord = x;
    ycord = y;
//    routeLength = 0;
//    routeTime = 0;
//    routeWaitingTime = 0;
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
//    routeLength = vehicle.routeLength;
//    routeTime = vehicle.routeTime;
//    routeWaitingTime = vehicle.routeWaitingTime;
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

//void Vehicle::removeCustomerFromRoute(int idCustomer) {
//    for (int i = 0; i < route.size(); i++) {
//        if (route[i] == idCustomer) {
//            route.erase(route.begin() + i);
//            timeSchedule.erase(timeSchedule.begin() + i);
//            break;
//        }
//    }
//}

//std::vector<int> Vehicle::getCustomers() {
//    return customersServed;
//}

double Vehicle::getCapacity() const {
    return capacity;
}

std::vector<int> Vehicle::getRoute() const {
    return route;
}

//double Vehicle::getRouteLength() const {
//    return routeLength;
//}
//
//double Vehicle::getRouteTime() const {
//    return routeTime;
//}

//double Vehicle::whenIsCustomerServed(int idCustomer) {
//    for (int i = 0; i < route.size(); i++) {
//        if (route[i] == idCustomer) {
//            return timeSchedule[i];
//        }
//    }
//    return -1;
//}

//bool Vehicle::isCustomerInRoute(int idCustomer) {
//    return std::find(route.begin(), route.end(), idCustomer) != route.end();
//}

void Vehicle::editWorkingHours(double start, double end) {
    readyTime.push_back(start);
    dueTime.push_back(end);
}

int Vehicle::getId() const {
    return id;
}

//double Vehicle::getXcord() const {
//    return xcord;
//}

//double Vehicle::getYcord() const {
//    return ycord;
//}

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

//void Vehicle::removeTimeFromSchedule(int idCustomer) {
//    for (int i = 0; i < route.size(); i++) {
//        if (route[i] == idCustomer) {
//            timeSchedule.erase(timeSchedule.begin() + i);
//            break;
//        }
//    }
//}

//int Vehicle::getNumberOfCustomers() const {
//    return (int)route.size() - 2;
//}

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

//int Vehicle::getNumberOfTimeWindows() const {
//    return (int)readyTime.size();
//}

std::vector<std::pair<double, double>> Vehicle::getAllTimeWindows() const {
    std::vector<std::pair<double, double>> timeWindows;
    for (int i = 0; i < readyTime.size(); ++i) {
        timeWindows.emplace_back(readyTime[i], dueTime[i]);
    }
    return timeWindows;
}

//void Vehicle::addAnotherTimeWindowIntoRoute() {
//    route.insert(route.begin() + (int)route.size() - 1, 0);
//    route.insert(route.begin() + (int)route.size() - 1, custSize);
//    auto timeScheduleEnd = timeSchedule[timeSchedule.size() - 2];
//    timeSchedule.insert(timeSchedule.begin() + (int)timeSchedule.size() - 1, getReadyTimeAt(timeScheduleEnd));
//    timeSchedule.insert(timeSchedule.begin() + (int)timeSchedule.size() - 1, getReadyTimeAt(timeScheduleEnd));
//}

//int Vehicle::getInitIndexForFirstVehicle() const { /**uz mam geter na number of customers asi by stacil iba ten... pozriet*/
//    return (int)route.size() - 2;
//}

void Vehicle::removeCustomerFromServed(int idCustomer) {
    for (int i = 0; i < customersServed.size(); i++) {
        if (customersServed[i] == idCustomer) {
            customersServed.erase(customersServed.begin() + i);
            --i;
            break;
        }
    }
}

//void Vehicle::setIsWorkingToTrue() {
//    isWorking = true;
//}

//void Vehicle::setIsWorkingToFalse() {
//    isWorking = false;
//}

bool Vehicle::getIsWorking() const {
    return isWorking;
}

void Vehicle::editTimeScheduleAtPlace(double time, int place) {
    timeSchedule[place] = time;
}
