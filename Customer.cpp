//
// Created by Andrej on 07/10/2023.
//

#include "Customer.h"

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


customer::customer(const customer &customer) {
    id = customer.id;
    xcord = customer.xcord;
    ycord = customer.ycord;
    demand = customer.demand;
    readyTime = customer.readyTime;
    dueDate = customer.dueDate;
    serviceTime = customer.serviceTime;
    routedStatus = customer.routedStatus;
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
