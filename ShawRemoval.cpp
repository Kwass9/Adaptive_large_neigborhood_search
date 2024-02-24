//
// Created by andrej on 15.11.2023.
//

#include <random>
#include <algorithm>
#include "ShawRemoval.h"
#include <cmath>
#include <map>

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
                                  std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                  std::vector<std::vector<double>> &timeSchedule, const int &ro, std::vector<double> &waitingTime,
                                  std::vector<double> &usedCapacity) {
    D.clear();
    auto r = generateRandomNumber(1, (int)customers.size() - 1);
    D.emplace_back(r);
    std::vector<std::pair<int, double>> L;
    while (D.size() < ro) {
        calculateRelatedness(distanceMatrix, routes, timeSchedule, r);
        calculateL(L, (int)customers.size()); //TODO toto bude treba skontrolovat este
        calculateD(ro,L, r, (int)customers.size()); //TODO toto bude treba skontrolovat este
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
        while (std::find(D.begin(), D.end(), r) != D.end() || r == 0 || r == 101) { //pokial r je v D vyber nove r
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

