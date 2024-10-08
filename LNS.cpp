#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <tuple>
#include <valarray>
#include <random>
#include <limits>
#include "Customer.h"
#include "solomon.h"
#include "SimulatedAnnealing.h"
#include "ShawRemoval.h"
#include "Vehicle.h"

std::string getAnswer(const std::string& question) {
    std::cout << question << std::endl;
    std::string answer;
    std::cin >> answer;
    return answer;
}

void exportResults(const std::vector<std::vector<double>>& bestSchedule, const std::vector<std::vector<int>>& bestRoutes, double bestDistance) {
    std::ofstream output;
    output.open("results.txt");
    if (output) {
        output << "BestSchedule" << std::endl;
        for (auto &r: bestSchedule) {
            for (double j: r) {
                output << j << " ";
            }
            output << std::endl;
        }
        output << "BestRoutes" << std::endl;
        for (auto &r: bestRoutes) {
            for (double j: r) {
                output << j << " ";
            }
            output << std::endl;
        }
        output << std::endl;
        output << "BestDistance" << std::endl;
        output << bestDistance << std::endl;
    }
}

void printResults(const std::vector<std::vector<double>>& bestSchedule, const std::vector<std::vector<int>>& bestRoutes, double bestDistance) {
    std::cout << "BestSchedule" << std::endl;
    for (auto &r: bestSchedule) {
        for (double j: r) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "BestRoutes" << std::endl;
    for (auto &r: bestRoutes) {
        for (double j: r) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "BestDistance" << std::endl;
    std::cout << bestDistance << std::endl;
}

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

double getAtributeForCustomer(std::string &str, const std::string& delimiter) {
    size_t cell;
    std::string word;
    if ((cell = str.find(delimiter)) != std::string::npos) {
        word = str.substr(0, cell);
        str.erase(0, cell + delimiter.length());
    }
    return std::stod(word);
}

double processString(std::string &str, const std::string& delimiter) {
    removeDelimiters(str, delimiter);
    try {
        return getAtributeForCustomer(str, delimiter);
    } catch (std::invalid_argument &e) {
        return -1;
    }
}

double setInitialTemperature(double w, double solution) {
    double acceptW = w * solution;
    double temperature = acceptW / -log(0.5);
    return temperature;
}

int calculateRo(double ksi, std::vector<customer> &customers) {
    //4 ≤ ro ≤ min(100,ξn)
    int uppreBound = (int)std::min(ksi * (int)customers.size(), 100.0);
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(5, uppreBound);
    return distribution(generator);
}

int main(int argc, char * argv[]) {
    std::string path;
    double alfa1;
    double alfa2;
    double lambda;
    double q;
    std::vector<customer> customers;
    std::vector<Vehicle> vehicles;
    bool startingCriteria; //premenna vybera ci sa zacina s najvzdialenejsim zakaznikom alebo s najskor zaciatocnou dobou
    std::string pathToVehicles;
    std::string distPar;

    if (argc == 9) {
        path = argv[1];
        //kvoli tomu ako sa win chova k specialnym znakom
        removeCharsFromString(path,"\"");
        if (path.empty()) {
            std::cerr << "Not a valid argument" << std::endl;
        }
        if (atof(argv[2]) >= 0 && atof(argv[3]) >= 0) {
            lambda = atof(argv[3]);
            q = atof(argv[2]);
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
        pathToVehicles = argv[7];
        distPar = argv[8]; // "euclidean" "haversine"
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
        while(getline(input, par)) {
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
            int id = (int)processString(i, delimiter);
            double x = processString(i, delimiter);
            double y = processString(i, delimiter);
            double demand = processString(i, delimiter);
            double readyTime = processString(i, delimiter);
            double dueDate = processString(i, delimiter);
            double serviceTime = processString(i, delimiter);
            int specialRequirements = (int)processString(i, delimiter);
            if (!customers.empty()) {
                //pokial su dve liny rovnake a maju rovnake casy obsluhy tak potrebuje obsluhu dvoch opatrovateliek naraz
                customer& custBack = customers.back();
                if (id == custBack.getId()) {
                    if (custBack.doesTimeWindowExist(readyTime, dueDate)) {
                        auto winIndex = custBack.getIndexOfTimeWindow(readyTime, dueDate);
                        custBack.getTimeWindowAt(winIndex).incrementVehiclesRequired();
                    }
                    else {
                        custBack.createNewTimeWindow(readyTime, dueDate, demand, serviceTime);
                    }
                }
                else {
                    customer customer(id, x, y, specialRequirements);
                    customer.createNewTimeWindow(readyTime, dueDate, demand, serviceTime);
                    customers.emplace_back(customer);
                }
            } else {
                customer customer(id, x, y, specialRequirements);
                customer.createNewTimeWindow(readyTime, dueDate, demand, serviceTime);
                if (readyTime == -1 || dueDate == -1 || serviceTime == -1) {
                    customer.getTimeWindows().back().incrementCurentVehiclesServing();
                }
                customers.emplace_back(customer);
            }
        }
    }

    input.open(pathToVehicles);
    data.clear();
    if (input) {
        while(getline(input, par)) {
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

    //inicializacia vozidiel
    for (auto & i : data) {
        if ((cell = i.find(delimiter)) != std::string::npos) {
            int id = (int)processString(i, delimiter);
            double x = processString(i, delimiter);
            double y = processString(i, delimiter);
            double readyTime = processString(i, delimiter);
            double dueDate = processString(i, delimiter);
            int isWorking = (int)processString(i, delimiter);
            if (!vehicles.empty()) {
                if (id == vehicles.back().getId()) {
                    vehicles.back().editWorkingHours(readyTime, dueDate);
                } else {
                    Vehicle vehicle(id, std::numeric_limits<int>::max(), x, y, readyTime, dueDate, (int)customers.size(), isWorking);
                    vehicles.emplace_back(vehicle);
                }
            } else {
                Vehicle vehicle(id, std::numeric_limits<int>::max(), x, y, readyTime, dueDate, (int)customers.size(), isWorking);
                vehicles.emplace_back(vehicle);
            }
        }
    }

    std::vector<customer*> unservedCustomers;
    std::vector<customer> notValidCustomers;

//-------------------------------------------------------------------------------------------------------------------

    double temperature;
    double fi = 9;
    double chi = 3;
    int p = 6;
    double w = 0.05; //vysledok horsi o w percent od aktualneho ma sancu na akceptovanie 0.5
    double c = 0.99975; //cooling rate
    double eta = 0.025; //kontrola mnozstva hluku /noise control
    double ksi = 0.4; //parameter na kontrolu kolko requestov bude removnutych |  4 ≤ ro ≤ min(100,ξn)
    int ro; //number of reguest removed in iteraton

    auto *solomon = new class solomon(customers, alfa1, alfa2, lambda, q, startingCriteria, eta, vehicles, unservedCustomers);
    if (std::equal(distPar.begin(), distPar.end(), "euclidean")) {
        solomon->calculateDistances(customers, solomon::euclideanDistance());
    } else if (std::equal(distPar.begin(), distPar.end(), "haversine")) {
        solomon->calculateDistances(customers, solomon::haversineDistance());
    } else {
        std::cerr << "Not valid distance parameter" << std::endl;
    }
    std::string parPrint;
    std::string parExport;
    std::string parRun;
    const std::string questionPrint = "Do you want to print the results? (y/n)";
    const std::string questionExport = "Do you want to export the results? (y/n)";
    const std::string questionRun = "Do you want to run the program? (y/n)";
    std::string answer = "y";
    while (answer == "y") {
        auto answer = getAnswer(questionRun);
        while (answer != "y" && answer != "n") {
            std::cout << "Invalid answer try again" << std::endl;
            answer = getAnswer(questionRun);
        }
        if (answer == "n") {
            break;
        }
        if (answer == "y") {
            for (auto &customer: customers) {
                if (customer.getSpecificRequirementsForVehicle() != -1) {
                    if (!vehicles[customer.getSpecificRequirementsForVehicle()].getIsWorking()) {
                        std::cout << "Customer " << customer.getId() << " has specific requirements for vehicle "
                                  << customer.getSpecificRequirementsForVehicle() << " but it is not working"
                                  << std::endl;
                        customer.setSpecificRequirementsForVehicle(-1);
                    }
                }
                if (!customer.isServedByEnoughVehicles()) {
                    unservedCustomers.push_back(&customer);
                }
            }
            for (int i = (int) unservedCustomers.size() - 1; i >= 0; --i) {
                if (unservedCustomers[i]->getTimeWindows()[0].getReadyTime() == -1) {
                    notValidCustomers.push_back(*unservedCustomers[i]);
                }
            }
            for (auto &customer : notValidCustomers) {
                for (int i = (int) unservedCustomers.size() - 1; i >= 0; --i) {
                    if (unservedCustomers[i]->getId() == customer.getId()) {
                        unservedCustomers.erase(unservedCustomers.begin() + i);
                    }
                }
            }
            unservedCustomers.erase(unservedCustomers.begin()); //odstranenie depa
        }
        solomon->insertSpecialRequirements(customers, vehicles, unservedCustomers);
        solomon->run(customers, unservedCustomers, vehicles);
        auto distanceMatrix = solomon->getDistanceMatrix();
        std::vector<std::vector<int>> routes;
        std::vector<std::vector<double>> timeSchedule;
        std::vector<double> usedCapacity;
        for (auto & vehicle : vehicles) {
            auto r = vehicle.getRoute();
            auto ts = vehicle.getTimeSchedule();
            auto uc = vehicle.getUsedCapacity();
            routes.push_back(r);
            timeSchedule.push_back(ts);
            usedCapacity.push_back(uc);
        }
        if (!unservedCustomers.empty()) {
            temperature = setInitialTemperature(w, 3000);
        } else {
            temperature = setInitialTemperature(w, solomon->getDistance());
        }
        auto *simulatedAnnealing = new class SimulatedAnnealing(temperature, c);
        if (unservedCustomers.empty()) {
            simulatedAnnealing->tryToAcceptNewSolution(solomon->getDistance(), vehicles, solomon->getWaitingTime(), customers);
        }
        auto *shawRemoval = new class Shaw_Removal(fi, chi, p, (int)customers.size(), notValidCustomers);
        int i = 0;
        std::cout << "Running..." << std::endl;
        while (i < 100) {
            ro = calculateRo(ksi, customers);
            shawRemoval->removeRequests(distanceMatrix, customers, ro, solomon->getWaitingTime(), vehicles, unservedCustomers);
            solomon->run(customers, unservedCustomers, vehicles);
            if (unservedCustomers.empty()) {
                simulatedAnnealing->tryToAcceptNewSolution(solomon->getDistance(),vehicles, solomon->getWaitingTime(), customers);
            } else {
                simulatedAnnealing->updateTemperature();
                if (simulatedAnnealing->hasPreviousSolution()) {
                    simulatedAnnealing->resetToCurrentSolution(customers, vehicles);
                    unservedCustomers.clear();
                }
            }
            i++;
        }
        auto bestSchedule = simulatedAnnealing->getBestTimeSchedule();
        auto bestDistance = simulatedAnnealing->getBestSolution();
        auto bestWaitingTime = simulatedAnnealing->getBestWaitingTime();
        auto bestRoutes = simulatedAnnealing->getBestRoutes();

        //-------------------------------------------------------------------------------------------------------------------

        answer = getAnswer(questionPrint);
        while (answer != "y" && answer != "n") {
            std::cout << "Invalid answer try again" << std::endl;
            answer = getAnswer(questionRun);
        }
        if (answer == "y") {
            printResults(bestSchedule, bestRoutes, bestDistance);
        }
        answer = getAnswer(questionExport);
        while(answer != "y" && answer != "n") {
            std::cout << "Invalid answer try again" << std::endl;
            answer = getAnswer(questionExport);
        }
        if (answer == "y") {
            exportResults(bestSchedule, bestRoutes, bestDistance);
        }
        delete simulatedAnnealing;
        delete shawRemoval;
        bestRoutes.clear();
        bestSchedule.clear();
        bestWaitingTime.clear();
        usedCapacity.clear();
        routes.clear();
        timeSchedule.clear();
        for (auto & customer : customers) {
            customer.prepareForNextRun();
        }
        for (auto & vehicle : vehicles) {
            vehicle.prepareForNextRun();
        }
    }
    delete solomon;
    std::cout << "Program finished" << std::endl;
    return 0;
}