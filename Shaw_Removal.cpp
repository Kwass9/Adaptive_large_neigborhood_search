//
// Created by andrej on 15.11.2023.
//

#include <random>
#include <algorithm>
#include "Shaw_Removal.h"

Shaw_Removal::Shaw_Removal(double f, double ch, double p, double o, int qve, std::vector<customer> customers) : fi(f), chi(ch), psi(p), omega(o), q(qve) {
    R.resize(customers.size());
}

std::vector<double> Shaw_Removal::calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                                    std::vector<customer> &customers,std::vector<std::vector<int>> &routes,
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

void Shaw_Removal::removeRequests(Data *data ,Solution *solution, int p) {
    auto distanceMatrix = data->getDistanceMatrix();
    auto customers = data->getCustomers();
    auto routes = solution->getRoutes();
    auto timeSchedule = solution->getTimeSchedule();
    int r = rand() % routes.size();
    std::vector<int> D; //randomly selected requests to be removed
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
    editSolution(solution, D, data);
}

void Shaw_Removal::editSolution(Solution *solution, std::vector<int> &D, Data *data) {
    auto timeSchedule = solution->getTimeSchedule();
    auto routes = solution->getRoutes();
    for (int i : D) {
        data->getCustomers()[i].markAsUnrouted();
    }
    solution->setUnvisitedCustomersCount(data->getCustomers().size() - D.size());
    for (int i = 0; i < solution->getTimeSchedule().size(); ++i) {
        for (int j = 0; j < solution->getTimeSchedule()[i].size(); ++j) {
            for (int k : D) {
                if (solution->getRoutes()[i][j] == k) {
                    solution->setWaitingTimeAt(k, 0);
                    auto wait = solution->getTimeSchedule()[i][j + 1] -
                    solution->getTimeSchedule()[i][j - 1] + data->getCustomers()[solution->getRoutes()[i][j]].getDemand() + data->getDistanceMatrix()[routes[i][j - 1]][routes[i][j + 1]];
                    auto indexNasledovnik = routes[i][j + 1];
                    solution->setWaitingTimeAt(indexNasledovnik, wait);
                    auto newUsedCapacity = solution->getUsedCapacity()[j] - data->getCustomers()[k].getDemand();
                    solution->setUsedCapacityAt(k, newUsedCapacity);
                    timeSchedule[i].erase(timeSchedule[i].begin() + j);
                    routes[i].erase(routes[i].begin() + j);
                    solution->setTimeSchedule(timeSchedule);
                    solution->setRoutes(routes);
                }
            }
        }
    }
}

//template<typename T>
//void Shaw_Removal::quickSort(std::vector<int> &relatedness, int min, int max,
//                             std::function<bool(const T&, const T&)> compare) {
//    auto pivot = max;
//    auto currentIndex = min;
//    auto swapMarker = min - 1;
//    while (currentIndex <= max) {
//        bool result = compare(relatedness[currentIndex], pivot);
//        if (result) {
//            swapMarker++;
//            if (currentIndex > swapMarker) {
//                std::swap(relatedness[currentIndex], relatedness[swapMarker]);
//            }
//        }
//        currentIndex++;
//    }
//    if (swapMarker > min) {
//        quick(relatedness, compare, min, swapMarker - 1);
//    }
//    if (swapMarker < max) {
//        quick(relatedness, compare, swapMarker + 1, max);
//    }
//}
