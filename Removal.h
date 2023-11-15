//
// Created by andrej on 15.11.2023.
//

#ifndef SOLOMON_REMOVAL_H
#define SOLOMON_REMOVAL_H

#include <vector>

class Removal {
private:
    std::vector<std::vector<int>> R;
public:
    Removal();
    std::vector<int> calculateR();
    const std::vector<std::vector<int>> &getR() const;
};


#endif //SOLOMON_REMOVAL_H
