//
// Created by Andrej on 07/10/2023.
//

#include <algorithm>
#include "Customer.h"

customer::customer(unsigned int idNum, double x, double y, double dem, double rdyTime, double dueD,
                   double serviceDuration) {
    id = idNum;
    xcord = x;
    ycord = y;
    demand = dem;
    readyTime = rdyTime;
    dueDate = dueD;
    serviceTime = serviceDuration;
    routedStatus = false;
}


customer::customer(const customer &customer) {
    id = customer.id;
    xcord = customer.xcord;
    ycord = customer.ycord;
    demand = customer.demand;
    readyTime = customer.readyTime;
    dueDate = customer.dueDate;
    serviceTime = customer.serviceTime;
    routedStatus = customer.routedStatus;
    numberOfVehiclesRequired = customer.numberOfVehiclesRequired;
    numberOfVehiclesCurrenlyServing = customer.numberOfVehiclesCurrenlyServing;
}

customer::~customer() = default;

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

double customer::getDemand() const {
    return demand;
}

double customer::getReadyTime() const {
    return readyTime;
}

double customer::getDueDate() const {
    return dueDate;
}

double customer::getServiceTime() const {
    return serviceTime;
}

void customer::markAsUnrouted() {
    routedStatus = false;
}

unsigned int customer::getId() const {
    return id;
}

void customer::incrementNumberOfVehiclesCurrentlyServing() {
    numberOfVehiclesCurrenlyServing++;
}

void customer::decrementNumberOfVehiclesCurrentlyServing() {
    numberOfVehiclesCurrenlyServing--;
}

int customer::getNumberOfVehiclesCurrentlyServing() const {
    return numberOfVehiclesCurrenlyServing;
}

void customer::setNumberOfVehiclesRequired(int number) {
    numberOfVehiclesRequired = number;
}

int customer::getNumberOfVehiclesRequired() const {
    return numberOfVehiclesRequired;
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

bool customer::isServedByEnoughVehicles() const {
    return numberOfVehiclesCurrenlyServing == numberOfVehiclesRequired;
}
