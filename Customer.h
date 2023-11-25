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
    customer(unsigned int idNum, double x, double y, unsigned int dem,
             double rdyTime, double dueD, double serviceDuration);
    ~customer();
    void markAsRouted();
    bool isRouted() const;
    double getXcord() const;
    double getYcord() const;
    double getDemand() const;
    double getReadyTime() const;
    double getDueDate() const;
    double getServiceTime() const;
    void markAsUnrouted();
};


#endif //SOLOMON_CUSTOMER_H