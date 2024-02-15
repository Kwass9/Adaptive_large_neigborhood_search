//
// Created by andrej on 9.2.2024.
//

#ifndef SOLOMON_TEST_H
#define SOLOMON_TEST_H

#include <vector>
#include "Customer.h"

class test {
private:
    int uncorectnessCounter = 0;

public:
    test() = default;
    static std::string corectnessTest(const std::vector<customer>& customers, const std::vector<std::vector<double>>& bestSchedule,
                        const std::vector<std::vector<int>>& bestRoutes, const std::vector<double>& waitingTime);
    void correctnessForCurrentSolution(const std::vector<customer>& customers, const std::vector<std::vector<double>>& bestSchedule,
                                      const std::vector<std::vector<int>>& bestRoutes, const std::vector<double>& waitingTime,
                                      std::vector<std::vector<double>> distanceMatrix);
    int getUncorectnessCounter() const { return uncorectnessCounter; }
};


#endif //SOLOMON_TEST_H
