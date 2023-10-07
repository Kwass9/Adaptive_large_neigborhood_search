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

void calculateDistances(std::vector<customer> &customers, std::vector<std::vector<int>> &distanceMatrix) {
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

    /**kontrolny vypis*/
    for (auto & i : data) {
        std::cout << i << std::endl;
    }
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

    std::vector<std::vector<int>> distanceMatrix;
    calculateDistances(customers, distanceMatrix);

    std::vector<int> route;
    std::vector<std::vector<int>> routes;

    /**co je lepsie drzat zoznam nenavstivenych alebo atributy u zakaznikov?*/
    //std::vector<int> unvisitedCustomers;

    static int vehicleCapacity = 200;

    std::vector<double> pushForward;
    double maxPushForward = 0;
    std::vector<double> timeWaitedAtCustomer;
    std::vector<double> beginingOfService;
    std::vector<double> beginingOfServiceNew; //toto etse neviem uplne ako ale je to k c12

    double currentTime = 0;
    double latestTimeForReturn = customers[0].getDueDate(); //pojde prec ked sa upravi uloha
    int currentPositon = 0;

    //nebude treba asi
    //najprv najdem zakaznika s najmensim x a y a ten bude prvy v trase
    int min = INT_MAX - 1;
    int minIndex = 0;

    //najdem najblizsieho zakaznika
    for (int i = 0; i < customers.size(); ++i) {
        if (!customers[i].isRouted()) {
            if (distanceMatrix[currentPositon][i] < min) {
                min = distanceMatrix[currentPositon][i];
                minIndex = i;
            }
        }
    }

    //c11
    //distanceMatrix[currentPositon][] + distanceMatrix[][] - distanceMatrix[currentPositon][];
    //c12
    //beginingOfServiceNew[] - beginingOfService[];
    //c1 best feasible insertion
    //c1 = alfa1*c11 + alfa2*c12;

    //c2
    //lambda*distanceMatrix[currentPositon][minIndex] + c1;

    //minC1
    //optimumC2
}
