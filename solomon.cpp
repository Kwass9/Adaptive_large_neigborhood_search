//
// Created by Andrej on 27/10/2023.
//

#include "solomon.h"
#include <vector>
#include <iostream>
#include "customer.h"
#include <cmath>
#include <tuple>
#include <climits>

solomon::solomon(std::vector<customer> &customers, /**std::vector<std::vector<int>> &routes,*/ double alfa1, double alfa2,
                 double lambda, double q, bool startingCriteria) {

    std::vector<std::vector<double>> distanceMatrix;
    calculateDistances(customers, distanceMatrix);
    std::vector<int> route;
    std::vector<std::vector<int>> routes;
    std::vector<std::vector<double>> timeSchedule;

    auto unvisitedCustomers = customers.size() - 1;

    static int vehicleCapacity = 200; /**prerobit na nacitanie zo suboru*/
    unsigned int currentlyUsedCapacity = 0;

    std::vector<double> timeWaitedAtCustomer;
    std::vector<double> pushForward;
    std::vector<double> beginingOfService;
    for (int i = 0; i <= customers.size(); ++i) {
        timeWaitedAtCustomer.push_back(0);
    }

    //zaciatok cesty
    route.emplace_back(0);
    route.emplace_back(customers.size());
    beginingOfService.emplace_back(0);
    beginingOfService.emplace_back(0);
    timeWaitedAtCustomer[route[1]] = customers[0].getDueDate();
    unsigned int index;
    if (startingCriteria) {
        index = findFurthestUnroutedCustomer(distanceMatrix, customers);
    } else {
        index = findCustomerWithEarliestDeadline(customers);
    }
    insertCustomerToRoad(route, std::make_pair(1, index), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                         distanceMatrix, pushForward);
    unvisitedCustomers--;

    /**------------------------------------------------------------------------------------------------------------*/

    while (unvisitedCustomers != 0) { //while unroute customers masti ich do ciest
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, beginingOfService, pushForward, route, customers,
                               currentlyUsedCapacity, vehicleCapacity, timeWaitedAtCustomer, unvisitedCustomers);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, customers);
            insertCustomerToRoad(route, c2, beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity, distanceMatrix, pushForward);
            unvisitedCustomers--;
        } else {
            for (int i = 0; i < route.size(); ++i) {
                std::cout << route[i] << " (" << beginingOfService[i] << ") | ";
            }
            timeSchedule.emplace_back(beginingOfService);
            std::cout << "Ostava zakaznikov: " << unvisitedCustomers << std::endl;
            std::cout << "Pouzita kapacita: " << currentlyUsedCapacity << std::endl;
            createNewRoute(currentlyUsedCapacity, routes, route, beginingOfService, pushForward);
            route.emplace_back(0);
            route.emplace_back(customers.size());
            beginingOfService.emplace_back(0);
            beginingOfService.emplace_back(0);
            timeWaitedAtCustomer[route[1]] = customers[0].getDueDate();
            index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, customers)
                                     : findCustomerWithEarliestDeadline(customers);
            insertCustomerToRoad(route, std::make_pair(1, index), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward);
            unvisitedCustomers--;
        }
        std::cout << "Ostava zakaznikov: " << unvisitedCustomers << std::endl;
    }
    routes.push_back(route);

    //zaverecny vypis
    for (auto & route : routes) {
        for (int j = 0; j < route.size(); ++j) {
            std::cout << route[j];
            if (j != route.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
    double totalDistance = 0;
    double totalScheduleTime = 0;
    double waitingTimeInSchedule = 0;
    auto numberOfCustomersServed = 0;
    for (auto & route : routes) {
        for (int j = 0; j < route.size() - 1; ++j) {
            totalDistance += distanceMatrix[route[j]][route[j + 1]];
        }
        for (int j = 0; j < route.size() - 2; ++j) {
            waitingTimeInSchedule += timeWaitedAtCustomer[route[j + 1]];
        }
        numberOfCustomersServed += route.size() - 2;
        totalScheduleTime += beginingOfService[route.size() - 2] + customers[route[route.size() - 2]].getServiceTime() +
                             distanceMatrix[route[route.size() - 2]][route[route.size() - 1]];
    }

    auto numberOfVehicles = routes.size();
    std::cout << "Time schedule: " << std::endl;
    for (auto & i : timeSchedule) {
        for (double j : i) {
            std::cout << j << " | ";
        }
        std::cout << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
    }
    std::cout << "Total distance: " << totalDistance << std::endl;
    std::cout << "Total schedule time: " << totalScheduleTime << std::endl;
    std::cout << "Number of vehicles: " << numberOfVehicles << std::endl;
    std::cout << "Waiting time at customers: " << waitingTimeInSchedule << std::endl;
    std::cout << "Number of customers served: " << numberOfCustomersServed << std::endl;
}

void solomon::calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix) {
    for (int i = 0; i <= customers.size(); ++i) {
        auto distVec = new std::vector<double>;
        for (int j = 0; j < customers.size(); ++j) {
            distVec->push_back(INT_MAX - 1);
        }
        distanceMatrix.emplace_back(*distVec);
    }
    for (int i = 0; i < customers.size(); ++i) {
        for (int j = 0; j < customers.size(); ++j) {
            if (i == j) {
                distanceMatrix[i][j] = 0;
            } else {
                distanceMatrix[i][j] = sqrt(pow(customers[i].getXcord() - customers[j].getXcord(), 2) +
                                            pow(customers[i].getYcord() - customers[j].getYcord(), 2));
            }
        }
    }
    for (int i = 0; i < customers.size(); ++i) {
        distanceMatrix[customers.size()][i] = sqrt(pow(customers[0].getXcord() - customers[i].getXcord(), 2) +
                                    pow(customers[0].getYcord() - customers[i].getYcord(), 2));
    }
}

unsigned int solomon::findCustomerWithEarliestDeadline(std::vector<customer> &customers) {
    double min = INT_MAX - 1;
    unsigned int minIndex = 0;
    for (int i = 0; i < customers.size(); ++i) {
        if (!customers[i].isRouted()
            && customers[i].getDueDate() < min) {
            min = customers[i].getDueDate();
            minIndex = i;
        }
    }
    return minIndex;
}

unsigned int solomon::findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix,
                                                   std::vector<customer> &customers) {
    double max = 0;
    unsigned int maxIndex = 0;
    for (int i = 0; i < customers.size(); ++i) {
        if (!customers[i].isRouted()
            && distanceMatrix[0][i] > max) {
            max = distanceMatrix[0][i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

void solomon::calculatePushForward(std::vector<double> &pushForward, const std::vector<int> &route, int u, int position,
                                   std::vector<double> &timeWaitedAtCustomer,
                                   std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers,
                                   double timeOfService, double waitingTime) {
    double epsilon = 0.0000001; //kvoli double
    auto i = route[position - 1];
    auto j = route[position];
    if (route.size() > 2 && j != customers.size()) {
        if (customers[j].getDueDate() > timeOfService + customers[u].getServiceTime() + distanceMatrix[u][j]) {
            pushForward.emplace_back(std::max(0.0,distanceMatrix[i][u] + distanceMatrix[u][j] + customers[u].getServiceTime()
                                                  - distanceMatrix[i][j] - timeWaitedAtCustomer[j] + waitingTime));
        }   else {
            pushForward.emplace_back(INT_MAX - 1);
        }
        for (int n = position + 1; n < route.size(); ++n) {
            i = pushForward.size() - 1;
            j = route[n];
            if (pushForward[i] - timeWaitedAtCustomer[j] > 0 + epsilon) {
                pushForward.emplace_back(pushForward[i] - timeWaitedAtCustomer[j]);
            } else if (pushForward[pushForward.size() - 1] != 0) {
                pushForward.emplace_back(0);
                break;
            }
        }
    } else if (j == customers.size()) {
        pushForward.emplace_back(std::max(0.0, distanceMatrix[i][u] + distanceMatrix[u][0] + customers[u].getServiceTime()
                                              - distanceMatrix[i][0]) + waitingTime);
    }
}

void solomon::calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                                    std::vector<int> &route, const std::vector<customer> &customers, int zakaznikU,
                                    std::vector<double> &beginingOfService, double timeOfService,
                                    std::vector<std::vector<double>> &distanceMatrix, int u) {
    double epsilon = 0.0000001;
    if (route.size() > 2 && route[zakaznikU] != customers.size() && pushForward[0] != 0) {
        int pf = 0;
        beginingOfService[zakaznikU] = beginingOfService[zakaznikU] + pushForward[pf];
        timeWaitedAtCustomer[route[zakaznikU]] = 0;
        pf++;
        for (int n = zakaznikU + 1; n < route.size(); ++n) {
            int j = route[n];
            if (timeWaitedAtCustomer[j] - pushForward[pf] < 0 + epsilon && pf < pushForward.size()) {
                beginingOfService[n] = beginingOfService[n] + pushForward[pf];
                timeWaitedAtCustomer[j] = 0;
                pf++;
            } else if (pf < pushForward.size()) {
                waitingTimeMath(timeWaitedAtCustomer, beginingOfService, route, customers, distanceMatrix, zakaznikU,
                                timeOfService, u);
                break;
            }
        }
    } else if (route.size() > 2 && route[zakaznikU] != customers.size() && pushForward[0] == 0) {
        waitingTimeMath(timeWaitedAtCustomer, beginingOfService, route, customers, distanceMatrix, zakaznikU,
                        timeOfService, u);
    } else if (route[route.size() - 1] == customers.size()) {
        beginingOfService[route.size() - 1] += pushForward[pushForward.size() - 1];
        timeWaitedAtCustomer[route[route.size() - 1]] = customers[0].getDueDate() - beginingOfService[route.size() - 1];
    }
}

bool solomon::lema11(std::vector<double> &beginingOfService, std::vector<double> &pushForward, std::vector<int> &route,
                     std::vector<customer> &customers, int u, int position, double  beginingOfServiceU) {
    if (beginingOfServiceU <= customers[u].getDueDate()) {
        int j = 0;
        while (pushForward[j] > 0.00000001 && j < route.size() - 1 && j < pushForward.size()) {
            if (route[position + j] == customers.size() && beginingOfServiceU + pushForward[j] <= customers[0].getDueDate()) {
                return true;
            }
            if (beginingOfService[position + j] + pushForward[j] > customers[route[position + j]].getDueDate()) {
                return false;
            }
            ++j;
        }
        return true;
    }
    return false;
}

std::vector<std::tuple<int, double, int>>
solomon::findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                      std::vector<double> &beginingOfService, std::vector<double> &pushForward, std::vector<int> &route,
                      std::vector<customer> &customers, unsigned int currentlyUsedCapacity, int maxCapacity,
                      std::vector<double> &timeWaitedAtCustomer, int unvisitedCustomers) {
    std::vector<std::tuple<int, double, int>> mnozinaC1;
    int minIndex = 0;
    for (int u = 1; u < customers.size(); ++u) {
        double min = INT_MAX - 1;
        if (!customers[u].isRouted()
            && currentlyUsedCapacity + customers[u].getDemand() <= maxCapacity) { //kontrola kapacity vozidla
            for (int i = 1; i < route.size(); ++i) {
                pushForward.clear();

                double timeOfService = beginingOfService[i - 1]
                                       + distanceMatrix[route[i - 1]][u]
                                       + customers[route[i - 1]].getServiceTime();
                double waitingTime = 0;
                if (timeOfService < customers[u].getReadyTime()) {
                    waitingTime = customers[u].getReadyTime() - timeOfService;
                    timeOfService = customers[u].getReadyTime();
                }
                calculatePushForward(pushForward, route, u, i, timeWaitedAtCustomer,
                                     distanceMatrix, customers, timeOfService, waitingTime);
                //podmienky lemma 1.1
                if (lema11(beginingOfService, pushForward, route, customers, u, i, timeOfService)) {
                    double c11 = 0;
                    double c12 = 0;
                    if (route.size() > 1) {
                        c11 = distanceMatrix[route[i - 1]][u] + distanceMatrix[u][route[i]] -
                              distanceMatrix[route[i]][route[i]];
                        c12 = pushForward[0];
                    }
                    double c1 = alfa1 * c11 + alfa2 * c12;
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

void solomon::insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion,
                                   std::vector<double> &beginingOfService, std::vector<customer> &customers,
                                   std::vector<double> &timeWaitedAtCustomer, unsigned int &currentlyUsedCapacity,
                                   std::vector<std::vector<double>> &distanceMatrix, std::vector<double> &pushForward) {
    int i = optimalInsertion.first;
    int u = optimalInsertion.second;
    if (route.size() > 1) {
        int predchodca = route[i - 1];
        double timeOfService = beginingOfService[i - 1]
                               + distanceMatrix[predchodca][u]
                               + customers[predchodca].getServiceTime();
        if (timeOfService < customers[u].getReadyTime()) {
            timeWaitedAtCustomer[u] = customers[u].getReadyTime() - timeOfService;
            timeOfService = customers[u].getReadyTime();
        } else {
            timeWaitedAtCustomer[u] = 0;
        }
        pushForward.clear();
        calculatePushForward(pushForward, route, u, i, timeWaitedAtCustomer, distanceMatrix, customers, timeOfService, timeWaitedAtCustomer[u]);
        calculateNewBeginings(pushForward, timeWaitedAtCustomer, route, customers, i, beginingOfService, timeOfService, distanceMatrix, u);
        route.insert(route.begin() + i, u);
        beginingOfService.insert(beginingOfService.begin() + i, timeOfService);
        currentlyUsedCapacity += customers[u].getDemand();
        customers[u].markAsRouted();
    } else {
        route.insert(route.begin() + i, u);
        beginingOfService.insert(beginingOfService.begin() + i, 0);
        customers[u].markAsRouted();
    }
}

void solomon::createNewRoute(unsigned int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes,
                                         std::vector<int> &route, std::vector<double> &beginingOfService,
                                         std::vector<double> &pushForward) {
    currentlyUsedCapacity = 0;
    pushForward.clear();
    std::vector<int> newRoute;
    newRoute.swap(route);
    routes.push_back(newRoute);
    route.clear();
    beginingOfService.clear();
}

void solomon::waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                              std::vector<int> &route, const std::vector<customer> &customers,
                              std::vector<std::vector<double>> &distanceMatrix, int index, double timeOfServicePrevious,
                              int u) {
    auto nextInRoute = route[index];
    int j = nextInRoute;
    if (nextInRoute == customers.size()) {
        int j = 0;
        auto distance = distanceMatrix[u][j];
        auto serviceTime = customers[u].getServiceTime();
        beginingOfService[index] = timeOfServicePrevious + distance + serviceTime;
        timeWaitedAtCustomer[nextInRoute] = customers[0].getDueDate() - beginingOfService[index];
    } else {
        auto distance = distanceMatrix[u][j];
        auto serviceTime = customers[u].getServiceTime();
        auto startNext = beginingOfService[index];
        timeWaitedAtCustomer[j] = startNext - (timeOfServicePrevious + distance + serviceTime);
    }
}
