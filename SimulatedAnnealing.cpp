//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include "SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing(double temperature, double coolingRate, Solution &solution)
                                       : temperature(temperature), coolingRate(coolingRate) {
    bestSolution = *new Solution(solution.getDistance(), solution.getTimeSchedule(), solution.getRoutes(), solution.getWaitingTime());
    currentSolution = *new Solution(solution.getDistance(), solution.getTimeSchedule(), solution.getRoutes(), solution.getWaitingTime());
}

double SimulatedAnnealing::getTemperature() const {
    return temperature;
}

double SimulatedAnnealing::getBestSolution() const {
    return bestSolution.getDistance();
}

const std::vector<std::vector<int>> &SimulatedAnnealing::getBestRoutes() const {
    return bestSolution.getRoutes();
}

const std::vector<std::vector<double>> &SimulatedAnnealing::getBestTimeSchedule() const {
    return bestSolution.getTimeSchedule();
}

const std::vector<double> &SimulatedAnnealing::getBestWaitingTime() const {
    return bestSolution.getWaitingTime();
}

void SimulatedAnnealing::updateTemperature() {
    temperature *= coolingRate;
}

bool SimulatedAnnealing::tryToAcceptNewSolution(Solution &solution) {
    auto distance = solution.getDistance();
    const auto& routes = solution.getRoutes();
    const auto& timeSchedule = solution.getTimeSchedule();
    const auto& waitingTime = solution.getWaitingTime();
    if (distance < currentSolution.getDistance()) {
        currentSolution.setDistance(distance);
        currentSolution.setRoutes(routes);
        currentSolution.setTimeSchedule(timeSchedule);
        currentSolution.setWaitingTime(waitingTime);
        if (distance < bestSolution.getDistance()) {
            bestSolution.setDistance(distance);
            bestSolution.setRoutes(routes);
            bestSolution.setTimeSchedule(timeSchedule);
            bestSolution.setWaitingTime(waitingTime);
            updateTemperature();
            return true;
        }
    } else {
        double probability = std::exp(std::abs(currentSolution.getDistance() - distance) / temperature);
        double random = (double)rand() / RAND_MAX;
        if (random < probability) {
            currentSolution.setDistance(distance);
            currentSolution.setRoutes(routes);
            currentSolution.setTimeSchedule(timeSchedule);
            currentSolution.setWaitingTime(waitingTime);
            updateTemperature();
            return true;
        } else {
            return false;
        }
    }
    updateTemperature();
    return true;
}

SimulatedAnnealing::~SimulatedAnnealing() {
    delete bestSolution;
    delete currentSolution;
}

