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
            double lambda, double q, bool startingCriteria, double eta);
    ~solomon();
    void run(std::vector<customer>& custs, int numberOfUnvisitedCustomers);
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
    double currentlyUsedCapacity;
    double alfa1;
    double alfa2;
    double lambda;
    double q;
    bool startingCriteria;
    double totalDistance;
    double maxN;

    static void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix);
    static unsigned int findCustomerWithEarliestDeadline(std::vector<customer> &customers);
    static unsigned int findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    static void calculatePushForward(std::vector<double> &pushForward, const std::vector<int>& route, int u, int position,
                              const std::vector<double> &timeWaitedAtCustomer, const std::vector<std::vector<double>> &distanceMatrix,
                              const std::vector<customer> &customers, double timeOfService, double waitingTime, const std::vector<double> &beginingOfService);
    static void calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                               std::vector<int> &route, const std::vector<customer>& customers, int zakaznikU,
                               std::vector<double> &beginingOfService, double timeOfService,
                               std::vector<std::vector<double>> &distanceMatrix, int u);
    static bool lema11(const std::vector<double> &beginingOfService, const std::vector<double> &pushForward,
                const std::vector<int> &route, const std::vector<customer> &customers, int u, int position, double timeOfService);
    std::vector<std::tuple<int, double, int>> findMinForC1(double a1, double a2, const std::vector<std::vector<double>> &dMatrix,
                                                           const std::vector<double> &begOfServ, std::vector<double> &pf,
                                                           const std::vector<int> &route, const std::vector<customer> &custs,
                                                           double curUsedCap, int maxCapacity,
                                                           const std::vector<double> &timeWaitedAtCust, int doesNoiseApply,
                                                           const std::vector<std::vector<int>>& existingRoutes);
    static std::pair<int, int> findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                                         std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    static void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
            ,std::vector<double> &beginingOfService
            ,std::vector<customer> &customers, std::vector<double> &timeWaitedAtCustomer
            ,double &currentlyUsedCapacity, std::vector<std::vector<double>> &distanceMatrix
            ,std::vector<double> &pushForward);
    void createNewRoute(double &curUsedCap, std::vector<std::vector<int>> &r,
                        std::vector<int> &route, std::vector<double> &begOfServ,
                        std::vector<double> &pf);
    static void waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                         std::vector<int> &route, const std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix,
                         int index, double timeOfService, int u);
    double calculateMaxN(double eta);
    double createNoise() const;
    static int doesNoiseApply();
};


#endif //SOLOMON_SOLOMON_H