//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_DATA_H
#define SOLOMON_DATA_H

#include <string>
#include <vector>
#include <memory>
#include "customer.h"

class Data {
private:
    std::vector<std::vector<double>> distanceMatrix;
    std::string path;
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
    ~Data();
    Data(Data &data);
    std::vector<std::vector<double>> getDistanceMatrix() const;
    double getAlfa1() const;
    double getAlfa2() const;
    double getLambda() const;
    double getQ() const;
    std::vector<customer> getCustomers() const;
    void setCustomers(std::vector<customer> &customers);
    bool isStartingCriteria() const;
    double getVehicleCapacity() const;
};


#endif //SOLOMON_DATA_H
