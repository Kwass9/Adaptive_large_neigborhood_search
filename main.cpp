
#include <vector>
#include <algorithm>
#include "solomon.h"
#include "Data.h"
#include "SimulatedAnnealing.h"

int main(int argc, char * argv[]) {
    double temperature = 10000;
    double coolingRate = 0.997;
    double optimum = 1637.7;
    auto *Data = new class Data(argc, argv);
    auto *solomon = new class solomon(Data); //initial solution
    Data->retrieveData(solomon->getSchedule(), solomon->getRoutes(), solomon->getWaitingTime(), solomon->getDistance());
    Data->finalPrint();
    delete solomon;
    auto *simulatedAnnealing = new class SimulatedAnnealing(temperature, coolingRate);

    /**doplnit aby initial solution vzdy prijal*/
    while (simulatedAnnealing->getTemperature() > optimum + 0.1 * optimum) {
        solomon = new class solomon(Data);
        simulatedAnnealing->tryToAcceptNewSolution(solomon->getRoutes(), solomon->getSchedule(), solomon->getWaitingTime(), solomon->getDistance());
        delete solomon;
    }
    Data->retrieveData(simulatedAnnealing->getBestTimeSchedule(), simulatedAnnealing->getBestRoutes(), simulatedAnnealing->getBestWaitingTime(), simulatedAnnealing->getBestSolution());
    Data->finalPrint();
    return 0;
}
