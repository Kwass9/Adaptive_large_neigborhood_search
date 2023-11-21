//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SIMULATEDANNEALING_H
#define SOLOMON_SIMULATEDANNEALING_H


#include <vector>
#include "customer.h"
#include "Solution.h"

class SimulatedAnnealing {
private:
    double temperature;
    double coolingRate;
    /**ak budes lokalne inicializovat solution nezabudni ho deletnut*/
    Solution currentSolution = Solution(0);
    Solution bestSolution = Solution(0);
public:
    SimulatedAnnealing(double temperature, double coolingRate, Solution &solution);
    ~SimulatedAnnealing();
    double getBestSolution() const;
    const std::vector<std::vector<int>> &getBestRoutes() const;
    const std::vector<std::vector<double>> &getBestTimeSchedule() const;
    const std::vector<double> &getBestWaitingTime() const;
    double getTemperature() const;
    void updateTemperature();
    bool tryToAcceptNewSolution(Solution &solution);
};


#endif //SOLOMON_SIMULATEDANNEALING_H
