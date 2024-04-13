//
// Created by Andrej on 07/10/2023.
//

#include <algorithm>
#include <climits>
#include <utility>
#include "Customer.h"

customer::customer(int idNum, double x, double y, int specificRequirementsForVehicle) {
    id = idNum;
    xcord = x;
    ycord = y;
//    routedStatus = false;
    this->specificRequirementsForVehicle = specificRequirementsForVehicle;
}


customer::customer(const customer &customer) {
    id = customer.id;
    xcord = customer.xcord;
    ycord = customer.ycord;
    timeWindows = customer.timeWindows;
    previouslyServedBy = customer.previouslyServedBy;
    previouslyServedByTime = customer.previouslyServedByTime;
    specificRequirementsForVehicle = customer.specificRequirementsForVehicle;
}

customer::~customer() {
    timeWindows.clear();
}

double customer::getYcord() const {
    return ycord;
}

double customer::getXcord() const {
    return xcord;
}

unsigned int customer::getId() const {
    return id;
}

void customer::addPreviouslyServedBy(int vehicleId) {
    previouslyServedBy.push_back(vehicleId);
}

std::vector<int> customer::getPreviouslyServedBy() const {
    return previouslyServedBy;
}

void customer::clearPreviouslyServedBy() {
    previouslyServedBy.clear();
}

void customer::setPreviouslyServedBy(std::vector<int> vehicles) {
    previouslyServedBy = std::move(vehicles);
}

bool customer::isPreviouslyServedBy(int vehicleId) const {
    return std::find(previouslyServedBy.begin(), previouslyServedBy.end(), vehicleId) != previouslyServedBy.end();
}

std::vector<double> customer::getPreviouslyServedByTimes() const {
    return previouslyServedByTime;
}

void customer::addPreviouslyServedByTime(double time) {
    previouslyServedByTime.push_back(time);
}

void customer::editPreviouslyServedByTime(double time, double position) {
    if (!previouslyServedByTime.empty()) {
        for (double & i : previouslyServedByTime) {
            if (i <= position + 0.0001 && i >= position - 0.0001) {
                i = time;
            }
        }
    }
}

int customer::getIndexOfPreviouslyServedBy(double time) {
    for (int i = 0; i < previouslyServedByTime.size(); i++) {
        if (previouslyServedByTime[i] == time) {
            return i;
        }
    }
    return -1;
}


int customer::getIdOfPreviouslyServedBy(double time) {
    return previouslyServedBy[getIndexOfPreviouslyServedBy(time)];
}

void customer::createNewTimeWindow(double readyTime, double dueDate, double demand, double serviceTime) {
    timeWindows.emplace_back(readyTime, dueDate, demand, serviceTime);
}

CustomersTimeWindow & customer::getTimeWindowAt(int index) {
    return timeWindows[index];
}

std::vector<CustomersTimeWindow>& customer::getTimeWindows() {
    return timeWindows;
}

bool customer::isServedByEnoughVehicles() const {
    return std::all_of(timeWindows.begin(), timeWindows.end(), [](const CustomersTimeWindow & timeWindow) {
        return timeWindow.isServedByEnoughVehicles();
    });
}

CustomersTimeWindow& customer::findTimeWindowWithTimes(double readyTime, double dueDate) {
    return *std::find_if(timeWindows.begin(), timeWindows.end(), [readyTime, dueDate](const CustomersTimeWindow & timeWindow) {
        return timeWindow.getReadyTime() == readyTime && timeWindow.getDueDate() == dueDate;
    });
}

void customer::createTimeWindowIfNotFount(double readyTime, double dueDate, double demand, double serviceTime) {
    if (std::find_if(timeWindows.begin(), timeWindows.end(), [readyTime, dueDate](const CustomersTimeWindow & timeWindow) {
        return timeWindow.getReadyTime() == readyTime && timeWindow.getDueDate() == dueDate;
    }) == timeWindows.end()) {
        createNewTimeWindow(readyTime, dueDate, demand, serviceTime);
    }
}

bool customer::doesTimeWindowExist(double readyTime, double dueDate) {
    return std::find_if(timeWindows.begin(), timeWindows.end(), [readyTime, dueDate](const CustomersTimeWindow & timeWindow) {
        return timeWindow.getReadyTime() == readyTime && timeWindow.getDueDate() == dueDate;
    }) != timeWindows.end();
}

int customer::getIndexOfTimeWindow(double readyTime, double dueDate) {
    auto index = (int)std::distance(timeWindows.begin(), std::find_if(timeWindows.begin(), timeWindows.end(), [readyTime, dueDate](const CustomersTimeWindow & timeWindow) {
        return timeWindow.getReadyTime() == readyTime && timeWindow.getDueDate() == dueDate;
    }));
    return index;
}

double customer::getReadyTimeAt(double serviceTime) const {
    double highestReasonable = 0;
    for (const auto & timeWindow : timeWindows) {
        if (timeWindow.getReadyTime() <= serviceTime) {
            if (timeWindow.getReadyTime() >= highestReasonable) {
                highestReasonable = timeWindow.getReadyTime();
            }
        }
    }
    return highestReasonable;
}

double customer::getDueTimeAt(double serviceTime) const {
    for (const auto & timeWindow : timeWindows) {
        if (timeWindow.getDueDate() > serviceTime) {
            return timeWindow.getDueDate();
        }
    }
    return INT_MAX;
}

std::pair<double, double> customer::getTimeWindow(double serviceTime) const {
    auto beginingOfTheWindowTime = getReadyTimeAt(serviceTime);
    return std::make_pair(beginingOfTheWindowTime, getDueTimeAt(beginingOfTheWindowTime));
}

CustomersTimeWindow & customer::getTimeWindowBeforeTime(double serviceTime) {
    int index = 0;
    for (int i = 0; i < timeWindows.size(); i++) {
        if (timeWindows[i].getReadyTime() <= serviceTime) {
            index = i;
        }
    }
    return timeWindows[index];
}

CustomersTimeWindow &customer::getTimeWindowAfterTime(double serviceTime) {
    int index = 0;
    for (int i = 0; i < timeWindows.size(); i++) {
        if (timeWindows[i].getDueDate() >= serviceTime) {
            index = i;
            break;
        }
    }
    return timeWindows[index];
}

void customer::setPreviouslyServedByTimes(std::vector<double> times) {
    previouslyServedByTime = std::move(times);
}

//TODO: check if this is correct
void customer::removePreviouslyServedBy(int position) {
    previouslyServedBy.erase(previouslyServedBy.begin() + position);
}

//TODO: check if this is correct
void customer::removePreviouslyServedByTime(int position) {
    previouslyServedByTime.erase(previouslyServedByTime.begin() + position);
}

int customer::findIndexOfPreviouslyServedBy(int vehicleId) {
    return (int)std::distance(previouslyServedBy.begin(), std::find(previouslyServedBy.begin(), previouslyServedBy.end(), vehicleId));
}

bool customer::hasSpecificRequirements() const {
    return specificRequirementsForVehicle != - 1;
}

void customer::setSpecificRequirementsForVehicle(int vehicleId) {
    specificRequirementsForVehicle = vehicleId;
}

int customer::getSpecificRequirementsForVehicle() const {
    return specificRequirementsForVehicle;
}

bool customer::wasServedByThisVehicleAtWindow(int vehicleID, int windowOrderNumber) const {
    for (int i = 0; i < previouslyServedBy.size(); i++) {
        if (previouslyServedBy[i] == vehicleID) {
            if (previouslyServedByTime[i] >= timeWindows[windowOrderNumber].getReadyTime() && previouslyServedByTime[i] <= timeWindows[windowOrderNumber].getDueDate()) {
                return true;
            }
        }
    }
    return false;
}

void customer::clearPreviouslyServedByTime() {
    previouslyServedByTime.clear();
}
