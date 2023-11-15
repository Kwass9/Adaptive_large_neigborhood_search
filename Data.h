//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_DATA_H
#define SOLOMON_DATA_H


#include <vector>
#include "customer.h"

class Data {
private:
    std::vector<std::vector<double>> distanceMatrix;
    std::vector<std::vector<double>> timeSchedule;
    std::vector<std::vector<int>> routes;
    std::vector<double> waitingTime;
    double distance;
public:
    Data(int argc, char * argv[]); /**toto asi bude od mainu potrebovat poslat path k suboru a budem to tu zut*/
    void removeCharsFromString(std::string &str, char* charsToRemove);
    void removeDelimiters(std::string &str, const std::string& delimiter);
    double getAtributeForCustomer(std::string &str, const std::string& delimiter);
    double processString(std::string &str, const std::string& delimiter);
    void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix);
    void finalPrint(std::vector<std::vector<int>> &routes, std::vector<std::vector<double>> &timeSchedule, double distance,
                    std::vector<double> &timeWaitedAtCustomer);
    void retrieveData(std::vector<std::vector<double>> &schedule, std::vector<std::vector<int>> &routes_,
                      std::vector<double> &waiting, double &dist);
    std::vector<std::vector<double>> &getDistanceMatrix();
};


#endif //SOLOMON_DATA_H
