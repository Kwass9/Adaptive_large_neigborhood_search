//
// Created by andrej on 18.11.2023.
//

#include <iostream>
#include "Solution.h"

Solution::Solution(int problemSize) {
    distance = 0;
    timeSchedule = std::vector<std::vector<double>>();
    routes = std::vector<std::vector<int>>();
    waitingTime = std::vector<double>();
    waitingTime.resize(problemSize);
    usedCapacity = std::vector<int>();
    usedCapacity.resize(1);
    unvisitedCustomers = problemSize - 1;
}

Solution::Solution(double d, const std::vector<std::vector<double>> timeSchedule, const std::vector<std::vector<int>> routes,
                   const std::vector<double> waitingTime) : distance(d), timeSchedule(timeSchedule), routes(routes),
                                                            waitingTime(waitingTime) {}

Solution::~Solution() {}

void Solution::finalPrint() {
    for (auto & route : routes) {
        for (int j = 0; j < route.size(); ++j) {
            std::cout << route[j];
            if (j != route.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
    double totalScheduleTime = 0;
    double waitingTimeInSchedule = 0;
    auto numberOfCustomersServed = 0;
    for (auto & i : timeSchedule) {
        auto t = i.size() - 1;
        totalScheduleTime += (i[t]);
    }

    for (int i = 1; i < waitingTime.size() - 1; ++i) {
        waitingTimeInSchedule += waitingTime[i];
    }
    auto numberOfVehicles = routes.size();
    std::cout << "Time schedule: " << std::endl;
    for (auto & i : timeSchedule) {
        for (double j : i) {
            std::cout << j << " | ";
        }
        std::cout << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
    }
    std::cout << "Total distance: " << distance << std::endl;
    std::cout << "Total schedule time: " << totalScheduleTime << std::endl;
    std::cout << "Number of vehicles: " << numberOfVehicles << std::endl;
    std::cout << "Waiting time at customers: " << waitingTimeInSchedule << std::endl;
    std::cout << "Number of customers served: " << numberOfCustomersServed << std::endl;
}

void Solution::retrieveData(std::vector<std::vector<double>> schedule, std::vector<std::vector<int>> routes_,
                        std::vector<double> waiting, double dist) {
    timeSchedule = std::move(schedule);
    routes = std::move(routes_);
    waitingTime = std::move(waiting);
    distance = dist;
}

double Solution::getDistance() const {
    return distance;
}

void Solution::setDistance(double distance) {
    Solution::distance = distance;
}

const std::vector<std::vector<double>> &Solution::getTimeSchedule() const {
    return timeSchedule;
}

void Solution::setTimeSchedule(const std::vector<std::vector<double>> &timeSchedule) {
    Solution::timeSchedule = timeSchedule;
}

const std::vector<std::vector<int>> &Solution::getRoutes() const {
    return routes;
}

void Solution::setRoutes(const std::vector<std::vector<int>> &routes) {
    Solution::routes = routes;
}

const std::vector<double> &Solution::getWaitingTime() const {
    return waitingTime;
}

void Solution::setWaitingTime(const std::vector<double> &waitingTime) {
    Solution::waitingTime = waitingTime;
}

const std::vector<int> &Solution::getUsedCapacity() const {
    return usedCapacity;
}

void Solution::setUsedCapacity(const std::vector<int> &usedCapacity) {
    Solution::usedCapacity = usedCapacity;
}

int Solution::getUnvisitedCustomersCount() const {
    return unvisitedCustomers;
}

void Solution::setUnvisitedCustomersCount(int unvisitedCustomers) {
    Solution::unvisitedCustomers = unvisitedCustomers;
}

void Solution::setUsedCapacityAt(int index, int value) {
    Solution::usedCapacity[index] = value;
}

void Solution::setWaitingTimeAt(int index, double value) {
    Solution::waitingTime[index] = value;
}
