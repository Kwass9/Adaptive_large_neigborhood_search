//
// Created by Andrej on 07/10/2023.
//

#include "customer.h"

customer::customer(unsigned int idNum,unsigned int x, unsigned int y, unsigned int dem, unsigned int rdyTime, unsigned int dueD,
                   unsigned int serviceDuration) {
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

unsigned int customer::getYcord() const {
    return ycord;
}

unsigned int customer::getXcord() const {
    return xcord;
}

bool customer::isRouted() const {
    return routedStatus;
}

unsigned int customer::getDemand() const {
    return demand;
}

unsigned int customer::getReadyTime() const {
    return readyTime;
}

unsigned int customer::getDueDate() const {
    return dueDate;
}

unsigned int customer::getServiceTime() const {
    return serviceTime;
}

