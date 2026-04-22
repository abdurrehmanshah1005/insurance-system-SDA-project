#ifndef WORKSHOP_H
#define WORKSHOP_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

struct Workshop {
    int    workshopId = 0;
    std::string name;
    std::string address;
    std::string contact;
    WorkshopStatus status = WorkshopStatus::REGISTERED;

    bool isActive() const { return status == WorkshopStatus::REGISTERED; }

    std::string toCSV() const {
        return std::to_string(workshopId) + "|" + name + "|" + address + "|" +
               contact + "|" + workshopStatusToString(status);
    }

    static Workshop fromCSV(const std::string& line) {
        Workshop w;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); w.workshopId = std::stoi(token);
        std::getline(ss, w.name,    '|');
        std::getline(ss, w.address, '|');
        std::getline(ss, w.contact, '|');
        std::getline(ss, token, '|'); w.status = stringToWorkshopStatus(token);
        return w;
    }

    void display() const {
        std::cout << "  ID: " << workshopId
                  << " | Name: " << name
                  << " | Address: " << address
                  << " | Contact: " << contact
                  << " | Status: " << workshopStatusToString(status) << "\n";
    }

    int getId() const { return workshopId; }
};

#endif // WORKSHOP_H
