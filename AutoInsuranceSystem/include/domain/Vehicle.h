#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <iostream>
#include <sstream>

struct Vehicle {
    int    vehicleId = 0;
    std::string registrationNo;
    std::string make;
    std::string model;
    int    year = 0;
    int    customerId = 0;

    std::string toCSV() const {
        return std::to_string(vehicleId) + "|" + registrationNo + "|" + make + "|" +
               model + "|" + std::to_string(year) + "|" + std::to_string(customerId);
    }

    static Vehicle fromCSV(const std::string& line) {
        Vehicle v;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); v.vehicleId = std::stoi(token);
        std::getline(ss, v.registrationNo, '|');
        std::getline(ss, v.make,           '|');
        std::getline(ss, v.model,          '|');
        std::getline(ss, token, '|'); v.year       = std::stoi(token);
        std::getline(ss, token, '|'); v.customerId = std::stoi(token);
        return v;
    }

    void display() const {
        std::cout << "  ID: " << vehicleId
                  << " | Reg#: " << registrationNo
                  << " | " << make << " " << model << " (" << year << ")"
                  << " | Owner ID: " << customerId << "\n";
    }

    int getId() const { return vehicleId; }
};

#endif // VEHICLE_H
