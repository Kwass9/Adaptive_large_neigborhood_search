//
// Created by Andrej on 07/10/2023.
//

#ifndef SOLOMON_CUSTOMER_H
#define SOLOMON_CUSTOMER_H

#include <vector>

class customer {
private:
    unsigned int id;
    double xcord;
    double ycord;
    double demand;
    double readyTime;
    double dueDate;
    double serviceTime;
    int numberOfVehiclesRequired = 1;
    int numberOfVehiclesCurrenlyServing = 0;
    std::vector<int> previouslyServedBy = {};
    bool routedStatus;
public:
    customer(unsigned int idNum, double x, double y, double dem,
             double rdyTime, double dueD, double serviceDuration);
    ~customer();
    customer(customer const &customer);
    void markAsRouted();
    bool isRouted() const;
    unsigned int getId() const;
    double getXcord() const;
    double getYcord() const;
    double getDemand() const;
    double getReadyTime() const;
    double getDueDate() const;
    double getServiceTime() const;
    void markAsUnrouted();
    void incrementNumberOfVehiclesCurrentlyServing();
    void decrementNumberOfVehiclesCurrentlyServing();
    int getNumberOfVehiclesCurrentlyServing() const;
    void setNumberOfVehiclesRequired(int number);
    int getNumberOfVehiclesRequired() const;
    void addPreviouslyServedBy(int vehicleId);
    std::vector<int> getPreviouslyServedBy() const;
    void clearPreviouslyServedBy();
    void setPreviouslyServedBy(std::vector<int> vehicles);
    bool isPreviouslyServedBy(int vehicleId);
    bool isServedByEnoughVehicles() const;
};


#endif //SOLOMON_CUSTOMER_H