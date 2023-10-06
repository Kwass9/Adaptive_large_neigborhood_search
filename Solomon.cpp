#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char * argv[]) {
    std::string path;
    if (argc == 6) {
        path = argv[1];
        if (path.empty()) {
            std::cerr << "Not a valid argument" << std::endl;
        }

        if (atof(argv[2]) >= 0 && atof(argv[3]) >= 0) {
            double lambda = atof(argv[2]);
            double q = atof(argv[3]);
        } else {
            std::cerr << "We need non negative value for argument lambda" << std::endl;
        }
        double alfa1 = atof(argv[4]);
        double alfa2 = atof(argv[5]);
        double check = 0.00001; // pre pripad ze by si pocitac zmyslel podat desatinnu ciarku
        if (alfa1 + alfa2 <= 1 + check || alfa1 + alfa2 >= 1 - check) {
            std::cerr << "Not valid value of alfas" << std::endl;
        }
    } else {
        std::cend("Nespravny pocet argumentov");
    }
    std::ifstream input;
    std::vector<std::string> data{};
    std::string par;
    std::string delimiter = ";";
    size_t cell = 0;
    std::string word;
    input.open(path);
    if (input) {
        while(!input.eof()) {
            getline(input, par);
            data.push_back(par);
        }
    } else {
        std::cerr << "Subor nebol otvoreny" << std::endl;
    }
    input.close();

}
