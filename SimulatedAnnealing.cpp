//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include <limits>
#include <iostream>
#include "SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing(double temperature, double coolingRate)
        : temperature(temperature), coolingRate(coolingRate) {
    bestSolution = std::numeric_limits<double>::max();
    currentSolution = std::numeric_limits<double>::max();
    srand((unsigned)time(nullptr));
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
    if (newSolution < currentSolution - 0.0001) {
        currentSolution = newSolution;
        currentRoutes.clear();
        currentRoutes = newRoutes;
        currentTimeSchedule.clear();
        currentTimeSchedule = newTimeSchedule;
        currentWaitingTime.clear();
        currentWaitingTime = newWaitingTime;
        if (newSolution < bestSolution) {
            bestSolution = newSolution;
            bestRoutes.clear();
            bestRoutes = newRoutes;
            bestTimeSchedule.clear();
            bestTimeSchedule = newTimeSchedule;
            bestWaitingTime.clear();
            bestWaitingTime = newWaitingTime;
        }
        updateTemperature(); //toto bude treba dat po kazdej iteracii asik
    } else {
        double probability = std::exp(std::abs(currentSolution - newSolution) / temperature);
        auto random = (double)rand() / RAND_MAX;
        if (random < probability) {
            currentSolution = newSolution;
            currentRoutes = newRoutes;
            currentTimeSchedule = newTimeSchedule;
            currentWaitingTime = newWaitingTime;
//            std::cout << "Accept new solution with probability: " << random << std::endl;
            updateTemperature();
        } else {
            /**resetuj hodnoty na povodne neakceptuj riesenie*/
            newRoutes.clear();
            newRoutes = currentRoutes;
            newTimeSchedule.clear();
            newTimeSchedule = currentTimeSchedule;
            newWaitingTime.clear();
            newWaitingTime = currentWaitingTime;
            return false;
        }
    }
    return true;
}