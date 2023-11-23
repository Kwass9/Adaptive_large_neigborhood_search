//
// Created by Andrej on 27/10/2023.
//

#ifndef SOLOMON_SOLOMON_H
#define SOLOMON_SOLOMON_H

#include <vector>
#include "Customer.h"
#include "Data.h"
#include "Solution.h"

class solomon {

public:
    solomon(class Data *data, class Solution *solution);
//    std::vector<std::vector<double>> &getSchedule();
    std::vector<std::vector<int>> &getRoutes();
//    std::vector<double> &getWaitingTime();
    double &getDistance();
    void run(class Data *data, class Solution *solution);
private:
//    std::vector<Customer> customers;
    int alfa1;
    int alfa2;
    int lambda;
    int maxCapacity;
//    std::vector<std::vector<double>> distanceMatrix;
    unsigned int index;
    std::vector<int> route;
//    std::vector<int> usedCapacity;
//    unsigned int currentlyUsedCapacity;
    std::vector<std::vector<int>> routes;
//    std::vector<std::vector<double>> timeScheduleSolomon;
//    std::vector<double> timeWaitedAtCustomer;
    std::vector<double> pushForward;
    std::vector<double> beginingOfService;
    double distance;
    unsigned int findCustomerWithEarliestDeadline(std::vector<Customer>& customers);
    unsigned int findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix, std::vector<Customer>& customers);
    void calculatePushForward(std::vector<double> &pushForward, const std::vector<int>& route, int u, int position,
                              std::vector<double> &timeWaitedAtCustomer, std::vector<std::vector<double>> &distanceMatrix,
                              std::vector<Customer> &customers, double timeOfService, double waitingTime, std::vector<double> &beginingOfService);
    void calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                               std::vector<int> &route, const std::vector<Customer>& customers, int zakaznikU,
                               std::vector<double> &beginingOfService, double timeOfService,
                               std::vector<std::vector<double>> &distanceMatrix, int u);
    bool lema11(std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                std::vector<int> &route, std::vector<Customer> &customers, int u, int position, double timeOfService);
    std::vector<std::tuple<int, double, int>> findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                                                           std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                                                           std::vector<int> &route, std::vector<Customer> &customers,
                                                           unsigned int currentlyUsedCapacity, int maxCapacity, std::vector<double> &timeWaitedAtCustomer,
                                                           int unvisitedCustomers);
    std::pair<int, int> findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                                         std::vector<std::vector<double>> &distanceMatrix, std::vector<Customer> &customers);
    void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
            , std::vector<double> &beginingOfService
            , std::vector<Customer>& customers, std::vector<double> &timeWaitedAtCustomer
            , unsigned int &currentlyUsedCapacity, std::vector<std::vector<double>> &distanceMatrix
            , std::vector<double> &pushForward);
    void createNewRoute(unsigned int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes,
                                    std::vector<int> &route, std::vector<double> &beginingOfService,
                                    std::vector<double> &pushForward);
    void waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                         std::vector<int> &route, const std::vector<Customer> &customers, std::vector<std::vector<double>> &distanceMatrix,
                         int index, double timeOfService, int u);
};


#endif //SOLOMON_SOLOMON_H
