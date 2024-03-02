//
// Created by Andrej on 27/10/2023.
//

#ifndef SOLOMON_SOLOMON_H
#define SOLOMON_SOLOMON_H

#include <vector>
#include "Customer.h"
#include "Vehicle.h"

class solomon {

public:
    solomon(std::vector<customer> &customers, double alfa1, double alfa2,
            double lambda, double q, bool startingCriteria, double eta,
            std::vector<Vehicle>& vehicles);
    ~solomon();
    void run(std::vector<customer>& custs, int numberOfUnvisitedCustomers,
             std::vector<Vehicle>& vehicles);
    double getDistance() const;
    std::vector<double> &getWaitingTime();
    const std::vector<std::vector<double>> &getDistanceMatrix() const;

private:
    std::vector<std::vector<double>> distanceMatrix;
    std::vector<double> timeWaitedAtCustomer;
    int unvisitedCustomers;
    double alfa1;
    double alfa2;
    double lambda;
    double q;
    bool startingCriteria;
    double totalDistance; //este pojde von...
    double maxN;
    int problemSize;

    static void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix);
    static std::vector<int> findCustomerWithEarliestDeadline(std::vector<customer> &customers);
    static std::vector<int> findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    static std::vector<double> calculatePushForward(const std::vector<int>& route, int u, int position,
                                                    const std::vector<double> &timeWaitedAtCustomer, const std::vector<std::vector<double>> &distanceMatrix,
                                                    const std::vector<customer> &customers, double timeOfService, double waitingTime, const std::vector<double> &beginingOfService,
                                                    const CustomersTimeWindow& timeWinCustomerU, const CustomersTimeWindow& timeWinCustomerJ);
    static void calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                               std::vector<int> &route, std::vector<customer>& customers, int zakaznikU,
                               std::vector<double> &beginingOfService, double timeOfService,
                               const std::vector<std::vector<double>> &distanceMatrix, int u);
    static bool lema11(const std::vector<double> &beginingOfService, const std::vector<double> &pushForward,
                       const std::vector<int> &route, std::vector<customer> &customers, int u, int position, double timeOfService,
                       const Vehicle &vehicle, const CustomersTimeWindow& timeWinCustomerU);

    std::vector<std::tuple<int, double, int, int>> findMinForC1(double a1, double a2, const std::vector<std::vector<double>> &dMatrix,
                                                           std::vector<customer> &custs,
                                                           const std::vector<double> &timeWaitedAtCust, int doesNoiseApply,
                                                           const std::vector<Vehicle> &vehicles, int vehicleIndex);
    std::tuple<int, double> calculateC1(std::vector<int> route, std::vector<std::vector<double>> dMatrix,
                                                          int i, int u, double a1, double a2, bool doesNoiseApply, double min,
                                                          int minIndex, std::vector<double> pf);
    /**pokial bolo treba pushnut vozidlo v jednej ceste no uz je aj v inej pridelene kvoli sucasnej obsluhe*/
    bool checkIfCustomerCanBePushedInRoute(const Vehicle &vehicle, int u, double timeOfService,
                                           std::vector<customer> &customers, double waitingTime,
                                           int vehicleIndex);
    void pushVehicleInOtherRoutes(Vehicle &vehicle, int u, double timeOfService,
                                  std::vector<customer> &customers, const std::vector<std::vector<double>> &distanceMatrix
                                  ,double waitingTime);
    static std::tuple<int, int, int> findOptimumForC2(std::vector<std::tuple<int, double, int, int>> &mnozinaC1, double lambda,
                                         std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers);
    static void insertCustomerToRoad(Vehicle &vehicle, std::tuple<int, int, int> optimalInsertion,
            std::vector<customer>& custs, const std::vector<std::vector<double>>& distanceMatrix,
            std::vector<double>& timeWaitedAtCustomer);
    static void waitingTimeMath(std::vector<double> &timeWaitedAtCustomer, std::vector<double> &beginingOfService,
                         std::vector<int> &route, const std::vector<customer> &customers, const std::vector<std::vector<double>> &distanceMatrix,
                         int index, double timeOfService, int u, int w);
    double calculateMaxN(double eta);
    double createNoise() const;
    static int doesNoiseApply();
    void finalPrint(std::vector<customer>& custs, std::vector<Vehicle>& vehicles);
};


#endif //SOLOMON_SOLOMON_H