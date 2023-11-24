//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include <limits>
#include "SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing(double temperature, double coolingRate)
        : temperature(temperature), coolingRate(coolingRate) {
    bestSolution = std::numeric_limits<double>::max();
    currentSolution = std::numeric_limits<double>::max();
}

SimulatedAnnealing::~SimulatedAnnealing() = default;

SimulatedAnnealing::SimulatedAnnealing(SimulatedAnnealing &simulatedAnnealing) {
    temperature = simulatedAnnealing.getTemperature();
    coolingRate = simulatedAnnealing.coolingRate;
    bestSolution = simulatedAnnealing.getBestSolution();
    currentSolution = simulatedAnnealing.getBestSolution();
}

double SimulatedAnnealing::getTemperature() const {
    return temperature;
}

double SimulatedAnnealing::getBestSolution() const {
    return bestSolution;
}

const std::vector<std::vector<int>> &SimulatedAnnealing::getBestRoutes() const {
    return bestRoutes;
}

const std::vector<std::vector<double>> &SimulatedAnnealing::getBestTimeSchedule() const {
    return bestTimeSchedule;
}

const std::vector<double> &SimulatedAnnealing::getBestWaitingTime() const {
    return bestWaitingTime;
}

void SimulatedAnnealing::updateTemperature() {
    temperature *= coolingRate;
}

bool SimulatedAnnealing::tryToAcceptNewSolution(double newSolution, std::vector<std::vector<int>> &newRoutes,
                                                std::vector<std::vector<double>> &newTimeSchedule,
                                                std::vector<double> &newWaitingTime) {
    auto distance = newSolution;
    const auto& routes = newRoutes;
    const auto& timeSchedule = newTimeSchedule;
    const auto& waitingTime = newWaitingTime;
    if (distance < currentSolution) {
        currentSolution = distance;
        currentRoutes = routes;
        currentTimeSchedule = timeSchedule;
        currentWaitingTime = waitingTime;
        if (distance < bestSolution) {
            bestSolution = distance;
            bestRoutes = routes;
            bestTimeSchedule = timeSchedule;
            bestWaitingTime = waitingTime;
            updateTemperature();
            return true;
        }
    } else {
        double probability = std::exp(std::abs(currentSolution - distance) / temperature);
        double random = (double)rand() / RAND_MAX;
        if (random < probability) {
            currentSolution = distance;
            currentRoutes = routes;
            currentTimeSchedule = timeSchedule;
            currentWaitingTime = waitingTime;
            updateTemperature();
            return true;
        } else {
            return false;
        }
    }
    updateTemperature();
    return true;
}