//
// Created by andrej on 9.2.2024.
//

#include <algorithm>
#include <string>
#include "test.h"

std::string test::corectnessTest(const std::vector<customer>& customers, const std::vector<std::vector<double>> &bestSchedule,
                          const std::vector<std::vector<int>>& bestRoutes) {
    for (const auto& customer : customers) {
        int routeIndex = -1;
        size_t customerIndexInRoute = 0;

        for (size_t i = 0; i < bestRoutes.size(); ++i) {
            const auto& bestRoute = bestRoutes[i];
            auto it = std::find(bestRoute.begin(), bestRoute.end(), customer.getId());
            if (it != bestRoute.end()) {
                routeIndex = i;
                customerIndexInRoute = it - bestRoute.begin();
                break;
            }
        }

        if (routeIndex == -1) {
            return "false";
        }

        if (routeIndex >= bestSchedule.size() ||
            customerIndexInRoute >= bestSchedule[routeIndex].size() ||
//            bestSchedule[routeIndex][customerIndexInRoute] < customer.getReadyTime() || //tu sa este asi musi priratat cas cakania malo by stacit kontrolovat due date
            bestSchedule[routeIndex][customerIndexInRoute] > customer.getDueDate()) {
            return "false";
        }
    }
    return "true";
}
