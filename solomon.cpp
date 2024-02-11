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
                 double lambda, double q, bool startingCriteria, double eta) {
    totalDistance = INT_MAX - 1;
    calculateDistances(customers, distanceMatrix);
    maxN = calculateMaxN(eta);
    unvisitedCustomers = customers.size() - 1;
    currentlyUsedCapacity = 0;
    this->alfa1 = alfa1;
    this->alfa2 = alfa2;
    this->lambda = lambda;
    this->q = q;
    this->startingCriteria = startingCriteria;
    for (int i = 0; i <= customers.size(); ++i) {
        timeWaitedAtCustomer.push_back(0);
    }
    run(customers, unvisitedCustomers);
}

solomon::~solomon() {
    distanceMatrix.clear();
};


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

unsigned int solomon::findCustomerWithEarliestDeadline(std::vector<customer> &customers) {
    double min = INT_MAX - 1;
    unsigned int minIndex = 0;
    for (int i = 1; i < customers.size(); ++i) {
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
    for (int i = 1; i < customers.size(); ++i) {
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
                                   double timeOfService, double waitingTime, std::vector<double> &beginingOfService) {
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
        auto newbegin = timeOfService + customers[u].getServiceTime() + distanceMatrix[u][0];
        pushForward.emplace_back(newbegin - beginingOfService[beginingOfService.size() - 1]);
    }
}

void solomon::calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                                    std::vector<int> &route, const std::vector<customer> &customers, int zakaznikU,
                                    std::vector<double> &beginingOfService, double timeOfService,
                                    std::vector<std::vector<double>> &distanceMatrix, int u) {
    double epsilon = 0.0000001;
    if (route.size() > 2 && route[zakaznikU] != customers.size() && pushForward[0] != 0) {
        int pf = 0;
        beginingOfService[zakaznikU] += pushForward[pf];
        timeWaitedAtCustomer[route[zakaznikU]] = 0;
        pf++;
        for (int n = zakaznikU + 1; n < route.size(); ++n) {
            int j = route[n];
            u = route[n - 1];
            timeOfService = beginingOfService[n - 1];
            if (pushForward[pf] > 0 + epsilon && pf < pushForward.size()) {
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
solomon::findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                      std::vector<double> &beginingOfService, std::vector<double> &pushForward, std::vector<int> &route,
                      std::vector<customer> &customers, unsigned int currentlyUsedCapacity, int maxCapacity,
                      std::vector<double> &timeWaitedAtCustomer) {
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
                                     distanceMatrix, customers, timeOfService, waitingTime, beginingOfService);
                //podmienky lemma 1.1
                if (lema11(beginingOfService, pushForward, route, customers, u, i, timeOfService)) {
                    double c11 = 0;
                    double c12 = 0;
                    if (route.size() > 1) {
                        c11 = distanceMatrix[route[i - 1]][u] + distanceMatrix[u][route[i]] -
                              distanceMatrix[route[i - 1]][route[i]];
                        c12 = pushForward[0];
                    }
                    double c1 = alfa1 * c11 + alfa2 * c12;
                    /**tu sa pouziju metody na hluk*/
                    /**This decision is taken by the adaptive mechanism described earlier by keeping track of how often
                    the noise applied insertions and the “clean” insertions are successful.*/
//                    if (useNoise) { //ci sa ma pouzit hluk sa ma rozhodnut pri kazdej iteracii cize typujem v maine...
//                        auto noise = createNoise();
//                        c1 = std::max(0.0, c1 + noise);
//                    }
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
        calculatePushForward(pushForward, route, u, i, timeWaitedAtCustomer, distanceMatrix, customers, timeOfService, timeWaitedAtCustomer[u], beginingOfService);
        calculateNewBeginings(pushForward, timeWaitedAtCustomer, route, customers, i, beginingOfService, timeOfService, distanceMatrix, u);
        route.insert(route.begin() + i, u);
//        for (int j : route) {
//            std::cout << j << " ";
//        }
//        std::cout << std::endl;
//        std::cout << "insertovany: " << u << std::endl;
//        for (double j : beginingOfService) {
//            std::cout << j << " ";
//        }
//        std::cout << "------------------------------------------" << std::endl;
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
    usedCapacity.push_back(currentlyUsedCapacity);
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
        j = 0;
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

void solomon::run(std::vector<customer> &customers, int numberOfUnvisitedCustomers) {
    std::vector<int> route; /**upravujem iba lokalku nie cestu v roude*/
    static int vehicleCapacity = 200;
    unvisitedCustomers = numberOfUnvisitedCustomers;
    int routeIndex = 0;
    unsigned int index;
    bool alreadyIn = false;


//    std::cout << "unvisited: " << unvisitedCustomers << " | "<< std::endl;
//    for (int i = 1; i < customers.size(); ++i) {
//        if (!customers[i].isRouted()) {
//            std::cout << i << std::endl;
//        }
//    }

    if (routes.empty()) {
        //zaciatok cesty
        route.emplace_back(0);
        route.emplace_back(customers.size());
        beginingOfService.emplace_back(0);
        beginingOfService.emplace_back(0);
        timeWaitedAtCustomer[route[1]] = customers[0].getDueDate();
        index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, customers)
                                 : findCustomerWithEarliestDeadline(customers);
//        if (index != 0) { //bol najdeny zapaznik
            insertCustomerToRoad(route, std::make_pair(1, index), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward);
            unvisitedCustomers--;
//        }
    } else {
        route = routes[routeIndex];
        beginingOfService = timeSchedule[routeIndex];
        currentlyUsedCapacity = usedCapacity[routeIndex];
        alreadyIn = true;
    }

    /**------------------------------------------------------------------------------------------------------------*/

    while (unvisitedCustomers != 0) {
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, beginingOfService, pushForward, route, customers,
                               currentlyUsedCapacity, vehicleCapacity, timeWaitedAtCustomer);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, customers);
            insertCustomerToRoad(route, c2, beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity, distanceMatrix, pushForward);
            unvisitedCustomers--;
        } else if (routeIndex >= routes.size() && !alreadyIn) {
            if (!route.empty()) {
                timeSchedule.emplace_back(beginingOfService);
                createNewRoute(currentlyUsedCapacity, routes, route, beginingOfService, pushForward);
            } else {
                beginingOfService.clear();
            }
            route.emplace_back(0);
            route.emplace_back(customers.size());
            beginingOfService.emplace_back(0);
            beginingOfService.emplace_back(0);
            timeWaitedAtCustomer[route[1]] = customers[0].getDueDate();
            index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, customers)
                                     : findCustomerWithEarliestDeadline(customers);
            if (index != 0) { //bol najdeny zakaznik
                insertCustomerToRoad(route, std::make_pair(1, index), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                     distanceMatrix, pushForward);
                unvisitedCustomers--;
                routeIndex++;
            } else {
                alreadyIn = true;
            }
        } else {
            if (alreadyIn && route.size() > 2) {
                routes[routeIndex].clear();
                routes[routeIndex] = route;
                timeSchedule[routeIndex].clear();
                timeSchedule[routeIndex] = beginingOfService;
                usedCapacity[routeIndex] = currentlyUsedCapacity;
            }
            routeIndex++;
            if (routeIndex <= routes.size() - 1) {
                route = routes[routeIndex];
                beginingOfService = timeSchedule[routeIndex];
                currentlyUsedCapacity = usedCapacity[routeIndex];
                alreadyIn = true;
            } else {
                route.clear();
                beginingOfService.clear();
                currentlyUsedCapacity = 0;
                alreadyIn = false;
            }
        }
    }
    if (!alreadyIn) {
        routes.push_back(route);
        timeSchedule.emplace_back(beginingOfService);
        usedCapacity.emplace_back(currentlyUsedCapacity);
    } else if (route.size() > 2) {
        routes[routeIndex].clear();
        routes[routeIndex] = route;
        timeSchedule[routeIndex].clear();
        timeSchedule[routeIndex] = beginingOfService; //seq fault //pravdepodobne doslo k vzniku novej cesty a ten index este nebol apendnuty cize dostal seq fault
        usedCapacity[routeIndex] = currentlyUsedCapacity;
    }

    /**------------------------------------------------------------------------------------------------------------*/
    //zaverecny vypis
//    for (auto & route : routes) {
//        for (int j = 0; j < route.size(); ++j) {
//            std::cout << route[j];
//            if (j != route.size() - 1) {
//                std::cout << " -> ";
//            }
//        }
//        std::cout << std::endl;
//        std::cout << "--------------------------------" << std::endl;
//    }
    totalDistance = 0;
    double totalScheduleTime = 0;
    double waitingTimeInSchedule = 0;
    auto numberOfCustomersServed = 0;
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size() - 2; ++j) {
            totalDistance += distanceMatrix[routes[i][j]][routes[i][j + 1]];
        }
        totalDistance += distanceMatrix[routes[i][routes[i].size() - 2]][0];
        numberOfCustomersServed += routes[i].size() - 2;
        int j = routes[i].size() - 1;
    }
    for (int i = 0; i < timeSchedule.size(); ++i) {
        auto t = timeSchedule[i].size() - 1;
        totalScheduleTime += (timeSchedule[i][t]);
    }

    for (int i = 1; i < timeWaitedAtCustomer.size() - 1; ++i) {
        waitingTimeInSchedule += timeWaitedAtCustomer[i];
    }
    auto numberOfVehicles = routes.size();
//    std::cout << "Total distance: " << totalDistance << std::endl;
//    std::cout << "Total schedule time: " << totalScheduleTime << std::endl;
//    std::cout << "Number of vehicles: " << numberOfVehicles << std::endl;
//    std::cout << "Waiting time at customers: " << waitingTimeInSchedule << std::endl;
//    std::cout << "Number of customers served: " << numberOfCustomersServed << std::endl;
}

double solomon::getDistance() const {
    return totalDistance;
}

std::vector<std::vector<int>> &solomon::getRoutes() {
    return routes;
}

std::vector<std::vector<double>> &solomon::getTimeSchedule() {
    return timeSchedule;
}

std::vector<double> &solomon::getWaitingTime() {
    return timeWaitedAtCustomer;
}

const std::vector<std::vector<double>> &solomon::getDistanceMatrix() const {
    return distanceMatrix;
}

std::vector<double> &solomon::getUsedCapacity() {
    return usedCapacity;
}

void solomon::setDistance(double distance) {
    totalDistance = distance;
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
