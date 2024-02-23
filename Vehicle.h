//
// Created by andrej on 23.2.2024.
//

#ifndef SOLOMON_VEHICLE_H
#define SOLOMON_VEHICLE_H


#include <vector>
#include "Customer.h"

class Vehicle {
private:
    std::vector<int> customersServed;
    double capacity;
    std::vector<int> route;
    double routeLength;
    double routeTime;
    double routeWaitingTime;
    std::vector<double> timeSchedule;
    double usedCapacity;
    std::vector<double> readyTime;
    std::vector<double> dueTime;
public:
    Vehicle(double capacity, std::vector<double> readyTime, std::vector<double> dueTime);
    ~Vehicle();
    Vehicle(Vehicle const &vehicle);
    void addCustomer(int idCustomer);
    void removeCustomerFromRoute(int idCustomer);
    std::vector<int> getCustomers();
    double getCapacity() const;
    std::vector<int> getRoute();
    double getRouteLength() const;
    double getRouteTime() const;
    double whenIsCustomerServed(int idCustomer);
    bool isCustomerInRoute(int idCustomer);
};


#endif //SOLOMON_VEHICLE_H
