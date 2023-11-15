//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_SIMULATEDANEALING_H
#define SOLOMON_SIMULATEDANEALING_H


#include <vector>

class SimulatedAnealing {
private:
    double temperature;
    double coolingRate;
    double bestSolution;
    double currentSolution;
    std::vector<std::vector<int>> bestRoutes;
    std::vector<std::vector<int>> currentRoutes;
    std::vector<std::vector<int>> newRoutes;
public:
    SimulatedAnealing();
    SimulatedAnealing(double temperature, double coolingRate);
    double getCoolingRate() const;
    void setCoolingRate(double coolingRate);
    double getBestSolution() const;
    void setBestSolution(double bestSolution);
    double getCurrentSolution() const;
    void setCurrentSolution(double currentSolution);
    const std::vector<std::vector<int>> &getBestRoutes() const;
    void setBestRoutes(const std::vector<std::vector<int>> &bestRoutes);
    const std::vector<std::vector<int>> &getCurrentRoutes() const;
    void setCurrentRoutes(const std::vector<std::vector<int>> &currentRoutes);
    const std::vector<std::vector<int>> &getNewRoutes() const;
    void setNewRoutes(const std::vector<std::vector<int>> &newRoutes);
    double getTemperature() const;
    void setTemperature(double temperature);

};


#endif //SOLOMON_SIMULATEDANEALING_H
