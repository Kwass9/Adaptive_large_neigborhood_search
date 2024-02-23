//
// Created by andrej on 23.2.2024.
//

#ifndef SOLOMON_VEHICLE_H
#define SOLOMON_VEHICLE_H


#include <vector>
#include "Customer.h"

class Vehicle {
private:
    double xcord;
    double ycord;
    unsigned int id{};
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
    Vehicle(unsigned int id, double capacity, double x, double y, double readyTime, double dueTime, int custSize);
    ~Vehicle();
    Vehicle(Vehicle const &vehicle);
    void addCustomer(int idCustomer);
    void addCustomerToRoute(int idCustomer, int position);
    void removeCustomerFromRoute(int idCustomer);
    void addTimeToSchedule(double time, int position);
    void removeTimeFromSchedule(int idCustomer);
    double getReadyTimeAt(int customersTime) const;
    double getDueTimeAt(int customersTime) const;
    int getNumberOfCustomers() const;
    std::vector<int> getCustomers();
    double getCapacity() const;
    std::vector<int> getRoute() const;
    double getRouteLength() const;
    double getRouteTime() const;
    double whenIsCustomerServed(int idCustomer);
    bool isCustomerInRoute(int idCustomer);
    void editWorkingHours(double start, double end);
    unsigned int getId() const;
    double getXcord() const;
    double getYcord() const;
    double getUsedCapacity() const;
    void setRoute(std::vector<int> r);
    void setTimeSchedule(std::vector<double> tS);
    void setUsedCapacity(double usedCap);
    const std::vector<double> &getTimeSchedule() const;
};

#endif //SOLOMON_VEHICLE_H
