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
    std::vector<CustomersTimeWindow> timeWindows;
    int specificRequirementsForVehicle = - 1;
public:
    customer(int idNum, double x, double y, int specificRequirementsForVehicle);
    ~customer();
    customer(customer const &customer);
    unsigned int getId() const;
    double getXcord() const;
    double getYcord() const;
    void addPreviouslyServedBy(int vehicleId);
    std::vector<int> getPreviouslyServedBy() const;
    std::vector<double> getPreviouslyServedByTimes() const;
    void addPreviouslyServedByTime(double time);
    void editPreviouslyServedByTime(double time, double position);
    void clearPreviouslyServedBy();
    void setPreviouslyServedBy(std::vector<int> vehicles);
    void removePreviouslyServedBy(int position);
    void setPreviouslyServedByTimes(std::vector<double> times);
    void removePreviouslyServedByTime(int position);
    bool isPreviouslyServedBy(int vehicleId) const;
    int getIndexOfPreviouslyServedBy(double time);
    int getIdOfPreviouslyServedBy(double time);
    int findIndexOfPreviouslyServedBy(int vehicleId);
    bool hasSpecificRequirements() const;
    int getSpecificRequirementsForVehicle() const;
    void setSpecificRequirementsForVehicle(int vehicleId);

    bool isServedByEnoughVehicles() const;
    bool wasServedByThisVehicleAtWindow(int vehicleID, int windowOrderNumber) const;
    void createNewTimeWindow(double readyTime, double dueDate, double demand, double serviceTime);
    CustomersTimeWindow & getTimeWindowAt(int index);
    std::vector<CustomersTimeWindow>& getTimeWindows();
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