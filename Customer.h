//
// Created by Andrej on 07/10/2023.
//

#ifndef SOLOMON_CUSTOMER_H
#define SOLOMON_CUSTOMER_H


class customer {
private:
    unsigned int id;
    double xcord;
    double ycord;
    double demand;
    double readyTime;
    double dueDate;
    double serviceTime;
    bool routedStatus = false;
public:
    customer(unsigned int idNum, double x, double y, double dem,
             double rdyTime, double dueD, double serviceDuration);
    ~customer();
    customer(customer const &customer);
    void markAsRouted();
    bool isRouted() const;
    unsigned int getId() const;
    double getXcord() const;
    double getYcord() const;
    double getDemand() const;
    double getReadyTime() const;
    double getDueDate() const;
    double getServiceTime() const;
    void markAsUnrouted();
};


#endif //SOLOMON_CUSTOMER_H