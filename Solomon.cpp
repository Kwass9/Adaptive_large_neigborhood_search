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

unsigned int getAtribudeForCustomer(std::string &str, const std::string& delimiter) {
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
    return getAtribudeForCustomer(str, delimiter);
}

void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix) {
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
    unsigned int min = INT_MAX - 1;
    unsigned int minIndex = 0;
    for (int i = 0; i < customers.size(); ++i) {
        if (!customers[i].isRouted()) {
            if (customers[i].getDueDate() < min) {
                min = customers[i].getDueDate();
                minIndex = i;
            }
        }
    }
    return minIndex;
}

//toto asi pojde uplne prec
double calculateMaxPushForward(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                               std::vector<int> &route) {
    double max = 0;
    for (int i = 0; i < route.size() - 1; ++i) {
        if (max < (pushForward[route[i]] - timeWaitedAtCustomer[route[i + 1]])) {
            max = pushForward[route[i]] - timeWaitedAtCustomer[route[i + 1]];
        }
    }
    return max;
}

//toto este asi nie je done
void calcualtePushForward(std::vector<double> &pushForward, std::vector<double> &timeWaitedAtCustomer,
                          std::vector<int> &route, std::vector<customer> &customers) {
    for (int i = 0; i < route.size() - 1; ++i) {
        /**tato rovnica nie je dobre je to len vystrel zatial...*/
        pushForward[route[i + 1]] = pushForward[route[i]] - timeWaitedAtCustomer[route[i + 1]] +
                                    customers[route[i + 1]].getServiceTime();
        //tu by bol nejaky break ze ak push forward bol absorbovany tou rezervou cakania tak uz dalsie nemusi pocitat...
    }
}

bool lema11(std::vector<double> &beginingOfService, std::vector<int> &pushForward,
            std::vector<int> &route, std::vector<customer> &customers, int u) {
    if (beginingOfService[u] <= customers[u].getDueDate()) {
        int i = 0;
        while (i < route.size()) {
            if (beginingOfService[route[i]] + pushForward[route[i]] <= customers[route[i]].getDueDate()) {
                ++i;
            } else {
                return false;
            }
        }
        return true;
    }
}

//funkcia rovno skontroluje aj lemma 1.1 ak nesplni tak ho nezaradi do mnoziny C1
std::vector<std::tuple<double, double, int>> findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                  std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                  std::vector<int> &route, std::vector<customer> &customers,
                  std::vector<double> &beginingOfServiceNew, int currentlyUsedCapacity, int maxCapacity) {
    std::vector<std::tuple<double, double, int>> mnozinaC1;
    double min = INT_MAX - 1;
    int minIndex = 0;
    for (unsigned int u = 0; u < customers.size(); ++u) {
        if (!customers[u].isRouted()
            && currentlyUsedCapacity + customers[u].getDemand() <= maxCapacity) { //kontrola kapacity vozidla
            for (int i = 0; i < route.size(); ++i) {
                //podmienky lemma 1.1
                if (beginingOfService[route[i]] + pushForward[route[i]] <= customers[route[i]].getDueDate()
                    && beginingOfService[u] <= customers[u].getDueDate()) {
                    double c11 = distanceMatrix[route[i]][u] + distanceMatrix[u][route[i + 1]] -
                                 distanceMatrix[route[i]][route[i + 1]];
                    double c12 = beginingOfServiceNew[route[i + 1]] - beginingOfService[route[i + 1]];
                    double c1 = alfa1 * c11 + alfa2 * c12;
                    if (c1 < min) {
                        min = c1;
                        minIndex = i;
                    }
                }
            }
            mnozinaC1.emplace_back(std::make_tuple(minIndex, min, u));
        }
    }
    return mnozinaC1;
}

std::pair<int, int> findOptimumForC2(std::vector<std::tuple<double, double, int>> mnozinaC1, double lambda,
                      std::vector<std::vector<double>> &distanceMatrix) {
    double C2 = INT_MAX - 1;
    int minIndex = 0;
    std::pair<int, int> optimalInsertion;
    for (int i = 0; i < mnozinaC1.size(); ++i) {
        double c2 = lambda * distanceMatrix[0][std::get<2>(mnozinaC1[i])] - std::get<1>(mnozinaC1[i]);
        if (c2 < C2) {
            C2 = c2;
            minIndex = i;
        }
    }
    optimalInsertion.first = std::get<0>(mnozinaC1[minIndex]);
    optimalInsertion.second = std::get<2>(mnozinaC1[minIndex]);
    return optimalInsertion;
}

//toto este nie je uplne hotove
//este nemam kontrolu na kapacitu vozidla asi este v C1 potrebujem nejaku podmienku
void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
                          ,std::vector<double> &beginingOfService, std::vector<customer> &customers
                          ,int currentlyUsedCapacity) {
    route.insert(route.begin() + optimalInsertion.first, optimalInsertion.second);
    //toto by slo urobit lepsie kusok nejak stopnut ked zistim ze sa PF a cakanie vykratia alebo cosi
    for (int i = optimalInsertion.first + 1; i < route.size(); ++i) {
        beginingOfService[route[i]] = beginingOfService[route[i - 1]] + customers[route[i - 1]].getServiceTime();
    }
    currentlyUsedCapacity += customers[optimalInsertion.second].getDemand();
}

std::vector<int> createNewRoute(int currentlyUsedCapacity, std::vector<std::vector<int>> &routes, std::vector<int> &route) {
    currentlyUsedCapacity = 0;
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

    if (argc == 6) {
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

//    /**kontrolny vypis*/
//    for (auto & i : data) {
//        std::cout << i << std::endl;
//    }

    //zbavenie sa hlavicky v subore
    for (int i = 0; i < 9; ++i) {
        data[i].erase();
    }

    //inicializacia zakaznikov
    for (auto & i : data) {
        if ((cell = i.find(delimiter)) != std::string::npos) {
            unsigned int id = processString(i, delimiter);
            unsigned int x = processString(i, delimiter);
            unsigned int y = processString(i, delimiter);
            unsigned int demand = processString(i, delimiter);
            unsigned int readyTime = processString(i, delimiter);
            unsigned int dueDate = processString(i, delimiter);
            unsigned int serviceTime = processString(i, delimiter);
            auto customer = new class customer(id, x, y, demand, readyTime, dueDate, serviceTime);
            customers.emplace_back(*customer);
        }
    }

    std::vector<std::vector<double>> distanceMatrix;
    calculateDistances(customers, distanceMatrix);

    std::vector<int> route;
    std::vector<std::vector<int>> routes;

    /**co je lepsie drzat zoznam nenavstivenych alebo atributy u zakaznikov?*/
    int unvisitedCustomers = customers.size() - 1;

    static int vehicleCapacity = 200;
    int currentlyUsedCapacity = 0;

    std::vector<double> timeWaitedAtCustomer;
    std::vector<double> pushForward;
    auto maxPushForward = calculateMaxPushForward(pushForward, timeWaitedAtCustomer, route);
    std::vector<double> beginingOfService;
    std::vector<double> beginingOfServiceNew; //toto este neviem uplne ako ale je to k c12

    double latestTimeForReturn = customers[0].getDueDate(); //pojde prec ked sa upravi uloha

    //zaciatok cesty
    auto minIndex = findCustomerWithEarliestDeadline(customers);
    route.push_back(minIndex);
    customers[minIndex].markAsRouted();
    insertCustomerToRoad(route, std::make_pair(0, minIndex), beginingOfService, customers, currentlyUsedCapacity);

    /**nemam vymyslene ako pouzivat PF a jeho vypocet este*/

    while (unvisitedCustomers != 0) { //while unroute customers masti ich do ciest
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, beginingOfService, pushForward, route, customers, beginingOfServiceNew,
                               currentlyUsedCapacity, vehicleCapacity);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix);
            insertCustomerToRoad(route, c2, beginingOfService, customers, currentlyUsedCapacity);
            unvisitedCustomers--;
        } else {
            route = createNewRoute(currentlyUsedCapacity, routes, route);
            minIndex = findCustomerWithEarliestDeadline(customers);
        }
    }
    //zaverecny vypis
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size(); ++j) {
            std::cout << routes[i][j] << " -> ";
        }
        std::cout << std::endl;
    }
    int totalDistance = 0;
    int numberOfVehicles = routes.size();
    std::cout << "Total distance: " << totalDistance << std::endl;
    std::cout << "Number of vehicles: " << numberOfVehicles << std::endl;
    return 0;
}
