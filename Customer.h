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
    std::vector<double> waitingTimeForService = {};
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
    bool isPreviouslyServedBy(int vehicleId) const;
    int getIndexOfPreviouslyServedBy(double time);
    int getIdOfPreviouslyServedBy(double time);

    std::vector<double> getWaitingTimeForService() const;
    double getWaitingTimeForServiceAt(int index) const;
    void addWaitingTimeForService(double time);
    void editWaitingTimeForService(double time, int index);
    void clearWaitingTimeForService();
    void setWaitingTimeForService(std::vector<double> times);

    bool isServedByEnoughVehicles() const;
    void createNewTimeWindow(double readyTime, double dueDate, double demand, double serviceTime);
    CustomersTimeWindow & getTimeWindowAt(int index);
    std::vector<CustomersTimeWindow> getTimeWindows() const;
    CustomersTimeWindow& findTimeWindowWithTimes(double readyTime, double dueDate);
    void createTimeWindowIfNotFount(double readyTime, double dueDate, double demand, double serviceTime);
    bool doesTimeWindowExist(double readyTime, double dueDate);
    int getIndexOfTimeWindow(double readyTime, double dueDate);

    double getReadyTimeAt(double serviceTime) const;
    double getDueTimeAt(double serviceTime) const;
    std::pair<double, double> getTimeWindow(double serviceTime) const;
    CustomersTimeWindow & getTimeWindowBeforeTime(double serviceTime);
    CustomersTimeWindow & getTimeWindowAfterTime(double serviceTime);
};


#endif //SOLOMON_CUSTOMER_H