//
// Created by andrej on 15.11.2023.
//

#include <climits>
#include <valarray>
#include <iostream>
#include <cstring>
#include <fstream>
#include "Data.h"

Data::Data(int argc, char * argv[]) {
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
        int hjl = 0;
        while(!input.eof()) {
            hjl++;
            if (hjl == 111) { //docasne riesenie aby som runol ostatne subory... pre nejaky dovod asi skrity charakter prida prazdny riadok na konci...
                break;
            }
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
    int h = 0;
    for (auto & i : data) {
        if ((cell = i.find(delimiter)) != std::string::npos) {
            h++;
            double id = processString(i, delimiter);
            double x = processString(i, delimiter);
            double y = processString(i, delimiter);
            double demand = processString(i, delimiter);
            double readyTime = processString(i, delimiter);
            double dueDate = processString(i, delimiter);
            double serviceTime = processString(i, delimiter);
            auto customer = new class customer(id, x, y, demand, readyTime, dueDate, serviceTime);
            customers.emplace_back(*customer);
        }
    }
    calculateDistances(customers, distanceMatrix);
}

void Data::calculateDistances(std::vector<customer> &customers, std::vector<std::vector<double>> &distanceMatrix) {
    for (int i = 0; i <= customers.size(); ++i) {
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
    for (int i = 0; i < customers.size(); ++i) {
        distanceMatrix[customers.size()][i] = sqrt(pow(customers[0].getXcord() - customers[i].getXcord(), 2) +
                                                   pow(customers[0].getYcord() - customers[i].getYcord(), 2));
    }
}

void Data::finalPrint(std::vector<std::vector<int>> &routes, std::vector<std::vector<double>> &timeSchedule, double distance,
                      std::vector<double> &timeWaitedAtCustomer) {
    for (auto & route : routes) {
        for (int j = 0; j < route.size(); ++j) {
            std::cout << route[j];
            if (j != route.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
    double totalDistance = 0;
    double totalScheduleTime = 0;
    double waitingTimeInSchedule = 0;
    auto numberOfCustomersServed = 0;
    for (int i = 0; i < routes.size(); ++i) {
        for (int j = 0; j < routes[i].size() - 2; ++j) {
            totalDistance += distanceMatrix[routes[i][j]][routes[i][j + 1]];
            std::cout << distanceMatrix[routes[i][j]][routes[i][j + 1]] << " | ";
        }
        std::cout << distanceMatrix[routes[i][routes[i].size() - 2]][0] << std::endl;
        totalDistance += distanceMatrix[routes[i][routes[i].size() - 2]][0];
        numberOfCustomersServed += routes[i].size() - 2;
        int j = routes[i].size() - 1;
    }
    for (int i = 0; i < timeSchedule.size(); ++i) {
        auto t = timeSchedule[i].size() - 1;
        totalScheduleTime += (timeSchedule[i][t]);
    }

    for (int i = 1; i < timeWaitedAtCustomer.size() - 1; ++i) {
        waitingTimeInSchedule += timeWaitedAtCustomer[i];
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
    std::cout << "Waiting time at customers: " << waitingTimeInSchedule << std::endl;
    std::cout << "Number of customers served: " << numberOfCustomersServed << std::endl;
}

void Data::retrieveData(std::vector<std::vector<double>> &schedule, std::vector<std::vector<int>> &routes_,
                        std::vector<double> &waiting, double &dist) {
    timeSchedule = schedule;
    routes = routes_;
    waitingTime = waiting;
    distance = dist;
}

std::vector<std::vector<double>> &Data::getDistanceMatrix() {
    return distanceMatrix;
}

void Data::removeCharsFromString(std::string &str, char *charsToRemove) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(std::remove(str.begin(),str.end(),charsToRemove[i]),str.end());
    }
}

void Data::removeDelimiters(std::string &str, const std::string &delimiter) {
    size_t cell;
    while ((cell = str.find(delimiter)) == 0) { //ak je na zaciatku medzera tak ju vymaz
        str.erase(0, cell + delimiter.length());
    }
}

double Data::getAtributeForCustomer(std::string &str, const std::string &delimiter) {
    size_t cell;
    std::string word;
    if ((cell = str.find(delimiter)) != std::string::npos) {
        word = str.substr(0, cell);
        str.erase(0, cell + delimiter.length());
    }
    return std::stoi(word);
}

double Data::processString(std::string &str, const std::string &delimiter) {
    removeDelimiters(str, delimiter);
    return getAtributeForCustomer(str, delimiter);
}
