#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <tuple>
#include "Customer.h"
#include "solomon.h"

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
    return std::stoi(word);
}

double processString(std::string &str, const std::string& delimiter) {
    removeDelimiters(str, delimiter);
    return getAtributeForCustomer(str, delimiter);
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
    auto *solomon = new class solomon(customers, alfa1, alfa2, lambda, q, startingCriteria);
    delete solomon;
    return 0;
}