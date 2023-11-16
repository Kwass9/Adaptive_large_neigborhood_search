//
// Created by andrej on 15.11.2023.
//

#include <random>
#include "Shaw_Removal.h"

Shaw_Removal::Shaw_Removal(double f, double ch, double p, double o, int qve) : fi(f), chi(ch), psi(p), omega(o), q(qve) {}

std::vector<int> Shaw_Removal::calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                                    std::vector<customer> &customers,
                                                    std::vector<std::vector<int>> &routes,
                                                    std::vector<std::vector<double>> &timeSchedule,
                                                    int r) {

        /**tieto for loopy sa daju napisat lepsie len mi to akurat nenapada... neist cez zakaznikov ale cez cesty*/
        for (int j = 0; j < customers.size(); ++j) {
            if (r != j) {
                int nasledovnik_r = - 1;
                int index_r = - 1;
                int index_j = - 1;
                int index_nasledovnik_r = - 1;
                int index_nasledovnik_j = - 1;
                int nasledovnik_j = - 1;
                int route_number_r = - 1;
                int route_number_j = - 1;
                for (int i = 0; i < routes.size(); ++i) {
                    for (int k = 0; k < routes[i].size(); ++k) {
                        if (routes[i][k] == r) {
                            nasledovnik_r = routes[i][k +1];
                            index_r = k;
                            index_nasledovnik_r = k + 1;
                            route_number_r = i;
                        }
                        if (routes[i][k] == j) {
                            nasledovnik_j = routes[i][k +1];
                            index_j = k;
                            index_nasledovnik_j = k + 1;
                            route_number_j = i;
                        }
                        if (r != -1 && j != -1) {
                            break;
                        }
                    }
                }
                for (int i = 0; i < routes.size(); ++i) {
                    for (int k = 0; k < routes[i].size(); ++k) {
                        if (routes[i][k] == j) {
                            nasledovnik_j = routes[i][k +1];
                        }
                    }
                }
                R[r][j] = fi * (distanceMatrix[r][nasledovnik_r] + distanceMatrix[j][nasledovnik_j])
                        + chi * (std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][index_nasledovnik_r])
                        + std::abs(timeSchedule[route_number_j][index_j] - timeSchedule[route_number_j][index_nasledovnik_j]))
                        + psi * (customers[r].getDemand() - customers[j].getDemand()); /**pozriet ci to bolo demand*/
                        //+ omega * (1 - () / std::min(,));
                        /**ktore vozidla vedia obsluzit zakaznika*/
            }
    }

    return std::vector<int>();
}

void Shaw_Removal::removeRequests(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers,
                                  std::vector<std::vector<int>> &routes, std::vector<std::vector<double>> &timeSchedule,
                                  int p) {
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
        //calculate relatedness here
        //sort L by relatedness (pozriet ako to je v knihe ale asi staci len lambda funkcia)



        auto y = rand() % L.size(); /**pozriet ako dostanem cislo v rozsahu 0-1*/
        D.emplace_back(L[std::pow(y, p) * (L.size() - 1)]);
        L.erase(L.begin() + std::pow(y, p) * (L.size() - 1));
    }
    for (int i = 0; i < D.size(); ++i) {
        customers[D[i]].markAsUnrouted();
    }
    for (int i = 0; i < timeSchedule.size(); ++i) {
        for (int j = 0; j < timeSchedule[i].size(); ++j) {
            for (int k = 0; k < D.size(); ++k) {
                if (routes[i][j] == D[k]) {
                    timeSchedule[i].erase(timeSchedule[i].begin() + j);
                    routes[i].erase(routes[i].begin() + j);
                }
            }
        }
    }
}

template<typename T>
void Shaw_Removal::quickSort(std::vector<int> &relatedness, int min, int max,
                             std::function<bool(const T&, const T&)> compare) {
    auto pivot = max;
    auto currentIndex = min;
    auto swapMarker = min - 1;
    while (currentIndex <= max) {
        bool result = compare(relatedness[currentIndex], pivot);
        if (result) {
            swapMarker++;
            if (currentIndex > swapMarker) {
                std::swap(relatedness[currentIndex], relatedness[swapMarker]);
            }
        }
        currentIndex++;
    }
    if (swapMarker > min) {
        quick(relatedness, compare, min, swapMarker - 1);
    }
    if (swapMarker < max) {
        quick(relatedness, compare, swapMarker + 1, max);
    }
}
