//
// Created by Andrej on 07/10/2023.
//

#ifndef SOLOMON_CUSTOMER_H
#define SOLOMON_CUSTOMER_H

#include <vector>
#include "CustomersTimeWindow.h"

class customer {
private:
    int id;
    double xcord;
    double ycord;
    std::vector<int> previouslyServedBy = {};
    std::vector<double> previouslyServedByTime = {};
    bool routedStatus;
    std::vector<CustomersTimeWindow> timeWindows;
public:
    customer(int idNum, double x, double y);
    ~customer();
    customer(customer const &customer);
    void markAsRouted();
    bool isRouted() const;
    unsigned int getId() const;
    double getXcord() const;
    double getYcord() const;
    void markAsUnrouted();
    void addPreviouslyServedBy(int vehicleId);
    std::vector<int> getPreviouslyServedBy() const;
    std::vector<double> getPreviouslyServedByTimes() const;
    void addPreviouslyServedByTime(double time);
    void editPreviouslyServedByTime(double time, double position);
    void clearPreviouslyServedBy();
    void setPreviouslyServedBy(std::vector<int> vehicles);
    bool isPreviouslyServedBy(int vehicleId);
    int getIndexOfPreviouslyServedBy(int time);

    bool isServedByEnoughVehicles() const;
    void createNewTimeWindow(double readyTime, double dueDate, double demand, double serviceTime);
    CustomersTimeWindow getTimeWindow(int index);
    std::vector<CustomersTimeWindow> getTimeWindows() const;
    CustomersTimeWindow& findTimeWindowWithTimes(double readyTime, double dueDate);
    void createTimeWindowIfNotFount(double readyTime, double dueDate, double demand, double serviceTime);
    bool doesTimeWindowExist(double readyTime, double dueDate);
    int getIndexOfTimeWindow(double readyTime, double dueDate);

};


#endif //SOLOMON_CUSTOMER_H