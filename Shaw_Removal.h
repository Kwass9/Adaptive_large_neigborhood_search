//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SHAW_REMOVAL_H
#define SOLOMON_SHAW_REMOVAL_H

#include <vector>
#include <functional>
#include "customer.h"
#include "Solution.h"
#include "Data.h"

class Shaw_Removal {
private:
    std::vector<std::vector<double>> R;
    double fi;
    double chi;
    double psi;
    double omega;
    int q;
public:
    Shaw_Removal(double fi, double chi, double psi, double omega, int q, std::vector<customer> customers);
    std::vector<int> calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                                          std::vector<customer> &customers,
                                          std::vector<std::vector<int>> &routes,
                                          std::vector<std::vector<double>> &timeSchedule,
                                          int r);
    void removeRequests(Data *data ,Solution *solution, int p);

//    template<typename T>
//    void quickSort(std::vector<int> &relatedness, int min, int max, std::function<bool(const T &, const T &)> compare);
};


#endif //SOLOMON_SHAW_REMOVAL_H
