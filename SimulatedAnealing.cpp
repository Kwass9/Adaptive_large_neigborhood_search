//
// Created by andrej on 15.11.2023.
//

#include "SimulatedAnealing.h"


SimulatedAnealing::SimulatedAnealing(double temperature, double coolingRate) : temperature(temperature),
                                                                               coolingRate(coolingRate) {

}

double SimulatedAnealing::getTemperature() const {
    return temperature;
}

void SimulatedAnealing::setTemperature(double temperature) {
    SimulatedAnealing::temperature = temperature;
}

double SimulatedAnealing::getCoolingRate() const {
    return coolingRate;
}

void SimulatedAnealing::setCoolingRate(double coolingRate) {
    SimulatedAnealing::coolingRate = coolingRate;
}

double SimulatedAnealing::getBestSolution() const {
    return bestSolution;
}

void SimulatedAnealing::setBestSolution(double bestSolution) {
    SimulatedAnealing::bestSolution = bestSolution;
}

double SimulatedAnealing::getCurrentSolution() const {
    return currentSolution;
}

void SimulatedAnealing::setCurrentSolution(double currentSolution) {
    SimulatedAnealing::currentSolution = currentSolution;
}

const std::vector<std::vector<int>> &SimulatedAnealing::getBestRoutes() const {
    return bestRoutes;
}

void SimulatedAnealing::setBestRoutes(const std::vector<std::vector<int>> &bestRoutes) {
    SimulatedAnealing::bestRoutes = bestRoutes;
}

const std::vector<std::vector<int>> &SimulatedAnealing::getCurrentRoutes() const {
    return currentRoutes;
}

void SimulatedAnealing::setCurrentRoutes(const std::vector<std::vector<int>> &currentRoutes) {
    SimulatedAnealing::currentRoutes = currentRoutes;
}

const std::vector<std::vector<int>> &SimulatedAnealing::getNewRoutes() const {
    return newRoutes;
}

void SimulatedAnealing::setNewRoutes(const std::vector<std::vector<int>> &newRoutes) {
    SimulatedAnealing::newRoutes = newRoutes;
}

