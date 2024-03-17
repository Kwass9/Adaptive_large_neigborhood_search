//
// Created by andrej on 29.2.2024.
//

#include "CustomersTimeWindow.h"

CustomersTimeWindow::CustomersTimeWindow(double readyTime, double dueDate, double demand, double serviceTime) {
    this->readyTime = readyTime;
    this->dueDate = dueDate;
    this->demand = demand;
    this->serviceTime = serviceTime;
}

CustomersTimeWindow::CustomersTimeWindow(const CustomersTimeWindow &customersTimeWindow) {
    readyTime = customersTimeWindow.readyTime;
    dueDate = customersTimeWindow.dueDate;
    vehiclesRequired = customersTimeWindow.vehiclesRequired;
    curentVehiclesServing = customersTimeWindow.curentVehiclesServing;
    demand = customersTimeWindow.demand;
    serviceTime = customersTimeWindow.serviceTime;
}

CustomersTimeWindow::~CustomersTimeWindow() = default;


double CustomersTimeWindow::getReadyTime() const {
    return readyTime;
}

double CustomersTimeWindow::getDueDate() const {
    return dueDate;
}

int CustomersTimeWindow::getVehiclesRequired() const {
    return vehiclesRequired;
}

void CustomersTimeWindow::incrementVehiclesRequired() {
    vehiclesRequired++;
}

void CustomersTimeWindow::decrementVehiclesRequired() {
    vehiclesRequired--;
}

void CustomersTimeWindow::incrementCurentVehiclesServing() {
    curentVehiclesServing++;
}

void CustomersTimeWindow::decrementCurentVehiclesServing() {
    curentVehiclesServing--;
}

bool CustomersTimeWindow::isServedByEnoughVehicles() const {
    return curentVehiclesServing == vehiclesRequired;
}

void CustomersTimeWindow::editWorkingHours(double start, double end) {
    readyTime = start;
    dueDate = end;
}

void CustomersTimeWindow::setReadyTime(double rTime) {
    readyTime = rTime;
}

void CustomersTimeWindow::setDueDate(double dueD) {
    dueDate = dueD;
}

void CustomersTimeWindow::setVehiclesRequired(int vehRequired) {
    vehiclesRequired = vehRequired;
}

int CustomersTimeWindow::getNumberOfVehiclesServing() const {
    return curentVehiclesServing;
}

//TODO
/**zmenit na pole a vracat zaradom hodnoty podla mnozstva aktualne obsluhujucich vozidiel*/
double CustomersTimeWindow::getDemand() const {
    return demand;
}

double CustomersTimeWindow::getServiceTime() const {
    return serviceTime;
}

void CustomersTimeWindow::setCurrenVehiclesServing(int vehiclesServing) {
    curentVehiclesServing = vehiclesServing;
}
