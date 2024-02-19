//
// Created by andrej on 15.11.2023.
//

#include <random>
#include <algorithm>
#include "ShawRemoval.h"
#include <cmath>
#include <iostream>
#include <map>

Shaw_Removal::Shaw_Removal(double f, double ch, double ps, double o, int p, int problemSize) : fi(f), chi(ch), psi(ps), omega(o), p(p) {
    R.resize(problemSize);
}

Shaw_Removal::~Shaw_Removal() = default;

void Shaw_Removal::calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                                       std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                                       std::vector<std::vector<double>> &timeSchedule,
                                                       int r) {
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

                /**prediskutovat ci to robim dobre*/
                DistanceRelatedness.emplace(routes[i][j], std::abs(distanceMatrix[r][nasledovnik_r] - distanceMatrix[routes[i][j]][routes[i][j + 1]]));
                TimeRelatedness.emplace(routes[i][j] ,std::abs(std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][nasledovnik_r_index])
                                             - std::abs(timeSchedule[i][j] - timeSchedule[i][j + 1])));

                //                R[routes[i][j]] = fi * (distanceMatrix[r][nasledovnik_r] + distanceMatrix[routes[i][j]][routes[i][j + 1]])
//                        + chi * (std::abs(timeSchedule[route_number_r][index_r] - timeSchedule[route_number_r][nasledovnik_r])
//                                 + std::abs(timeSchedule[i][j] - timeSchedule[i][j + 1]));
                /**tieto dve zrejme nebudem pouzivat*/
//                        + psi * std::abs(customers[r].getDemand() - customers[j].getDemand());
                        //+ omega * (1 - () / std::min(,));
                /**kolko vozidiel vie obsluzit zakaznika - netusim ako to uplne robit no...*/
            }
        }
    }
/**    It is assumed that di j, Tx and li are normalized such that 0 ≤ R(i, j) ≤ 2(ϕ+χ)+ψ+ω. This is done
    by scaling di j, Tx and li such that they only take on values from [0,1].*/

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
//    std::cout << "--------------------------1-------------------------------------------" << std::endl;
    while (D.size() < ro) {
//        std::cout << "--------------------------2-------------------------------------------" << std::endl;
//        std::cout << "D.size() = " << D.size() << std::endl;
//        std::cout << "ro = " << ro << std::endl;

        calculateRelatedness(distanceMatrix, customers, routes, timeSchedule, r);
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
            D.emplace_back(L[std::pow(y, p) * (L.size() - 1)].first); /**k tomuto sa zastavit pri konzultacii este*/
            L.erase(L.begin() + std::pow(y, p) * (L.size() - 1));
        }
        if (D.size() < ro) {
            r = D[rand() % D.size()];
            while (std::find(D.begin(), D.end(), r) != D.end() || r == 0 || r == 101) { //pokial r je v D vyber nove r
//                std::cout << "--------------------------3-------------------------------------------" << std::endl;
                r = rand() % customers.size();
            }
        }
    }
//    std::cout << "--------------------------4-------------------------------------------" << std::endl;
    editSolution(distanceMatrix,customers,routes,timeSchedule, D, waitingTime, usedCapacity);
//    std::cout << "--------------------------5-------------------------------------------" << std::endl;
}

/**chyba sposobi zly vypocet waiting time pre vrchol 101 co sposobi ze insert nevie spravne posuvat vrchol 101 a zacne vytvarat nove a nove cesty aby obsluzil vrcholy
 * co znamena dlhsiu vzdialenost*/
void Shaw_Removal::editSolution(std::vector<std::vector<double>> &distanceMatrix,
                                std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                                std::vector<std::vector<double>> &timeSchedule,
                                std::vector<int> &D, std::vector<double>& waitingTime, std::vector<double>& usedCapacity) {
//    for (int i : D) {
//        std::cout << i << std::endl;
//    }
//    std::cout << "-----------------" << std::endl;
    for (int k : D) {
        for (int i = 0; i < timeSchedule.size(); ++i) {
            for (int j = 1; j < timeSchedule[i].size(); ++j) {
                if (routes[i][j] == k) {
                    customers[k].markAsUnrouted();
                    waitingTime[k] = 0;
                    for (int x : routes[i]) {
                        std::cout << x << " " << std::endl;
                    }
                    std::cout << std::endl;
                    std::cout << "i: " << i << " k: " << k << " demand: " << customers[k].getDemand() << " usedCapacity: " << usedCapacity[i] << std::endl;
                    usedCapacity[i] -= customers[k].getDemand();
                    std::cout << "i: " << i << " k: " << k << " usedCapacity: " << usedCapacity[i] << std::endl;
                    timeSchedule[i].erase(timeSchedule[i].begin() + j);
                    routes[i].erase(routes[i].begin() + j);
                    if (routes[i].size() == 2) {
                        routes[i].clear();
                        routes.erase(routes.begin() + i);
                        timeSchedule[i].clear();
                        timeSchedule.erase(timeSchedule.begin() + i);
                        usedCapacity.erase(usedCapacity.begin() + i);
//                        std::cout << "--------------------" << i << "----------------------" << std::endl;
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
//                            timeSchedule[i][nasledovnik] = newTimeOfService;
//                            if (indexNasledovnik != 101) {
                                timeSchedule[i][nasledovnik] = newTimeOfService;
                                waitingTime[indexNasledovnik] = 0;
//                            }
//                            else {
//                                waitingTime[indexNasledovnik] = customers[0].getDueDate() - timeSchedule[i][nasledovnik];
//                            }
//                            waitingTime[indexNasledovnik] = 0; //nemozem dat 101 ze caka 0 takto to nepojde insert nie je tak napisany mal by mat due date - kedy ma byt = waiting time
                        }
                        ++l;
                    }
                    break;
                }
            }
        }
    }
//    for (int i = 0; i < routes.size(); ++i) {
//        for (int j = 0; j < routes[i].size(); ++j) {
//            std::cout << routes[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << "Waiting time: " << waitingTime[101] << std::endl;
//    for (int i = 1; i < customers.size(); ++i) {
//        if (!customers[i].isRouted()) {
//            std::cout << i << std::endl;
//        }
//    }

}
