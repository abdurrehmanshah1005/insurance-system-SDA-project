#ifndef STAFF_H
#define STAFF_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

struct Staff {
    int    staffId = 0;
    std::string name;
    StaffRole role = StaffRole::SALESMAN;
    std::string password;

    bool authenticate(const std::string& inputPassword) const {
        return password == inputPassword;
    }

    std::string toCSV() const {
        return std::to_string(staffId) + "|" + name + "|" +
               staffRoleToString(role) + "|" + password;
    }

    static Staff fromCSV(const std::string& line) {
        Staff s;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); s.staffId = std::stoi(token);
        std::getline(ss, s.name,     '|');
        std::getline(ss, token, '|'); s.role    = stringToStaffRole(token);
        std::getline(ss, s.password, '|');
        return s;
    }

    void display() const {
        std::cout << "  ID: " << staffId
                  << " | Name: " << name
                  << " | Role: " << staffRoleToString(role) << "\n";
    }

    int getId() const { return staffId; }
};

#endif // STAFF_H
