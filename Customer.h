//
// Created by Andrej on 07/10/2023.
//

#ifndef SOLOMON_CUSTOMER_H
#define SOLOMON_CUSTOMER_H


class Customer {
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
    Customer(unsigned int idNum, double x, double y, unsigned int dem,
             double rdyTime, double dueD, double serviceDuration);
    ~Customer();
    Customer(const Customer &other);
    void markAsRouted();
    void markAsUnrouted();
    bool isRouted() const;
    double getXcord() const;
    double getYcord() const;
    unsigned int getDemand() const;
    double getReadyTime() const;
    double getDueDate() const;
    double getServiceTime() const;
    unsigned int getId() const;
    bool isRoutedStatus() const;
};


#endif //SOLOMON_CUSTOMER_H
