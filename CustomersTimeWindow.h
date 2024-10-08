//
// Created by andrej on 29.2.2024.
//

#ifndef SOLOMON_CUSTOMERSTIMEWINDOW_H
#define SOLOMON_CUSTOMERSTIMEWINDOW_H


class CustomersTimeWindow {
private:
    double readyTime;
    double dueDate;
    int vehiclesRequired = 1;
    int curentVehiclesServing = 0;
    double demand;
    double serviceTime;
public:
    int getVehiclesRequired() const;
    double getReadyTime() const;
    double getDueDate() const;
    CustomersTimeWindow(double readyTime, double dueDate, double demand, double serviceTime);
    ~CustomersTimeWindow();
    CustomersTimeWindow(CustomersTimeWindow const &customersTimeWindow);
    void incrementVehiclesRequired();
    void incrementCurentVehiclesServing();
    bool isServedByEnoughVehicles() const;
    double getDemand() const;
    double getServiceTime() const;
    int getNumberOfVehiclesServing() const;
    void setCurrenVehiclesServing(int vehiclesServing);
    void prepareForNextRun();
};


#endif //SOLOMON_CUSTOMERSTIMEWINDOW_H
