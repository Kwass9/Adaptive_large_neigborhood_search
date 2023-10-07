//
// Created by Andrej on 07/10/2023.
//

#ifndef SOLOMON_CUSTOMER_H
#define SOLOMON_CUSTOMER_H


class customer {
private:
    unsigned int id;
    unsigned int xcord;
    unsigned int ycord;
    unsigned int demand;
    unsigned int readyTime;
    unsigned int dueDate;
    unsigned int serviceTime;
    bool routed = false;
public:
    customer(unsigned int idNum, unsigned int x, unsigned int y, unsigned int dem,
             unsigned int rdyTime, unsigned int dueD, unsigned int serviceDuration);
    ~customer();
    void markAsRouted();
};


#endif //SOLOMON_CUSTOMER_H
