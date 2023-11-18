//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include "SimulatedAnnealing.h"


SimulatedAnnealing::SimulatedAnnealing(class Data *data ,double temperature, double coolingRate) : temperature(temperature),
                                                                                 coolingRate(coolingRate) {
}

double SimulatedAnnealing::getTemperature() const {
    return temperature;
}

double SimulatedAnnealing::getBestSolution() const {
    return bestSolution;
}

double SimulatedAnnealing::getCurrentSolution() const {
    return currentSolution;
}

const std::vector<std::vector<int>> &SimulatedAnnealing::getBestRoutes() const {
    return bestRoutes;
}

const std::vector<std::vector<int>> &SimulatedAnnealing::getCurrentRoutes() const {
    return currentRoutes;
}

const std::vector<std::vector<int>> &SimulatedAnnealing::getNewRoutes() const {
    return newRoutes;
}

void SimulatedAnnealing::updateTemperature() {
    temperature *= coolingRate;
}

void SimulatedAnnealing::tryToAcceptNewSolution(std::vector<std::vector<int>> &routes,
                                                std::vector<std::vector<double>> &timeSchedule,
                                                double distance) {
    if (distance < currentSolution) {
        currentSolution = distance;
        currentRoutes = routes;
        currentTimeSchedule = timeSchedule;
        if (distance < bestSolution) {
            bestSolution = distance;
            bestRoutes = routes;
            bestTimeSchedule = timeSchedule;
        }
    } else {
        double probability = exp((currentSolution - distance) / temperature);
        double random = (double) rand() / RAND_MAX;
        if (random < probability) {
            currentSolution = distance;
            currentRoutes = routes;
        }
    }
    updateTemperature();
}

