//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SHAW_REMOVAL_H
#define SOLOMON_SHAW_REMOVAL_H

#include <vector>
#include <functional>
#include <map>
#include "Customer.h"
#include "Vehicle.h"

class Shaw_Removal {
private:
    std::vector<double> R;
    double fi;
    double chi;
    int p;
    std::vector<int> D;
    std::vector<customer> notValidCustomers;
    void calculateRelatedness(std::vector<std::vector<double>> &distanceMatrix,
                              std::vector<Vehicle> &vehicles,
                              int r);
    static void editSolution(std::vector<std::vector<double>> &distanceMatrix,
                             std::vector<customer> &customers,
                             std::vector<int> &D,
                             std::vector<double>& waitingTime, std::vector<Vehicle> &vehicles,
                             std::vector<customer*> &unservedCustomers);
    void calculateD(const int &ro, std::vector<std::pair<int, double>>& L, int r, int s);
    void calculateL(std::vector<std::pair<int, double>>& L, int s);
    static void normalize(std::map<int ,double>& map);
    static int generateRandomNumber(int min, int max);
    static double generateRandomDouble(double min, double max);
public:
    Shaw_Removal(double fi, double chi, int p, int problemSize, std::vector<customer>& notValidCustomers);
    ~Shaw_Removal();
    void removeRequests(std::vector<std::vector<double>> &distanceMatrix,
                        std::vector<customer> &customers,
                        const int &ro,
                        std::vector<double> &waitingTime,
                        std::vector<Vehicle> &vehicles, std::vector<customer*> &unservedCustomers);
};


#endif //SOLOMON_SHAW_REMOVAL_H