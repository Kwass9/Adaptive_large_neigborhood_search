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
