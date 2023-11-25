//
// Created by andrej on 15.11.2023.
//

#include <random>
#include <algorithm>
#include <iostream>
#include "ShawRemoval.h"

Shaw_Removal::Shaw_Removal(double f, double ch, double p, double o, int qve, int problemSize) : fi(f), chi(ch), psi(p), omega(o), q(qve) {
    R.resize(problemSize);
}

Shaw_Removal::~Shaw_Removal() = default;

std::vector<double> Shaw_Removal::calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                                       std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                                       std::vector<std::vector<double>> &timeSchedule,
                                                       int r) {
    int nasledovnik_r;
    int route_number_r;
    int index_r;
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size(); ++j) {
            if (routes[i][j] == r) {
                nasledovnik_r = routes[i][j + 1];
                route_number_r = i;
                index_r = j;
            }
        }
    }
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size(); ++j) {
            if (index_r != j && i != route_number_r) {
                R[j] = fi * (distanceMatrix[r][nasledovnik_r] + distanceMatrix[routes[i][j]][routes[i][j + 1]])
                       + chi * (std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][routes[route_number_r][index_r + 1]])
                                + std::abs(timeSchedule[i][j] - timeSchedule[i][j + 1]))
                       + psi * (customers[r].getDemand() - customers[j].getDemand()); /**pozriet ci to bolo demand*/
                //+ omega * (1 - () / std::min(,));
                /**ktore vozidla vedia obsluzit zakaznika*/
            }
        }
    }
    return R;
}

int Shaw_Removal::removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                                  std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                  std::vector<std::vector<double>> &timeSchedule, int p, std::vector<double> &waitingTime,
                                  std::vector<double> &usedCapacity) {
    D.clear();
    srand((unsigned)time(nullptr));
    auto r = rand() % customers.size();
    //randomly selected requests to be removed
    D.emplace_back(r);
    std::vector<int> L; //requests
    for (int i = 0; i < customers.size(); ++i) {
        if (i != r) {
            L.emplace_back(i);
        }
    }
    while (D.size() < q) {
        r = D[rand() % D.size()];
        calculateRelatedness(distanceMatrix, customers, routes, timeSchedule, r);
        std::sort(L.begin(), L.end(), [&](int a, int b) { return R[a] < R[b]; });
        auto y = (double)rand() / RAND_MAX;
        D.emplace_back(L[std::pow(y, p) * (L.size() - 1)]);
        L.erase(L.begin() + std::pow(y, p) * (L.size() - 1));
    }
    editSolution(distanceMatrix,customers,routes,timeSchedule, D, waitingTime, usedCapacity);
    return D.size();
}

void Shaw_Removal::editSolution(std::vector<std::vector<double>> &distanceMatrix,
                                std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                std::vector<std::vector<double>> &timeSchedule,
                                std::vector<int> &D, std::vector<double>& waitingTime, std::vector<double>& usedCapacity) {
    for (int i : D) {
        std::cout << i << " vyhodeny zakaznik" << std::endl;
    }
    for (int k : D) {
        for (int i = 0; i < timeSchedule.size(); ++i) {
            for (int j = 0; j < timeSchedule[i].size(); ++j) {
                if (routes[i][j] == k) {
                    waitingTime[k] = 0;
                    auto wait = timeSchedule[i][j + 1] -
                                timeSchedule[i][j - 1] + customers[routes[i][j]].getDemand() + distanceMatrix[routes[i][j - 1]][routes[i][j + 1]];
                    auto indexNasledovnik = routes[i][j + 1];
                    waitingTime[indexNasledovnik] = wait;
                    usedCapacity[i] = usedCapacity[j] - customers[k].getDemand();
                    timeSchedule[i].erase(timeSchedule[i].begin() + j);
                    routes[i].erase(routes[i].begin() + j);
                    if (routes[i].size() == 2) {
                        routes.erase(routes.begin() + i);
                        timeSchedule.erase(timeSchedule.begin() + i);
                        usedCapacity.erase(usedCapacity.begin() + i);
                    }
                    customers[k].markAsUnrouted();
                    break;
                }
            }
        }
    }
}
