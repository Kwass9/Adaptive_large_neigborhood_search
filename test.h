//
// Created by andrej on 9.2.2024.
//

#ifndef SOLOMON_TEST_H
#define SOLOMON_TEST_H

#include <vector>
#include "Customer.h"

class test {
public:
    static std::string corectnessTest(const std::vector<customer>& customers, const std::vector<std::vector<double>>& bestSchedule,
                        const std::vector<std::vector<int>>& bestRoutes);
};


#endif //SOLOMON_TEST_H
