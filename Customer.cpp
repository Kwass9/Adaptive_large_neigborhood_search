//
// Created by Andrej on 07/10/2023.
//

#include <iostream>
#include "Customer.h"

Customer::Customer(unsigned int idNum, double x, double y, unsigned int dem, double rdyTime, double dueD,
                   double serviceDuration) {
    id = idNum;
    xcord = x;
    ycord = y;
    demand = dem;
    readyTime = rdyTime;
    dueDate = dueD;
    serviceTime = serviceDuration;
}

Customer::Customer(const Customer &other) : id(other.id), xcord(other.xcord), ycord(other.ycord), demand(other.demand),
                                            readyTime(other.readyTime), dueDate(other.dueDate),
                                            serviceTime(other.serviceTime), routedStatus(other.routedStatus) {}

Customer::~Customer() {
    /**sem sa este pozriem lebo asi robim zbytocne realokacie*/
    //    std::cout << "Customer " << id << " destroyed" << std::endl;
};

void Customer::markAsRouted() {
    routedStatus = true;
}

double Customer::getYcord() const {
    return ycord;
}

double Customer::getXcord() const {
    return xcord;
}

bool Customer::isRouted() const {
    return routedStatus;
}

unsigned int Customer::getDemand() const {
    return demand;
}

double Customer::getReadyTime() const {
    return readyTime;
}

double Customer::getDueDate() const {
    return dueDate;
}

double Customer::getServiceTime() const {
    return serviceTime;
}

void Customer::markAsUnrouted() {
    routedStatus = false;
}

unsigned int Customer::getId() const {
    return id;
}

bool Customer::isRoutedStatus() const {
    return routedStatus;
}

