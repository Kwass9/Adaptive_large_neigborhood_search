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
        return customers[a].getTimeWindows().back().getDueDate() < customers[b].getTimeWindows().back().getDueDate(); /**nad tymto este porozmyslat*/
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
                                                  double timeOfService, double waitingTime, const std::vector<double> &beginingOfService,
                                                  const CustomersTimeWindow& timeWinCustomerU, const CustomersTimeWindow& timeWinCustomerJ) {
    double epsilon = 0.0000001; //kvoli double
    auto i = route[position - 1];
    auto j = route[position];
    std::vector<double> pf;
    if (route.size() > 2 && j != customers.size()) {
        if (timeWinCustomerJ.getDueDate() > timeOfService + timeWinCustomerU.getServiceTime() + distanceMatrix[u][j]) {
            pf.emplace_back(std::max(0.0,distanceMatrix[i][u] + distanceMatrix[u][j] + timeWinCustomerU.getServiceTime()
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
        /**este pozriet ci to aktualizovanie zaciatku obsluhy nema byt aj tu... asi nie lebo toto riesi posledny vrchol no pozriet asi este*/
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
            auto timeWindowVehicle = vehicle.getTimeWindow(beginingOfServiceU);
            if (route[position + j] == customers.size() && nextService <= timeWindowVehicle.second && nextService >= timeWindowVehicle.first) { //este asi bude treba kontrolovat nieco s distance matrix atd podla toho ci sa potrebuje opatrovatelka aj vratit (auto do depa)
                return true;
            }
            if (nextService > timeWindowVehicle.second || nextService < timeWindowVehicle.first) {
                return false;
            }
            if (route[position + j] == customers.size()) {
                timeWindowAfter = &customers[0].getTimeWindowAfterTime(nextService);
            } else {
                timeWindowAfter = &customers[position + j].getTimeWindowAfterTime(nextService);
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
                      const std::vector<Vehicle>& vehicles, int vehicleIndex) {
    std::vector<std::tuple<int, double, int, int, int>> mnozinaC1;
    int minIndex = 0;
    auto maxCapacity = vehicles[vehicleIndex].getCapacity();
    auto curUsedCap = vehicles[vehicleIndex].getUsedCapacity();
    auto route = vehicles[vehicleIndex].getRoute();
    auto begOfServ = vehicles[vehicleIndex].getTimeSchedule();

    for (int u = 1; u < custs.size(); ++u) {
        double min = INT_MAX - 1;
        std::vector<int> minIndexesLocal;
        std::vector<double> minLocal;
        std::vector<int> wLocal; /**asi pridat min i potom by som ho nemusel zakazdym pocitat pri inserte...*/
        std::vector<bool> validTimeWindows(custs[u].getTimeWindows().size(), false);

        for (int w = 0; w < custs[u].getTimeWindows().size(); ++w) {
            auto timeWindow = custs[u].getTimeWindowAt(w);
            if (!custs[u].isRouted()
                && curUsedCap + timeWindow.getDemand() <= maxCapacity
                && timeWindow.getNumberOfVehiclesServing() < timeWindow.getVehiclesRequired()) {


                for (int i = 1; i < route.size(); ++i) {
                    double timeOfService;
                    if (timeWindow.getNumberOfVehiclesServing() == 0) {
                        auto timeWindowPredchodca = custs[route[i - 1]].getTimeWindowBeforeTime(begOfServ[i - 1]);
                        timeOfService = begOfServ[i - 1]
                                + dMatrix[route[i - 1]][u]
                                + timeWindowPredchodca.getServiceTime();
                    } else {
                        auto times = custs[u].getPreviouslyServedByTimes();
                        for (double time : times) {
                            /**este nie uplne riesenie ktore by som chcel*/
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
                                                   dMatrix, custs, timeOfService, waitingTime, begOfServ, timeWindow, winJ);

                    //pokial nebol este vlozeny do ziadnej inej trasy
                    if (timeWindow.getNumberOfVehiclesServing() == 0) {
                        if (lema11(begOfServ, pf, route, custs, u, i, timeOfService, vehicles[vehicleIndex], timeWindow)) {
                            auto res = calculateC1(route, dMatrix, i, u, a1, a2, doesNoiseApply, min, minIndex, pf);
                            minIndex = std::get<0>(res);
                            min = std::get<1>(res);
                            validTimeWindows[w] = true;
                            minIndexesLocal.emplace_back(minIndex);
                            minLocal.emplace_back(min);
                            wLocal.emplace_back(w);
                        }
                    } else if (lema11(begOfServ, pf, route, custs, u, i, timeOfService, vehicles[vehicleIndex], timeWindow)) {
                        if (custs[u].getIndexOfPreviouslyServedBy(timeOfService) != -1) {
                            int vehIndex = custs[u].getIdOfPreviouslyServedBy(timeOfService);
                            /**tu by mal este byt asi problem pokial sa pracuje s viacerymi oknami*/
                            waitingTime = timeWaitedAtCust[u];
                            /**toto by teoreticky slo potom kontrolovat v cykle pre viac ciest - rozumej ak by bolo treba 3 service naraz*/
                            if (checkIfCustomerCanBePushedInRoute(vehicles[vehIndex], u, timeOfService, custs,
                                                                  waitingTime, i)) {
                                auto res = calculateC1(route, dMatrix, i, u, a1, a2, doesNoiseApply, min, minIndex, pf);
                                minIndex = std::get<0>(res);
                                min = std::get<1>(res);
                                validTimeWindows[w] = true;
                                minIndexesLocal.emplace_back(minIndex);
                                minLocal.emplace_back(min);
                                wLocal.emplace_back(w);
                            }
                        }
                    }
                }
            }
            if (custs[u].getTimeWindows().size() == 1) {
                if (min < INT_MAX / 2 - 1) {
                    mnozinaC1.emplace_back(minIndex, min, u, w, validTimeWindows.size());
                }
            } else {
                auto check = std::all_of(validTimeWindows.begin(), validTimeWindows.end(), [](bool a) {return a;});
                if (min < INT_MAX / 2 - 1 && check) {
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
    }
    return mnozinaC1;
}

/**slo by pridat nejaku logiku okolo viacerych okien nejak vazit benefit ich vlozenia alebo cosy*/
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
    for (int i = 0; i < std::get<4>(mnozinaC1[maxIndex]); ++i) {
        for (int j = 0; j < mnozinaC1.size(); ++j) {
            if (std::get<2>(mnozinaC1[j]) == std::get<2>(mnozinaC1[maxIndex])) {
                optimalInsertions.emplace_back(std::get<0>(mnozinaC1[j]), std::get<2>(mnozinaC1[j]),
                                               std::get<3>(mnozinaC1[j]), std::get<4>(mnozinaC1[j]));
            }
        }
    }
    return optimalInsertions;
}

void solomon::insertCustomerToRoad(Vehicle& vehicle, std::vector<std::tuple<int, int, int, int>> optimalInsertion, std::vector<customer>& custs,
                                   const std::vector<std::vector<double>>& distanceMatrix, std::vector<double>& timeWaitedAtCustomer) {
    int n = std::get<3>(optimalInsertion[0]);
    int u = std::get<1>(optimalInsertion[0]);
    for (int j = 0; j < n; ++j) {
        int i = std::get<0>(optimalInsertion[j]);
        int w = std::get<2>(optimalInsertion[j]);

        CustomersTimeWindow& timeWindowU = custs[u].getTimeWindowAt(w);
        auto route = vehicle.getRoute();
        auto beginingOfService = vehicle.getTimeSchedule();
        int predchodca = route[i - 1];
        double timeOfService;
        if (timeWindowU.getNumberOfVehiclesServing() == 0) {
            timeOfService = beginingOfService[i - 1]
                                   + distanceMatrix[predchodca][u]
                                   + custs[predchodca].getTimeWindows()[0].getServiceTime();
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

        auto pf = calculatePushForward(route, u, i, timeWaitedAtCustomer, distanceMatrix, custs, timeOfService,
                                       timeWaitedAtCustomer[u], beginingOfService, timeWindowU, custs[0].getTimeWindows()[0]);
        calculateNewBeginings(pf, timeWaitedAtCustomer, route, custs, i, beginingOfService, timeOfService,
                              distanceMatrix, u);
        vehicle.setTimeSchedule(beginingOfService);
        vehicle.addCustomer(u);
        vehicle.addCustomerToRoute(u, i);
        vehicle.addTimeToSchedule(timeOfService, i);
        vehicle.setUsedCapacity(vehicle.getUsedCapacity() + timeWindowU.getDemand());
        timeWindowU.incrementCurentVehiclesServing();
        custs[u].addPreviouslyServedBy(vehicle.getId());
        custs[u].addPreviouslyServedByTime(timeOfService);
    }
    if (custs[u].isServedByEnoughVehicles()) {
        custs[u].markAsRouted();
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

void solomon::run(std::vector<customer> &custs, int numberOfUnvisitedCustomers, std::vector<Vehicle>& vehicles) {
    unvisitedCustomers = numberOfUnvisitedCustomers;
    int routeIndex = 0;
    std::vector<int> index;
    auto useNoise = doesNoiseApply();
    int windowsUsed = 0;

    if (numberOfUnvisitedCustomers == problemSize) {
        //zaciatok prvej cesty
        timeWaitedAtCustomer[custs.size()] = vehicles[routeIndex].getDueTimeAt(0); //prve casove okno koniec //este musim domysliet
        index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, custs)
                                 : findCustomerWithEarliestDeadline(custs);
        auto indexSize = index.size();
        int dec = 0;
        while (vehicles[routeIndex].getRoute().size() <= 2) {
            auto indexVybrateho = index[dec];
            dec++;
            auto timeWinCustomerU = custs[indexVybrateho].getTimeWindowAt(0);
            auto timeWinCustomerJ = custs[0].getTimeWindowAt(0);
            if (lema11(vehicles[routeIndex].getTimeSchedule(), calculatePushForward(vehicles[routeIndex].getRoute(), indexVybrateho, 1,
                                                                                    timeWaitedAtCustomer, distanceMatrix, custs,
                                                                                    vehicles[routeIndex].getTimeSchedule()[0],
                                                                                    timeWaitedAtCustomer[indexVybrateho],
                                                                                    vehicles[routeIndex].getTimeSchedule(), timeWinCustomerU, timeWinCustomerJ),
                       vehicles[routeIndex].getRoute(), custs, indexVybrateho, 1,
                       vehicles[routeIndex].getTimeSchedule()[0], vehicles[routeIndex], timeWinCustomerU))
            {
                std::vector<std::tuple<int, int, int, int>> vec;
                vec.emplace_back(1, indexVybrateho, 0, custs[indexVybrateho].getTimeWindows().size());
                insertCustomerToRoad(vehicles[routeIndex], vec, custs, distanceMatrix, timeWaitedAtCustomer);
                if (custs[indexVybrateho].isServedByEnoughVehicles()) {
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
            if (custs[std::get<1>(c2[0])].isServedByEnoughVehicles()) {
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
                auto indexVybrateho = index[dec];
                int correctWindow = 0;
                if (dec <= indexSize - 1) {
                    dec++;
                } else {
                    break;
                }
                double timeOfService = 0;
                auto windows = custs[indexVybrateho].getTimeWindows();
                std::vector<CustomersTimeWindow>::iterator windowIt;
                std::vector<CustomersTimeWindow>::iterator windowItC;
                for (windowIt = windows.begin(); windowIt != windows.end(); ++windowIt) {
                    if (!windowIt->isServedByEnoughVehicles()) {
                        timeOfService = windowIt->getReadyTime();
                        break;
                    }
                }
                int winReq = 0;
                for (windowItC = windows.begin(); windowItC != windows.end(); ++windowItC) {
                    if (!windowItC->isServedByEnoughVehicles()) {
                        winReq++;
                    }
                }
                if (windowIt == windows.end()) {
                    break; /**este nie uplne co by som chcel*/
                }
                int position = 1;
                std::vector<std::tuple<int, int, int, int>> vec; //i,u,j,n
                while (windowIt != windows.end()) {
                    auto waitingTime = timeWaitedAtCustomer[indexVybrateho];
                    int windowIndex = 0;
                    if (windowIt->getNumberOfVehiclesServing() != 0) {
                        auto prevServedByTime = custs[indexVybrateho].getPreviouslyServedByTimes();
                        windowIndex = custs[indexVybrateho].getIndexOfTimeWindow(windowIt->getReadyTime(), windowIt->getDueDate());
                        timeOfService = prevServedByTime[windowIndex];
                    }
                    if (timeOfService >= vehicles[routeIndex].getReadyTimeAt(timeOfService) + distanceMatrix[0][indexVybrateho]
                        && vehicles[routeIndex].getDueTimeAt(vehicles[routeIndex].getReadyTimeAt(timeOfService))
                            >= timeOfService + distanceMatrix[indexVybrateho][0] + windowIt->getServiceTime() + windowIt->getServiceTime()) {
                        auto pf = calculatePushForward(vehicles[routeIndex].getRoute(), indexVybrateho, 1,
                                                       timeWaitedAtCustomer, distanceMatrix, custs,
                                                       timeOfService,
                                                       waitingTime,
                                                       vehicles[routeIndex].getTimeSchedule(),
                                                       custs[indexVybrateho].getTimeWindowAt(position - 1),
                                                       custs[0].getTimeWindowAt(0));
                        if (lema11(vehicles[routeIndex].getTimeSchedule(), pf,
                                   vehicles[routeIndex].getRoute(), custs, indexVybrateho, 1,
                                   timeOfService, vehicles[routeIndex], custs[indexVybrateho].getTimeWindowAt(position - 1))) {
                            vec.emplace_back(position, indexVybrateho, windowIndex, custs[indexVybrateho].getTimeWindows().size()); //miesto nula musi byt cislo okna
                            correctWindow++;
                            position++;
                            if (correctWindow == winReq) {
                                insertCustomerToRoad(vehicles[routeIndex], vec, custs,
                                                     distanceMatrix, timeWaitedAtCustomer);
                                if (custs[indexVybrateho].isServedByEnoughVehicles()) {
                                    unvisitedCustomers--;
                                }
                                break;
                            }
                        }
                    }
                    windowIt++;
                }
            }
        }
        if (routeIndex == vehicles.size()) {
            break;
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
    for (int i = 1; i < timeWaitedAtCustomer.size() - 2; ++i) {
        waitingTimeInSchedule += timeWaitedAtCustomer[i];
    }
    std::cout << "Total distance: " << totalDistance << std::endl;
    std::cout << "Total schedule time: " << totalScheduleTime << std::endl;
    std::cout << "Total waiting time: " << waitingTimeInSchedule << std::endl;
}

bool solomon::checkIfCustomerCanBePushedInRoute(const Vehicle &vehicle, int u, double timeOfService,
                                                std::vector<customer> &customers, double waitingTime,
                                                int vehicleIndex) {
    auto route = vehicle.getRoute();
    const auto& timeSchedule = vehicle.getTimeSchedule();

    /**tieto tri riadky by slo vynat ako privat metodu lebo ich opakujem na niekolkych miestach*/
    auto window = customers[u].getTimeWindow(timeOfService);
    auto windowIndex = customers[u].getIndexOfTimeWindow(window.first, window.second);
    auto windowU = customers[u].getTimeWindowAt(windowIndex);

    auto windowDepot = customers[0].getTimeWindowAt(0);

    auto pf = calculatePushForward(route, u, vehicleIndex, timeWaitedAtCustomer, distanceMatrix, customers, timeOfService, waitingTime,
                                   timeSchedule, windowU, windowDepot);
    return lema11(timeSchedule, pf, route, customers, u, vehicleIndex, timeOfService, vehicle, windowU);
}

void solomon::pushVehicleInOtherRoutes(Vehicle &vehicle, int u, double timeOfService, std::vector<customer> &customers,
                                       const std::vector<std::vector<double>> &distMatrix, double waitingTime) {
    auto route = vehicle.getRoute();
    auto timeSchedule = vehicle.getTimeSchedule();
    int routeIndex;
    for (int i = 0; i < route.size() - 1; ++i) {
        if (route[i] == u) {
            routeIndex = i;
        }
    }
    auto pf = calculatePushForward(route, u, routeIndex, timeWaitedAtCustomer, distMatrix, customers,
                                   timeOfService, waitingTime, timeSchedule, customers[u].getTimeWindowAt(0),
                                   customers[0].getTimeWindowAt(0));
    if (lema11(timeSchedule, pf, route, customers, u, routeIndex, timeOfService,
               vehicle, customers[u].getTimeWindowAt(0))) {
        std::vector<std::tuple<int, int, int, int>> vec;
        vec.emplace_back(routeIndex, u, 0, customers[u].getTimeWindows().size());
        insertCustomerToRoad(vehicle, vec, customers, distMatrix, timeWaitedAtCustomer);
    }
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

///**toto by teoreticky malo fungovat ak staci aby sa opakovala iba jedna s obsluhujucich*/
//bool solomon::checkIfCustomerAcceptsThisVehicle(const customer& customer, int routeIndex, int timeWindowIndex, const CustomersTimeWindow& timeWindow) {
//    if (customer.getTimeWindows().size() != 1) {
//        if (customer.getPreviouslyServedBy().empty()) {
//            return true;
//        }
//        if (customer.isPreviouslyServedBy(routeIndex) && timeWindow.getNumberOfVehiclesServing() == 0) {
//            return true;
//        }
//
//    }
//    return false;
//}
