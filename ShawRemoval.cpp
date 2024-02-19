//
// Created by andrej on 15.11.2023.
//

#include <random>
#include <algorithm>
#include "ShawRemoval.h"
#include <cmath>
#include <map>
#include <random>

Shaw_Removal::Shaw_Removal(double f, double ch, double ps, double o, int p, int problemSize) : fi(f), chi(ch), psi(ps), omega(o), p(p) {
    R.resize(problemSize);
}

Shaw_Removal::~Shaw_Removal() = default;

void Shaw_Removal::calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix, std::vector<std::vector<int>> &routes,
                                                       std::vector<std::vector<double>> &timeSchedule, int r) {
    int nasledovnik_r = 0;
    int route_number_r = 0;
    int index_r;
    int nasledovnik_r_index;
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
        for (int j = 0; j < routes[i].size() - 1; ++j) {
            if (routes[i][j] != 0 && routes[i][j] != 101 && routes[i][j] != r) {
                DistanceRelatedness.emplace(routes[i][j], std::abs(distanceMatrix[r][nasledovnik_r] - distanceMatrix[routes[i][j]][routes[i][j + 1]]));
                TimeRelatedness.emplace(routes[i][j] ,std::abs(std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][nasledovnik_r_index])
                                             - std::abs(timeSchedule[i][j] - timeSchedule[i][j + 1])));
            }
        }
    }

    //normalizacia
    double maxCurrently = 0;
    std::map<int, double>::iterator currentItr;
    for (currentItr = DistanceRelatedness.begin(); currentItr != DistanceRelatedness.end(); ++currentItr) {
        if (currentItr->second > maxCurrently) {
            maxCurrently = currentItr->second;
        }
    }
    for (currentItr = DistanceRelatedness.begin(); currentItr != DistanceRelatedness.end(); ++currentItr) {
        currentItr->second /= maxCurrently;
    }
    maxCurrently = 0;
    for (currentItr = TimeRelatedness.begin(); currentItr != TimeRelatedness.end(); ++currentItr) {
        if (currentItr->second > maxCurrently) {
            maxCurrently = currentItr->second;
        }
    }
    for (currentItr = TimeRelatedness.begin(); currentItr != TimeRelatedness.end(); ++currentItr) {
        currentItr->second /= maxCurrently;
    }

    for (auto & route : routes) {
        for (int j = 1; j < route.size() - 1; ++j) {
            R[route[j]] = fi * DistanceRelatedness[route[j]] + chi * TimeRelatedness[route[j]];
            R[route[j]] /= (2 * (fi + chi));
        }
    }
}

void Shaw_Removal::removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                                  std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                  std::vector<std::vector<double>> &timeSchedule, const int &ro, std::vector<double> &waitingTime,
                                  std::vector<double> &usedCapacity) {
    D.clear();
    srand((unsigned)time(nullptr));
    auto r = rand() % customers.size();
    while (r == 0 || r == 101) {
        r = rand() % customers.size();
    }
//    randomly selected requests to be removed
    D.emplace_back(r);
    std::vector<std::pair<int, double>> L;
    while (D.size() < ro) {
        calculateRelatedness(distanceMatrix, routes, timeSchedule, r);
        L.clear();
        for (int i = 1; i < customers.size(); ++i) {
            if (std::find(D.begin(), D.end(), i) == D.end()) {
                L.emplace_back(i, R[i]); /**mal som tu i + 1 dal som to + 1 doprec lebo nechapem co som tam vlastne robil tym...*/
            }
        }
        std::sort(L.begin(), L.end(), [&](std::pair<int, double> a, std::pair<int, double> b) { return a.second < b.second; });
        auto y = (double)rand() / RAND_MAX;
        if (std::find(D.begin(), D.end(), L[std::pow(y, p) * (L.size() - 1)].first) == D.end()
            && L[std::pow(y, p) * (L.size() - 1)].first != r
            && L[std::pow(y, p) * (L.size() - 1)].first != 0
            && L[std::pow(y, p) * (L.size() - 1)].first != customers.size()) {
            D.emplace_back(L[std::pow(y, p) * (L.size() - 1)].first);
            L.erase(L.begin() + std::pow(y, p) * (L.size() - 1));
        }
        if (D.size() < ro) {
            r = D[rand() % D.size()];
            while (std::find(D.begin(), D.end(), r) != D.end() || r == 0 || r == 101) { //pokial r je v D vyber nove r
                r = rand() % customers.size();
            }
        }
    }
    editSolution(distanceMatrix,customers,routes,timeSchedule, D, waitingTime, usedCapacity);
}

void Shaw_Removal::editSolution(std::vector<std::vector<double>> &distanceMatrix,
                                std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                std::vector<std::vector<double>> &timeSchedule,
                                std::vector<int> &D, std::vector<double>& waitingTime, std::vector<double>& usedCapacity) {
    for (int k : D) {
        for (int i = 0; i < timeSchedule.size(); ++i) {
            for (int j = 1; j < timeSchedule[i].size(); ++j) {
                if (routes[i][j] == k) {
                    customers[k].markAsUnrouted();
                    waitingTime[k] = 0;
                    usedCapacity[i] -= customers[k].getDemand();
                    timeSchedule[i].erase(timeSchedule[i].begin() + j);
                    routes[i].erase(routes[i].begin() + j);
                    if (routes[i].size() == 2) {
                        routes[i].clear();
                        routes.erase(routes.begin() + i);
                        timeSchedule[i].clear();
                        timeSchedule.erase(timeSchedule.begin() + i);
                        usedCapacity.erase(usedCapacity.begin() + i);
                        break;
                    }

                    auto l = j;
                    while (l < timeSchedule[i].size()) {
                        auto indexNasledovnik = routes[i][l];
                        auto indexPredchodca = routes[i][l - 1];
                        auto nasledovnik = l;
                        auto predchodca = l - 1;
                        auto newTimeOfService = timeSchedule[i][predchodca] + customers[indexPredchodca].getServiceTime()
                                                + distanceMatrix[indexPredchodca][indexNasledovnik];
                        if (newTimeOfService < customers[indexNasledovnik].getReadyTime() && indexNasledovnik != 101) {
                            timeSchedule[i][nasledovnik] = customers[indexNasledovnik].getReadyTime();
                            waitingTime[indexNasledovnik] = customers[indexNasledovnik].getReadyTime() - newTimeOfService;
                        } else {
                            timeSchedule[i][nasledovnik] = newTimeOfService;
                            waitingTime[indexNasledovnik] = 0;
                        }
                        ++l;
                    }
                    break;
                }
            }
        }
    }
}
