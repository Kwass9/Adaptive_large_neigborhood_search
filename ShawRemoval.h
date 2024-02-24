//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SHAW_REMOVAL_H
#define SOLOMON_SHAW_REMOVAL_H

#include <vector>
#include <functional>
#include <map>
#include "Customer.h"

class Shaw_Removal {
private:
    std::vector<double> R;
    double fi;
    double chi;
    double psi;
    double omega;
    int p;
    std::vector<int> D;
    void calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                              std::vector<std::vector<int>> &routes,
                              std::vector<std::vector<double>> &timeSchedule,
                              int r);
    static void editSolution(std::vector<std::vector<double>> &distanceMatrix,
                             std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                             std::vector<std::vector<double>> &timeSchedule, std::vector<int> &D,
                             std::vector<double>& waitingTime, std::vector<double>& usedCapacity);
    void calculateD(const int &ro, std::vector<std::pair<int, double>>& L, int r, int s);
    void calculateL(std::vector<std::pair<int, double>>& L, int s);
    static void normalize(std::map<int ,double>& map);
    static int generateRandomNumber(int min, int max);
    static double generateRandomDouble(double min, double max);
public:
    Shaw_Removal(double fi, double chi, double psi, double omega, int p, int problemSize);
    ~Shaw_Removal();
    void removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                        std::vector<customer> &customers, std::vector<std::vector<int>> &routes,
                        std::vector<std::vector<double>> &timeSchedule, const int &ro, std::vector<double> &waitingTime,
                        std::vector<double> &usedCapacity);
};


#endif //SOLOMON_SHAW_REMOVAL_H