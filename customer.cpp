//
// Created by Andrej on 07/10/2023.
//

#include <iostream>
#include "customer.h"

customer::customer(unsigned int idNum, double x, double y, unsigned int dem, double rdyTime, double dueD,
                   double serviceDuration) {
    id = idNum;
    xcord = x;
    ycord = y;
    demand = dem;
    readyTime = rdyTime;
    dueDate = dueD;
    serviceTime = serviceDuration;
}

customer::customer(const customer &other) :id(other.id), xcord(other.xcord), ycord(other.ycord), demand(other.demand),
                                            readyTime(other.readyTime), dueDate(other.dueDate),
                                            serviceTime(other.serviceTime), routedStatus(other.routedStatus) {}

customer::~customer() {
    std::cout << "Customer " << id << " destroyed" << std::endl;
};

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

unsigned int customer::getDemand() const {
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

bool customer::isRoutedStatus() const {
    return routedStatus;
}

