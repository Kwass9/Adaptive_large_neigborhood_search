//
// Created by Andrej on 27/10/2023.
//

#include "solomon.h"
#include <vector>
#include <iostream>
#include "Customer.h"
#include <cmath>
#include <tuple>
#include <climits>
#include <random>
#include <algorithm>

solomon::solomon(std::vector<customer> &customers, double alfa1, double alfa2,
                 double lambda, double q, bool startingCriteria, double eta,
                 std::vector<Vehicle>& vehicles, std::vector<customer*>& unservedCustomers) {
    totalDistance = INT_MAX - 1;
    distanceMatrix.resize(customers.size() + 1, std::vector<double>(customers.size() + 1, INT_MAX - 1));
    maxN = calculateMaxN(eta);
    this->alfa1 = alfa1;
    this->alfa2 = alfa2;
    this->lambda = lambda;
    this->q = q;
    this->startingCriteria = startingCriteria;
    for (int i = 0; i <= customers.size(); ++i) {
        timeWaitedAtCustomer.push_back(0);
    }
//    insertSpecialRequirements(customers, vehicles, unservedCustomers);
//    run(customers, unservedCustomers, vehicles);
}

solomon::~solomon() {
    distanceMatrix.clear();
}


void solomon::calculateDistances(std::vector<customer> &customers,
                                 const std::function<double(const customer &c1, const customer &c2)>& distanceFunction) {
    for (int i = 0; i < customers.size(); ++i) {
        for (int j = 0; j <= customers.size(); ++j) {
            if (j < customers.size()) {
                if (i == j) {
//                    this->distanceMatrix[i][j] = 0;
                    distanceMatrix[i][j] = 0;
                } else {
                    distanceMatrix[i][j] = distanceFunction(customers[i], customers[j]);
                }
            }
            if (j == customers.size()) {
                distanceMatrix[i][j] = distanceMatrix[i][0];
            }
        }
    }
    for (int i = 0; i <= customers.size(); ++i) {
        if (i < customers.size()) {
            distanceMatrix[i][customers.size()] = distanceFunction(customers[i], customers[0]);
        }
        if (i == customers.size()) {
            distanceMatrix[i][customers.size()] = 0;
        }
    }
}

std::function<double(const customer &c1, const customer &c2)>
solomon::euclideanDistance() {
    return ([](const customer &c1, const customer &c2) {
        return sqrt(pow(c1.getXcord() - c2.getXcord(), 2) + pow(c1.getYcord() - c2.getYcord(), 2));
    });
}

std::function<double(const customer &c1, const customer &c2)>
solomon::haversineDistance() {
    return ([](const customer &c1, const customer &c2) {
        double R = 6371;
        double lat1 = c1.getXcord();
        double lon1 = c1.getYcord();
        double lat2 = c2.getXcord();
        double lon2 = c2.getYcord();
        double dLat = (lat2 - lat1) * M_PI / 180;
        double dLon = (lon2 - lon1) * M_PI / 180;
        lat1 = lat1 * M_PI / 180;
        lat2 = lat2 * M_PI / 180;
        double a = sin(dLat / 2) * sin(dLat / 2) +
                   sin(dLon / 2) * sin(dLon / 2) * cos(lat1) * cos(lat2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        double d = R * c * 1000; // Distance in m
        double time = 0.015 * d + 1.0987; //regresia excel //cas v minutach
        if (time > 40) {
            time = 40;
        }
        return time;
    });
}

void solomon::findCustomerWithEarliestDeadline(std::vector<customer*> &customers) {
    std::sort(customers.begin(), customers.end(), [](customer *a, customer *b) {
        return a->getTimeWindows().back().getDueDate() < b->getTimeWindows().back().getDueDate();
    });
}

void solomon::findFurthestUnroutedCustomer(std::vector<std::vector<double>> &dMatrix,
                                                   std::vector<customer*> &customers) {
    std::sort(customers.begin(), customers.end(),[&dMatrix](customer *a, customer *b) {
                  return dMatrix[0][a->getId()] > dMatrix[0][b->getId()];
              });
}

std::vector<double> solomon::calculatePushForward(const std::vector<int> &route, int u, int position,
                                                  const std::vector<double> &timeWaitedAtCustomer,
                                                  const std::vector<std::vector<double>> &distanceMatrix, const std::vector<customer> &customers,
                                                  double timeOfService, double waitingTime, const std::vector<double> &beginingOfService,
                                                  const CustomersTimeWindow& timeWinCustomerU, const CustomersTimeWindow& timeWinCustomerJ) {
    double epsilon = 0.0000001; //kvoli double
    auto i = route[position - 1];
    auto j = route[position];
    std::vector<double> pf;
    if (route.size() > 2 && j != customers.size()) {
        if (timeWinCustomerJ.getDueDate() > timeOfService + timeWinCustomerU.getServiceTime() + distanceMatrix[u][j]) {
            pf.emplace_back(std::max(0.0,distanceMatrix[i][u] + distanceMatrix[u][j] + timeWinCustomerU.getServiceTime()
                                                  - distanceMatrix[i][j] - timeWaitedAtCustomer[j] + waitingTime)); /**co sa snazim docielit tym -time[j]*/
        }   else {
            pf.emplace_back(INT_MAX - 1);
        }
        for (int n = position + 1; n < route.size(); ++n) {
            i = (int)pf.size() - 1;
            j = route[n];
            if (pf[i] - timeWaitedAtCustomer[j] > 0 + epsilon) {
                pf.emplace_back(pf[i] - timeWaitedAtCustomer[j]);
            } else if (pf[pf.size() - 1] != 0) {
                pf.emplace_back(0);
                break;
            }
        }
    } else if (j == customers.size()) {
        auto newbegin = timeOfService + timeWinCustomerU.getServiceTime() + distanceMatrix[u][0];
        pf.emplace_back(newbegin - beginingOfService[beginingOfService.size() - 1]);
        if (pf.back() < 0) {
            pf.back() = INT_MAX - 1;
        }
    }
    return pf;
}

void solomon::calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                                    std::vector<int> &route, std::vector<customer> &customers, int zakaznikU,
                                    std::vector<double> &beginingOfService, double timeOfService,
                                    const std::vector<std::vector<double>> &distanceMatrix, int u) {
    double epsilon = 0.0000001;
    if (route.size() > 2 && route[zakaznikU] != customers.size() && pushForward[0] != 0) {
        int pf = 0;
        auto oldTime = beginingOfService[zakaznikU];
        customers[route[zakaznikU]].editPreviouslyServedByTime(beginingOfService[zakaznikU] + pushForward[pf], oldTime);
        beginingOfService[zakaznikU] += pushForward[pf];
        timeWaitedAtCustomer[route[zakaznikU]] = 0;
        pf++;
        for (int n = zakaznikU + 1; n < route.size(); ++n) {
            int j = route[n];
            u = route[n - 1];
            timeOfService = beginingOfService[n - 1];
            if (pushForward[pf] > 0 + epsilon && pf < pushForward.size()) {
                oldTime = beginingOfService[n];
                customers[route[n]].editPreviouslyServedByTime(beginingOfService[n] + pushForward[pf], oldTime);
                beginingOfService[n] += pushForward[pf];
                timeWaitedAtCustomer[j] = 0;
                pf++;
            } else if (pf < pushForward.size()) {
                waitingTimeMath(timeWaitedAtCustomer, beginingOfService, route, customers, distanceMatrix, n,
                                timeOfService, u, 0);
                break;
            }
        }
    } else if (route.size() > 2 && route[zakaznikU] != customers.size() && pushForward[0] == 0) {
        waitingTimeMath(timeWaitedAtCustomer, beginingOfService, route, customers, distanceMatrix, zakaznikU,
                        timeOfService, u, 0);
    } else if (route[route.size() - 1] == customers.size()) {
        beginingOfService[route.size() - 1] += pushForward[pushForward.size() - 1];
        timeWaitedAtCustomer[route[route.size() - 1]] =
                customers[0].getTimeWindowAt(0).getDueDate() - beginingOfService[route.size() - 1];
    }
}

bool solomon::lema11(const std::vector<double> &beginingOfService, const std::vector<double> &pushForward, const std::vector<int> &route,
                     std::vector<customer> &customers, int u, int position, double  beginingOfServiceU, const  Vehicle &vehicle,
                     const CustomersTimeWindow& timeWinCustomerU) {
    CustomersTimeWindow* timeWindowAfter;
    if (beginingOfServiceU <= timeWinCustomerU.getDueDate()) {
        int j = 0;
        while (pushForward[j] > 0.00000001 && j < route.size() - 1 && j < pushForward.size()) {
            auto nextService = beginingOfService[position + j] + pushForward[j];
            double serviceTimeNext;
            if (route[position + j] == customers.size()) {
                serviceTimeNext = customers[0].getTimeWindowBeforeTime(beginingOfService[position + j]).getServiceTime();
            } else {
                serviceTimeNext = customers[route[position + j]].getTimeWindowBeforeTime(beginingOfService[position + j]).getServiceTime();
            }
            auto timeWindowVehicle = vehicle.getTimeWindow(beginingOfServiceU);
            if (route[position + j] == customers.size() && nextService + serviceTimeNext <= timeWindowVehicle.second && nextService >= timeWindowVehicle.first) { //este asi bude treba kontrolovat nieco s distance matrix atd podla toho ci sa potrebuje opatrovatelka aj vratit (auto do depa)
                return true;
            }
            /**asi pf uz berie v potaz hranu nie som si isty*/
            if (nextService + serviceTimeNext /**+ distanceMatrix[route[position + j]][0]*/ > timeWindowVehicle.second || nextService < timeWindowVehicle.first) {
                return false;
            }
            if (route[position + j] == customers.size()) {
                timeWindowAfter = &customers[0].getTimeWindowAfterTime(nextService);
            } else {
                timeWindowAfter = &customers[route[position + j]].getTimeWindowAfterTime(nextService);
            }
            if (nextService <= timeWindowAfter->getDueDate()) {
                ++j;
            } else {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::vector<std::tuple<int, double, int, int, int>>
solomon::findMinForC1(const double a1, const double a2, const std::vector<std::vector<double>> &dMatrix,
                      std::vector<customer> &custs,
                      const std::vector<double> &timeWaitedAtCust, const int doesNoiseApply,
                      const std::vector<Vehicle>& vehicles, int vehicleIndex,
                      std::vector<customer*> &unservedCusts) {
    std::vector<std::tuple<int, double, int, int, int>> mnozinaC1;
    int minIndex = 0;
    auto maxCapacity = vehicles[vehicleIndex].getCapacity();
    auto curUsedCap = vehicles[vehicleIndex].getUsedCapacity();
    auto route = vehicles[vehicleIndex].getRoute();
    auto begOfServ = vehicles[vehicleIndex].getTimeSchedule();

    for (auto & unservedCust : unservedCusts) {
        int u = (int)unservedCust->getId();
        double min = INT_MAX - 1;
        minIndex = 0;
        std::vector<int> minIndexesLocal;
        std::vector<double> minLocal;
        std::vector<int> wLocal;
        std::vector<bool> validTimeWindows(custs[u].getTimeWindows().size(), false);

        std::vector<std::vector<int>> fakeRoutes;
        std::vector<std::vector<double>> fakeBegOfServ;

        for (int w = 0; w < custs[u].getTimeWindows().size(); ++w) {
            min = INT_MAX - 1;
            minIndex = 0;
            if (!custs[u].wasServedByThisVehicleAtWindow(vehicleIndex, w)) {
                auto timeWindow = custs[u].getTimeWindowAt(w);
                if (curUsedCap + timeWindow.getDemand() <= maxCapacity
                    && timeWindow.getNumberOfVehiclesServing() < timeWindow.getVehiclesRequired()) {

                    for (int i = 1 + w; i < route.size(); ++i) {

                        //TODO - checkIfCustomerCanBePushedInRoute toto uz riesi zrejme cize duplicitny kod
                        if (w == 0) {
                            double timeOfService;
                            if (timeWindow.getNumberOfVehiclesServing() == 0) {
                                auto timeWindowPredchodca = custs[route[i - 1]].getTimeWindowBeforeTime(begOfServ[i - 1]);
                                timeOfService = begOfServ[i - 1]
                                                + dMatrix[route[i - 1]][u]
                                                + timeWindowPredchodca.getServiceTime();
                            } else {
                                auto times = custs[u].getPreviouslyServedByTimes();
                                for (double time: times) {
                                    if (time > timeWindow.getReadyTime() && time < timeWindow.getDueDate()) {
                                        timeOfService = time;
                                        break;
                                    }
                                }
                            }
                            double waitingTime = 0;
                            if (timeWindow.getReadyTime() > timeOfService) {
                                waitingTime = timeWindow.getReadyTime() - timeOfService;
                                timeOfService = timeWindow.getReadyTime();
                            }

                            auto winJ = custs[0].getTimeWindowAt(0);
                            if (i < route.size() - 1) {
                                auto winJP = custs[route[i]].getTimeWindow(begOfServ[i]);
                                auto winJI = custs[route[i]].getIndexOfTimeWindow(winJP.first, winJP.second);
                                winJ = custs[route[i]].getTimeWindowAt(winJI);
                            }
                            auto pf = calculatePushForward(route, u, i, timeWaitedAtCust,
                                                           dMatrix, custs, timeOfService, waitingTime, begOfServ,
                                                           timeWindow, winJ);
                            //pokial nebol este vlozeny do ziadnej inej trasy
                            if (timeWindow.getNumberOfVehiclesServing() == 0) {
                                if (lema11(begOfServ, pf, route, custs, u, i, timeOfService, vehicles[vehicleIndex],
                                           timeWindow)) {
                                    auto res = calculateC1(route, dMatrix, i, u, a1, a2, doesNoiseApply, min, minIndex, pf);
                                    minIndex = std::get<0>(res);
                                    min = std::get<1>(res);
                                    validTimeWindows[w] = true;
                                    minIndexesLocal.emplace_back(minIndex);
                                    minLocal.emplace_back(min);
                                    wLocal.emplace_back(w);
                                    /**fake route - creation, este dokoncit lebo cas tam nie je aktualizovany*/
                                    if (custs[u].getTimeWindows().size() > 1) {
                                        fakeRoutes.emplace_back(route);
                                        fakeBegOfServ.emplace_back(begOfServ);
                                        calculateNewBeginings(pf, timeWaitedAtCustomer, fakeRoutes.back(), custs, i,
                                                              fakeBegOfServ.back(), timeOfService, dMatrix, i);
                                        fakeBegOfServ.back().insert(fakeBegOfServ.back().begin() + i, timeOfService);
                                        fakeRoutes.back().insert(fakeRoutes.back().begin() + i, u);
                                    }
                                }
                            } else if (lema11(begOfServ, pf, route, custs, u, i, timeOfService, vehicles[vehicleIndex],
                                              timeWindow)) {
                                if (custs[u].getIndexOfPreviouslyServedBy(timeOfService) != -1) {
                                    int vehIndex = custs[u].getIdOfPreviouslyServedBy(timeOfService);
                                    waitingTime = timeWaitedAtCust[u];
                                    if (checkIfCustomerCanBePushedInRoute(vehicles[vehIndex], u, timeOfService, custs,
                                                                          waitingTime)) {
                                        auto res = calculateC1(route, dMatrix, i, u, a1, a2, doesNoiseApply, min, minIndex,
                                                               pf);
                                        minIndex = std::get<0>(res);
                                        min = std::get<1>(res);
                                        validTimeWindows[w] = true;
                                        minIndexesLocal.emplace_back(minIndex);
                                        minLocal.emplace_back(min);
                                        wLocal.emplace_back(w);
                                        /**fake route - creation, este dokoncit lebo cas tam nie je aktualizovany*/
                                        if (custs[u].getTimeWindows().size() > 1) {
                                            fakeRoutes.emplace_back(route);
                                            fakeBegOfServ.emplace_back(begOfServ);
                                            calculateNewBeginings(pf, timeWaitedAtCustomer, fakeRoutes.back(), custs, i,
                                                                  fakeBegOfServ.back(), timeOfService, dMatrix, i);
                                            fakeBegOfServ.back().insert(fakeBegOfServ.back().begin() + i, timeOfService);
                                            fakeRoutes.back().insert(fakeRoutes.back().begin() + i, u);
                                        }
                                    }
                                }
                            }
                        } else {
                            int fRsize = (int) fakeRoutes.size();
                            for (int x = 0; x < fRsize; ++x) {
                                auto timeWindow = custs[u].getTimeWindowAt(w);
                                auto timeWindowPredchodca = custs[fakeRoutes[x][i - 1]].getTimeWindowBeforeTime(
                                        fakeBegOfServ[x][i - 1]);
                                double timeOfService = fakeBegOfServ[x][i - 1]
                                                       + dMatrix[fakeRoutes[x][i - 1]][u]
                                                       + timeWindowPredchodca.getServiceTime();
                                double waitingTime = 0;
                                if (timeWindow.getReadyTime() > timeOfService) {
                                    waitingTime = timeWindow.getReadyTime() - timeOfService;
                                    timeOfService = timeWindow.getReadyTime();
                                }
                                auto winJ = custs[0].getTimeWindowAt(0);
                                if (i < fakeRoutes[x].size() - 1) {
                                    auto winJP = custs[fakeRoutes[x][i]].getTimeWindow(fakeBegOfServ[x][i]);
                                    auto winJI = custs[fakeRoutes[x][i]].getIndexOfTimeWindow(winJP.first, winJP.second);
                                    winJ = custs[fakeRoutes[x][i]].getTimeWindowAt(winJI);
                                }
                                auto pf = calculatePushForward(fakeRoutes[x], u, i, timeWaitedAtCust,
                                                               dMatrix, custs, timeOfService, waitingTime, fakeBegOfServ[x],
                                                               timeWindow, winJ);
                                //pokial nebol este vlozeny do ziadnej inej trasy
                                if (timeWindow.getNumberOfVehiclesServing() == 0) {
                                    if (lema11(fakeBegOfServ[x], pf, fakeRoutes[x], custs, u, i, timeOfService,
                                               vehicles[vehicleIndex], timeWindow)) {
                                        auto res = calculateC1(fakeRoutes[x], dMatrix, i, u, a1, a2, doesNoiseApply, min,
                                                               minIndex, pf);
                                        minIndex = std::get<0>(res);
                                        min = std::get<1>(res);
                                        validTimeWindows[w] = true;
                                        minIndexesLocal.emplace_back(minIndex - 1);
                                        minLocal.emplace_back(min);
                                        wLocal.emplace_back(w);
                                        /**fake route - creation*/
                                        if (custs[u].getTimeWindows().size() > 1) {
                                            fakeRoutes.emplace_back(fakeRoutes.back());
                                            fakeBegOfServ.emplace_back(fakeBegOfServ.back());
                                            /**tu si asi gabem ten waiting time*/
                                            calculateNewBeginings(pf, timeWaitedAtCustomer, fakeRoutes.back(), custs, i,
                                                                  fakeBegOfServ.back(), timeOfService, dMatrix, i);
                                            fakeBegOfServ.back().insert(fakeBegOfServ.back().begin() + i, timeOfService);
                                            fakeRoutes.back().insert(fakeRoutes.back().begin() + i, u);
                                        }
                                    }
                                } else if (lema11(fakeBegOfServ[x], pf, fakeRoutes[x], custs, u, i, timeOfService,
                                                  vehicles[vehicleIndex], timeWindow)) {
                                    if (custs[u].getIndexOfPreviouslyServedBy(timeOfService) != -1) {
                                        int vehIndex = custs[u].getIdOfPreviouslyServedBy(timeOfService);
                                        waitingTime = timeWaitedAtCust[u];
                                        if (checkIfCustomerCanBePushedInRoute(vehicles[vehIndex], u, timeOfService, custs,
                                                                              waitingTime)) {
                                            auto res = calculateC1(fakeRoutes[x], dMatrix, i, u, a1, a2, doesNoiseApply,
                                                                   min,
                                                                   minIndex, pf);
                                            minIndex = std::get<0>(res);
                                            min = std::get<1>(res);
                                            validTimeWindows[w] = true;
                                            minIndexesLocal.emplace_back(minIndex);
                                            minLocal.emplace_back(min);
                                            wLocal.emplace_back(w);
                                            /**fake route - creation*/
                                            if (custs[u].getTimeWindows().size() > 1) {
                                                fakeRoutes.emplace_back(fakeRoutes.back());
                                                fakeBegOfServ.emplace_back(fakeBegOfServ.back());
                                                calculateNewBeginings(pf, timeWaitedAtCustomer, fakeRoutes.back(), custs, i,
                                                                      fakeBegOfServ.back(), timeOfService, dMatrix, i);
                                                fakeBegOfServ.back().insert(fakeBegOfServ.back().begin() + i,
                                                                            timeOfService);
                                                fakeRoutes.back().insert(fakeRoutes.back().begin() + i, u);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (custs[u].getTimeWindows().size() == 1) {
                if (min < (int)(INT_MAX / 2) - 1) {
                    mnozinaC1.emplace_back(minIndex, min, u, w, validTimeWindows.size());
                }
            }
        }
        if (custs[u].getTimeWindows().size() > 1) {
            auto check = std::all_of(validTimeWindows.begin(), validTimeWindows.end(), [](bool a) {return a;});
            if (min < (int)(INT_MAX / 2) - 1 && check) {
                for (int i = 0; i < custs[u].getTimeWindows().size(); ++i) {
                    double minFound = INT_MAX - 1;
                    for (int j = 0; j < minIndexesLocal.size(); ++j) {
                        if (wLocal[j] == i && minLocal[j] < minFound) {
                            minIndex = j;
                            minFound = minLocal[j];
                        }
                    }
                    mnozinaC1.emplace_back(minIndexesLocal[minIndex], minLocal[minIndex], u, wLocal[minIndex], validTimeWindows.size());
                }
            }
        }
    }
    return mnozinaC1;
}

std::vector<std::tuple<int, int, int, int>> solomon::findOptimumForC2(std::vector<std::tuple<int, double, int, int, int>> &mnozinaC1, double lambda,
                                              std::vector<std::vector<double>> &distanceMatrix,
                                              std::vector<customer> &customers) {
    double C2 = 0;
    int maxIndex = 0;
    for (int i = 0; i < mnozinaC1.size(); ++i) {
        double c2 = lambda * distanceMatrix[0][std::get<2>(mnozinaC1[i])] - std::get<1>(mnozinaC1[i]);
        if (c2 > C2) {
            C2 = c2;
            maxIndex = i;
        } else if (c2 == C2) { //pri zhode usetrenia vyberie zakaznika ktory ma skorsi deadline
            auto timeWindowI = customers[std::get<2>(mnozinaC1[i])].getTimeWindowAt(std::get<3>(mnozinaC1[i]));
            auto timeWindowMaxIndex = customers[std::get<2>(mnozinaC1[maxIndex])].getTimeWindowAt(
                    std::get<3>(mnozinaC1[maxIndex])); /**asi ide napisat jednoduchsie*/

            if (timeWindowI.getDueDate() < timeWindowMaxIndex.getDueDate()) {
                C2 = c2;
                maxIndex = i;
            }
        }
    }
    std::vector<std::tuple<int, int, int, int>> optimalInsertions;
    for (int j = 0; j < mnozinaC1.size(); ++j) {
        if (std::get<2>(mnozinaC1[j]) == std::get<2>(mnozinaC1[maxIndex])) {
            optimalInsertions.emplace_back(std::get<0>(mnozinaC1[j]), std::get<2>(mnozinaC1[j]),
                                           std::get<3>(mnozinaC1[j]), std::get<4>(mnozinaC1[j]));
        }
    }
    return optimalInsertions;
}

void solomon::insertCustomerToRoad(Vehicle& vehicle, std::vector<std::tuple<int, int, int, int>> optimalInsertion, std::vector<customer>& custs,
                                   const std::vector<std::vector<double>>& distanceMatrix, std::vector<double>& timeWaitedAtCustomer, std::vector<customer*> &unservedCustomers) {
    int n = std::get<3>(optimalInsertion[0]);
    int u = std::get<1>(optimalInsertion[0]);
    for (int j = 0; j < n; ++j) {
        int i = std::get<0>(optimalInsertion[j]);
        int w = std::get<2>(optimalInsertion[j]);

        CustomersTimeWindow& timeWindowU = custs[u].getTimeWindowAt(w);
        auto route = vehicle.getRoute();
        auto beginingOfService = vehicle.getTimeSchedule();
        int predchodca = route[i + j - 1];
        double timeOfService;
        if (timeWindowU.getNumberOfVehiclesServing() == 0) {
            auto timeWindowPredchodca = custs[predchodca].getTimeWindowBeforeTime(beginingOfService[i + j - 1]);
            timeOfService = beginingOfService[i + j - 1]
                                   + distanceMatrix[predchodca][u]
                                   + timeWindowPredchodca.getServiceTime();
        } else {
            auto times = custs[u].getPreviouslyServedByTimes();
            for (double time : times) {
                if (time <= timeWindowU.getDueDate()) {
                    timeOfService = time;
                    break;
                }
            }
        }
        if (timeOfService < timeWindowU.getReadyTime()) {
            timeWaitedAtCustomer[u] = timeWindowU.getReadyTime() - timeOfService;
            timeOfService = timeWindowU.getReadyTime();
        } else {
            timeWaitedAtCustomer[u] = 0;
        }
        auto winJ = custs[0].getTimeWindowAt(0);
        if (i + j < route.size() - 1) {
            auto winJP = custs[route[i + j]].getTimeWindow(beginingOfService[i + j]);
            auto winJI = custs[route[i + j]].getIndexOfTimeWindow(winJP.first, winJP.second);
            winJ = custs[route[i + j]].getTimeWindowAt(winJI);
        }
        auto pf = calculatePushForward(route, u, i + j, timeWaitedAtCustomer, distanceMatrix, custs, timeOfService,
                                       timeWaitedAtCustomer[u], beginingOfService, timeWindowU, winJ);
        calculateNewBeginings(pf, timeWaitedAtCustomer, route, custs, i + j, beginingOfService, timeOfService,
                              distanceMatrix, u);
        vehicle.setTimeSchedule(beginingOfService);
        vehicle.addCustomer(u);
        vehicle.addCustomerToRoute(u, i + j);
        vehicle.addTimeToSchedule(timeOfService, i + j);
        vehicle.setUsedCapacity(vehicle.getUsedCapacity() + timeWindowU.getDemand());
        timeWindowU.incrementCurentVehiclesServing();
        custs[u].addPreviouslyServedBy(vehicle.getId());
        custs[u].addPreviouslyServedByTime(timeOfService);
    }

    /**tu nebude treba cyklus ale bude stacit podmienka*/
    for (int i = 0; i < unservedCustomers.size(); ++i) {
        if (unservedCustomers[i]->getId() == u && unservedCustomers[i]->isServedByEnoughVehicles()) {
            unservedCustomers.erase(unservedCustomers.begin() + i);
        }
    }
}

void solomon::waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                              std::vector<int> &route, std::vector<customer> &customers,
                              const std::vector<std::vector<double>> &distanceMatrix, int index, double timeOfServicePrevious,
                              int u, int w) {
    auto nextInRoute = route[index];
    auto previousInRoute = route[index - 1];
    int j = nextInRoute;
    if (nextInRoute == customers.size()) {
        j = 0;
        auto distance = distanceMatrix[previousInRoute][j];
        auto serviceTime = customers[previousInRoute].getTimeWindows()[0].getServiceTime();
        beginingOfService[index] = timeOfServicePrevious + distance + serviceTime;
        timeWaitedAtCustomer[nextInRoute] = customers[0].getTimeWindows()[0].getDueDate() - beginingOfService[index];
    } else {
        auto distance = distanceMatrix[u][j];
        auto serviceTime = customers[u].getTimeWindows()[w].getServiceTime();
        auto startNext = beginingOfService[index];
        timeWaitedAtCustomer[j] = startNext - (timeOfServicePrevious + distance + serviceTime);
    }
}

void solomon::run(std::vector<customer> &custs, std::vector<customer*>& unservedCustomers, std::vector<Vehicle>& vehicles) {
    int routeIndex = 0;
    auto useNoise = doesNoiseApply();
    int windowsUsed = 0;

    if (unservedCustomers.size() == custs.size() - 1) {
        //TODO - insertBeginingOfRoute toto by sa dalo zjednotit s insertIntoNewRoute
        insertBeginingOfRoute(custs, vehicles, routeIndex, startingCriteria, distanceMatrix, timeWaitedAtCustomer, unservedCustomers);
    }
    while (!unservedCustomers.empty()) {
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, custs,
                               timeWaitedAtCustomer, useNoise, vehicles, routeIndex, unservedCustomers);
        if (!c1.empty()) {
            for (int i = 1; i < c1.size(); ++i) {
//                std::get<0>(c1[i - 1]) < std::get<0>(c1[i]) &&
//                if (std::get<1>(c1[i - 1]) == std::get<1>(c1[i]) && useNoise == 1) {
//                    std::cout << "--------------------rovnake----------------------" << std::endl;
//                    std::cout << "c1: " << std::get<0>(c1[i - 1]) << " " << std::get<1>(c1[i - 1]) << " " << std::get<2>(c1[i - 1]) << " " << std::get<3>(c1[i - 1]) << std::endl;
                    std::cout << "c1: " << std::get<0>(c1[i]) << " " << std::get<1>(c1[i]) << " " << std::get<2>(c1[i]) << " " << std::get<3>(c1[i]) << std::endl;
////                    std::cout << "--------------------rovnake----------------------" << std::endl;
//                }
            }
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, custs);
            std::cout << "c2: " << std::get<0>(c2[0]) << " " << std::get<1>(c2[0]) << " " << std::get<2>(c2[0]) << " " << std::get<3>(c2[0]) << std::endl;
            if (c2.size() > 1) {
                std::cout << "c2: " << std::get<0>(c2[1]) << " " << std::get<1>(c2[1]) << " " << std::get<2>(c2[1]) << " " << std::get<3>(c2[1]) << std::endl;
            }
            insertCustomerToRoad(vehicles[routeIndex], c2, custs, distanceMatrix, timeWaitedAtCustomer, unservedCustomers);
        } else {
            for (int i = 0; i < vehicles[routeIndex].getRoute().size(); ++i) {
                std::cout << vehicles[routeIndex].getRoute()[i] << " ";
            }
            std::cout << std::endl;
            for (double i : vehicles[routeIndex].getTimeSchedule()) {
                std::cout << i << " ";
            }
            std::cout << std::endl;

            routeIndex++;
            if (routeIndex == vehicles.size()) {
                break;
            }
            timeWaitedAtCustomer[custs.size()] = vehicles[routeIndex].getDueTimeAt(0);
            insertIntoNewRoute(custs, vehicles, routeIndex, startingCriteria, distanceMatrix, timeWaitedAtCustomer, unservedCustomers);
            std::cout << "---------------------" << routeIndex << "--------------------------" << std::endl;
            for (int i = 0; i < vehicles[routeIndex].getRoute().size(); ++i) {
                std::cout << vehicles[routeIndex].getRoute()[i] << " ";
            }
            std::cout << std::endl;
        }
    }
    finalPrint(custs, vehicles);
    std::cout << "useNoise: " << useNoise << std::endl;
}

double solomon::getDistance() const {
    return totalDistance;
}

std::vector<double> &solomon::getWaitingTime() {
    return timeWaitedAtCustomer;
}

const std::vector<std::vector<double>> &solomon::getDistanceMatrix() const {
    return distanceMatrix;
}

double solomon::calculateMaxN(double eta) {
    double maxDistance = 0;
    for (int i = 0; i < distanceMatrix.size() - 1; i++) {
        for (int j = 0; j < distanceMatrix[i].size() - 1; j++) {
            if (distanceMatrix[i][j] > maxDistance) {
                maxDistance = j;
            }
        }
    }
    return eta * maxDistance;
}

double solomon::createNoise() const {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(-maxN, maxN);
    return distribution(generator);
}

int solomon::doesNoiseApply() {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(0, 1);
    return distribution(generator);
}

void solomon::finalPrint(std::vector<customer> &custs, std::vector<Vehicle> &vehicles) {
    totalDistance = 0;
    double totalScheduleTime = 0;
    double waitingTimeInSchedule = 0;
    auto numberOfCustomersServed = 0;
    for (auto & v : vehicles) {
        auto r = v.getRoute();
        std::cout << v.getId() << ": ";
        for (int j = 0; j <= r.size() - 2; ++j) {
            totalDistance += distanceMatrix[r[j]][r[j + 1]];
            std::cout << r[j] << " ";
        }
        std::cout << r[r.size() - 1] << std::endl;
        totalDistance += distanceMatrix[r[r.size() - 2]][0];
        numberOfCustomersServed += (int)r.size() - 2;
    }
    for (auto & v : vehicles) {
        auto ts = v.getTimeSchedule();
        auto t = ts.size() - 1;
        totalScheduleTime += (ts[t]);
        std::cout << v.getId() << ": " << ts[t] << std::endl;
    }
    for (int i = 1; i < timeWaitedAtCustomer.size() - 2; ++i) {
        waitingTimeInSchedule += timeWaitedAtCustomer[i];
        std::cout << i << ": " << timeWaitedAtCustomer[i] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Total distance: " << totalDistance << std::endl;
    std::cout << "Total schedule time: " << totalScheduleTime << std::endl;
    std::cout << "Total waiting time: " << waitingTimeInSchedule << std::endl;
}

bool solomon::checkIfCustomerCanBePushedInRoute(const Vehicle &vehicle, int u, double timeOfService,
                                                std::vector<customer> &customers, double waitingTime) {
    auto route = vehicle.getRoute();
    const auto& timeSchedule = vehicle.getTimeSchedule();

    /**tieto tri riadky by slo vynat ako privat metodu lebo ich opakujem na niekolkych miestach*/
    auto window = customers[u].getTimeWindow(timeOfService);
    auto windowIndex = customers[u].getIndexOfTimeWindow(window.first, window.second);
    auto windowU = customers[u].getTimeWindowAt(windowIndex);

    int positionInRoute;
    for (int i = 1; i < route.size() - 1; ++i) {
        if (route[i] == u && timeSchedule[i] == timeOfService) {
            positionInRoute = i;
        }
    }
    auto windowJP = customers[route[positionInRoute]].getTimeWindow(timeSchedule[positionInRoute]);
    auto windowJIndex = customers[route[positionInRoute]].getIndexOfTimeWindow(windowJP.first, windowJP.second);
    auto windowJ = customers[route[positionInRoute]].getTimeWindowAt(windowJIndex);

    auto pf = calculatePushForward(route, u, positionInRoute, timeWaitedAtCustomer, distanceMatrix, customers, timeOfService, waitingTime,
                                   timeSchedule, windowU, windowJ);
    return lema11(timeSchedule, pf, route, customers, u, positionInRoute, timeOfService, vehicle, windowU);
}

std::tuple<int, double> solomon::calculateC1(std::vector<int> route, std::vector<std::vector<double>> dMatrix,
                                                               int i, int u, double a1, double a2, bool doesNoiseApply, double min,
                                                               int minIndex, std::vector<double> pf) {
    double c11 = 0;
    double c12 = 0;
    if (route.size() > 1) {
        c11 = dMatrix[route[i - 1]][u] + dMatrix[u][route[i]] -
                dMatrix[route[i - 1]][route[i]];
        c12 = pf[0];
    }
    double c1 = a1 * c11 + a2 * c12;
    if (doesNoiseApply) {
        auto noise = createNoise();
        c1 = std::max(0.0, c1 + noise);
    }
    if (c1 < min) {
        min = c1;
        minIndex = i;
    }
    return std::tuple<int, double>{std::make_tuple(minIndex, min)};
}

/**malo by helpnut generalizovat uvodny insert pri tvorbe trasy tym refaktor run metody*/
void solomon::insertSpecialRequirements(std::vector<customer>& custs, std::vector<Vehicle> &vehicles, std::vector<customer*> &unservedCustomers) {
    for (int i = 0; i < custs.size(); ++i) {
        if (custs[i].hasSpecificRequirements()) {
            auto req = custs[i].getSpecificRequirementsForVehicle();

            auto startAt = custs[i].getReadyTimeAt(0);
            auto timeOfService = distanceMatrix[0][i];
            if (startAt > timeOfService) {
                timeWaitedAtCustomer[i] = startAt - timeOfService;
                timeOfService = startAt;
            } else {
                timeWaitedAtCustomer[i] = 0;
            }

//            TODO - checkIfCustomerCanBePushedInRoute toto uz riesi zrejme cize duplicitny kod
            auto pf = calculatePushForward(vehicles[req].getRoute(), i, 1, timeWaitedAtCustomer, distanceMatrix, custs, timeOfService,
                                 timeWaitedAtCustomer[i],vehicles[req].getTimeSchedule(), custs[i].getTimeWindowAt(0),
                                 custs[0].getTimeWindowAt(0));
            if (lema11(vehicles[req].getTimeSchedule(), pf,vehicles[req].getRoute(), custs, i, 1,
                   0, vehicles[req], custs[i].getTimeWindowAt(0))) {
                std::vector<std::tuple<int, int, int, int>> tup;
                for (int j = 0; j < custs[i].getTimeWindows().size(); ++j) {
                    tup.emplace_back(1, custs[i].getId(), j, custs[i].getTimeWindows().size());
                }
                insertCustomerToRoad(vehicles[req], tup, custs, distanceMatrix, timeWaitedAtCustomer, unservedCustomers);
            }
        }
    }
}

void solomon::insertBeginingOfRoute(std::vector<customer>& custs, std::vector<Vehicle>& vehicles, int routeIndex, bool criteria,
                                    std::vector<std::vector<double>> &dMatrix, std::vector<double> &timeWaitedAtCust, std::vector<customer*> &unservedCustomers) {
    timeWaitedAtCust[custs.size()] = vehicles[routeIndex].getDueTimeAt(0); //prve casove okno koniec //este musim domysliet
    criteria ? findFurthestUnroutedCustomer(dMatrix, unservedCustomers)
                     : findCustomerWithEarliestDeadline(unservedCustomers);
    int dec = 0;
    while (vehicles[routeIndex].getRoute().size() <= 2) {
        auto indexVybrateho = (int)unservedCustomers[dec]->getId();
        dec++;
        if (dec == unservedCustomers.size()) {
            break;
        }
        auto timeWinCustomerU = custs[indexVybrateho].getTimeWindowAt(0);
        auto timeWinCustomerJ = custs[0].getTimeWindowAt(0);

        //TODO - checkIfCustomerCanBePushedInRoute toto uz riesi zrejme cize duplicitny kodstd::vector<customer*> &unservedCustomers
        if (lema11(vehicles[routeIndex].getTimeSchedule(), calculatePushForward(vehicles[routeIndex].getRoute(), indexVybrateho, 1,
                                                                                timeWaitedAtCust, dMatrix, custs,
                                                                                vehicles[routeIndex].getTimeSchedule()[0],
                                                                                timeWaitedAtCust[indexVybrateho],
                                                                                vehicles[routeIndex].getTimeSchedule(), timeWinCustomerU, timeWinCustomerJ),
                   vehicles[routeIndex].getRoute(), custs, indexVybrateho, 1,
                   vehicles[routeIndex].getTimeSchedule()[0], vehicles[routeIndex], timeWinCustomerU))
        {
            std::vector<std::tuple<int, int, int, int>> vec;
            vec.emplace_back(1, indexVybrateho, 0, custs[indexVybrateho].getTimeWindows().size());
            insertCustomerToRoad(vehicles[routeIndex], vec, custs, dMatrix, timeWaitedAtCust, unservedCustomers);
        }
    }
}

void solomon::insertIntoNewRoute(std::vector<customer> &custs, std::vector<Vehicle> &vehicles, int routeIndex,
                                 int customerIndex, std::vector<std::vector<double>> &dMatrix,
                                 std::vector<double> &timeWaitedAtCust, std::vector<customer*> &unservedCustomers) {
    startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, unservedCustomers)
                                  : findCustomerWithEarliestDeadline(unservedCustomers);
    auto indexSize = unservedCustomers.size();
    int dec = 0;
    while (vehicles[routeIndex].getRoute().size() <= 2 && indexSize != 0 && dec < indexSize) {
        auto indexVybrateho = (int)unservedCustomers[dec]->getId();
        int correctWindow = 0;
        if (dec <= indexSize - 1) {
            dec++;
        } else {
            break;
        }
        double timeOfService = 0;
        auto windows = custs[indexVybrateho].getTimeWindows();
        auto windowIt = windows.begin();
        int winReq = 0;
        for (auto & window : windows) {
            if (!window.isServedByEnoughVehicles()) {
                winReq++;
            }
        }
        if (windowIt == windows.end()) {
            break; /**este nie uplne co by som chcel*/
        }
        int position = 1;
        std::vector<std::tuple<int, int, int, int>> vec; //i,u,j,n
        int windowIndex = 0;
        while (windowIt != windows.end()) {
            while (windowIt->isServedByEnoughVehicles()) {
                windowIndex++;
                windowIt++;
                if (windowIt == windows.end()) {
                    break;
                }
           }
            if (windowIt == windows.end()) {
                break;
            }
            if (!windowIt->isServedByEnoughVehicles()) {
                timeOfService = windowIt->getReadyTime();
            }
            auto waitingTime = timeWaitedAtCustomer[indexVybrateho];
            if (windowIt->getNumberOfVehiclesServing() != 0) {
                auto prevServedByTime = custs[indexVybrateho].getPreviouslyServedByTimes();
                windowIndex = custs[indexVybrateho].getIndexOfTimeWindow(windowIt->getReadyTime(), windowIt->getDueDate()); /**neprepisal medzi iteraciami windowIndex*/
                timeOfService = prevServedByTime[windowIndex];
            }
            if (timeOfService < vehicles[routeIndex].getAllTimeWindows()[0].first) {
                waitingTime = vehicles[routeIndex].getAllTimeWindows()[0].first - timeOfService;
                timeOfService = vehicles[routeIndex].getAllTimeWindows()[0].first;
            }
            if (/**timeOfService >= vehicles[routeIndex].getReadyTimeAt(timeOfService) + distanceMatrix[0][indexVybrateho]
                &&*/ vehicles[routeIndex].getDueTimeAt(vehicles[routeIndex].getReadyTimeAt(timeOfService))
                   >= timeOfService + 2 * distanceMatrix[indexVybrateho][0] + windowIt->getServiceTime()) {

                //TODO - checkIfCustomerCanBePushedInRoute toto uz riesi zrejme cize duplicitny kod
                auto pf = calculatePushForward(vehicles[routeIndex].getRoute(), indexVybrateho, 1,
                                               timeWaitedAtCustomer, distanceMatrix, custs,
                                               timeOfService,
                                               waitingTime,
                                               vehicles[routeIndex].getTimeSchedule(),
                                               custs[indexVybrateho].getTimeWindowAt(position - 1),
                                               custs[0].getTimeWindowAt(0));
                pf[0] += distanceMatrix[0][indexVybrateho]; //pf pridava len jednu hranu, druhu pridavam manualne lebo v pripadoch kde uz cesta doklada dalsie vrcholy staci jedna hrana
                if (lema11(vehicles[routeIndex].getTimeSchedule(), pf,
                           vehicles[routeIndex].getRoute(), custs, indexVybrateho, 1,
                           timeOfService, vehicles[routeIndex], custs[indexVybrateho].getTimeWindowAt(position - 1))) {
                    vec.emplace_back(1, indexVybrateho, windowIndex, winReq); //miesto nula musi byt cislo okna
                    correctWindow++;
                    position++;
                    if (correctWindow == winReq) {
                        insertCustomerToRoad(vehicles[routeIndex], vec, custs,
                                             distanceMatrix, timeWaitedAtCustomer, unservedCustomers);
                        break;
                    }
                }
            }
            windowIndex++;
            windowIt++;
        }
    }
}
