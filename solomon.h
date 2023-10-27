//
// Created by Andrej on 27/10/2023.
//

#ifndef SOLOMON_SOLOMON_H
#define SOLOMON_SOLOMON_H

#include <vector>
#include "customer.h"


class solomon {

public:
    solomon(std::vector<customer> &customers, /**std::vector<std::vector<int>> &routes,*/ double alfa1, double alfa2,
            double lambda, double q, bool startingCriteria);
private:
    void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix);
    unsigned int findCustomerWithEarliestDeadline(std::vector<customer> &customers);
    unsigned int findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    void calculatePushForward(std::vector<double> &pushForward, const std::vector<double>& beginingOfService,
                              const std::vector<int>& route, int u, int position,
                              std::vector<double> &timeWaitedAtCustomer, std::vector<std::vector<double>> &distanceMatrix,
                              std::vector<customer> &customers);
    void calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                               std::vector<int> &route, const std::vector<customer>& customers, int zakaznikU,
                               std::vector<double> &beginingOfService, double timeOfService);
    bool lema11(std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                std::vector<int> &route, std::vector<customer> &customers, int u, int position,
                std::vector<std::vector<double>> &distanceMatrix);
    std::vector<std::tuple<int, double, int>> findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                                                           std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                                                           std::vector<int> &route, std::vector<customer> &customers,
                                                           unsigned int currentlyUsedCapacity, int maxCapacity, std::vector<double> &timeWaitedAtCustomer,
                                                           int unvisitedCustomers);
    std::pair<int, int> findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                                         std::vector<std::vector<double>> &distanceMatrix, std::vector<int> &route, std::vector<customer> &customers);
    void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
            ,std::vector<double> &beginingOfService
            ,std::vector<customer> &customers, std::vector<double> &timeWaitedAtCustomer
            ,unsigned int &currentlyUsedCapacity, std::vector<std::vector<double>> &distanceMatrix
            ,std::vector<double> &pushForward);
    std::vector<int> createNewRoute(unsigned int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes,
                                    std::vector<int> &route, std::vector<double> &timeWaitedAtCustomer, std::vector<customer> &customers,
                                    std::vector<double> &pushForward);
};


#endif //SOLOMON_SOLOMON_H
