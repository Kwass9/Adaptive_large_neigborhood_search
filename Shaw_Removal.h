//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SHAW_REMOVAL_H
#define SOLOMON_SHAW_REMOVAL_H

#include <vector>
#include "customer.h"

class Shaw_Removal {
private:
    std::vector<std::vector<double>> R;
    double fi;
    double chi;
    double psi;
    double omega;
    int q;
public:
    Shaw_Removal(double fi, double chi, double psi, double omega, int q);
    std::vector<int> calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                          std::vector<customer> &customers,
                                          std::vector<std::vector<int>> &routes,
                                          std::vector<std::vector<double>> &timeSchedule,
                                          int r);
    void removeRequests(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers,
                        std::vector<std::vector<int>> &routes, std::vector<std::vector<double>> &timeSchedule,
                        int p);

};


#endif //SOLOMON_SHAW_REMOVAL_H
