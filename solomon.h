//
// Created by Andrej on 27/10/2023.
//

#ifndef SOLOMON_SOLOMON_H
#define SOLOMON_SOLOMON_H

#include <vector>
#include "Customer.h"


class solomon {

public:
    solomon(std::vector<customer> &customers, double alfa1, double alfa2,
            double lambda, double q, bool startingCriteria);
    ~solomon();
    void run(std::vector<customer>& customers, int numberOfUnvisitedCustomers);
    double getDistance() const;
    std::vector<std::vector<int>> &getRoutes();
    std::vector<std::vector<double>> &getTimeSchedule();
    std::vector<double> &getWaitingTime();
    const std::vector<std::vector<double>> &getDistanceMatrix() const;
    std::vector<double> &getUsedCapacity();

private:
    std::vector<std::vector<double>> distanceMatrix;
    std::vector<std::vector<int>> routes;
    std::vector<std::vector<double>> timeSchedule;
    std::vector<double> waitingTime;
    std::vector<double> beginingOfService;
    std::vector<double> pushForward;
    std::vector<double> timeWaitedAtCustomer;
    std::vector<customer> customers;
    std::vector<double> usedCapacity;
    int unvisitedCustomers;
    unsigned int currentlyUsedCapacity;
    double alfa1;
    double alfa2;
    double lambda;
    double q;
    bool startingCriteria;
    double totalDistance;

    void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix);
    unsigned int findCustomerWithEarliestDeadline(std::vector<customer> &customers);
    unsigned int findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    void calculatePushForward(std::vector<double> &pushForward, const std::vector<int>& route, int u, int position,
                              std::vector<double> &timeWaitedAtCustomer, std::vector<std::vector<double>> &distanceMatrix,
                              std::vector<customer> &customers, double timeOfService, double waitingTime, std::vector<double> &beginingOfService);
    void calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                               std::vector<int> &route, const std::vector<customer>& customers, int zakaznikU,
                               std::vector<double> &beginingOfService, double timeOfService,
                               std::vector<std::vector<double>> &distanceMatrix, int u);
    bool lema11(std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                std::vector<int> &route, std::vector<customer> &customers, int u, int position, double timeOfService);
    std::vector<std::tuple<int, double, int>> findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                                                           std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                                                           std::vector<int> &route, std::vector<customer> &customers,
                                                           unsigned int currentlyUsedCapacity, int maxCapacity, std::vector<double> &timeWaitedAtCustomer,
                                                           int unvisitedCustomers);
    std::pair<int, int> findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                                         std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
            ,std::vector<double> &beginingOfService
            ,std::vector<customer> &customers, std::vector<double> &timeWaitedAtCustomer
            ,unsigned int &currentlyUsedCapacity, std::vector<std::vector<double>> &distanceMatrix
            ,std::vector<double> &pushForward);
    void createNewRoute(unsigned int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes,
                        std::vector<int> &route, std::vector<double> &beginingOfService,
                        std::vector<double> &pushForward);
    void waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                         std::vector<int> &route, const std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix,
                         int index, double timeOfService, int u);
};


#endif //SOLOMON_SOLOMON_H