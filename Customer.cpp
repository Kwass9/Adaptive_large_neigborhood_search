//
// Created by Andrej on 07/10/2023.
//

#include <algorithm>
#include "Customer.h"

customer::customer(int idNum, double x, double y) {
    id = idNum;
    xcord = x;
    ycord = y;
    routedStatus = false;
}


customer::customer(const customer &customer) {
    id = customer.id;
    xcord = customer.xcord;
    ycord = customer.ycord;
    routedStatus = customer.routedStatus;
}

customer::~customer() {
    timeWindows.clear();
}

void customer::markAsRouted() {
    routedStatus = true;
}

double customer::getYcord() const {
    return ycord;
}

double customer::getXcord() const {
    return xcord;
}

bool customer::isRouted() const {
    return routedStatus;
}

void customer::markAsUnrouted() {
    routedStatus = false;
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
    previouslyServedBy = vehicles;
}

bool customer::isPreviouslyServedBy(int vehicleId) {
    return std::find(previouslyServedBy.begin(), previouslyServedBy.end(), vehicleId) != previouslyServedBy.end();
}

std::vector<double> customer::getPreviouslyServedByTimes() const {
    return previouslyServedByTime;
}

void customer::addPreviouslyServedByTime(double time) {
    previouslyServedByTime.push_back(time);
}

void customer::editPreviouslyServedByTime(double time, double position) {
    for (double & i : previouslyServedByTime) {
        if (i == position) {
            i = time;
        }
    }
}

int customer::getIndexOfPreviouslyServedBy(int time) {
    for (int i = 0; i < previouslyServedByTime.size(); i++) {
        if (previouslyServedByTime[i] == time) {
            return i;
        }
    }
    return -1;
}

void customer::createNewTimeWindow(double readyTime, double dueDate, double demand, double serviceTime) {
    timeWindows.emplace_back(readyTime, dueDate, demand, serviceTime);
}

CustomersTimeWindow customer::getTimeWindow(int index) {
    return timeWindows[index];
}

std::vector<CustomersTimeWindow> customer::getTimeWindows() const {
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
    return index - 1;
}
