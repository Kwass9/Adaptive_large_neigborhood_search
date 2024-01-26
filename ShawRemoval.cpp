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
    int nasledovnik_r = 0;
    int route_number_r = 0;
    int index_r;
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size() - 1; ++j) {
            if (routes[i][j] == r) {
                nasledovnik_r = routes[i][j + 1];
                route_number_r = i;
                index_r = j;
            }
        }
    }

//    std::cout << "r: " << r << std::endl;
//    std::cout << "route_number_r = " << route_number_r << std::endl;
//    std::cout << "index_r = " << index_r << std::endl;
//    std::cout << "nasledovnik_r = " << nasledovnik_r << std::endl;

    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size() - 1; ++j) {
            if (index_r != j && i != route_number_r) {

                /**tato cast ma invalid read cize zrejme miss index*/
                // index_r + 1 je asi chyba
//                auto y = timeSchedule[0][routes[route_number_r][index_r + 1]];

//                R[routes[i][j]] = timeSchedule[route_number_r][routes[route_number_r][index_r + 1]];
//                std::cout << "routes[route_number_r][index_r + 1] = " << routes[route_number_r][index_r + 1] << std::endl;

                R[routes[i][j]] = fi * (distanceMatrix[r][nasledovnik_r] + distanceMatrix[routes[i][j]][routes[i][j + 1]])
                        + chi * (std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][nasledovnik_r])
                                 + std::abs(timeSchedule[i][j] - timeSchedule[i][j + 1]));
                        + psi * std::abs(customers[r].getDemand() - customers[j].getDemand());
                        //+ omega * (1 - () / std::min(,));
                /**kolko vozidiel vie obsluzit zakaznika - netusim ako to uplne robit no...*/
            }
        }
    }
/**    It is assumed that di j, Tx and li are normalized such that 0 ≤ R(i, j) ≤ 2(ϕ+χ)+ψ+ω. This is done
    by scaling di j, Tx and li such that they only take on values from [0,1].*/
    return R;
}

int Shaw_Removal::removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                                  std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                  std::vector<std::vector<double>> &timeSchedule, int p, std::vector<double> &waitingTime,
                                  std::vector<double> &usedCapacity) {
    D.clear();
    srand((unsigned)time(nullptr));
    auto r = rand() % customers.size();
//    randomly selected requests to be removed
    D.emplace_back(r);
    std::vector<std::pair<int, double>> L;
    while (D.size() < p) {
        calculateRelatedness(distanceMatrix, customers, routes, timeSchedule, r);
        L.clear();
        for (int i = 1; i < customers.size(); ++i) {
            if (std::find(D.begin(), D.end(), i) == D.end()) {
                L.emplace_back(i, R[i]);
            }
        }
        std::sort(L.begin(), L.end(), [&](std::pair<int, double> a, std::pair<int, double> b) { return a.second < b.second; });
        auto y = (double)rand() / RAND_MAX;
        if (std::find(D.begin(), D.end(), L[std::pow(y, p) * (L.size() - 1)].first) == D.end()) {
            D.emplace_back(L[std::pow(y, p) * (L.size() - 1)].first);
            L.erase(L.begin() + std::pow(y, p) * (L.size() - 1));
        }
        if (D.size() < p) {
            r = D[rand() % D.size()];
            while (std::find(D.begin(), D.end(), r) != D.end()) { //pokial r je v D vyber nove r
                r = rand() % customers.size();
            }
        }
    }
    editSolution(distanceMatrix,customers,routes,timeSchedule, D, waitingTime, usedCapacity);
    return D.size();
}

void Shaw_Removal::editSolution(std::vector<std::vector<double>> &distanceMatrix,
                                std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                std::vector<std::vector<double>> &timeSchedule,
                                std::vector<int> &D, std::vector<double>& waitingTime, std::vector<double>& usedCapacity) {
//    for (int i : D) {
//        std::cout << i << " vyhodeny zakaznik" << std::endl;
//    }
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
                        routes[i].clear();
                        routes.erase(routes.begin() + i);
                        timeSchedule[i].clear();
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
