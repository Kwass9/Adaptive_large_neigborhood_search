//
// Created by andrej on 15.11.2023.
//

#include <valarray>
#include <limits>
#include <iostream>
#include <random>
#include "SimulatedAnnealing.h"
#include "test.h"

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

void SimulatedAnnealing::tryToAcceptNewSolution(double newSolution, std::vector<Vehicle> &vehicles,
                                                std::vector<double> &newWaitingTime) {
    std::vector<std::vector<int>> newRoutes;
    std::vector<std::vector<double>> newTimeSchedule;
    std::vector<double> newUsedCapacity;
    for (const auto & vehicle : vehicles) {
        newRoutes.push_back(vehicle.getRoute());
        newTimeSchedule.push_back(vehicle.getTimeSchedule());
        newUsedCapacity.push_back(vehicle.getUsedCapacity());
    }
    std::cout << "New solution: " << newSolution << std::endl;
    if (newSolution < currentSolution - 0.0001) {
        std::cout << "New better solution: " << newSolution << std::endl;
        currentSolution = newSolution;
        currentRoutes.clear();
        currentRoutes = newRoutes;
        currentTimeSchedule.clear();
        currentTimeSchedule = newTimeSchedule;
        currentWaitingTime.clear();
        currentWaitingTime = newWaitingTime;
        currentUsedCapacity.clear();
        currentUsedCapacity = newUsedCapacity;
        if (newSolution < bestSolution) {
            bestSolution = newSolution;
            bestRoutes.clear();
            bestRoutes = newRoutes;
            bestTimeSchedule.clear();
            bestTimeSchedule = newTimeSchedule;
            bestWaitingTime.clear();
            bestWaitingTime = newWaitingTime;
            bestUsedCapacity.clear();
            bestUsedCapacity = newUsedCapacity;
        }
    } else {
        auto differenceInSolutions = newSolution - currentSolution;
        double probability = std::exp(-(differenceInSolutions) / temperature);
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<double> distribution(0, 1);
        auto random = distribution(generator);
        if (random < probability) {
            currentSolution = newSolution;
            currentRoutes = newRoutes;
            currentTimeSchedule = newTimeSchedule;
            currentWaitingTime = newWaitingTime;
            currentUsedCapacity = newUsedCapacity;
            std::cout << "Accept new solution: " << newSolution << std::endl;
        } else {
            std::cout << "Reject new solution: " << newSolution << std::endl;
            for (int i = 0; i < vehicles.size(); ++i) {
                vehicles[i].setRoute(currentRoutes[i]);
                vehicles[i].setTimeSchedule(currentTimeSchedule[i]);
                vehicles[i].setUsedCapacity(currentUsedCapacity[i]);

            }
            newWaitingTime = currentWaitingTime;
            newUsedCapacity.clear();
        }
    }
    for (int i = 0; i < bestTimeSchedule.size(); ++i) {
        std::cout << bestSolution << std::endl;
        for (double j : bestTimeSchedule[i]) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
        for (int j : bestRoutes[i]) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
    updateTemperature();
}