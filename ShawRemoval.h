//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SHAW_REMOVAL_H
#define SOLOMON_SHAW_REMOVAL_H

#include <vector>
#include <functional>
#include "Customer.h"

class Shaw_Removal {
private:
    std::vector<double> R; /**asi bude nejak treba povedat pre ktore r sa to pocitalo*/
    double fi;
    double chi;
    double psi;
    double omega;
    int p;
    std::vector<int> D;
public:
    Shaw_Removal(double fi, double chi, double psi, double omega, int p, int problemSize);
    ~Shaw_Removal();
    std::vector<double> calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                             std::vector<customer> &customers,
                                             std::vector<std::vector<int>> &routes,
                                             std::vector<std::vector<double>> &timeSchedule,
                                             int r);
    int removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                        std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                        std::vector<std::vector<double>> &timeSchedule, int ro, std::vector<double> &waitingTime,
                        std::vector<double> &usedCapacity);
    void editSolution(std::vector<std::vector<double>> &distanceMatrix,
                      std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                      std::vector<std::vector<double>> &timeSchedule, std::vector<int> &D,
                      std::vector<double>& waitingTime, std::vector<double>& usedCapacity);
};


#endif //SOLOMON_SHAW_REMOVAL_H