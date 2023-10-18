#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <valarray>
#include <tuple>
#include "customer.h"

void removeCharsFromString( std::string &str, char* charsToRemove ) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(std::remove(str.begin(),str.end(),charsToRemove[i]),str.end());
    }
}

void removeDelimiters(std::string &str, const std::string& delimiter) {
    size_t cell;
    while ((cell = str.find(delimiter)) == 0) { //ak je na zaciatku medzera tak ju vymaz
        str.erase(0, cell + delimiter.length());
    }
}

unsigned int getAtributeForCustomer(std::string &str, const std::string& delimiter) {
    size_t cell;
    std::string word;
    if ((cell = str.find(delimiter)) != std::string::npos) {
        word = str.substr(0, cell);
        str.erase(0, cell + delimiter.length());
    }
    return std::stoi(word);
}

unsigned int processString(std::string &str, const std::string& delimiter) {
    removeDelimiters(str, delimiter);
    return getAtributeForCustomer(str, delimiter);
}

void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix) {
    for (int i = 0; i < customers.size(); ++i) {
        auto distVec = new std::vector<double>;
        for (int j = 0; j < customers.size(); ++j) {
            distVec->push_back(INT_MAX - 1);
        }
        distanceMatrix.emplace_back(*distVec);
    }
    for (int i = 0; i < customers.size(); ++i) {
        for (int j = 0; j < customers.size(); ++j) {
            if (i == j) {
                distanceMatrix[i][j] = 0;
            } else {
                distanceMatrix[i][j] = sqrt(pow(customers[i].getXcord() - customers[j].getXcord(), 2) +
                                            pow(customers[i].getYcord() - customers[j].getYcord(), 2));
            }
        }
    }
}

unsigned int findCustomerWithEarliestDeadline(std::vector<customer> &customers) {
    double min = INT_MAX - 1;
    unsigned int minIndex = 0;
    for (int i = 0; i < customers.size(); ++i) {
        if (!customers[i].isRouted()
                && customers[i].getDueDate() < min) {
            min = customers[i].getDueDate();
            minIndex = i;
        }
    }
    return minIndex;
}

unsigned int findFurthestUnroutedCustomer(std::vector<std::vector<double>> &distanceMatrix, std::vector<customer> &customers) {
    double max = 0;
    unsigned int maxIndex = 0;
    for (int i = 0; i < customers.size(); ++i) {
        if (!customers[i].isRouted()
            && distanceMatrix[0][i] > max) {
            max = distanceMatrix[0][i];
            maxIndex = i;
        }
//        else if (!customers[i].isRouted()
//            && distanceMatrix[0][i] == max
//            && customers[i].getDueDate() < customers[maxIndex].getDueDate()) {
//            max = distanceMatrix[0][i];
//            maxIndex = i;
//        }
    }
    return maxIndex;
}


void calculatePushForward(std::vector<double> &pushForward, const std::vector<double>& beginingOfService,
                          const std::vector<int>& route, int u, int position,
                          std::vector<double> &timeWaitedAtCustomer, std::vector<std::vector<double>> &distanceMatrix,
                          std::vector<customer> &customers) {
    double epsilon = 0.0000001; //kvoli double
    if (route.size() > 2) {
        auto i = route[position];
        auto j = route[position + 1];
        if (customers[j].getDueDate() < customers[u].getReadyTime() + customers[u].getServiceTime() + distanceMatrix[u][j]) {
              pushForward[j] = INT_MAX - 1;
        } else {
            pushForward[j] = std::max(0.0,distanceMatrix[i][u] + distanceMatrix[u][j] + customers[u].getServiceTime()
                                          - distanceMatrix[i][j] - timeWaitedAtCustomer[j]);
        }
        for (int n = position + 2; n < route.size() - 1 - position; ++n) {
            j = route[n];
            i = route[n - 1];
            if (pushForward[i] - timeWaitedAtCustomer[j] > 0 + epsilon) { /**taketo podmienky este treba po celom kode prepisat na ternarne*/
                pushForward[j] = pushForward[i] - timeWaitedAtCustomer[j];
            } else {
                pushForward[j] = 0;
                break;
            }
        }
    }
}

void calculateNewBeginings(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                                   std::vector<int> &route, const std::vector<customer>& customers, int zakaznikU,
                                   std::vector<double> &beginingOfService) {
    double epsilon = 0.0000001;
    if (route.size() > 1) { //ten if pojde asi von
        for (int n = zakaznikU; n < route.size() - 1; ++n) {
            int j = route[n + 1];
            if (pushForward[j] - timeWaitedAtCustomer[j] > 0 + epsilon) {
                beginingOfService[j] = beginingOfService[n] + pushForward[j] - timeWaitedAtCustomer[j];
                timeWaitedAtCustomer[j] = 0;
            } else {
                timeWaitedAtCustomer[j] -= pushForward[j];
                break;
            }
        }
    }
}

bool lema11(std::vector<double> &beginingOfService, std::vector<double> &pushForward,
            std::vector<int> &route, std::vector<customer> &customers, int u, int position,
            std::vector<std::vector<double>> &distanceMatrix) {
    int i = route[position];
    auto beginingOfServiceU = beginingOfService[position] + distanceMatrix[i][u] + customers[position].getServiceTime();
    if (beginingOfServiceU <= customers[u].getDueDate()) {
        if (position == route.size() - 1) {
            return false;
        }
        int j = position + 1;
        while (pushForward[route[j]] > 0.00000001 && j < route.size() - 1) { //position je cislo iteracie
            if (beginingOfService[j] + pushForward[route[j]] > customers[route[j]].getDueDate()) {
                return false;
            }
            ++j;
        }
        return true;
    }
    return false;
}

std::vector<std::tuple<int, double, int>> findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                  std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                  std::vector<int> &route, std::vector<customer> &customers,
                  unsigned int currentlyUsedCapacity, int maxCapacity, std::vector<double> &timeWaitedAtCustomer) {
    std::vector<std::tuple<int, double, int>> mnozinaC1;
    int minIndex = 0;
    for (int u = 1; u < customers.size(); ++u) {
        double min = INT_MAX - 1;
        if (!customers[u].isRouted()
            && currentlyUsedCapacity + customers[u].getDemand() <= maxCapacity) { //kontrola kapacity vozidla
            for (int i = 0; i < route.size() - 1; ++i) {
                pushForward.clear();
                calculatePushForward(pushForward, beginingOfService, route, u, i, timeWaitedAtCustomer,
                                     distanceMatrix, customers);
                //podmienky lemma 1.1
                if (lema11(beginingOfService, pushForward, route, customers, u, i, distanceMatrix)) {
                    double c11;
                    double c12;
                    if (route.size() > 1) {
                        c11 = distanceMatrix[route[i]][u] + distanceMatrix[u][route[i + 1]] -
                              distanceMatrix[route[i]][route[i + 1]];
                        //pushForward[i + 1] - timeWaitedAtCustomer[i + 1] tento vyraz znamena beginingOfServiceNew
                        c12 = pushForward[i + 1] - timeWaitedAtCustomer[i + 1] - beginingOfService[i + 1];
                    } else { //nachystane ked sa uloha zmeni na ulohu bez depa ze vyrazim od prveho customera s opatrovatelkou
                        c11 = distanceMatrix[route[i]][u];
                        c12 = pushForward[i + 1] - timeWaitedAtCustomer[i + 1] - beginingOfService[route[i]];
                    }
                    double c1 = alfa1 * c11 + alfa2 * c12;
                    if (c1 < min) {
                        min = c1;
                        minIndex = i;
                    }
                }
            }
            if (min != INT_MAX - 1) { //ak sa nasiel nejaky vhodny zakaznik
                mnozinaC1.emplace_back(std::make_tuple(minIndex, min, u));
            }
        }
    }
    return mnozinaC1;
}

std::pair<int, int> findOptimumForC2(std::vector<std::tuple<int, double, int>> &mnozinaC1, double lambda,
                      std::vector<std::vector<double>> &distanceMatrix, std::vector<int> &route, std::vector<customer> &customers) {
    double C2 = 0;
    int maxIndex = 0;
    std::pair<int, int> optimalInsertion;
    for (int i = 0; i < mnozinaC1.size(); ++i) {
        double c2 = lambda * distanceMatrix[0][std::get<2>(mnozinaC1[i])] - std::get<1>(mnozinaC1[i]);
        if (c2 > C2) {
            C2 = c2;
            maxIndex = i;
        } else if (c2 == C2) { //pri zhode usetrenia vyberie zakaznika ktory ma skorsi deadline
            if (customers[i].getDueDate() < customers[maxIndex].getDueDate()) {
                C2 = c2;
                maxIndex = i;
            }
        }
    }
    optimalInsertion.first = std::get<0>(mnozinaC1[maxIndex]); //i
    optimalInsertion.second = std::get<2>(mnozinaC1[maxIndex]); //u
    return optimalInsertion;
}

//toto este nie je uplne hotove
void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
                          ,std::vector<double> &beginingOfService
                          ,std::vector<customer> &customers, std::vector<double> &timeWaitedAtCustomer
                          ,unsigned int &currentlyUsedCapacity, std::vector<std::vector<double>> &distanceMatrix
                          ,std::vector<double> &pushForward) {
    int i = optimalInsertion.first;
    int u = optimalInsertion.second;
    if (route.size() > 1) {
        int predchodca = route[i - 1];
        route.insert(route.begin() + i, u);
        double timeOfService = beginingOfService[i - 1]
                               + distanceMatrix[predchodca][u]
                               + customers[predchodca].getServiceTime();
        if (timeOfService < customers[u].getReadyTime()) {
            timeWaitedAtCustomer[u] = customers[u].getReadyTime() - timeOfService;
            timeOfService = customers[u].getReadyTime();
        } else {
            timeWaitedAtCustomer[u] = 0;
        }
        beginingOfService.insert(beginingOfService.begin() + i, timeOfService);
        pushForward.clear();
        calculatePushForward(pushForward, beginingOfService, route, u, i, timeWaitedAtCustomer, distanceMatrix, customers);
        calculateNewBeginings(pushForward, timeWaitedAtCustomer, route, customers, i, beginingOfService);
        pushForward.clear();
        currentlyUsedCapacity += customers[u].getDemand();
        customers[u].markAsRouted();
    } else {
        route.insert(route.begin() + i, u);
        beginingOfService.insert(beginingOfService.begin() + i, 0);
        customers[u].markAsRouted();
    }
}

std::vector<int> createNewRoute(unsigned int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes,
                                std::vector<int> &route, std::vector<double> &timeWaitedAtCustomer, std::vector<customer> &customers,
                                std::vector<double> &pushForward) {
    currentlyUsedCapacity = 0;
    for (int i = 0; i < customers.size(); ++i) {
        pushForward[i] = 0;
        timeWaitedAtCustomer[i] = 0;
    }
    routes.push_back(route);
    std::vector<int> newRoute;
    return newRoute;
}

int main(int argc, char * argv[]) {
    std::string path;
    double alfa1;
    double alfa2;
    double lambda;
    double q;
    std::vector<customer> customers;
    bool startingCriteria; //premenna vybera ci sa zacina s najvzdialenejsim zakaznikom alebo s najskor zaciatocnou dobou

    if (argc == 7) {
        path = argv[1];
        //kvoli tomu ako sa win chova k specialnym znakom
        removeCharsFromString(path,"\"");
        if (path.empty()) {
            std::cerr << "Not a valid argument" << std::endl;
        }
        if (atof(argv[2]) >= 0 && atof(argv[3]) >= 0) {
            lambda = atof(argv[2]);
            q = atof(argv[3]);
        } else {
            std::cerr << "We need non negative value for argument lambda" << std::endl;
        }
        alfa1 = atof(argv[4]);
        alfa2 = atof(argv[5]);
        double check = 0.00001; // pre pripad ze by si pocitac zmyslel podat desatinnu ciarku
        if (alfa1 + alfa2 >= 1 + check || alfa1 + alfa2 <= 1 - check) {
            std::cerr << "Not valid value of alfas" << std::endl;
        }
        if (atoi(argv[6])) {
            startingCriteria = true; //condition a
        } else if (!atoi(argv[6])) {
            startingCriteria = false; //condition b
        } else {
            std::cerr << "Not valid value of starting criteria" << std::endl;
        }
    } else {
        std::cend("Nespravny pocet argumentov");
    }
    std::ifstream input;
    std::vector<std::string> data{};
    std::string par;
    std::string delimiter = " ";
    size_t cell = 0;
    input.open(path);
    if (input) {
        while(!input.eof()) {
            getline(input, par);
            par += delimiter;
            data.push_back(par);
        }
    } else {
        std::cerr << "Subor nebol otvoreny" << std::endl;
    }
    input.close();

    //zbavenie sa hlavicky v subore
    for (int i = 0; i < 9; ++i) {
        data[i].erase();
    }

    //inicializacia zakaznikov
    for (auto & i : data) {
        if ((cell = i.find(delimiter)) != std::string::npos) {
            unsigned int id = processString(i, delimiter);
            double x = processString(i, delimiter);
            double y = processString(i, delimiter);
            unsigned int demand = processString(i, delimiter);
            double readyTime = processString(i, delimiter);
            double dueDate = processString(i, delimiter);
            double serviceTime = processString(i, delimiter);
            auto customer = new class customer(id, x, y, demand, readyTime, dueDate, serviceTime);
            customers.emplace_back(*customer);
        }
    }

    std::vector<std::vector<double>> distanceMatrix;
    calculateDistances(customers, distanceMatrix);
    std::vector<int> route;
    std::vector<std::vector<int>> routes;
    std::vector<std::vector<double>> timeSchedule;

    auto unvisitedCustomers = customers.size() - 1;

    static int vehicleCapacity = 200; /**prerobit na nacitanie zo suboru*/
    unsigned int currentlyUsedCapacity = 0;

    std::vector<double> timeWaitedAtCustomer;
    std::vector<double> pushForward;
    std::vector<double> beginingOfService;
    for (int i = 0; i < customers.size(); ++i) {
        pushForward.push_back(0);
        timeWaitedAtCustomer.push_back(0);
    }

    //zaciatok cesty
    insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                         distanceMatrix, pushForward); //depo na zaciatku trasy
    insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                         distanceMatrix, pushForward); //depo na konci trasy navrat
    beginingOfService[1] = customers[0].getDueDate();
    timeWaitedAtCustomer[0] = customers[0].getDueDate();
    unsigned int index;
    if (startingCriteria) {
        index = findFurthestUnroutedCustomer(distanceMatrix, customers);
    } else {
        index = findCustomerWithEarliestDeadline(customers);
    }
    insertCustomerToRoad(route, std::make_pair(1, index), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                         distanceMatrix, pushForward);
    unvisitedCustomers--;

    while (unvisitedCustomers != 0) { //while unroute customers masti ich do ciest
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, beginingOfService, pushForward, route, customers,
                               currentlyUsedCapacity, vehicleCapacity, timeWaitedAtCustomer);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, route, customers);
            c2.first += 1; //first je index predchodcu v ceste first + 1 je nova pozicia kam sa naj zakaznik vlozi
            insertCustomerToRoad(route, c2, beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity, distanceMatrix, pushForward);
            unvisitedCustomers--;
        } else {
            std::vector<double> previousBeginings = beginingOfService;
            timeSchedule.emplace_back(previousBeginings);
            std::cout << "Ostava zakaznikov: " << unvisitedCustomers << std::endl;
            std::cout << "Pouzita kapacita: " << currentlyUsedCapacity << std::endl;
            route = createNewRoute(currentlyUsedCapacity, routes, route, timeWaitedAtCustomer, customers, pushForward);
            beginingOfService.clear();
            beginingOfService.shrink_to_fit();
            insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward); //depo na zaciatku trasy
            insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward); //depo na konci trasy navrat
            beginingOfService[1] = customers[0].getDueDate();
            timeWaitedAtCustomer[0] = customers[0].getDueDate();
            index = startingCriteria ? findFurthestUnroutedCustomer(distanceMatrix, customers)
                                     : findCustomerWithEarliestDeadline(customers);
            insertCustomerToRoad(route, std::make_pair(1, index), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward);
            unvisitedCustomers--;
        }
        std::cout << "Ostava zakaznikov: " << unvisitedCustomers << std::endl;
    }
    routes.push_back(route);

    //zaverecny vypis
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size(); ++j) {
            std::cout << routes[i][j];
            if (j != routes[i].size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
    double totalDistance = 0;
    double totalScheduleTime = 0;
    auto numberOfCustomersServed = 0;
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size() - 1; ++j) {
            totalDistance += distanceMatrix[routes[i][j]][routes[i][j + 1]]; //tu zle ratam distance asi
        }
        numberOfCustomersServed += routes[i].size() - 2;
        totalScheduleTime += beginingOfService[routes[i].size() - 2] + customers[routes[i][routes[i].size() - 2]].getServiceTime() +
                             distanceMatrix[routes[i][routes[i].size() - 2]][routes[i][routes[i].size() - 1]];
    }

    auto numberOfVehicles = routes.size();
    std::cout << "Time schedule: " << std::endl;
    for (auto & i : timeSchedule) {
        for (double j : i) {
            std::cout << j << " | ";
        }
        std::cout << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
    }
    std::cout << "Total distance: " << totalDistance << std::endl;
    std::cout << "Total schedule time: " << totalScheduleTime << std::endl;
    std::cout << "Number of vehicles: " << numberOfVehicles << std::endl;
    std::cout << "Number of customers served: " << numberOfCustomersServed << std::endl;
    return 0;
}
