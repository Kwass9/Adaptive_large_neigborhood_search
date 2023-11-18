//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_DATA_H
#define SOLOMON_DATA_H

#include <string>
#include <vector>
#include "customer.h"

class Data {
private:
    std::vector<std::vector<double>> distanceMatrix;
    std::vector<std::vector<double>> timeSchedule;
    std::vector<std::vector<int>> routes;
    std::vector<double> waitingTime;
    std::string path;
    double distance;
    double alfa1;
    double alfa2;
    double lambda;
    double q;
    std::vector<customer> customers;
    bool startingCriteria;
    double vehicleCapacity = 200;
    void removeCharsFromString(std::string &path, char* charsToRemove);
    void removeDelimiters(std::string &path, const std::string& delimiter);
    double getAtributeForCustomer(std::string &path, const std::string& delimiter);
    double processString(std::string &path, const std::string& delimiter);
    void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix);
public:
    Data(int argc, char * argv[]);
    void finalPrint();
    void retrieveData(std::vector<std::vector<double>> schedule, std::vector<std::vector<int>> routes_,
                      std::vector<double> waiting, double distance);
    std::vector<std::vector<double>> getDistanceMatrix() const;
    double getAlfa1() const;
    double getAlfa2() const;
    double getLambda() const;
    double getQ() const;
    std::vector<customer>getCustomers() const;
    bool isStartingCriteria() const;
    double getVehicleCapacity() const;
};


#endif //SOLOMON_DATA_H
