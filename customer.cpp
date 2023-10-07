//
// Created by Andrej on 07/10/2023.
//

#include "customer.h"

customer::customer(unsigned int idNum,unsigned int x, unsigned int y, unsigned int dem, unsigned int rdyTime, unsigned int dueD,
                   unsigned int serviceDuration) {
    id = idNum;
    xcord = x;
    ycord = y;
    demand = dem;
    readyTime = rdyTime;
    dueDate = dueD;
    serviceTime = serviceDuration;
}


customer::~customer() = default;

void customer::markAsRouted() {
    routed = true;
}

