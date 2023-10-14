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
    unsigned int min = INT_MAX - 1;
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

//nieco este bude treba urobit s begining of service new
void calculatePushForward(std::vector<double> &pushForward, const std::vector<double>& beginingOfService,
                          const std::vector<int>& route, int u,
                          std::vector<double> timeWaitedAtCustomer, std::vector<std::vector<double>> distanceMatrix,
                          std::vector<customer> customers) {
    double epsilon = 0.0000001; //kvoli double
    /**indexi bude asi treba prepocitat na indexi v ceste ako v calcnewbeginings*/
    pushForward[u + 1] = distanceMatrix[u - 1][u] + distanceMatrix[u][u + 1] + customers[u].getServiceTime()
            - distanceMatrix[u - 1][u + 1] - timeWaitedAtCustomer[u + 1];
    if (route.size() > 1) { //ten if pojde asi von
        for (int i = u + 2; i < route.size() - i; ++i) {
            if (pushForward[i - 1] - timeWaitedAtCustomer[i] > 0 + epsilon) { /**taketo podmienky este treba po celom kode prepisat na ternarne*/
                pushForward[i] = pushForward[i - 1] - timeWaitedAtCustomer[i];
            } else {
                pushForward[i] = 0;
                break;
            }
        }
    }
}
/**bude problem s indexami v begining[route[i + 1]] v metode nad*/
void calculateNewBeginings(std::vector<double> pushForward, std::vector<double> &timeWaitedAtCustomer,
                                   std::vector<int> route, const std::vector<customer>& customers, int u,
                                   std::vector<double> &beginingOfService) {
    double epsilon = 0.0000001;
    if (route.size() > 1) { //ten if pojde asi von
        for (int i = u; i < route.size() - 1; ++i) {
            if (pushForward[i + 1] - timeWaitedAtCustomer[i + 1] > 0 + epsilon) {
                beginingOfService[route[i + 1]] = beginingOfService[route[i]] + pushForward[route[i + 1]]
                        - timeWaitedAtCustomer[route[i + 1]];
                timeWaitedAtCustomer[route[i + 1]] = 0;
            } else {
                timeWaitedAtCustomer[route[i + 1]] -= pushForward[route[i + 1]];
                break;
            }
        }
    }
}

bool lema11(std::vector<double> beginingOfService, std::vector<double> pushForward,
            std::vector<int> route, std::vector<customer> customers, int u, int i,
            std::vector<std::vector<double>> distanceMatrix) {
    auto beginingOfServiceU = beginingOfService[route[u - 1]] + distanceMatrix[route[u - 1]][u];
    if (beginingOfServiceU <= customers[u].getDueDate()) {
        int j = u + 1;
        while (pushForward[j] > 0 || j < route.size() - i) {
            if (beginingOfService[route[j]] + pushForward[route[j]] > customers[route[j]].getDueDate()) {
                return false;
            }
            ++j;
        }
        return true;
    }
    return false;
}

std::vector<std::tuple<double, double, int>> findMinForC1(double alfa1, double alfa2, std::vector<std::vector<double>> &distanceMatrix,
                  std::vector<double> &beginingOfService, std::vector<double> &pushForward,
                  std::vector<int> &route, std::vector<customer> &customers,
                  int currentlyUsedCapacity, int maxCapacity, std::vector<double> &timeWaitedAtCustomer) {
    std::vector<std::tuple<double, double, int>> mnozinaC1;
    double min = INT_MAX - 1;
    int minIndex = 0;
    for (unsigned int u = 0; u < customers.size(); ++u) {
        if (!customers[u].isRouted()
            && currentlyUsedCapacity + customers[u].getDemand() <= maxCapacity) { //kontrola kapacity vozidla
            for (int i = 0; i < route.size() - 1; ++i) {
                calculatePushForward(pushForward, beginingOfService, route, u, timeWaitedAtCustomer,
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
            mnozinaC1.emplace_back(std::make_tuple(minIndex, min, u));
        }
    }
    return mnozinaC1;
}

std::pair<int, int> findOptimumForC2(std::vector<std::tuple<double, double, int>> mnozinaC1, double lambda,
                      std::vector<std::vector<double>> &distanceMatrix, std::vector<int> &route) {
    double C2 = INT_MAX - 1;
    int maxIndex = 0;
    std::pair<int, int> optimalInsertion;
    for (int i = 0; i < mnozinaC1.size(); ++i) {
        double c2 = lambda * distanceMatrix[0][std::get<2>(mnozinaC1[i])] - std::get<1>(mnozinaC1[i]);
        if (c2 > C2) { //sem by to chcelo este pridat kontrolu ktory vkladany prvok je lepsi na zaklade skorsieho dueto
            C2 = c2;
            maxIndex = i;
        }
    }
    optimalInsertion.first = std::get<0>(mnozinaC1[maxIndex]); //i
    optimalInsertion.second = std::get<2>(mnozinaC1[maxIndex]); //u
    return optimalInsertion;
}

//toto este nie je uplne hotove
/**first pomenovat ako i a second ako u*/
void insertCustomerToRoad(std::vector<int> &route, std::pair<int, int> optimalInsertion
                          ,std::vector<double> &beginingOfService
                          ,std::vector<customer> &customers, std::vector<double> &timeWaitedAtCustomer
                          ,int currentlyUsedCapacity, std::vector<std::vector<double>> &distanceMatrix
                          ,std::vector<double> &pushForward) {
    route.insert(route.begin() + optimalInsertion.first, optimalInsertion.second);
    int timeOfService = beginingOfService[route[optimalInsertion.first - 1]] /**skontrolovat*/
            + distanceMatrix[route[optimalInsertion.first - 1]][optimalInsertion.second]
            + customers[optimalInsertion.first - 1].getServiceTime();
    beginingOfService.insert(beginingOfService.begin() + optimalInsertion.first, timeOfService); //tu padne pri neviem preco zatial -- pozri r176
    calculateNewBeginings(pushForward, timeWaitedAtCustomer, route, customers, optimalInsertion.first, beginingOfService);

    /**spustit vypocet na zaciatky a na upravu dlzky cakania*/
    currentlyUsedCapacity += customers[optimalInsertion.second].getDemand();
    customers[optimalInsertion.second].markAsRouted();
}

std::vector<int> createNewRoute(int &currentlyUsedCapacity, std::vector<std::vector<int>> &routes, std::vector<int> &route) {
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
//    std::cout << "Distance matrix: " << distanceMatrix.size() << std::endl;
    //    for (int i = 0; i < distanceMatrix.size(); ++i) {
//        for (int j = 0; j < distanceMatrix[i].size(); ++j) {
//            std::cout << distanceMatrix[i][j] << " ";
//        }
//    }
    std::vector<int> route;
    std::vector<std::vector<int>> routes;

    /**co je lepsie drzat zoznam nenavstivenych alebo atributy u zakaznikov?*/
    int unvisitedCustomers = customers.size() - 1;

    static int vehicleCapacity = 200;
    int currentlyUsedCapacity = 0;

    std::vector<double> timeWaitedAtCustomer;
    std::vector<double> pushForward;
    std::vector<double> beginingOfService;
    //std::vector<double> beginingOfServiceNew; //bude stacit asi len obyc premenna double
    for (int i = 0; i < beginingOfService.size(); ++i) {
    //    beginingOfServiceNew.push_back(0);
        pushForward.push_back(0);
        timeWaitedAtCustomer.push_back(0);
    }
    double latestTimeForReturn = customers[0].getDueDate(); //pojde prec ked sa upravi uloha

    //zaciatok cesty
    insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                         distanceMatrix, pushForward);
    auto minIndex = findCustomerWithEarliestDeadline(customers);
    insertCustomerToRoad(route, std::make_pair(1, minIndex), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                         distanceMatrix, pushForward);
    beginingOfService[1] = customers[minIndex].getReadyTime(); //dost surove uvidim ako to zmenim
    unvisitedCustomers--;

    while (unvisitedCustomers != 0) { //while unroute customers masti ich do ciest
        auto c1 = findMinForC1(alfa1, alfa2, distanceMatrix, beginingOfService, pushForward, route, customers,
                               currentlyUsedCapacity, vehicleCapacity, timeWaitedAtCustomer);
        if (!c1.empty()) {
            auto c2 = findOptimumForC2(c1, lambda, distanceMatrix, route);
            c2.first += 1; //index pola sa rata od nula nie jedla tak podavam ale je mozne ze iba mentalne podavam seba akurat :D
            insertCustomerToRoad(route, c2, beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity, distanceMatrix, pushForward);
            unvisitedCustomers--;
        } else {
            route = createNewRoute(currentlyUsedCapacity, routes, route);
            insertCustomerToRoad(route, std::make_pair(0, 0), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward);
            minIndex = findCustomerWithEarliestDeadline(customers);
            insertCustomerToRoad(route, std::make_pair(1, minIndex), beginingOfService, customers, timeWaitedAtCustomer, currentlyUsedCapacity,
                                 distanceMatrix, pushForward);
            beginingOfService[1] = customers[minIndex].getReadyTime(); //dost surove uvidim ako to zmenim
            unvisitedCustomers--;
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
