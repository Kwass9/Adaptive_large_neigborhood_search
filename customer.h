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
    bool routedStatus = false;
public:
    customer(unsigned int idNum, unsigned int x, unsigned int y, unsigned int dem,
             unsigned int rdyTime, unsigned int dueD, unsigned int serviceDuration);
    ~customer();
    void markAsRouted();
    bool isRouted() const;
    unsigned int getXcord() const;
    unsigned int getYcord() const;
    unsigned int getDemand() const;
    unsigned int getReadyTime() const;
    unsigned int getDueDate() const;
    unsigned int getServiceTime() const;
};


#endif //SOLOMON_CUSTOMER_H
