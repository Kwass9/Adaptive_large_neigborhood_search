//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SIMULATEDANNEALING_H
#define SOLOMON_SIMULATEDANNEALING_H


#include <vector>
#include "Customer.h"
#include "test.h"

class SimulatedAnnealing {
private:
    double temperature;
    double coolingRate;
    double bestSolution;
    std::vector<std::vector<int>> bestRoutes;
    std::vector<std::vector<double>> bestTimeSchedule;
    std::vector<double> bestWaitingTime;
    std::vector<double> bestUsedCapacity;
    double currentSolution;
    std::vector<std::vector<int>> currentRoutes;
    std::vector<std::vector<double>> currentTimeSchedule;
    std::vector<double> currentWaitingTime;
    std::vector<double> currentUsedCapacity;
    std::vector<customer> currentCustomers;
    std::vector<customer> bestCustomers;
    std::vector<Vehicle> currentVehicles;
    std::vector<Vehicle> bestVehicles;
public:
    SimulatedAnnealing(double temperature, double coolingRate);
    ~SimulatedAnnealing();
    SimulatedAnnealing(SimulatedAnnealing &simulatedAnnealing);
    double getBestSolution() const;
    const std::vector<std::vector<int>> &getBestRoutes() const;
    const std::vector<std::vector<double>> &getBestTimeSchedule() const;
    const std::vector<double> &getBestWaitingTime() const;
    double getTemperature() const;
    void updateTemperature();
    void tryToAcceptNewSolution(double newSolution, std::vector<Vehicle> &vehicles,
                                std::vector<double> &newWaitingTime, std::vector<customer>& customers);
    bool hasPreviousSolution() const;
    void resetToCurrentSolution(std::vector<customer>& customers, std::vector<Vehicle> &vehicles);
};


#endif //SOLOMON_SIMULATEDANNEALING_H