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
                            std::cout << bestSchedule[servedBy[0]][j] << " " << timeWindows[i].getReadyTime() << " " << timeWindows[i].getDueDate() << std::endl;
                            std::cout << servedBy[0] << " " << j << std::endl;
                            std::cout << "1" << std::endl;
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
                    std::cout << bestSchedule[r1][indexFirst] << " " << bestSchedule[r2][indexSecond] << " " << timeWindows[i].getDueDate() << std::endl;
                    std::cout << r1 << " " << r2 << std::endl;
                    std::cout << indexFirst << " " << indexSecond << std::endl;
                    std::cout << "2" << std::endl;
                }
            } else if (timeWindows[i].getNumberOfVehiclesServing() == 1 && servedBy.size() == 2) {
                auto index = -1;
                auto r = servedBy[i];
                int counter = 0;
//                for (int k = 0; k < servedBy.size(); k++) {
                    for (int j = 0; j < bestRoutes[servedBy[i]].size(); j++) {
                        if (bestRoutes[servedBy[i]][j] == customer.getId()) {
                            index = j;
//                            if (r == -1 && k == 1) {
//                                uncorectnessCounter++;
//                            }
//                            r = servedBy[k];
                            if (counter == i) {
                                break;
                            }
                        }
                    }
                    if (bestSchedule[r][index] < timeWindows[i].getReadyTime() ||
                        bestSchedule[r][index] > timeWindows[i].getDueDate()) {
                        uncorectnessCounter++;
                        std::cout << bestSchedule[r][index] << " " << timeWindows[i].getReadyTime() << " " << timeWindows[i].getDueDate() << std::endl;
                        std::cout << r << " " << index << std::endl;
                        std::cout << "3" << std::endl;
                    }
//                }
            } else if (timeWindows[i].getNumberOfVehiclesServing() == 2 && servedBy.size() == 3) {
                auto indexFirst = 0;
                auto r1 = servedBy[0];
                auto indexSecond = 0;
                auto indexThird = 0;
                auto r2 = servedBy[1];
                if (r1 == r2) {
                    r2 = servedBy[2];
                }
                for (int k = 0; k < bestRoutes[r1].size(); k++) {
                    if (bestRoutes[r1][k] == customer.getId()) {
                        if (indexFirst == 0) {
                            indexFirst = k;
                        } else {
                            indexThird = k;
                        }
                    }
                }
                for (int k = 0; k < bestRoutes[r2].size(); k++) {
                    if (bestRoutes[r2][k] == customer.getId()) {
                        indexSecond = k;
                    }
                }
                if (indexFirst == indexSecond) {
                    continue;
                } else if (indexThird == indexSecond) {
                    indexFirst = indexSecond;
                }


                if (!bestSchedule[r1][indexFirst] > timeWindows[i].getDueDate() || !bestSchedule[r2][indexSecond] > timeWindows[i].getDueDate()
                    || bestSchedule[r1][indexFirst] != bestSchedule[r2][indexSecond]) {
                    uncorectnessCounter++;
                    std::cout << bestSchedule[r1][indexFirst] << " " << bestSchedule[r2][indexSecond] << " " << timeWindows[i].getDueDate() << std::endl;
                    std::cout << r1 << " " << r2 << std::endl;
                    std::cout << indexFirst << " " << indexSecond << std::endl;
                    std::cout << "4" << std::endl;
                }
            } else if (timeWindows[i].getNumberOfVehiclesServing() == 1 && servedBy.size() == 3) {
                auto first = servedBy[0];
                auto second = servedBy[1];
                if (first == second) {
                    second = servedBy[2];
                }
                if (first == second) {
                    uncorectnessCounter++;
                }
                auto index = -1;
                auto r = second;
                for (int j = 0; j < bestRoutes[r].size(); ++j) {
                    if (bestRoutes[r][j] == customer.getId()) {
                        index = j;
                    }
                }
                if (i == 1) { break;}
                if (bestSchedule[r][index] + waitingTime[customer.getId()] < timeWindows[i].getReadyTime() ||
                    bestSchedule[r][index] > timeWindows[i].getDueDate()) {
                    uncorectnessCounter++;
                    std::cout << bestSchedule[r][index] << " " << timeWindows[i].getReadyTime() << " " << timeWindows[i].getDueDate() << std::endl;
                    std::cout << r << " " << index << std::endl;
                    std::cout << "5" << std::endl;
                }
            } else if (!customer.getPreviouslyServedBy().empty() || !customer.getPreviouslyServedByTimes().empty()) {
                uncorectnessCounter++;
            }
        }
    }
}
