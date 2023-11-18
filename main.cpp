
#include <vector>
#include <algorithm>
#include "solomon.h"
#include "Data.h"
#include "SimulatedAnnealing.h"
#include "Shaw_Removal.h"
#include "Solution.h"

int main(int argc, char * argv[]) {
    double temperature = 10000;
    double coolingRate = 0.997;
    double optimum = 1637.7;
    double fi;
    double chi;
    double psi;
    double omega;
    int q;

    auto Data = new class Data(argc, argv);
    auto solution = new class Solution();
    auto solomon = new class solomon(Data, solution);
    solomon->run(); /**prehodit veci s vnutra konstruktora do metody run prisposobit inicializaciu hodnotam solution*/
    auto simulatedAnnealing = new class SimulatedAnnealing(temperature, coolingRate, solution->getRoutes(), solution->getSchedule(), solution->getWaitingTime(), solution->getDistance()); /**prerobit tak aby sa posielalo iba solution*/
    auto shawRemoval = new class Shaw_Removal(fi, chi, psi, omega, q, Data->getCustomers()); /**pridat init parametrov asi by som ich dal do data poslal data a vytiahol co treba*/
    while (simulatedAnnealing->getTemperature() > optimum + 0.1 * optimum) {
        shawRemoval->removeRequests(solution);
        solomon->run();
        simulatedAnnealing->tryToAcceptNewSolution(solution->getRoutes(), solution->getSchedule(), solution->getWaitingTime(), solution->getDistance()); /**prerobit tak aby sa posielalo iba solution*/
    }
    Data->retrieveData(simulatedAnnealing->getBestTimeSchedule(), simulatedAnnealing->getBestRoutes(), simulatedAnnealing->getBestWaitingTime(), simulatedAnnealing->getBestSolution());
    Data->finalPrint();
    delete Data;
    delete simulatedAnnealing;
    delete shawRemoval;
    delete solomon;
    delete solution;
    return 0;
}
