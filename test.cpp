//
// Created by andrej on 9.2.2024.
//

#include <algorithm>
#include <string>
#include "test.h"

std::string test::corectnessTest(const std::vector<customer>& customers, const std::vector<std::vector<double>> &bestSchedule,
                          const std::vector<std::vector<int>>& bestRoutes, const std::vector<double>& waitingTime) {
    for (const auto& customer : customers) {
        int routeIndex = -1;
        size_t customerIndexInRoute = 0;

        for (size_t i = 0; i < bestRoutes.size(); ++i) {
            const auto& bestRoute = bestRoutes[i];
            auto it = std::find(bestRoute.begin(), bestRoute.end(), customer.getId());
            if (it != bestRoute.end()) {
                routeIndex = i;
                customerIndexInRoute = it - bestRoute.begin();
                break;
            }
        }

        if (routeIndex == -1) {
            return "false";
        }

        if (routeIndex >= bestSchedule.size() ||
            customerIndexInRoute >= bestSchedule[routeIndex].size() ||
            bestSchedule[routeIndex][customerIndexInRoute] + waitingTime[customer.getId() - 1] < customer.getReadyTime() || //tu sa este asi musi priratat cas cakania malo by stacit kontrolovat due date
            bestSchedule[routeIndex][customerIndexInRoute] > customer.getDueDate()) {
            return "false";
        }
    }
    return "true";
}

void test::correctnessForCurrentSolution(const std::vector<customer> &customers,
                                         const std::vector<std::vector<double>> &bestSchedule,
                                         const std::vector<std::vector<int>> &bestRoutes,
                                         const std::vector<double> &waitingTime,
                                         std::vector<std::vector<double>> distanceMatrix) {
    for (const auto& customer : customers) {
        int routeIndex = -1;
        size_t customerIndexInRoute = 0;

        for (size_t i = 0; i < bestRoutes.size(); ++i) {
            const auto& bestRoute = bestRoutes[i];
            auto it = std::find(bestRoute.begin(), bestRoute.end(), customer.getId());
            if (it != bestRoute.end()) {
                routeIndex = i;
                customerIndexInRoute = it - bestRoute.begin();
                break;
            }
        }

        if (routeIndex == -1) {
            uncorectnessCounter++;
        }

        if (routeIndex >= bestSchedule.size() ||
            customerIndexInRoute >= bestSchedule[routeIndex].size() ||
            bestSchedule[routeIndex][customerIndexInRoute] < customer.getReadyTime() || //tu sa este asi musi priratat cas cakania malo by stacit kontrolovat due date
            bestSchedule[routeIndex][customerIndexInRoute] > customer.getDueDate()) {
            uncorectnessCounter++;
        }
    }
    for (size_t i = 0; i < bestRoutes.size(); ++i) {
        double routeTime = 0;
        for (size_t j = 0; j < bestRoutes[i].size(); ++j) {
            if (j == 0) {
                routeTime += distanceMatrix[0][bestRoutes[i][j]];
            } else if (j > 0 && j < bestRoutes[i].size() - 1) {
                routeTime += distanceMatrix[bestRoutes[i][j - 1]][bestRoutes[i][j]] + waitingTime[bestRoutes[i][j]];
            } else {
                routeTime += distanceMatrix[bestRoutes[i][j - 1]][0];
            }
            if (routeTime > bestSchedule[i][j] + 0.00001 || routeTime < bestSchedule[i][j] - 0.00001) {
                uncorectnessCounter++;
            }
            if (j < bestRoutes[i].size() - 1 && j > 0) {
                routeTime += 10; //cas obsluhy
            }
        }
    }
}
