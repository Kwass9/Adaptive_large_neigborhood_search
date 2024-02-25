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
                 std::vector<Vehicle>& vehicles) {
    totalDistance = INT_MAX - 1;
    calculateDistances(customers, distanceMatrix);
    maxN = calculateMaxN(eta);
    unvisitedCustomers = (int)customers.size() - 1;
    this->alfa1 = alfa1;
    this->alfa2 = alfa2;
    this->lambda = lambda;
    this->q = q;
    this->startingCriteria = startingCriteria;
    for (int i = 0; i <= customers.size(); ++i) {
        timeWaitedAtCustomer.push_back(0);
    }
    problemSize = (int)customers.size() - 1;
    run(customers, unvisitedCustomers, vehicles);
}

solomon::~solomon() {
    distanceMatrix.clear();
}


void solomon::calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix) {
    for (int i = 0; i <= customers.size(); ++i) {
        std::vector<double> distVec(customers.size() + 1, INT_MAX - 1);
        distanceMatrix.push_back(distVec);
    }
    for (int i = 0; i < customers.size(); ++i) {
        for (int j = 0; j <= customers.size(); ++j) {
            if (j < customers.size()) {
                if (i == j) {
                    distanceMatrix[i][j] = 0;
                } else {
                    distanceMatrix[i][j] = sqrt(pow(customers[i].getXcord() - customers[j].getXcord(), 2) +
                                                pow(customers[i].getYcord() - customers[j].getYcord(), 2));
                }
            }
            if (j == customers.size()) {
                distanceMatrix[i][j] = distanceMatrix[i][0];
            }
        }
    }
    for (int i = 0; i <= customers.size(); ++i) {
        if (i < customers.size()) {
            distanceMatrix[i][customers.size()] = sqrt(pow(customers[i].getXcord() - customers[0].getXcord(), 2) +
                                                       pow(customers[i].getYcord() - customers[0].getYcord(), 2));
        }
        if (i == customers.size()) {
            distanceMatrix[i][customers.size()] = 0;
        }
    }
}

std::vector<int> solomon::findCustomerWithEarliestDeadline(std::vector<customer> &customers) {
    std::vector<int> customerIDs;
    for (int i = 1; i < customers.size(); ++i) {
        if (!customers[i].isRouted()) {
            customerIDs.push_back(i);
        }
    }
    std::sort(customerIDs.begin(), customerIDs.end(), [&customers](int a, int b) {
        return customers[a].getDueDate() < customers[b].getDueDate();
    });
    return customerIDs;
}

std::vector<int> solomon::findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix,
                                                   std::vector<customer> &customers) {
    std::vector<int> customerIDs;
    for (int i = 1; i < customers.size(); ++i) {
        if (!customers[i].isRouted()) {
            customerIDs.push_back(i);
        }
    }
    std::sort(customerIDs.begin(), customerIDs.end(),[&distanceMatrix](int a, int b) {
                  return distanceMatrix[0][a] > distanceMatrix[0][b];
              });
    return customerIDs;
}

std::vector<double> solomon::calculatePushForward(const std::vector<int> &route, int u, int position,
                                   const std::vector<double> &timeWaitedAtCustomer,
                                   const std::vector<std::vector<double>> &distanceMatrix, const std::vector<customer> &customers,
                                   double timeOfService, double waitingTime, const std::vector<double> &beginingOfService) {
    double epsilon = 0.0000001; //kvoli double
    auto i = route[position - 1];
    auto j = route[position];
    std::vector<double> pf;
    if (route.size() > 2 && j != customers.size()) {
        if (customers[j].getDueDate() > timeOfService + customers[u].getServiceTime() + distanceMatrix[u][j]) {
            pf.emplace_back(std::max(0.0,distanceMatrix[i][u] + distanceMatrix[u][j] + customers[u].getServiceTime()
                                                  - distanceMatrix[i][j] - timeWaitedAtCustomer[j] + waitingTime));
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
        auto newbegin = timeOfService + customers[u].getServiceTime() + distanceMatrix[u][0];
        pf.emplace_back(newbegin - beginingOfService[beginingOfService.size() - 1]);
    }
    return pf;
}

void solomon::calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                                    std::vector<int> &route, std::vector<customer> &customers, int zakaznikU,
                                    std::vector<double> &beginingOfService, double timeOfService,
                                    std::vector<std::vector<double>> &distanceMatrix, int u) {
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
                oldTime = beginingOfService[zakaznikU];
                customers[route[zakaznikU]].editPreviouslyServedByTime(beginingOfService[zakaznikU] + pushForward[pf], oldTime);
                beginingOfService[n] += pushForward[pf];
                timeWaitedAtCustomer[j] = 0;
                pf++;
            } else if (pf < pushForward.size()) {
                waitingTimeMath(timeWaitedAtCustomer, beginingOfService, route, customers, distanceMatrix, n,
                                timeOfService, u);
                break;
            }
        }
    } else if (route.size() > 2 && route[zakaznikU] != customers.size() && pushForward[0] == 0) {
        waitingTimeMath(timeWaitedAtCustomer, beginingOfService, route, customers, distanceMatrix, zakaznikU,
                        timeOfService, u);
    } else if (route[route.size() - 1] == customers.size()) {
        /**este pozriet ci to aktualizovanie zaciatku obsluhy nema byt aj tu... asi nie lebo toto riesi posledny vrchol no pozriet asi este*/
        beginingOfService[route.size() - 1] += pushForward[pushForward.size() - 1];
        timeWaitedAtCustomer[route[route.size() - 1]] = customers[0].getDueDate() - beginingOfService[route.size() - 1];
    }
}

bool solomon::lema11(const std::vector<double> &beginingOfService, const std::vector<double> &pushForward, const std::vector<int> &route,
                     const std::vector<customer> &customers, int u, int position, double  beginingOfServiceU) {
    if (beginingOfServiceU <= customers[u].getDueDate()) {
        int j = 0;
        while (pushForward[j] > 0.00000001 && j < route.size() - 1 && j < pushForward.size()) {
            if (route[position + j] == customers.size() && beginingOfServiceU + pushForward[j] <= customers[0].getDueDate()) {
                return true;
            }
            if ((position + j != route.size() - 1) && beginingOfService[position + j] + pushForward[j] > customers[route[position + j]].getDueDate()) {
                return false;
            }
            ++j;
        }
        return true;
    }
    return false;
}

std::vector<std::tuple<int, double, int>>
solomon::findMinForC1(const double a1, const double a2, const std::vector<std::vector<double>> &dMatrix,
                      const std::vector<customer> &custs,
                      const std::vector<double> &timeWaitedAtCust, const int doesNoiseApply,
                      const std::vector<Vehicle>& vehicles, int vehicleIndex) {
    std::vector<std::tuple<int, double, int>> mnozinaC1;
    int minIndex = 0;
    auto maxCapacity = vehicles[vehicleIndex].getCapacity();
    auto curUsedCap = vehicles[vehicleIndex].getUsedCapacity();
    auto route = vehicles[vehicleIndex].getRoute();
    auto begOfServ = vehicles[vehicleIndex].getTimeSchedule();
    for (int u = 1; u < custs.size(); ++u) {
        double min = INT_MAX - 1;
        if (!custs[u].isRouted()
            && curUsedCap + custs[u].getDemand() <= maxCapacity) { //kontrola kapacity vozidla
            for (int i = 1; i < route.size(); ++i) {
                double timeOfService = begOfServ[i - 1]
                                       + dMatrix[route[i - 1]][u]
                                       + custs[route[i - 1]].getServiceTime();
                double waitingTime = 0;

//                if (!custs[u].getPreviouslyServedBy().empty()) {
//                    auto servedBy = custs[u].getPreviouslyServedBy();
//                    for (int j = 0; j < servedBy.size(); ++j) {
//                        auto r = vehicles[j].getRoute();
//                        for (int k = 0; k < r.size(); ++k) {
//                            if (r[k] == u) {
//                                //time window zatial len pre jedno okno (vrchol 20)
//                                auto begService = vehicles[servedBy[j]].getTimeSchedule()[k];
////                                auto endService = begService + custs[u].getServiceTime();
//                                if (begService > timeOfService) {
//                                    waitingTime = begService - timeOfService;
//                                    timeOfService = begService;
//                                }
//                            }
//                        }
//                    }
//                }

                if (timeOfService < custs[u].getReadyTime()) {
                    waitingTime = custs[u].getReadyTime() - timeOfService;
                    timeOfService = custs[u].getReadyTime();
                }
                auto pf = calculatePushForward(route, u, i, timeWaitedAtCust,
                                     dMatrix, custs, timeOfService, waitingTime, begOfServ);
                //podmienky lemma 1.1
                if (lema11(begOfServ, pf, route, custs, u, i, timeOfService)) {
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
                }
            }
            if (min < INT_MAX / 2 - 1) { //ak sa nasiel nejaky vhodny zakaznik
                mnozinaC1.emplace_back(minIndex, min, u);
            }
        }
    }
    return mnozinaC1;
}

std::pair<int, int> solomon::findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                                              std::vector<std::vector<double>> &distanceMatrix,
                                              std::vector<customer> &customers) {
    double C2 = 0;
    int maxIndex = 0;
    std::pair<int, int> optimalInsertion;
    for (int i = 0; i < mnozinaC1.size(); ++i) {
        double c2 = lambda * distanceMatrix[0][std::get<2>(mnozinaC1[i])] - std::get<1>(mnozinaC1[i]);
        if (c2 > C2) {
            C2 = c2;
            maxIndex = i;
        } else if (c2 == C2) { //pri zhode usetrenia vyberie zakaznika ktory ma skorsi deadline
            if (customers[i].getDueDate() < customers[maxIndex].getDueDate()) {
                C2 = c2;
                maxIndex = i;
            }
        }
    }
    optimalInsertion.first = std::get<0>(mnozinaC1[maxIndex]); //i
    optimalInsertion.second = std::get<2>(mnozinaC1[maxIndex]); //u
    return optimalInsertion;
}


void solomon::insertCustomerToRoad(Vehicle& vehicle, std::pair<int, int> optimalInsertion, std::vector<customer>& custs,
                                   std::vector<std::vector<double>>& distanceMatrix, std::vector<double>& timeWaitedAtCustomer) {
    int i = optimalInsertion.first;
    int u = optimalInsertion.second;
    auto route = vehicle.getRoute();
    auto beginingOfService = vehicle.getTimeSchedule();
    int predchodca = route[i - 1];

    double timeOfService;
    if (custs[u].getPreviouslyServedBy().empty()) {
        timeOfService = beginingOfService[i - 1]
                               + distanceMatrix[predchodca][u]
                               + custs[predchodca].getServiceTime();
    } else {
        auto times = custs[u].getPreviouslyServedByTimes();
        timeOfService = times[0];
    }

    if (timeOfService < custs[u].getReadyTime()) {
        timeWaitedAtCustomer[u] = custs[u].getReadyTime() - timeOfService;
        timeOfService = custs[u].getReadyTime();
    } else {
        timeWaitedAtCustomer[u] = 0;
    }

    auto pf = calculatePushForward(route, u, i, timeWaitedAtCustomer, distanceMatrix, custs, timeOfService,
                                   timeWaitedAtCustomer[u], beginingOfService);
    calculateNewBeginings(pf, timeWaitedAtCustomer, route, custs, i, beginingOfService, timeOfService,
                          distanceMatrix, u);
    vehicle.setTimeSchedule(beginingOfService);
    vehicle.addCustomer(u);
    vehicle.addCustomerToRoute(u, i);
    vehicle.addTimeToSchedule(timeOfService, i);
    vehicle.setUsedCapacity(vehicle.getUsedCapacity() + custs[u].getDemand());
    custs[u].incrementNumberOfVehiclesCurrentlyServing();
    custs[u].addPreviouslyServedBy(vehicle.getId());
    custs[u].addPreviouslyServedByTime(timeOfService);
    if (custs[u].isServedByEnoughVehicles()) {
        custs[u].markAsRouted();
    }
}

void solomon::waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                              std::vector<int> &route, const std::vector<customer> &customers,
                              std::vector<std::vector<double>> &distanceMatrix, int index, double timeOfServicePrevious,
                              int u) {
    auto nextInRoute = route[index];
    auto previousInRoute = route[index - 1];
    int j = nextInRoute;
    if (nextInRoute == customers.size()) {
        j = 0;
        auto distance = distanceMatrix[previousInRoute][j];
        auto serviceTime = customers[previousInRoute].getServiceTime();
        beginingOfService[index] = timeOfServicePrevious + distance + serviceTime;
        timeWaitedAtCustomer[nextInRoute] = customers[0].getDueDate() - beginingOfService[index];
    } else {
        auto distance = distanceMatrix[u][j];
        auto serviceTime = customers[u].getServiceTime();
        auto startNext = beginingOfService[index];
        timeWaitedAtCustomer[j] = startNext - (timeOfServicePrevious + distance + serviceTime);
    }
}

void solomon::run(std::vector<customer> &custs, int numberOfUnvisitedCustomers, std::vector<Vehicle>& vehicles) {
    unvisitedCustomers = numberOfUnvisitedCustomers;
    int routeIndex = 0;
    std::vector<int> index;
    auto useNoise = doesNoiseApply();

    if (numberOfUnvisitedCustomers == problemSize) {
        //zaciatok prvej cesty
        timeWaitedAtCustomer[custs.size()] = vehicles[routeIndex].getDueTimeAt(0); //prve casove okno koniec //este musim domysliet
        index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, custs)
                                 : findCustomerWithEarliestDeadline(custs);
        auto indexSize = index.size();
        int dec = 0;
        while (vehicles[routeIndex].getRoute().size() <= 2) {
            auto indexVYbrateho = index[indexSize - dec - 1];
            dec++;
            if (lema11(vehicles[routeIndex].getTimeSchedule(), calculatePushForward(vehicles[routeIndex].getRoute(), indexVYbrateho, 1,
                                                                                    timeWaitedAtCustomer, distanceMatrix, custs,
                                                                                    vehicles[routeIndex].getTimeSchedule()[0],
                                                                                    timeWaitedAtCustomer[indexVYbrateho],
                                                                                    vehicles[routeIndex].getTimeSchedule()),
                       vehicles[routeIndex].getRoute(), custs, indexVYbrateho, 1,
                       vehicles[routeIndex].getTimeSchedule()[0]))
            {
                insertCustomerToRoad(vehicles[routeIndex], std::make_pair(1, indexVYbrateho), custs, distanceMatrix, timeWaitedAtCustomer);
                if (custs[indexVYbrateho].isServedByEnoughVehicles()) {
                    unvisitedCustomers--;
                }
            }
        }
    }

    /**------------------------------------------------------------------------------------------------------------*/

    while (unvisitedCustomers != 0) {
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, custs,
                               timeWaitedAtCustomer, useNoise, vehicles, routeIndex);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, custs);
            insertCustomerToRoad(vehicles[routeIndex], c2, custs, distanceMatrix, timeWaitedAtCustomer);
            if (custs[c2.second].isServedByEnoughVehicles()) {
                unvisitedCustomers--;
            }
        } else {
            routeIndex++;
            timeWaitedAtCustomer[custs.size()] = vehicles[routeIndex].getDueTimeAt(0); //prve casove okno koniec //este musim domysliet
            index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, custs)
                                     : findCustomerWithEarliestDeadline(custs);
            auto indexSize = index.size();
            int dec = 0;
            while (vehicles[routeIndex].getRoute().size() <= 2) {
                auto indexVYbrateho = index[dec];
                if (dec < indexSize - 1) {
                    dec++;
                } else {
                    break;
                }
                auto timeOfService = custs[indexVYbrateho].getReadyTime();
                auto waitingTime = timeWaitedAtCustomer[indexVYbrateho];
                if (custs[indexVYbrateho].getNumberOfVehiclesCurrentlyServing() != 0) {
                    timeOfService = custs[indexVYbrateho].getPreviouslyServedByTimes()[0]; /**zatial nenaplnena musim naplnit este*/
                    /**riesenie je zatial ak zakaznik potrebuje len jednu obsluhu za den*/
                }
                if (timeOfService > vehicles[routeIndex].getReadyTimeAt(timeOfService) + distanceMatrix[0][indexVYbrateho]) {
                    auto pf = calculatePushForward(vehicles[routeIndex].getRoute(), indexVYbrateho, 1,
                                                   timeWaitedAtCustomer, distanceMatrix, custs,
                                                   timeOfService,
                                                   waitingTime,
                                                   vehicles[routeIndex].getTimeSchedule());
                    if (lema11(vehicles[routeIndex].getTimeSchedule(), pf,
                               vehicles[routeIndex].getRoute(), custs, indexVYbrateho, 1,
                               timeOfService)) {
                        insertCustomerToRoad(vehicles[routeIndex], std::make_pair(1, indexVYbrateho), custs,
                                             distanceMatrix, timeWaitedAtCustomer);
                        if (custs[indexVYbrateho].isServedByEnoughVehicles()) {
                            unvisitedCustomers--;
                        }
                    }
                }
            }
        }
    }
    finalPrint(custs, vehicles);
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
        for (int j = 0; j < v.getRoute().size() - 2; ++j) {
            totalDistance += distanceMatrix[r[j]][r[j + 1]];
        }
        totalDistance += distanceMatrix[r[r.size() - 2]][0];
        numberOfCustomersServed += (int)r.size() - 2;
        int j = (int)r.size() - 1;
    }
    for (auto & v : vehicles) {
        auto ts = v.getTimeSchedule();
        auto t = ts.size() - 1;
        totalScheduleTime += (ts[t]);
    }
    for (int i = 1; i < timeWaitedAtCustomer.size() - 1; ++i) {
        waitingTimeInSchedule += timeWaitedAtCustomer[i];
    }
}
