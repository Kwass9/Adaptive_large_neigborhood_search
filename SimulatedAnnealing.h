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
    std::vector<std::vector<int>> newRoutes;
public:
    SimulatedAnnealing(class Data *data, double temperature, double coolingRate);
    double getBestSolution() const;
    double getCurrentSolution() const;
    const std::vector<std::vector<int>> &getBestRoutes() const;
    const std::vector<std::vector<int>> &getCurrentRoutes() const;
    const std::vector<std::vector<int>> &getNewRoutes() const;
    double getTemperature() const;
    void updateTemperature();
    void tryToAcceptNewSolution(std::vector<std::vector<int>> &routes, std::vector<std::vector<double>> &timeSchedule,
                                double distance);
};


#endif //SOLOMON_SIMULATEDANNEALING_H
