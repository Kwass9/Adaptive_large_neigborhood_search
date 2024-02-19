//
// Created by andrej on 9.2.2024.
//

#include <algorithm>
#include <string>
#include <iostream>
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
                                         std::vector<std::vector<double>> distanceMatrix,
                                         std::vector<double>& usedCapacity) {
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
//            std::cout << "routeTime: " << routeTime << " bestSchedule: " << bestSchedule[i][j] << " i: " << i << " j: " << j << std::endl;
            if (routeTime > bestSchedule[i][j] + 0.00001 || routeTime < bestSchedule[i][j ] - 0.00001) {
                uncorectnessCounter++;
//                std::cout << "-----------------------------------------------------------" << std::endl;
//                std::cout << "routeTime: " << routeTime << " bestSchedule: " << bestSchedule[i][j] << " i: " << i << " j: " << j << std::endl;
            }
            if (j < bestRoutes[i].size() - 1 && j > 0) {
                routeTime += 10; //cas obsluhy
            }
            for (int i = 0; i < usedCapacity.size(); ++i) {
                double capacity = 0;
                for (int j = 1; j < bestRoutes[i].size() - 1; ++j) {
                    auto c = bestRoutes[i][j];
                    capacity += customers[c].getDemand();
                }
                if (capacity > usedCapacity[i] + 0.00001 || capacity < usedCapacity[i] - 0.00001) {
                    uncorectnessCounter++;
//                    std::cout << "---------------------------" << i << "--------------------------------" << std::endl;
//                    std::cout << "capacity: " << capacity << " usedCapacity: " << usedCapacity[i] << std::endl;
                }
                if (usedCapacity[i] < 0) {
                    uncorectnessCounter++;
//                    std::cout << "---------------------------" << i << "--------------------------------" << std::endl;
//                    std::cout << "capacity: " << capacity << " usedCapacity: " << usedCapacity[i] << std::endl;
                }
                if (usedCapacity[i] > 200) {
                    uncorectnessCounter++;
//                    std::cout << "---------------------------" << i << "--------------------------------" << std::endl;
//                    std::cout << "capacity: " << capacity << " usedCapacity: " << usedCapacity[i] << std::endl;
                }
//                std::cout << std::endl;
            }


        }
    }
}
