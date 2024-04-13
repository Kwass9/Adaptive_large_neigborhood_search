//
// Created by andrej on 15.11.2023.
//

#include <random>
#include <algorithm>
#include "ShawRemoval.h"
#include <cmath>
#include <map>
#include <iostream>

Shaw_Removal::Shaw_Removal(double f, double ch, double ps, double o, int p, int problemSize) : fi(f), chi(ch), psi(ps), omega(o), p(p) {
    R.resize(problemSize);
}

Shaw_Removal::~Shaw_Removal() = default;

void Shaw_Removal::calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix, std::vector<Vehicle> &vehicles, int r) {
    int nasledovnik_r = 0;
    int route_number_r = 0;
    int index_r;
    int nasledovnik_r_index;

    /**vytahal som si data do jednej struktury nech nemusim prerabat stary vypocet*/
    //TODO bude sa dat prepisat kod po zmene riesenia v solomonovy, tak aby sa dal lahsie citat
    std::vector<std::vector<int>> routes;
    std::vector<std::vector<double>> timeSchedule;
    for (auto & vehicle : vehicles) {
        routes.emplace_back(vehicle.getRoute());
        timeSchedule.emplace_back(vehicle.getTimeSchedule());
    }


    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 1; j < routes[i].size() - 1; ++j) {
            if (routes[i][j] == r) {
                nasledovnik_r = routes[i][j + 1];
                nasledovnik_r_index = j + 1;
                route_number_r = i;
                index_r = j;
            }
        }
    }
    std::map<int ,double> DistanceRelatedness;
    std::map<int ,double> TimeRelatedness;
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 1; j < routes[i].size() - 1; ++j) {
            if (routes[i][j] != 0 && routes[i][j] != distanceMatrix.size() && routes[i][j] != r) {
                DistanceRelatedness.emplace(routes[i][j], std::abs(distanceMatrix[r][nasledovnik_r] - distanceMatrix[routes[i][j]][routes[i][j + 1]]));
                TimeRelatedness.emplace(routes[i][j] ,std::abs(std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][nasledovnik_r_index])
                                             - std::abs(timeSchedule[i][j] - timeSchedule[i][j + 1])));
            }
        }
    }

    normalize(DistanceRelatedness);
    normalize(TimeRelatedness);

    for (auto & route : routes) {
        for (int j = 1; j < route.size() - 1; ++j) {
            R[route[j]] = fi * DistanceRelatedness[route[j]] + chi * TimeRelatedness[route[j]];
            R[route[j]] /= (2 * (fi + chi));
        }
    }
}

void Shaw_Removal::removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                                  std::vector<customer> &customers,
                                  const int &ro,
                                  std::vector<double> &waitingTime,
                                  std::vector<Vehicle> &vehicles,
                                  std::vector<customer*> &unservedCustomers) {
    D.clear();
    auto r = generateRandomNumber(1, (int)customers.size() - 1);
    auto it = std::find_if(unservedCustomers.begin(), unservedCustomers.end(),
                      [r](const customer* customerPtr) { return customerPtr->getId() == r; });
    while (it != unservedCustomers.end() || customers[r].hasSpecificRequirements()) {
        r = generateRandomNumber(1, (int)customers.size() - 1);
        it = std::find_if(unservedCustomers.begin(), unservedCustomers.end(),
                          [r](const customer* customerPtr) { return customerPtr->getId() == r; });
    }
    D.emplace_back(r);
    std::vector<std::pair<int, double>> L;
    while (D.size() < ro) {
        calculateRelatedness(distanceMatrix,vehicles, r);
        calculateL(L, (int)customers.size());
        calculateD(ro,L, r, (int)customers.size());
    }
    editSolution(distanceMatrix,customers,D,waitingTime, vehicles, unservedCustomers);
}

void Shaw_Removal::editSolution(std::vector<std::vector<double>> &distanceMatrix,
                                std::vector<customer> &customers,
                                std::vector<int> &D,
                                std::vector<double>& waitingTime, std::vector<Vehicle> &vehicles,
                                std::vector<customer*> &unservedCustomers) {

    std::vector<std::vector<int>> routes;
    std::vector<std::vector<double>> timeSchedule;
    for (const auto & vehicle : vehicles) {
        routes.emplace_back(vehicle.getRoute());
        timeSchedule.emplace_back(vehicle.getTimeSchedule());
    }


    for (int k : D) {
        for (int i = 0; i < timeSchedule.size(); ++i) {
            for (int j = 1; j < timeSchedule[i].size(); ++j) {
                if (routes[i][j] == k) {
                    if (std::find(unservedCustomers.begin(), unservedCustomers.end(), &customers[k]) == unservedCustomers.end()) {
                        unservedCustomers.emplace_back(&customers[k]);
                    }
                    waitingTime[k] = 0;
                    auto nWindows = customers[k].getTimeWindows().size();

                    auto winP = customers[k].getTimeWindow(timeSchedule[i][j]);
                    auto winIndex = customers[k].getIndexOfTimeWindow(winP.first, winP.second);
                    auto win = customers[k].getTimeWindows()[winIndex];
                    vehicles[i].setUsedCapacity(vehicles[i].getUsedCapacity() - win.getDemand());
                    for (int i = 0; i < nWindows; ++i) {
                        customers[k].getTimeWindows()[winIndex].setCurrenVehiclesServing(0);
//                        std::cout << "||||||||||||||||" << customers[k].getTimeWindows()[winIndex].getNumberOfVehiclesServing() << std::endl;
                    }
                    timeSchedule[i].erase(timeSchedule[i].begin() + j);
                    routes[i].erase(routes[i].begin() + j);

                    vehicles[i].setRoute(routes[i]);
                    vehicles[i].setTimeSchedule(timeSchedule[i]);
                    vehicles[i].removeCustomerFromServed(k);
//                    for (int o = 0; o < nWindows; ++o) {
//                        for (int l = (int)customers[k].getPreviouslyServedBy().size() - 1; l >= 0; l--) {
//                            if (customers[k].getPreviouslyServedBy()[l] == i) {
//                                customers[k].removePreviouslyServedByTime(l);
//                                customers[k].removePreviouslyServedBy(l);
                    customers[k].clearPreviouslyServedBy();
                    customers[k].clearPreviouslyServedByTime();
//                                break;
//                            }
//                        }
//                    }

//                    auto l = j;
//                    while (l < timeSchedule[i].size()) {
//                        auto indexNasledovnik = routes[i][l];
//                        if (indexNasledovnik == customers.size()) {
//                            indexNasledovnik = 0;
//                        }
//                        auto indexPredchodca = routes[i][l - 1];
//                        auto nasledovnik = l;
//                        auto predchodca = l - 1;
//
//                        auto winPPredchodca = customers[indexPredchodca].getTimeWindow(timeSchedule[i][predchodca]);
//                        auto winIndexPredchodca = customers[indexPredchodca].getIndexOfTimeWindow(winPPredchodca.first, winPPredchodca.second);
//                        auto winPredchodca = customers[indexPredchodca].getTimeWindows()[winIndexPredchodca];
//
//                        auto winPNasledovnik = customers[indexNasledovnik].getTimeWindow(timeSchedule[i][nasledovnik]);
//                        auto winIndexNasledovnik = customers[indexNasledovnik].getIndexOfTimeWindow(winPNasledovnik.first, winPNasledovnik.second);
//                        auto winNasledovnik = customers[indexNasledovnik].getTimeWindows()[winIndexNasledovnik];
//
//                        auto newTimeOfService = timeSchedule[i][predchodca] + winPredchodca.getServiceTime()
//                                                + distanceMatrix[indexPredchodca][indexNasledovnik];
//                        if (newTimeOfService < winNasledovnik.getReadyTime()) {
//                            timeSchedule[i][nasledovnik] = winNasledovnik.getReadyTime();
//                            vehicles[i].setTimeSchedule(timeSchedule[i]);
//                            auto iServ = customers[indexNasledovnik].findIndexOfPreviouslyServedBy(i);
//                            customers[indexNasledovnik].editPreviouslyServedByTime(winNasledovnik.getReadyTime(), iServ);
//                            waitingTime[indexNasledovnik] = winNasledovnik.getReadyTime() - newTimeOfService;
//                        } else {
//                            timeSchedule[i][nasledovnik] = newTimeOfService;
//                            vehicles[i].setTimeSchedule(timeSchedule[i]);
//                            auto iServ = customers[indexNasledovnik].findIndexOfPreviouslyServedBy(i);
//                            customers[indexNasledovnik].editPreviouslyServedByTime(winNasledovnik.getReadyTime(), iServ);
//                            waitingTime[indexNasledovnik] = 0;
//                        }
//                        ++l;
//                    }
                    j = 0;
                }
            }
        }
    }
    for (int i = 0; i < routes.size(); i++) {
        for (int j = 1; j < routes[i].size(); ++j) {
            auto indexNasledovnik = routes[i][j];
            if (indexNasledovnik == customers.size()) {
                indexNasledovnik = 0;
            }
            auto indexPredchodca = routes[i][j - 1];
            auto nasledovnik = j;
            auto predchodca = j - 1;

            auto winPPredchodca = customers[indexPredchodca].getTimeWindow(timeSchedule[i][predchodca]);
            auto winIndexPredchodca = customers[indexPredchodca].getIndexOfTimeWindow(winPPredchodca.first, winPPredchodca.second);
            auto winPredchodca = customers[indexPredchodca].getTimeWindows()[winIndexPredchodca];

            auto winPNasledovnik = customers[indexNasledovnik].getTimeWindow(timeSchedule[i][nasledovnik]);
            auto winIndexNasledovnik = customers[indexNasledovnik].getIndexOfTimeWindow(winPNasledovnik.first, winPNasledovnik.second);
            auto winNasledovnik = customers[indexNasledovnik].getTimeWindows()[winIndexNasledovnik];

            auto newTimeOfService = timeSchedule[i][predchodca] + winPredchodca.getServiceTime()
                                            + distanceMatrix[indexPredchodca][indexNasledovnik];
            if (winNasledovnik.getNumberOfVehiclesServing() != 2) {
                if (newTimeOfService < winNasledovnik.getReadyTime()) {
                    timeSchedule[i][nasledovnik] = winNasledovnik.getReadyTime();
                    vehicles[i].setTimeSchedule(timeSchedule[i]);
                    auto servedByTimeToEdit = customers[indexNasledovnik].getPreviouslyServedByTimes()[winIndexNasledovnik];
                    customers[indexNasledovnik].editPreviouslyServedByTime(winNasledovnik.getReadyTime(), servedByTimeToEdit);
                    waitingTime[indexNasledovnik] = winNasledovnik.getReadyTime() - newTimeOfService;
                } else {
                    timeSchedule[i][nasledovnik] = newTimeOfService;
                    vehicles[i].setTimeSchedule(timeSchedule[i]);
                    if (indexNasledovnik != 0) {
                        auto servedByTimeToEdit = customers[indexNasledovnik].getPreviouslyServedByTimes()[winIndexNasledovnik];
                        customers[indexNasledovnik].editPreviouslyServedByTime(winNasledovnik.getReadyTime(), servedByTimeToEdit);
                    } else {
                        auto servedByTimeToEdit = timeSchedule[i][timeSchedule[i].size() - 1];
                        customers[indexNasledovnik].editPreviouslyServedByTime(winNasledovnik.getReadyTime(), servedByTimeToEdit);
                    }
                    waitingTime[indexNasledovnik] = 0;
                }
            }
        }
    }
//    if (!customers[124].getPreviouslyServedBy().empty()) {
//        auto ser124 = customers[124].getPreviouslyServedBy()[0];
//        for (int i = 0; i < routes[ser124].size(); i++) {
//            for (int l : routes[ser124]) {
//                std::cout << l << " ";
//            }
//            std::cout << std::endl;
//            for (double l : timeSchedule[ser124]) {
//                std::cout << l << " ";
//            }
//            std::cout << std::endl;
//        }
//        for (int i = 0; i < customers[124].getPreviouslyServedBy().size(); i++) {
//            std::cout << customers[124].getPreviouslyServedBy()[i] << " ";
//        }
//        std::cout << std::endl;
//        for (int i = 0; i < customers[124].getPreviouslyServedByTimes().size(); ++i) {
//            std::cout << customers[124].getPreviouslyServedByTimes()[i] << " ";
//        }
//    }
}

int Shaw_Removal::generateRandomNumber(int min, int max) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

double Shaw_Removal::generateRandomDouble(double min, double max) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

void Shaw_Removal::normalize(std::map<int, double>& map) {
    double max = 0;
    for (auto &itr : map) {
        if (itr.second > max) {
            max = itr.second;
        }
    }
    for (auto &itr : map) {
        itr.second /= max;
    }
}

void Shaw_Removal::calculateD(const int &ro, std::vector<std::pair<int, double>>& L, int r, int s) {
    auto y = generateRandomDouble(0, 1);
    auto index = (int)std::pow(y, p) * (L.size() - 1);
    auto itr = L.begin();
    std::advance(itr, std::min(static_cast<int>(std::pow(y, p) * static_cast<int>(L.size() - 1)), static_cast<int>(L.size() - 1)));

    if (std::find(D.begin(), D.end(), L[index].first) == D.end()
        && L[index].first != r
        && L[index].first != 0
        && L[index].first != s) {
        D.emplace_back(L[index].first);
        L.erase(itr);
    }
    if (D.size() < ro) { //TODO toto bude treba skontrolovat este
        r = generateRandomNumber(1, s - 1);
        while (std::find(D.begin(), D.end(), r) != D.end()) { //pokial r je v D vyber nove r
            r = generateRandomNumber(1, s - 1);
        }
    }
}

void Shaw_Removal::calculateL(std::vector<std::pair<int, double>> &L, int s) {
    L.clear();
    for (int i = 1; i < s; ++i) {
        if (std::find(D.begin(), D.end(), i) == D.end()) {
            L.emplace_back(i, R[i]); /**mal som tu i + 1 dal som to + 1 doprec lebo nechapem co som tam vlastne robil tym...*/
        }
    }
    std::sort(L.begin(), L.end(), [&](std::pair<int, double> a, std::pair<int, double> b) { return a.second < b.second; });
}

