//
// Created by andrej on 18.11.2023.
//

#ifndef SOLOMON_SOLUTION_H
#define SOLOMON_SOLUTION_H


#include <vector>

class Solution {
private:
    double distance;
    std::vector<std::vector<double>> timeSchedule;
    std::vector<std::vector<int>> routes;
    std::vector<double> waitingTime;
    std::vector<int> usedCapacity;
    int unvisitedCustomers;

public:
    Solution();
    ~Solution();
    Solution(double d, std::vector<std::vector<double>> t, std::vector<std::vector<int>> r,
             std::vector<double> w);
    void finalPrint();
    void retrieveData(std::vector<std::vector<double>> schedule, std::vector<std::vector<int>> routes_,
                      std::vector<double> waiting, double dist);
    double getDistance() const;
    void setDistance(double distance);
    const std::vector<std::vector<double>> &getTimeSchedule() const;
    void setTimeSchedule(const std::vector<std::vector<double>> &timeSchedule);
    const std::vector<std::vector<int>> &getRoutes() const;
    void setRoutes(const std::vector<std::vector<int>> &routes);
    const std::vector<double> &getWaitingTime() const;
    void setWaitingTime(const std::vector<double> &waitingTime);
    void setWaitingTimeAt(int index, double value);
    const std::vector<int> &getUsedCapacity() const;
    void setUsedCapacity(const std::vector<int> &usedCapacity);
    void setUsedCapacityAt(int index, int value);
    int getUnvisitedCustomers() const;
    void setUnvisitedCustomers(int unvisitedCustomers);
};


#endif //SOLOMON_SOLUTION_H
