
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

    double fi = 9;
    double chi = 3;
    double psi = 2;
    double omega = 5;
    double p = 6;
    double pWorst = 3;
    double w = 0.05;
    double c = 0.99975;
    double sigma1 = 33;
    double sigma2 = 9;
    double sigma3 = 13;
    double r = 0.1;
    double eta = 0.025;
    double ksi = 0.4;
    int q;

    /**bude treba skontrolovat co kde akym sposobom posielam lebo uz sam uplne netusim ako to zliepam*/
    auto Data = new class Data(argc, argv);
    auto *solution = new class Solution();
    auto *solomon = new class solomon(Data, solution);
    //solomon->run(Data, solution); /**prehodit veci s vnutra konstruktora do metody run prisposobit inicializaciu hodnotam solution*/

    auto *simulatedAnnealing = new class SimulatedAnnealing(temperature, coolingRate, *solution);
    auto *shawRemoval = new class Shaw_Removal(fi, chi, psi, omega, q, Data->getCustomers()); /**pridat init parametrov asi by som ich dal do data poslal data a vytiahol co treba*/
    while (simulatedAnnealing->getTemperature() > optimum + 0.1 * optimum) {
        shawRemoval->removeRequests(Data, solution, p);
        solomon->run(Data, solution);
        simulatedAnnealing->tryToAcceptNewSolution(*solution);
    }
    solution->retrieveData(simulatedAnnealing->getBestTimeSchedule(), simulatedAnnealing->getBestRoutes(),
                           simulatedAnnealing->getBestWaitingTime(), simulatedAnnealing->getBestSolution());
    solution->finalPrint();
    delete Data;
    delete simulatedAnnealing;
    delete shawRemoval;
    delete solomon;
    delete solution;
    return 0;
}
