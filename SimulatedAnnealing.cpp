//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include "SimulatedAnnealing.h"


SimulatedAnnealing::SimulatedAnnealing(double temperature, double coolingRate, std::vector<std::vector<int>> &routes,
                                       std::vector<std::vector<double>> &scheduleTime, std::vector<double> &waitingTime, double distance)
                                       : temperature(temperature), coolingRate(coolingRate), bestSolution(distance), currentSolution(distance),
                                       bestTimeSchedule(scheduleTime), currentTimeSchedule(scheduleTime), bestRoutes(routes), currentRoutes(routes),
                                       bestWaitingTime(waitingTime), currentWaitingTime(waitingTime) {}

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

bool SimulatedAnnealing::tryToAcceptNewSolution(std::vector<std::vector<int>> &routes,
                                                std::vector<std::vector<double>> &timeSchedule,
                                                std::vector<double> &waitingTime,
                                                double distance) {
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

