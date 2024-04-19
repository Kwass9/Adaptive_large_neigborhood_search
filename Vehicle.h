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
    int id{};
    std::vector<int> customersServed;
    double capacity;
    std::vector<int> route;
//    double routeLength;
//    double routeTime;
//    double routeWaitingTime;
    std::vector<double> timeSchedule;
    double usedCapacity;
    std::vector<double> readyTime;
    std::vector<double> dueTime;
    int custSize;
    bool isWorking = false;
public:
    Vehicle(int id, double capacity, double x, double y, double readyTime, double dueTime, int custSize, int isWorking);
    ~Vehicle();
    Vehicle(Vehicle const &vehicle);
    void addCustomer(int idCustomer);
    void removeCustomerFromServed(int idCustomer);
    void addCustomerToRoute(int idCustomer, int position);
//    void removeCustomerFromRoute(int idCustomer);
    void addTimeToSchedule(double time, int position);
//    void removeTimeFromSchedule(int idCustomer);
    double getReadyTimeAt(double customersTime) const;
    double getDueTimeAt(double customersTime) const;
    std::pair<double, double> getTimeWindow(double customersTime) const;
    std::vector<std::pair<double, double>> getAllTimeWindows() const;
//    void addAnotherTimeWindowIntoRoute();
//    int getInitIndexForFirstVehicle() const;
//    int getNumberOfTimeWindows() const;
//    int getNumberOfCustomers() const;
//    std::vector<int> getCustomers();
    double getCapacity() const;
    std::vector<int> getRoute() const;
//    double getRouteLength() const;
//    double getRouteTime() const;
//    double whenIsCustomerServed(int idCustomer);
//    bool isCustomerInRoute(int idCustomer);
    void editWorkingHours(double start, double end);
    int getId() const;
//    double getXcord() const;
//    double getYcord() const;
    double getUsedCapacity() const;
    void setRoute(std::vector<int> r);
    void setTimeSchedule(std::vector<double> tS);
    void editTimeScheduleAtPlace(double time, int place);
    void setUsedCapacity(double usedCap);
    const std::vector<double> &getTimeSchedule() const;
//    void setIsWorkingToTrue();
//    void setIsWorkingToFalse();
    bool getIsWorking() const;
};

#endif //SOLOMON_VEHICLE_H
