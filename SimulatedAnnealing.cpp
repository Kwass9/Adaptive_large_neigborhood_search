//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include <limits>
#include <iostream>
#include <random>
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
    temperature *= coolingRate; /**toto ma byt *= nie / no neviem spravne urobit rovnicu nizsie tak docasne takto*/
}

void SimulatedAnnealing::tryToAcceptNewSolution(double newSolution, std::vector<std::vector<int>> &newRoutes,
                                                std::vector<std::vector<double>> &newTimeSchedule,
                                                std::vector<double> &newWaitingTime) {
    if (newSolution < currentSolution - 0.0001) {
        std::cout << "New better solution: " << newSolution << std::endl;
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
    } else {
        auto differenceInSolutions = newSolution - currentSolution;
        double probability = std::exp(-(differenceInSolutions) / temperature); /**nie je doimplementovane...*/
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<double> distribution(0, 1);
        auto random = distribution(generator);
//        std::cout << "Random: " << random << " Probability: " << probability << std::endl;
        if (random < probability) {
            currentSolution = newSolution;
            currentRoutes = newRoutes;
            currentTimeSchedule = newTimeSchedule;
            currentWaitingTime = newWaitingTime;
            std::cout << "Accept new solution: " << newSolution << std::endl;
        } else {
            newRoutes.clear(); //pozriet este
            newRoutes = currentRoutes;
            newTimeSchedule.clear();
            newTimeSchedule = currentTimeSchedule;
            newWaitingTime.clear();
            newWaitingTime = currentWaitingTime;
        }
    }
    updateTemperature();
}