//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SIMULATEDANNEALING_H
#define SOLOMON_SIMULATEDANNEALING_H


#include <vector>
#include "customer.h"

class SimulatedAnnealing {
private:
    double temperature;
    double coolingRate;
    double bestSolution;
    double currentSolution;
    std::vector<std::vector<double>> bestTimeSchedule;
    std::vector<std::vector<double>> currentTimeSchedule;
    std::vector<std::vector<int>> bestRoutes;
    std::vector<std::vector<int>> currentRoutes;
    std::vector<double> bestWaitingTime;
    std::vector<double> currentWaitingTime;
public:
    SimulatedAnnealing(double temperature, double coolingRate);
    double getBestSolution() const;
    const std::vector<std::vector<int>> &getBestRoutes() const;
    const std::vector<std::vector<double>> &getBestTimeSchedule() const;
    const std::vector<double> &getBestWaitingTime() const;
    double getTemperature() const;
    void updateTemperature();
    bool tryToAcceptNewSolution(std::vector<std::vector<int>> &routes, std::vector<std::vector<double>> &timeSchedule,
                                std::vector<double> &waitingTime, double distance);
};


#endif //SOLOMON_SIMULATEDANNEALING_H
