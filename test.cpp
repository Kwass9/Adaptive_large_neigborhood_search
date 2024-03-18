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
        long customerIndexInRoute;

        for (int i = 0; i < bestRoutes.size(); ++i) {
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

//        if (routeIndex >= bestSchedule.size() ||
//            customerIndexInRoute >= bestSchedule[routeIndex].size() ||
//            bestSchedule[routeIndex][customerIndexInRoute] + waitingTime[customer.getId() - 1] < customer.getReadyTime() ||
//            bestSchedule[routeIndex][customerIndexInRoute] > customer.getDueDate()) {
//            return "false";
//        }
    }
    return "true";
}

void test::correctnessForCurrentSolution(const std::vector<customer> &customers,
                                         const std::vector<std::vector<double>> &bestSchedule,
                                         const std::vector<std::vector<int>> &bestRoutes,
                                         const std::vector<double> &waitingTime,
                                         std::vector<std::vector<double>> distanceMatrix,
                                         std::vector<double>& usedCapacity, std::vector<Vehicle> &vehicles) {
    for (customer customer : customers) {
        int routeIndex = -1;
        size_t customerIndexInRoute = 0;

        for (int i = 0; i < bestRoutes.size(); ++i) {
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
        if (customer.getTimeWindows()[0].getReadyTime() != -1) {
            if (routeIndex >= bestSchedule.size() ||
                customerIndexInRoute >= bestSchedule[routeIndex].size()) {
                int nope = 0;
                for (int i = 0; i < customer.getTimeWindows().size(); ++i) {
                    if (bestSchedule[routeIndex][customerIndexInRoute] + waitingTime[customer.getId() - 1] < customer.getTimeWindows()[i].getReadyTime() ||
                        bestSchedule[routeIndex][customerIndexInRoute] > customer.getTimeWindows()[i].getDueDate()) {
                           nope++;
                    }
                    if (nope == customer.getTimeWindows().size()) {
                        uncorectnessCounter++;
                    }
                }
            }
        }
    }

    /**nascitavanie asi nebude pracovat spravne s waiting timom + vrcholy ktore budu pozadovat dve obsluhy naraz tak nescita spravne myslim*/
    for (size_t i = 0; i < bestRoutes.size(); ++i) {
        double routeTime = vehicles[i].getReadyTimeAt(0); //este nevie pracovat s viac casovymi oknami pre jedno vozidlo
        for (size_t j = 0; j < bestRoutes[i].size(); ++j) {
            if (j == 0) {
                routeTime += distanceMatrix[0][bestRoutes[i][j]];
            } else if (j < bestRoutes[i].size() - 1) {
//                if (customers[bestRoutes[i][j]].getNumberOfVehiclesCurrentlyServing() == 1) {
                    routeTime += distanceMatrix[bestRoutes[i][j - 1]][bestRoutes[i][j]] + waitingTime[bestRoutes[i][j]];
//                } else {
//                    routeTime = customers[bestRoutes[i][j]].getPreviouslyServedByTimes()[0];
//                }
            } else {
                routeTime += distanceMatrix[bestRoutes[i][j - 1]][0];
            }
            if (routeTime > bestSchedule[i][j] + 0.00001 || routeTime < bestSchedule[i][j ] - 0.00001) {
//                std::cout << "routeTime: " << routeTime << " i: " << i << " j: " << j << std::endl;
//                std::cout << "bestSchedule: " << bestSchedule[i][j] << std::endl;
                uncorectnessCounter++;
            }
            if (j < bestRoutes[i].size() - 1 && j > 0) {
//                routeTime += customers[bestRoutes[i][j]].getServiceTime(); //cas obsluhy
                routeTime += 10;
            }

            /**pridat kontrolu cakania*/


//            for (int k = 0; k < usedCapacity.size(); ++k) {
//                double capacity = 0;
//                for (int l = 1; l < bestRoutes[k].size() - 1; ++l) {
//                    auto c = bestRoutes[k][l];
//                    capacity += customers[c].getDemand();
//                }
//                if (capacity > usedCapacity[k] + 0.00001 || capacity < usedCapacity[k] - 0.00001) {
//                    uncorectnessCounter++;
//                }
//                if (usedCapacity[k] < 0) {
//                    uncorectnessCounter++;
//                }
//                if (usedCapacity[k] > 200) {
//                    uncorectnessCounter++;
//                }
//            }



        }
    }
}
