//
// Created by Andrej on 27/10/2023.
//

#include "solomon.h"
#include <vector>
#include <iostream>
#include "customer.h"
#include <cmath>
#include <tuple>

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
//    insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
//                         distanceMatrix, pushForward); //depo na zaciatku trasy
//    insertCustomerToRoad(route, std::make_pair(0, customers.size()), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
//                         distanceMatrix, pushForward); //depo na konci trasy navrat
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
//    beginingOfService[2] = beginingOfService[1] + customers[beginingOfService[1]].getServiceTime() + distanceMatrix[beginingOfService[1]][0];
//    timeWaitedAtCustomer[0] = customers[0].getDueDate() - beginingOfService[2];
    unvisitedCustomers--;

    /**------------------------------------------------------------------------------------------------------------*/

    while (unvisitedCustomers != 0) { //while unroute customers masti ich do ciest
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, beginingOfService, pushForward, route, customers,
                               currentlyUsedCapacity, vehicleCapacity, timeWaitedAtCustomer, unvisitedCustomers);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, route, customers);
//            c2.first += 1; //first je index predchodcu v ceste first + 1 je nova pozicia kam sa naj zakaznik vlozi
            insertCustomerToRoad(route, c2, beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity, distanceMatrix, pushForward);
            unvisitedCustomers--;
        } else {
            std::vector<double> previousBeginings = beginingOfService;
            for (int i = 0; i < route.size(); ++i) {
                std::cout << route[i] << " (" << beginingOfService[i] << ") | ";
            }
            timeSchedule.emplace_back(previousBeginings);
            std::cout << "Ostava zakaznikov: " << unvisitedCustomers << std::endl;
            std::cout << "Pouzita kapacita: " << currentlyUsedCapacity << std::endl;
            route = createNewRoute(currentlyUsedCapacity, routes, route, timeWaitedAtCustomer, customers, pushForward);
            beginingOfService.clear();
            beginingOfService.shrink_to_fit();
//            insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
//                                 distanceMatrix, pushForward); //depo na zaciatku trasy
//            insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
//                                 distanceMatrix, pushForward); //depo na konci trasy navrat
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
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size(); ++j) {
            std::cout << routes[i][j];
            if (j != routes[i].size() - 1) {
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
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size() - 1; ++j) {
            totalDistance += distanceMatrix[routes[i][j]][routes[i][j + 1]]; //tu zle ratam distance asi
        }
        for (int j = 0; j < routes[i].size() - 2; ++j) {
            waitingTimeInSchedule += timeWaitedAtCustomer[routes[i][j + 1]];
        }
        numberOfCustomersServed += routes[i].size() - 2;
//        totalScheduleTime += beginingOfService[routes[i].size() - 2] + customers[routes[i][routes[i].size() - 2]].getServiceTime() +
//                             distanceMatrix[routes[i][routes[i].size() - 2]][routes[i][routes[i].size() - 1]];
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
//        else if (!customers[i].isRouted()
//            && distanceMatrix[0][i] == max
//            && customers[i].getDueDate() < customers[maxIndex].getDueDate()) {
//            max = distanceMatrix[0][i];
//            maxIndex = i;
//        }
    }
    return maxIndex;
}

void solomon::calculatePushForward(std::vector<double> &pushForward, const std::vector<double> &beginingOfService,
                                   const std::vector<int> &route, int u, int position,
                                   std::vector<double> &timeWaitedAtCustomer,
                                   std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers) {
    double epsilon = 0.0000001; //kvoli double
    auto i = route[position - 1];
    auto j = route[position];
    if (route.size() > 2 && j != customers.size()) {
//        if (customers[j].getDueDate() < customers[u].getReadyTime() + customers[u].getServiceTime() + distanceMatrix[u][j]) { /**toto je zbytocne ten max to spravi za toto*/
//              pushForward.emplace_back(INT_MAX - 1);
//        } else {
        if (customers[j].getDueDate() > customers[u].getReadyTime() + customers[u].getServiceTime() + distanceMatrix[u][j]) {
            pushForward.emplace_back(std::max(0.0,distanceMatrix[i][u] + distanceMatrix[u][j] + customers[u].getServiceTime()
                                                  - distanceMatrix[i][j] - timeWaitedAtCustomer[j]));
        }   else {
            pushForward.emplace_back(INT_MAX - 1);
        }

//        }
        for (int n = position + 1; n < route.size(); ++n) {
            i = pushForward.size() - 1;
            j = route[n];
            if (pushForward[i] - timeWaitedAtCustomer[j] > 0 + epsilon) {
                pushForward.emplace_back(pushForward[i] - timeWaitedAtCustomer[j]);
            } else {
                pushForward.emplace_back(0);
                break;
            }
        }
    } else if (j == customers.size()) {
        pushForward.emplace_back(std::max(0.0, distanceMatrix[i][u] + distanceMatrix[u][0] + customers[u].getServiceTime()
                                              - distanceMatrix[i][0]) + timeWaitedAtCustomer[u]);
    }
}

void solomon::calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                                    std::vector<int> &route, const std::vector<customer> &customers, int zakaznikU,
                                    std::vector<double> &beginingOfService, double timeOfService) {
    double epsilon = 0.0000001;
    if (route.size() > 2 && !route[route.size() - 1] == customers.size()) { //ten if pojde asi von
        int j = route[zakaznikU];
        beginingOfService[j] = timeOfService + pushForward[0] - timeWaitedAtCustomer[j];
        for (int n = zakaznikU + 1; n < route.size() - 1; ++n) {
            int j = route[n + 1];
            if (pushForward[j] - timeWaitedAtCustomer[j] > 0 + epsilon) {
                beginingOfService[j] = beginingOfService[n] + pushForward[j] - timeWaitedAtCustomer[j];
                timeWaitedAtCustomer[j] = 0;
            } else {
                timeWaitedAtCustomer[j] -= pushForward[j];
                break;
            }
        }
    } else if (route[route.size() - 1] == customers.size()) {
        beginingOfService[route.size() - 1] += pushForward[pushForward.size() - 1];
        timeWaitedAtCustomer[route[route.size() - 1]] = customers[0].getDueDate() - beginingOfService[route.size() - 1];
    }
}

bool solomon::lema11(std::vector<double> &beginingOfService, std::vector<double> &pushForward, std::vector<int> &route,
                     std::vector<customer> &customers, int u, int position,
                     std::vector<std::vector<double>> &distanceMatrix) {
    int i = route[position - 1];
    auto beginingOfServiceU = beginingOfService[position - 1] + distanceMatrix[i][u] + customers[position - 1].getServiceTime();
    if (beginingOfServiceU <= customers[u].getDueDate()) {
//        if (position == route.size() - 1) { /**na co toto kontrolujem??*/
//            return false;
//        }
        int j = 0;
        while (pushForward[j] > 0.00000001 && j < route.size() - 1 && j < pushForward.size()) { //position je cislo iteracie
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
                calculatePushForward(pushForward, beginingOfService, route, u, i, timeWaitedAtCustomer,
                                     distanceMatrix, customers);
                //podmienky lemma 1.1
                if (lema11(beginingOfService, pushForward, route, customers, u, i, distanceMatrix)) {
                    double c11 = 0;
                    double c12 = 0;
                    if (route.size() > 1) {
                        c11 = distanceMatrix[route[i - 1]][u] + distanceMatrix[u][route[i]] -
                              distanceMatrix[route[i]][route[i]];
                        //pushForward[i + 1] - timeWaitedAtCustomer[i + 1] tento vyraz znamena beginingOfServiceNew
//                        c12 = std::max(0.0,pushForward[i + 1] - timeWaitedAtCustomer[i + 1]) - beginingOfService[i + 1]; /**toto by bolo fajn asi otestovat s max a bez max co da lepsie vysledky*/
                        c12 = pushForward[i + 1] - timeWaitedAtCustomer[i + 1] - beginingOfService[i + 1];
                    }
                    double c1 = alfa1 * c11 + alfa2 * c12;
                    if (c1 < min) {
                        min = c1;
                        minIndex = i;
                    }
                }
            }
            if (min != INT_MAX - 1) { //ak sa nasiel nejaky vhodny zakaznik
                mnozinaC1.emplace_back(std::make_tuple(minIndex, min, u));
            }
        }
    }
    return mnozinaC1;
}

std::pair<int, int> solomon::findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                                              std::vector<std::vector<double>> &distanceMatrix, std::vector<int> &route,
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
//        pushForward.shrink_to_fit();
        calculatePushForward(pushForward, beginingOfService, route, u, i, timeWaitedAtCustomer, distanceMatrix, customers);
        calculateNewBeginings(pushForward, timeWaitedAtCustomer, route, customers, i, beginingOfService, timeOfService);
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

std::vector<int> solomon::createNewRoute(unsigned int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes,
                                         std::vector<int> &route, std::vector<double> &timeWaitedAtCustomer,
                                         std::vector<customer> &customers, std::vector<double> &pushForward) {
    currentlyUsedCapacity = 0;
    for (int i = 0; i < customers.size(); ++i) {
        pushForward[i] = 0;
        timeWaitedAtCustomer[i] = 0;
    }
    routes.push_back(route);
    std::vector<int> newRoute;
    return newRoute;
}
