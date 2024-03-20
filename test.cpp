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

//        if (routeIndex == -1) {
//            uncorectnessCounter++;
//        }
        if (customer.getTimeWindows()[0].getReadyTime() != -1) {
            if (routeIndex >= bestSchedule.size() ||
                customerIndexInRoute >= bestSchedule[routeIndex].size()) {
                int nope = 0;
                for (int i = 0; i < customer.getTimeWindows().size(); ++i) {
                    if (bestSchedule[routeIndex][customerIndexInRoute] + waitingTime[customer.getId()] < customer.getTimeWindows()[i].getReadyTime() ||
                        bestSchedule[routeIndex][customerIndexInRoute] > customer.getTimeWindows()[i].getDueDate()) {
                           nope++;
                    }
                    if (nope == customer.getTimeWindows().size()) {
                        uncorectnessCounter++;
                    }
                }
            } else if (routeIndex == -1) { //nebol zaradeny do cesty
                uncorectnessCounter++;
            }
        }

        auto servedBy = customer.getPreviouslyServedBy();
        auto servedByTimes = customer.getPreviouslyServedByTimes();
        auto timeWindows = customer.getTimeWindows();
        for (int i = 0; i < timeWindows.size(); ++i) {
            if (timeWindows[i].getNumberOfVehiclesServing() == 1 && servedBy.size() == 1) {
                for (int j = 0; j < bestRoutes[servedBy[0]].size(); j++) {
                    if (bestRoutes[servedBy[0]][j] == customer.getId()) {
                        if (bestSchedule[servedBy[0]][j] + waitingTime[customer.getId()] < timeWindows[i].getReadyTime() ||
                            bestSchedule[servedBy[0]][j] > timeWindows[i].getDueDate()) {
                            uncorectnessCounter++;
                        }
                    }
                }
                /**este nehlada spravne index 2*/
            } else if (timeWindows[i].getNumberOfVehiclesServing() == 2 && servedBy.size() == 2) {
                auto indexFirst = 0;
                auto r1 = 0;
                auto indexSecond = 0;
                auto r2 = 0;
                for (int k = 0; k < servedBy.size(); k++) {
                    for (int j = 0; j < bestRoutes[servedBy[k]].size(); j++) {
                        if (bestRoutes[servedBy[k]][j] == customer.getId()) {
                            if (indexFirst == 0) {
                                indexFirst = j;
                                r1 = servedBy[k];
                            } else {
                                indexSecond = j;
                                r2 = servedBy[k];
                            }
                        }
                    }
                }
                if (!bestSchedule[r1][indexFirst] > timeWindows[i].getDueDate() || !bestSchedule[r2][indexSecond] > timeWindows[i].getDueDate()
                    || bestSchedule[r1][indexFirst] != bestSchedule[r2][indexSecond]) {
                    uncorectnessCounter++;
                }
            } else if (timeWindows[i].getNumberOfVehiclesServing() == 1 && servedBy.size() == 2) {

            } else if (timeWindows[i].getNumberOfVehiclesServing() == 2 && servedBy.size() == 3) {

            } else if (timeWindows[i].getNumberOfVehiclesServing() == 1 && servedBy.size() == 3) {

            } else if (!customer.getPreviouslyServedBy().empty() || !customer.getPreviouslyServedByTimes().empty()) {
                uncorectnessCounter++;
            }
        }
    }
}
