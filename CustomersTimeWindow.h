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
    void setReadyTime(double rTime);
    void setDueDate(double dueD);
    void setVehiclesRequired(int vehRequired);
    void incrementVehiclesRequired();
    void decrementVehiclesRequired();
    void incrementCurentVehiclesServing();
    void decrementCurentVehiclesServing();
    bool isServedByEnoughVehicles() const;
    void editWorkingHours(double start, double end);
    double getDemand() const;
    double getServiceTime() const;
    int getNumberOfVehiclesServing() const;
    void setCurrenVehiclesServing(int vehiclesServing);
};


#endif //SOLOMON_CUSTOMERSTIMEWINDOW_H
