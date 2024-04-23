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
    bestRoutes = simulatedAnnealing.getBestRoutes();
    currentRoutes = simulatedAnnealing.getBestRoutes();
    bestTimeSchedule = simulatedAnnealing.getBestTimeSchedule();
    currentTimeSchedule = simulatedAnnealing.getBestTimeSchedule();
    bestWaitingTime = simulatedAnnealing.getBestWaitingTime();
    currentWaitingTime = simulatedAnnealing.getBestWaitingTime();
    bestUsedCapacity = simulatedAnnealing.bestUsedCapacity;
    currentUsedCapacity = simulatedAnnealing.currentUsedCapacity;
    bestCustomers = simulatedAnnealing.bestCustomers;
    currentCustomers = simulatedAnnealing.currentCustomers;
    bestVehicles = simulatedAnnealing.bestVehicles;
    currentVehicles = simulatedAnnealing.currentVehicles;
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
                                                std::vector<double> &newWaitingTime, std::vector<customer>& customers) {
    std::vector<std::vector<int>> newRoutes;
    std::vector<std::vector<double>> newTimeSchedule;
    std::vector<double> newUsedCapacity;
    for (const auto & vehicle : vehicles) {
        newRoutes.push_back(vehicle.getRoute());
        newTimeSchedule.push_back(vehicle.getTimeSchedule());
        newUsedCapacity.push_back(vehicle.getUsedCapacity());
    }
    if (newSolution < currentSolution - 0.0001) {
        currentSolution = newSolution;
        currentRoutes.clear();
        currentRoutes = newRoutes;
        currentTimeSchedule.clear();
        currentTimeSchedule = newTimeSchedule;
        currentWaitingTime.clear();
        currentWaitingTime = newWaitingTime;
        currentUsedCapacity.clear();
        currentUsedCapacity = newUsedCapacity;
        currentCustomers.clear();
        currentCustomers = customers;
        currentVehicles.clear();
        currentVehicles = vehicles;
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
            bestCustomers.clear();
            bestCustomers = customers;
            bestVehicles.clear();
            bestVehicles = vehicles;
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
            currentCustomers.clear();
            currentCustomers = customers;
            currentVehicles.clear();
            currentVehicles = vehicles;
        } else {
            vehicles.clear();
            vehicles = currentVehicles;
            customers.clear();
            customers = currentCustomers;
            newWaitingTime = currentWaitingTime;
            newUsedCapacity.clear();
        }
    }
    updateTemperature();
}

bool SimulatedAnnealing::hasPreviousSolution() const {
    return bestSolution != std::numeric_limits<double>::max();
}

void SimulatedAnnealing::resetToCurrentSolution(std::vector<customer> &customers, std::vector<Vehicle> &vehicles) {
    vehicles.clear();
    vehicles = currentVehicles;
    customers.clear();
    customers = currentCustomers;
}
